#define MODULE_NAME	"3c90xc-flash"

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/pci.h>
#include <linux/proc_fs.h>

#include "../../flash/global.h"

struct window0 {
	__u32 garbage0;
	__u32 BiosRomAddr;
	__u8 BiosRomData; __u8 garbage1; __u16 EepromCommand;
	__u16 EepromData; __u16 IntStatus_Command;};

#define	SelectRegisterWindow	0x0800

#define PCI_DEVICE_ID_3COM_3C985     0x0001
#define PCI_DEVICE_ID_3COM_3C900TPO  0x9000
#define PCI_DEVICE_ID_3COM_3C900COMB O0x9001
#define PCI_DEVICE_ID_3COM_3C905TX   0x9050
#define PCI_DEVICE_ID_3COM_3C905T4   0x9051
#define PCI_DEVICE_ID_3COM_3C905B_TX 0x9055

#ifndef PCI_DEVICE_ID_3COM_3C905C
#define PCI_DEVICE_ID_3COM_3C905C	0x9200
#endif

static struct pci_device_id vortex_pci_tbl[] = {
	{ PCI_VENDOR_ID_3COM, PCI_DEVICE_ID_3COM_3C905TX,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ PCI_VENDOR_ID_3COM, PCI_DEVICE_ID_3COM_3C905T4,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ PCI_VENDOR_ID_3COM, PCI_DEVICE_ID_3COM_3C905B_TX,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ PCI_VENDOR_ID_3COM, PCI_DEVICE_ID_3COM_3C905C,
		PCI_ANY_ID, PCI_ANY_ID, 0, 0,
		128 * 1024 },
	{ 0,}
};

