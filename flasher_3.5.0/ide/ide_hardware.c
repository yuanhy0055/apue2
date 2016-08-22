/*
 * ide_hardware.c: main module for reading and programming of flashproms with
 * the hardware from Andreas Ziermann.
 * Please look at
 *
 * http://www.loet.de
 *
 *
 * Dirk Pfau 09/2001 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * History:
 *
 *
 */

/* ------------------------------------------------------------------------ */

#include <linux/version.h>
#include <linux/module.h>
#include "ide_flash.h"
#include <linux/ide.h>

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr,int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr,int dev), \
                          char * info, __u32 max_romsize);

void release_flash_proc(void);

struct proc_dir_entry *ide_proc;

static void flash_put_dip32(__u8 byte,__u32 addr,int dev);
static void flash_put_plcc32(__u8 byte,__u32 addr,int dev);
static __u8 flash_get_dip32(__u32 addr,int dev);
static __u8 flash_get_plcc32(__u32 addr,int dev);
static __u32 test_base(void);
static __u16 ide_base=0;

/* ------------------------------------------------------------------------ */

///MODULE_PARM(io_addr, "1-4h");
///MODULE_PARM_DESC(io_addr, "address of used ide port" );

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_DESCRIPTION("ide Flasher for Linux");
#if LINUX_VERSION_CODE > 0x02040A 
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static int ide_init_module(void)
#else
int init_module(void)
#endif
{
char data[MAX_INFO_SIZE];

	ide_base = test_base();

	if (!ide_base)
	{
		printk(KERN_DEBUG "%s: no hardware found.\n", MODULE_NAME);
		return -EIO;
	}

// create our own directory at "/proc"

	if(!(ide_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root)))
		goto error_ide_dir;

#if LINUX_VERSION_CODE > 0x020400
	ide_proc->owner = THIS_MODULE;
#endif


	sprintf(data,	"hardware:       ide flasher\n"
			"socket:         DIP32\n"
			"base-io:        0x%X\n",
			ide_base);
// create "DIP32" entry at our own directory

	if (!init_flash_proc("DIP32", ide_proc, flash_get_dip32, flash_put_dip32, \
	                   data, MAXFLASHSIZE))
		goto error_dip32;


	sprintf(data,	"hardware:     ide flasher\n"
			"socket:       PLCC32\n"
			"base-io:      0x%X\n",
			ide_base);

// create "PLCC32" entry at our own directory

	if (!init_flash_proc("PLCC32", ide_proc, flash_get_plcc32, flash_put_plcc32, \
	                   data, MAXFLASHSIZE))
		goto error_plcc32;

	printk(KERN_DEBUG "%s: ide-flasher found at 0x%x.\n", MODULE_NAME, ide_base);
	printk(KERN_DEBUG "%s: Module installed.\n", MODULE_NAME );

return 0;
error_plcc32:
	release_flash_proc();
error_dip32:
	printk(KERN_DEBUG "%s: error while proc_init.\n", MODULE_NAME );
	remove_proc_entry( MODULE_NAME, &proc_root );
error_ide_dir:
	release_region(ide_base, 8);
return -EIO;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static void ide_cleanup(void)
#else
void cleanup_module(void)
#endif
{ 
	release_flash_proc();
	remove_proc_entry( MODULE_NAME, &proc_root );
	release_region(ide_base, 8);

	printk(KERN_DEBUG "%s: cleanup_module successfull.\n", MODULE_NAME );

return;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
module_init(ide_init_module);
module_exit(ide_cleanup);
#endif

/* ------------------------------------------------------------------------ */

// we test the 8255A.
// we write data to the 8255A. if we read data from 8255A, we should receive
// the same data.

static int check_hardware_ide(__u32 ide_base)
{
__u32 flags=0;
__u32 test=MAGIC_COOKIE;

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif  

// initialize chip

	outb(0x80,INIT_IDE_FLASH);

// write data

	outb((__u8)(test & 0xFF),ADDRESS_0);
	outb((__u8)((test>>8) & 0xFF),ADDRESS_1);
	outb((__u8)((test>>16) & 0xFF),ADDRESS_2);
	
// read data

	test = 0;
	test |= inb(ADDRESS_0);
	test |= inb(ADDRESS_1)<<8;
	test |= inb(ADDRESS_2)<<16;

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif  

// initialize chip

	outb(0x80,INIT_IDE_FLASH);

	if (test == MAGIC_COOKIE) return 1;

return 0;
}

/* ------------------------------------------------------------------------ */

// first we must check, which "ide controller" is used.
// we test the first unused ide-controller.

static __u32 test_base(void)
{
__u32 base;
int i=0;


	while ((base=io_addr[i++])) 
	{
// is the io reserved by another module?

#if (LINUX_VERSION_CODE < 0x020400)
	    if (!check_region(base, 8))
	    {
			if (check_hardware_ide(base))
			{
			    request_region(base, 8, MODULE_NAME);
			    return base;
			}
	    } 
#else
        if (request_region(base, 8, MODULE_NAME)!=NULL)
        {
            if (check_hardware_ide(base))
            {
                return base;
            }
            release_region(base, 8);
        }
#endif
	}

        /* not found before - scan ide ports detected by the ide kernel module */
    for (i = 0; i < MAX_HWIFS; ++i)
	{
    	ide_hwif_t *iface = 0;

    	iface = &ide_hwifs[i];

    	if (iface->io_ports[IDE_DATA_OFFSET] != 0)
        {
        	base = iface->io_ports[IDE_DATA_OFFSET];
#if (LINUX_VERSION_CODE < 0x020400)
			if (!check_region(base, 8))
			{
				if (check_hardware_ide(base))
				{
					request_region (base, 8, MODULE_NAME);

					printk(KERN_DEBUG "%s: ide-flasher found at 0x%lx (%s from ide kernel module).\n",
					MODULE_NAME, base, iface->name);

					return base;
				}
			}
#else
			if (request_region(base, 8, MODULE_NAME)!=NULL)
			{
				if (check_hardware_ide(base))
				{
					printk(KERN_DEBUG "%s: ide-flasher found at 0x%lx (%s from ide kernel module).\n",
					MODULE_NAME, base, iface->name);
					return base;
				}
				release_region(base, 8);
			}
#endif
		}
    }
return 0;
}

/* ------------------------------------------------------------------------ */

static void flash_put(__u8 byte,__u32 addr,__u32 DATA_PORT)
{
__u32 flags=0;

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif  
 
//	outb(0x80,INIT_IDE_FLASH);

	outb((__u8)(addr & 0xFF),ADDRESS_0);
	outb((__u8)((addr>>8) & 0xFF),ADDRESS_1);
	outb((__u8)((addr>>16) & 0x07),ADDRESS_2);

	outb(byte,DATA_PORT);

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif  

return;
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get(__u32 addr,__u32 DATA_PORT)
{
__u32 flags=0;
__u8 byte=0;

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif  

//	outb(0x80,INIT_IDE_FLASH);

	outb((__u8)(addr & 0xFF),ADDRESS_0);
	outb((__u8)((addr>>8) & 0xFF),ADDRESS_1);
	outb((__u8)((addr>>16) & 0x07),ADDRESS_2);

	byte = inb(DATA_PORT);

#if  (LINUX_VERSION_CODE >= 0x020608) 
	local_irq_restore(flags);
#else	
	restore_flags(flags);
#endif

return byte;
}

/* ------------------------------------------------------------------------ */

static void flash_put_dip32(__u8 byte,__u32 addr,int dev)
{
return flash_put(byte, addr, DATA_DIP32);
}

/* ------------------------------------------------------------------------ */

static void flash_put_plcc32(__u8 byte,__u32 addr,int dev)
{
return flash_put(byte, addr, DATA_PLCC32);
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get_dip32(__u32 addr,int dev)
{
return flash_get(addr, DATA_DIP32);
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get_plcc32(__u32 addr,int dev)
{
return flash_get(addr, DATA_PLCC32);
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * jpm, int len)
{
	int olen;
	int j1=0;//30
	int j2=0;//1
	int j3=0;//VPP
	long tmp;
    
	if(flash->pinmode)
	{
		tmp=flash->pinmode&VPPMASK;
		if(tmp==VPP5)
		{
			j3=1;
			tmp=flash->pinmode&P1MASK;
			if(tmp==P1L||tmp==P1A)j2=1;
			if(tmp==P1V||tmp==P1H)j2=2;
			tmp=flash->pinmode&P30MASK;
			if(tmp==P30L||tmp==P30A)j1=1;
			if(tmp==P30V||tmp==P30H)j1=2;
		} else if(tmp==VPP12)
		{			
			j3=2;
			tmp=flash->pinmode&P1MASK;
			if(tmp==P1L||tmp==P1A)j2=1;
			if(tmp==P1H)j2=0;
			if(tmp==P1V)j2=2;
			tmp=flash->pinmode&P30MASK;
			if(tmp==P30L||tmp==P30A)j1=1;
			if(tmp==P30H)j1=0;
			if(tmp==P30V)j1=2;
		}
	
		olen=snprintf(jpm,len,
			"jumper:         JP1 %s\n"
			"                JP2 %s\n"
			"                JP3 %s\n",
			jumper[j1],
			jumper[j2],
			jumper[j3]);
	
	}
	else olen=snprintf(jpm,len,"No jumper for this Chip defined\n");
	
	return olen;
}

/* ------------------------------------------------------------------------ */

