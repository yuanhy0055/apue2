/*
 * e100.c: main module for reading and programming of onboard flashproms
 * at networkcard e100 from intel.
 *
 * Dirk Pfau 11/2001 dpfau@gmx.de
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
#include "e100.h"

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr,int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr,int dev), \
                          char * info, __u32 max_romsize);

int init_eeprom_proc(const char * name, struct flash_hw * curr_flash_hw, \
                          __u8 (*eeprom_get)(__u32 addr,int dev), \
                          void (*eeprom_put)(__u8 byte,__u32 addr,int dev));

void release_flash_proc(void);

void release_eeprom_proc(void);

struct proc_dir_entry *e100_proc;

static struct csr *csr_first[Flash_Max_Devices];
static __u8 * flash[Flash_Max_Devices];

static int count=0;
static __u16 mask=0x01;

static void flash_put(__u8 byte,__u32 addr,int dev);
static __u8 flash_get(__u32 addr,int dev);

static void eeprom_put(__u8 byte,__u32 addr,int dev);
static __u8 eeprom_get(__u32 addr,int dev);

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_DESCRIPTION("flasher for flashprom on intel e100 for linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

static int e100_init(struct pci_dev *pcid, __u32 driver_data)
{
char name[20];
struct flash_hw * curr_flash_hw;

	if (pci_enable_device(pcid))
		goto error_pci_enable;

        csr_first[count] = ioremap(pci_resource_start(pcid, 0),
                 pci_resource_len(pcid, 0));
        if (!csr_first[count]) {
                printk (KERN_ERR "%s: cannot remap MMIO region %lx @ %lx\n", MODULE_NAME,
                                pci_resource_len(pcid, 0), pci_resource_start(pcid, 0));
                goto error_ioremap0;
        }

        flash[count] = ioremap(pci_resource_start(pcid, 2),
                 pci_resource_len(pcid, 2));
        if (!flash[count]) {
                printk (KERN_ERR "%s: cannot remap MMFLASH region %lx @ %lx\n", MODULE_NAME,
                                pci_resource_len(pcid, 2), pci_resource_start(pcid, 2));
                goto error_ioremap2;
        }

	writew(mask,&csr_first[count]->flash_cr);

/* configure procfs */

	sprintf(name, "device_%d", count);
	if (!(curr_flash_hw = init_flash_proc(name, e100_proc, flash_get, flash_put, \
	                   "hardware:     e100\n", driver_data)))
	{
        	printk(KERN_DEBUG "%s: error while flash_proc_init.\n", MODULE_NAME );
		goto error_init_flash_proc;
	}

	if (init_eeprom_proc("eeprom", curr_flash_hw, eeprom_get, eeprom_put))
	{
        	printk(KERN_DEBUG "%s: error while eeprom_proc_init.\n", MODULE_NAME );
		goto error_init_eeprom_proc;
	}

	count++;

return 0;

error_init_eeprom_proc:
	release_flash_proc();
error_init_flash_proc:
	iounmap (flash[count]);
error_ioremap2:
	iounmap (csr_first[count]);
error_ioremap0:
error_pci_enable:
	return -ENODEV;
}

/* ------------------------------------------------------------------------ */

static int __init e100_init_module(void)
{
struct pci_dev *pcid;
int i=0;
int count_devices=0;

	printk( "%s: loading module\n", MODULE_NAME );

/* create entry at "/proc/$MODULE_NAME" */

	if(!(e100_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root)))
	{
		printk(KERN_DEBUG "%s: can't create /proc entry.\n", MODULE_NAME);
		return -EIO;
	}

/* test all entries at pci_device_id[] */

do {

/* let us start at the beginning */

    pcid = NULL;

/* it could be more than one device inside the computer */

    while ((pcid = pci_find_device(e100_pci_tbl[i].vendor, e100_pci_tbl[i].device, pcid)) != NULL)
    {
/* if we find a device, "count_devices" isn't zero */

	if (!e100_init(pcid,e100_pci_tbl[i].driver_data)) count_devices=1;
    }
   } while (e100_pci_tbl[++i].vendor);

	if (!count_devices) goto no_device_found;

return 0;

no_device_found:
	remove_proc_entry( MODULE_NAME, &proc_root );
return -EIO;
}

/* ------------------------------------------------------------------------ */

static void __exit e100_cleanup_module(void)
{ 
/* if we didn't find a device, count is 0 */
/* it should be fixed in future */
	while (--count>=0)
	{
		iounmap (csr_first[count]);
		iounmap (flash[count]);
	};

	release_eeprom_proc();
	release_flash_proc();
	remove_proc_entry( MODULE_NAME, &proc_root );

	printk( "%s: cleanup_module successfull.\n", MODULE_NAME );
return;
}

/* ------------------------------------------------------------------------ */

static void flash_put(__u8 byte,__u32 addr,int dev)
{
	writeb(byte,&flash[dev][addr]);

return;
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get(__u32 addr,int dev)
{

return readb(&flash[dev][addr]);
}

/* ------------------------------------------------------------------------ */

static void eeprom_put(__u8 byte,__u32 addr,int dev)
{

return;
}

/* ------------------------------------------------------------------------ */

static __u8 eeprom_get(__u32 addr,int dev)
{

return 0;
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * buf, int len)
{
return 0;
}

/* ------------------------------------------------------------------------ */

int eeprom_info(struct flash_hw * lowlevel, char * buf, int len)
{
return snprintf(buf,len,
		"MAC-Address :   %02X:%02X:%02X:%02X:%02X:%02X\n"
		"SubSystem ID:   0x%04X\n"
		"SubVendor ID:   0x%04X\n"
		"Revision  ID:   0x%X\n"
		"Signature   :   %s\n"
		"WOL         :   %d\n"
		"Boot Disable:   %d\n"
		"Power Mgmt  :   %d\n"
		"PHY Address :   %s\n",
		0,0,0,0,0,0,
		0,
		0,
		0,
		"00",
		0,
		0,
		0,
		"00"
             );
}

/* ------------------------------------------------------------------------ */

module_init(e100_init_module);
module_exit(e100_cleanup_module);

/* ------------------------------------------------------------------------ */
