#define MODULE_NAME	"ide-flash"

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>

#include "../flash/global.h"

#define	IDE0		0x1F0
#define	IDE1		0x170
#define	IDE2		0x1E8
#define	IDE3		0x168
#define	IDE4		0x1E0
#define	IDE5		0x160

#define	ADDRESS_0	(ide_base+2)		/* address bits 0-7   */
#define	ADDRESS_1	(ide_base+1)		/* address bits 8-15  */
#define	ADDRESS_2	(ide_base)		/* address bits 16-18 */

#define	INIT_IDE_FLASH	(ide_base+3)

#define	DATA_DIP32	(ide_base+4)		/* data port for DIP socket */
#define	DATA_PLCC32	(ide_base+5)		/* data port for PLCC socket */

#define	DIP32		0
#define	PLCC32		1

#define MAXFLASHSIZE	512*1024

#define	MAGIC_COOKIE	0x7D84C2

static __u16 io_addr[]={IDE0, IDE1, IDE2, IDE3, IDE4, IDE5, 0};

char * jumper[3] = { "OPEN","1-2","2-3" };
