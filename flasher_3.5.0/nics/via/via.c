/*
 * via.c: main module for reading and programming of onboard flashproms
 * at networkcard whith via-rhineII from dlink, ....
 *
 * Dirk Pfau 08/2003 dpfau@gmx.de
 * Gernot Fink 12/2003 g.fink@gmx.net
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
#include "via.h"

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr,int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr,int dev), \
                          char * info, __u32 max_romsize);

void release_flash_proc(void);

struct proc_dir_entry *via_rhine_proc;

char * reg_win[Flash_Max_Devices];
static int count=0;

static void flash_put(__u8 byte,__u32 addr,int dev);
static __u8 flash_get(__u32 addr,int dev);

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_DESCRIPTION("flasher for flashprom on via-rhineII for linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

static int via_init(struct pci_dev *pcid, __u32 driver_data)
{
char name[20];

	if (pci_enable_device(pcid))
		goto error_pci_enable;

// map the register space into memory

	outb((inb(pci_resource_start(pcid, 0)+0x7B) | 0x80), pci_resource_start(pcid, 0)+0x7B);

	reg_win[count] = ioremap (pci_resource_start(pcid, 1), pci_resource_len(pcid,1));

	if (!reg_win[count])
	{
		printk (KERN_ERR "%s: cannot map MMIO region\n", MODULE_NAME);
		goto error_ioremap;
	}

// configure procfs

	sprintf(name, "device_%d", count);
	if (!init_flash_proc(name, via_rhine_proc, flash_get, flash_put, \
	                   "hardware:     via-rhine\n", driver_data))
	{
		printk(KERN_DEBUG "%s: error while proc_init.\n", MODULE_NAME );
		goto error_init_proc;
	}

	count++;

return 0;

error_init_proc:
	iounmap(reg_win[count]);
error_ioremap:
error_pci_enable:
	return -ENODEV;
}

/* ------------------------------------------------------------------------ */

static int __init via_init_module(void)
{
struct pci_dev *pcid;
int i=0;
int count_devices=0;

	printk( "%s: loading module\n", MODULE_NAME );

// create entry at "/proc/$MODULE_NAME"

	if(!(via_rhine_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root)))
	{
		printk(KERN_DEBUG "%s: can't create /proc entry.\n", MODULE_NAME);
		return -EIO;
	}

/* test all entries at pci_device_id[] */

do {

/* let us start at the beginning */

    pcid = NULL;

/* it could be more than one device inside the computer */

    while ((pcid = pci_find_device(via_pci_tbl[i].vendor, via_pci_tbl[i].device, pcid)) != NULL)
    {
/* if we find a device, "count_devices" isn't zero */

        if (!via_init(pcid,via_pci_tbl[i].driver_data)) count_devices=1;
    }
   } while (via_pci_tbl[++i].vendor);

        if (!count_devices) goto no_device_found;

return 0;

no_device_found:
        remove_proc_entry( MODULE_NAME, &proc_root );
return -EIO;
}

/* ------------------------------------------------------------------------ */

static void __exit via_cleanup_module(void)
{ 
	while (--count>=0)
		iounmap(reg_win[count]);
	
	release_flash_proc();
	remove_proc_entry( MODULE_NAME, &proc_root );

	printk( "%s: cleanup_module successfull.\n", MODULE_NAME );
return;
}

/* ------------------------------------------------------------------------ */

static void flash_put(__u8 byte,__u32 addr,int dev)
{
	writew((__u16)addr, reg_win[dev]+BPMA);
	writeb(byte, reg_win[dev]+BPMD);
	udelay(DELAY);
	writeb(0x02, reg_win[dev]+EBPRD);
	udelay(DELAY);
return;
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get(__u32 addr,int dev)
{
	writew((__u16)addr, reg_win[dev]+BPMA);
	writeb(0x01, reg_win[dev]+EBPRD);
	udelay(DELAY);
return readb(reg_win[dev]+BPIN);
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * buf, int len)
{
return 0;
}

/* ------------------------------------------------------------------------ */

module_init(via_init_module);
module_exit(via_cleanup_module);

/* ------------------------------------------------------------------------ */
