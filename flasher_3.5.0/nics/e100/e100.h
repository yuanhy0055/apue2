#define MODULE_NAME	"e100-flash"

#define PCI_DEVICE_ID_INTEL_82557   0x1229
#define PCI_DEVICE_ID_INTEL_82559ER 0x1209

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/pci.h>
#include <linux/proc_fs.h>

#include "../../flash/global.h"

struct csr {	__u16 scb_sw; __u16 scb_cw;
		__u32 scb_gp;
		__u32 port;
		__u16 flash_cr; __u16 eeprom_cr;
		__u32 mdi_cr;
		__u32 rec_dma_bc;
		__u8 eric; __u8 fct; __u8 fcxx; __u8 pmdr;};


static struct pci_device_id e100_pci_tbl[] = {
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82557,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		64 * 1024 },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82559ER,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ 0,}
};


