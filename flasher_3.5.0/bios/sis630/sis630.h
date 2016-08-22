#define MODULE_NAME    "sis630-flash"

#define BASE_ADDR      0xfff80000
#define MAX_FLASH_SIZE 0x00080000

#include <linux/init.h>

#include <asm/io.h>

#include <linux/pci.h>
#include <linux/proc_fs.h>

#include "../../flash/global.h"
