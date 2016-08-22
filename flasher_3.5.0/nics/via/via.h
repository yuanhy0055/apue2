#define MODULE_NAME	"via-rhine-flash"

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/pci.h>
#include <linux/proc_fs.h>

#include "../../flash/global.h"

#ifndef PCI_VENDOR_ID_VIA
#define PCI_VENDOR_ID_VIA           0x1106
#endif
#ifndef PCI_DEVICE_ID_VIA_6102
#define PCI_DEVICE_ID_VIA_6102      0x3065
#endif

#define	BPMA	0x8C
#define	BPMD	0x8F
#define	BPCMD	0x90
#define	EBPRD	0x90
#define	EBPWR	0x90
#define	BPIN	0x91

#define	DELAY	20

static struct pci_device_id via_pci_tbl[] = {
	{ PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_6102,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		64 * 1024 },
	{ 0,}
};

