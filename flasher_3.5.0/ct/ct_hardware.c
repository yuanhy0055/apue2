/*
 * ct_hardware.c: main module for reading and programming of flashproms with
 * the "Pannenhelfer (c't-flasher)" from the magazine "c't" in Germany. 
 * This is a special hardware (8bit isa card). Please have a look at
 *
 * http://www.heise.de/ct/ftp/projekte/flasher/default.shtml
 *
 *
 * Dirk Pfau 07/1998 - 05/2002 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * Supported devices:
 *
 * version 2.x:
 *
 * /proc/ctflasher/data:		programming/reading the flash
 * /proc/ctflasher/info:		get informations about the hardware
 * /proc/ctflasher/erase:		only erase the flash
 *
 *
 * History:
 * v0.1 07/1998
 * v....
 * v1.0	04/2001  put all parts of the program into a kernel module
 * v1.1 04/2001  autodetection of flashprom and hardware
 * v1.2 05/2001  support for ATMEL AT29Cxxx
 * v1.3 05/2001  support for AMD AM28Fxxx
 *
 *
 */

/* ------------------------------------------------------------------------ */

#include <linux/version.h>
#include <linux/module.h>
#include "ct.h"

__u16 map_io=0;
__u32 window_size=0;
__u32 prom_io=0;

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr,int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr,int dev), \
                          char * info, __u32 max_romsize);

void release_flash_proc(void);

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_DESCRIPTION("c't Flasher for Linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

#if LINUX_VERSION_CODE > 0x020400

static int test_io(__u16 addr)
{
// is the io reserved by another module?

	if (request_region(addr, IO_SIZE, MODULE_NAME)==NULL)
		return -1;
// do the io things like the c't flashers?

	if ((inl(addr) & 0xC7C7C7C7) != 0x80808080)
	{
		release_region(addr, IO_SIZE);
		return -2;
	}
	if (!((inb(addr) == inb(addr+1)) && (inw(addr) == inw(addr+2))))
	{
		release_region(addr, IO_SIZE);
		return -3;
	}
	release_region(addr, IO_SIZE);
	map_io = addr;

return 0;
}
#else
static int test_io(__u16 addr)
{
// is the io reserved by another module?

	if (check_region(addr, IO_SIZE))
		return -1;

// do the io things like the c't flashers?

	if ((inl(addr) & 0xC7C7C7C7) != 0x80808080)
		return -2;

	if (!((inb(addr) == inb(addr+1)) && (inw(addr) == inw(addr+2))))
		return -3;

	map_io = addr;

return 0;
}
#endif

/* ------------------------------------------------------------------------ */

static int io_check(void)
{
__u16 addr = IO_RANGE_START;

	while( test_io(addr) && (addr < IO_RANGE_END) )
	{
		addr += IO_SIZE;
		schedule();
	}

// have we found the c't flasher?

	if (map_io) return 0;

// no

return -1;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static int __init ct_init(void)
#else
int init_module(void)
#endif
{
char data[MAX_INFO_SIZE];

// lets have a look, where the hardware is ...

	if (io_check())
	{
		printk(KERN_ERR "%s: c't flasher not found!\n", MODULE_NAME);
		return -EIO;
	}

	printk(KERN_DEBUG "%s: c't flasher found at 0x%03X\n", MODULE_NAME, map_io);
// io of the isa card

	window_size = WINDOWSIZE;
	prom_io = ((__u32)inb(map_io) << 11) | 0x80000;

        printk(KERN_DEBUG "%s: window at 0x%08lX\n", MODULE_NAME, prom_io);
        if(!request_mem_region(prom_io, window_size, MODULE_NAME))
        {
                printk(KERN_ERR "%s: can't reserve io!\n", MODULE_NAME);
                return -EIO;
        }

	request_region(map_io, IO_SIZE, MODULE_NAME);

// initialize flash_proc

	sprintf(data, "hardware:       c't flasher\n"
			"flash-io:       0x%lX\n"
			"windowsize:     0x%lX\n"
			"map-io:         0x%X\n",
			prom_io, window_size, map_io);

	if (!(init_flash_proc(MODULE_NAME, &proc_root, flash_get, flash_put, \
	                   data, MAXFLASHSIZE)))
		goto error_flash_proc;

        printk(KERN_DEBUG "%s: Module installed.\n", MODULE_NAME );

return 0;

error_flash_proc:
	printk(KERN_DEBUG "%s: error while proc_init.\n", MODULE_NAME );
	release_region(map_io, IO_SIZE);
	release_mem_region(prom_io, window_size);
return -EIO;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static void __exit ct_cleanup(void)
#else
void cleanup_module(void)
#endif
{ 
	release_flash_proc();
	release_region(map_io, IO_SIZE);
	release_mem_region(prom_io, window_size);
	printk(KERN_DEBUG "%s: cleanup_module successfull.\n", MODULE_NAME );

return;
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * buf, int len)
{
return 0;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
module_init(ct_init);
module_exit(ct_cleanup);
#endif

/* ------------------------------------------------------------------------ */
