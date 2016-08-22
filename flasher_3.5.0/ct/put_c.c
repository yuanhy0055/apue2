/*
 * put_c.c: io module for writing data to flashprom with
 * the "Pannenhelfer (c't-flasher)" from the magazine "c't" in Germany. 
 * This is a special hardware (8bit isa card). Please have a look at
 *
 * http://www.heise.de/ct/ftp/projekte/flasher/default.shtml
 *
 *
 * Dirk Pfau 07/1998 - 11/2001 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * History:
 * 11/2001 assembler routines rewritten in C
 *
 *
 *
 *
 */

/* ------------------------------------------------------------------------ */

#include "ct.h"

extern __u16 map_io;
extern __u32 window_size;
extern __u32 prom_io;

/* ------------------------------------------------------------------------ */

void flash_put(__u8 byte, __u32 addr,int dev)
{
__u8 i=0;

	while((i < 32) && !((window_size >> i++) & 0x01));

	i--;
	i &= 0x1F;

	outb((__u8)((addr>>i)&0xFF), map_io);

	isa_writeb(byte,((window_size-1)&addr)+prom_io);

return;
}

/* ------------------------------------------------------------------------ */
