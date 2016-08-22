#define MODULE_NAME	"ctflasher"

#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>

#include "../flash/global.h"

#define WINDOWSIZE	2*1024
#define IO_SIZE		4

#define	IO_RANGE_START	0x100
#define	IO_RANGE_END	0x400

#define	MAXFLASHSIZE	512*1024

void flash_put(__u8 byte,__u32 addr,int dev);

__u8 flash_get(__u32 addr,int dev);
