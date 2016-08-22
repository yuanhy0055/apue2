#define MODULE_NAME	"rtl8139-flash"

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/pci.h>
#include <linux/proc_fs.h>

#include "../../flash/global.h"

#define PCI_VENDOR_ID_SMC2          0x1113
#define PCI_DEVICE_ID_SMC2_1211TX   0x1211


#ifndef PCI_VENDOR_ID_REALTEK
#define PCI_VENDOR_ID_REALTEK           0x10ec
#endif
#ifndef PCI_DEVICE_ID_REALTEK_8139
#define PCI_DEVICE_ID_REALTEK_8139      0x8139
#endif

static struct pci_device_id realtek_pci_tbl[] = {
	{ PCI_VENDOR_ID_REALTEK, PCI_DEVICE_ID_REALTEK_8139,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ PCI_VENDOR_ID_SMC2, PCI_DEVICE_ID_SMC2_1211TX,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ 0,}
};

