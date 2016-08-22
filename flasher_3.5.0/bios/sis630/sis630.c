/*
 * sis630.c: main module for reading and programming of onboard flashproms
 * at mainboards with sis630 from sis.
 *
 * Felix Kloeckner 07/2003 root@hamburg.de
 *
 * This program is under the terms of the BSD/GPL License.
 *
 * History:
 * derived from sis8130.c (Dirk Pfau 12/2002 dpfau@gmx.de)
 *
 */

/* ------------------------------------------------------------------------ */

#include <linux/version.h> 
#include <linux/module.h>
#include "sis630.h"

char * flash_base_ptr;

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
		   __u8 (*flash_get)(__u32 addr,int dev), \
		   void (*flash_put)(__u8 byte,__u32 addr,int dev), \
		   char * info, __u32 max_romsize);

void release_flash_proc(void);

struct proc_dir_entry *sis_proc;

static void flash_put(__u8 byte,__u32 addr,int dev);
static __u8 flash_get(__u32 addr,int dev);

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Felix Kloeckner <root@hamburg.de>");
MODULE_DESCRIPTION("flasher for flashprom on sis630 for linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

/* ------------------------------------------------------------------------ */

static int sis_unlock_flash(void){

  //check_region ???

  struct pci_dev *sis_pci_dev;
  unsigned char byte_reg;
  
  if ((sis_pci_dev = pci_find_device(PCI_VENDOR_ID_SI, PCI_DEVICE_ID_SI_630, 0)) == 0){
    printk(KERN_ERR "%s: sorry, no sis630 found.\n", MODULE_NAME);
    return -1;
  } 
    
  if (pci_request_regions (sis_pci_dev, MODULE_NAME)){
    printk (KERN_ERR "%s: cannot reserve PCI IO/MMIO region.\n", MODULE_NAME);
    return -1;
  }

  /*
    enable 0xfff8000 - 0xffff000 flash-ROM Interface on sis630

    bit 3: Subtractive Decode to Internal registers Enable. 0 : Disable 1 : Enable
           When this bit is enabled, SiS630 will do subtractive decode on addresses for internal registers.

    bit 1: BIOS positive Decode Enable. 0 : Disable 1 : Enable
           When enabled, SiS630 will positively respond to PCI memory cycles toward E segment and F segment.
	   Otherwise, it will respond subtractively.

    bit 0: Extended BIOS Enable. (FFF80000~FFFDFFFF)
           When enabled, SiS630 will positively respond to PCI cycles toward the Extended segment.
	   Otherwise, it will have no response.
  */
  pci_read_config_byte(sis_pci_dev, 0x40, &byte_reg);
  pci_write_config_byte(sis_pci_dev, 0x40, byte_reg | 0xb);

  /*
    enable flash-ROM writable on sis630

    bit 7:6 : Flash EPROM Control
              Bit If bit 7 is set to '0' after CPURST de-asserted, EPROM can be flashed when bit 6 is set to '1'.
              Once bit 7 is set to '1', EPROM can not be flashed until the system is reset.
  */
  pci_read_config_byte(sis_pci_dev, 0x45, &byte_reg);
  pci_write_config_byte(sis_pci_dev, 0x45, byte_reg | 0x40);

  pci_release_regions (sis_pci_dev);

  if (byte_reg & 0x80){
    printk (KERN_ERR "%s: sorry, flash-ROM is write-protected, reset your system to release write-protection.\n", MODULE_NAME);
    return -1;
  }

  // enable 0xfff8000 - 0xffff000 flash-ROM Interface, enable flash-ROM writable on sis950 superIO
  // command port is 0x2e
  // data port is 0x2f

  // magic_io-string to enter MB PnP-mode
  outb(0x87, 0x2e);
  outb(0x01, 0x2e);
  outb(0x55, 0x2e);
  outb(0x55, 0x2e);
  if (inb(0x2f) == 0x87){ // check for respond
    outb(0x24, 0x2e); //select reg 0x24: Clock Selection and Flash ROM I/F Control Register
    byte_reg = inb(0x2f);  //read selected register
    
    outb(0x24, 0x2e); //select reg 0x24
    
    // bit 7 : Flash ROM Interface Address Segment 4 (000F0000h-000FFFFFh, 000E0000h- 000EFFFFh) enable
    // bit 6 : Flash ROM Interface Address Segment 3 (FFF80000h-FFFDFFFFh, FFFE0000h- FFFEFFFFh) enable
    // bit 5 : Flash ROM Interface Address Segment 2 (FFEF0000h-0FEFFFFFh, FFEE0000h- FFEEFFFFh) enable
    // bit 4 : Flash ROM Interface Address Segment 1 (FFFF0000h-FFFFFFFFh, FFFE0000h- FFFEFFFFh) enable
    // bit 3 : 4M bits Flash ROM enable (Pin 75 is selected as FA18)
    // bit 2 : Flash ROM I/F write enable
    // bit 1 : reserved
    // bit 0 : CLKIN Frequency: 0: 48 MHz / 1: 24 MHz.
    outb(byte_reg | 0xfc, 0x2f); 

    outb(0x02, 0x2e); //
    outb(0x02, 0x2f); // exit MB PnP-mode
  } else { // try alternative command/data port
    // different string, different ports, rest is the same
    outb(0x87, 0x4e);
    outb(0x01, 0x4e);
    outb(0x55, 0x4e);
    outb(0xaa, 0x4e);
    if (inb(0x4f) == 0x87){
      outb(0x24, 0x4e);
      byte_reg = inb(0x4f);
    
      outb(0x24, 0x4e);
      outb(byte_reg | 0xfc, 0x4f); 

      outb(0x02, 0x4e);
      outb(0x02, 0x4f);
    } else{
      printk (KERN_ERR "%s: can't access sis950 superIO.\n", MODULE_NAME);
      return -1;
    }
  }
  return 0;
}

/* ------------------------------------------------------------------------ */

static int __init sis_init_module(void){
  const __u32 base_addr   = BASE_ADDR;
  const __u32 window_size = MAX_FLASH_SIZE;
  const int DATA_SIZE     = 1024;
  char data[DATA_SIZE];

  printk( "%s: loading module\n", MODULE_NAME );

  if (sis_unlock_flash()){
    return -EIO;
  }

  if (!(flash_base_ptr = ioremap(BASE_ADDR, MAX_FLASH_SIZE))){
    printk (KERN_ERR "%s: can't do ioremap\n", MODULE_NAME);
    goto error_ioremap;;
  }

  // create entry at "/proc/$MODULE_NAME"
  if(!(sis_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root))){
    printk(KERN_DEBUG "%s: can't create /proc entry.\n", MODULE_NAME);
    goto error_create_proc;
  }

  snprintf(data, DATA_SIZE, "hardware:     sis630\n"
	  "baseAddr:     0x%lX\n"
	  "max size:     0x%lX\n",
	  base_addr, window_size);

  if (!init_flash_proc(MODULE_NAME, &proc_root, flash_get, flash_put, data, window_size)){
    printk(KERN_DEBUG "%s: error while proc_init.\n", MODULE_NAME );
    goto error_init_flash_proc;
  }

  return 0;

 error_init_flash_proc:
  remove_proc_entry( MODULE_NAME, &proc_root);
 error_create_proc:
  iounmap(flash_base_ptr);
 error_ioremap:
  return -EIO;
}

/* ------------------------------------------------------------------------ */

static void __exit sis_cleanup_module(void){ 
  release_flash_proc();
  remove_proc_entry( MODULE_NAME, &proc_root );
  iounmap(flash_base_ptr);

  printk( "%s: cleanup_module successfull.\n", MODULE_NAME );
  return;
}

/* ------------------------------------------------------------------------ */

static void flash_put(__u8 byte,__u32 addr,int dev){
  writeb(byte, addr + flash_base_ptr);
  return;
}

/* ------------------------------------------------------------------------ */

static __u8 flash_get(__u32 addr,int dev){
  return readb(addr + flash_base_ptr);
}

/* ------------------------------------------------------------------------ */

int hw_info(struct flashdat * flash, char * buf, int len)
{
return 0;
}

/* ------------------------------------------------------------------------ */

module_init(sis_init_module);
module_exit(sis_cleanup_module);

/* ------------------------------------------------------------------------ */
