#define	FLASH_VERSION	"3.5.0"
#define MODULE_NAME	"flashprom"

#include <linux/version.h>

#if LINUX_VERSION_CODE >= 0x020600 
#include <linux/jiffies.h>
#include <linux/interrupt.h>
#endif  

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include "global.h"

#define UNKNOWN		0
#define	AM29		1
#define	AT29		2
#define	AM28A		3
#define	V29		4
#define	CAT28		5
#define	I28a		6
#define	I28		7
#define	SST39a		8
#define	SST39		9
#define	W29		10
#define	EON29F00x	11
#define	EON29F0x0	12

#define	NOT_VERIFIED	0
#define	VERIFIED	1
#define	NOT_SURE	2

#define DQ0(X)		(X & 0x01)
#define DQ1(X)		(X & 0x02)
#define DQ2(X)		(X & 0x04)
#define DQ3(X)		(X & 0x08)
#define DQ4(X)		(X & 0x10)
#define DQ5(X)		(X & 0x20)
#define DQ6(X)		(X & 0x40)
#define DQ7(X)		(X & 0x80)

/* ------------------------------------------------------------------------ */

void chip_put_byte(struct flash_hw *flash_hw,__u8 byte,__u32 addr);
__u8 chip_get_byte(struct flash_hw *flash_hw,__u32 addr);

int data_polling_algorithm(struct flash_hw *flash_hw, __u32 addr,__u8 byte, __u32 wait);
int toggle_bit_algorithm(struct flash_hw *flash_hw, __u32 wait);
int flash_erase(struct flash_hw *flash_hw);
int chip_erase_check(struct flash_hw *flash_hw);

/* ------------------------------------------------------------------------ */

