/*
 * 3c90xc.c: main module for reading and programming of onboard flashproms
 * at networkcard 3c90xc from 3com.
 *
 * Dirk Pfau 05/2002 dpfau@gmx.de
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
#include "3c90xc.h"

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr,int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr,int dev), \
                          char * info, __u32 max_romsize);

void release_flash_proc(void);

struct proc_dir_entry * vortex_proc;

static struct window0 * win0[Flash_Max_Devices];
static int count=0;

static void flash_put(__u8 byte,__u32 addr, int dev);
static __u8 flash_get(__u32 addr, int dev);

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_DESCRIPTION("flasher for flashprom on 3com 3c90xc for linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

static int __devinit vortex_init(struct pci_dev *pcid, __u32 driver_data)
{
char name[20];

	if (pci_enable_device(pcid))
		goto error_enable_pci;

	win0[count] = (struct window0 *)pci_resource_start(pcid, 0);

// configure procfs

	sprintf(name, "device_%d", count);
	if (!init_flash_proc(name, vortex_proc, flash_get, flash_put, \
	                   "hardware:     3c90xc\n", driver_data))
	{
        	printk(KERN_DEBUG "%s: error while proc_init.\n", MODULE_NAME );
		goto error_proc_init;
	}

	count++;

return 0;

error_proc_init:
error_enable_pci:
	return -ENODEV;

}

/* ------------------------------------------------------------------------ */

static int __init vortex_init_module(void)
{
struct pci_dev *pcid;
int i=0;
int count_devices=0;

	printk( "%s: loading module\n", MODULE_NAME );

// create entry at "/proc/$MODULE_NAME"

	if(!(vortex_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root)))
	{
		printk(KERN_DEBUG "%s: can't create /proc entry.\n", MODULE_NAME);
		return -EIO;
	}

/* test all entries at pci_device_id[] */

do {

/* let us start at the beginning */

    pcid = NULL;

/* it could be more than one device inside the computer */

    while ((pcid = pci_find_device(vortex_pci_tbl[i].vendor, vortex_pci_tbl[i].device, pcid)) != NULL)
    {
/* if we find a device, "count_devices" isn't zero */

        if (!vortex_init(pcid,vortex_pci_tbl[i].driver_data)) count_devices=1;
    }
   } while (vortex_pci_tbl[++i].vendor);

	if (!count_devices) goto no_device_found;

return 0;

no_device_found:
	remove_proc_entry( MODULE_NAME, &proc_root );
return -EIO;
}

/* ------------------------------------------------------------------------ */

static void __exit vortex_cleanup_module(void)
{ 
	release_flash_proc();
        remove_proc_entry( MODULE_NAME, &proc_root );

	printk( "%s: cleanup_module successfull.\n", MODULE_NAME );
return;
}

/* ------------------------------------------------------------------------ */

static void flash_put(__u8 byte,__u32 addr, int dev)
{
	outw(SelectRegisterWindow+0, (__u32)&win0[dev]->IntStatus_Command);
	outl(addr , (__u32)&win0[dev]->BiosRomAddr);
	outb(byte , (__u32)&win0[dev]->BiosRomData);

return;
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get(__u32 addr, int dev)
{
	outw(SelectRegisterWindow+0, (__u32)&win0[dev]->IntStatus_Command);
	outl(addr, (__u32)&win0[dev]->BiosRomAddr);
return inb((__u32)&win0[dev]->BiosRomData);
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * buf, int len)
{
return 0;
}

/* ------------------------------------------------------------------------ */

module_init(vortex_init_module);
module_exit(vortex_cleanup_module);

/* ------------------------------------------------------------------------ */
