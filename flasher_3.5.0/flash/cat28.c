/*
 * cat28.c: algorithm for CATALYST CAT28Fxxx
 *
 * Dirk Pfau 05/2001 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * prepared for v2.0
 *
 */


#include "flash.h"

/* ------------------------------------------------------------------------ */

void chip_reset_cat28(struct flash_hw *flash_hw)
{
	chip_put_byte( flash_hw, 0xFF, 0x0000 );
	chip_put_byte( flash_hw, 0xFF, 0x0000 );
return;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_cat28(struct flash_hw *flash_hw)
{
__u16 id;

	chip_reset_cat28(flash_hw);

	chip_put_byte( flash_hw, 0x90, 0x0000 );
	
	id = (chip_get_byte( flash_hw,0x0000) << 8);

	chip_put_byte( flash_hw, 0x90, 0x0000 );
	
	id |= chip_get_byte( flash_hw,0x0001);

return id;
}

/* ------------------------------------------------------------------------ */

int verify_id_cat28(struct flash_hw *flash_hw,__u16 id)
{
        chip_reset_cat28(flash_hw);

        if ( id != ((chip_get_byte( flash_hw,0x00000) << 8) | chip_get_byte( flash_hw,0x00001)))
                return VERIFIED;

return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

int program_byte_cat28(struct flash_hw *flash_hw,__u32 addr,__u8 byte)
{
// instruction for programming a byte

	chip_put_byte( flash_hw, 0x40, addr );
	chip_put_byte( flash_hw, byte, addr );

	udelay(10);

	chip_put_byte( flash_hw, 0xC0, addr );

	udelay(6);

return chip_get_byte( flash_hw, addr );
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_cat28(struct flash_hw *flash_hw,struct file * file,
	const char * buf, size_t count, loff_t *ppos)
{
size_t addr;
int i;
__u8 byte;

     if (access_ok(VERIFY_READ, (void *)buf, count)==0)
                return -EINVAL;

// erase flash

	if (!flash_hw->curr_pos)
		if (flash_erase(flash_hw)) return -EFAULT;

	for ( addr=0; addr < count; addr++)
	{
		if (addr+flash_hw->curr_pos >= flash_hw->usable_romsize)
			return -ENOSPC;

		if(copy_from_user(&byte, (void *)buf+addr, sizeof(__u8)))
                        return -EFAULT;


		i = 0;

// if programming failed, do it again up to 25 times

		while( program_byte_cat28(flash_hw, addr+flash_hw->curr_pos, byte) != byte )
		{
			if (++i > 25)
				return -ENXIO;
		}
	}

	flash_hw->curr_pos += addr;

return (ssize_t)addr;
}

/* ------------------------------------------------------------------------ */

int chip_erase_verify_cat28(struct flash_hw *flash_hw, __u32 *addr)
{
	while( *addr < flash_hw->usable_romsize)
	{
		chip_put_byte(flash_hw, 0xA0, *addr);

		udelay(6);

		if (chip_get_byte(flash_hw, *addr) != 0xFF)
			return -1;

		(*addr)++;
	}

return 0;
}

/* ------------------------------------------------------------------------ */

int chip_erase_cat28(struct flash_hw *flash_hw)
{
int i;
__u32 addr;

// first we will reset the chip

	chip_reset_cat28(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

//first we must program all bytes to 0x00

	for (addr=0; addr < flash_hw->usable_romsize; addr++)
	{
		i = 0;

// if programming failed, do it again up to 25 times

		while( program_byte_cat28(flash_hw,addr,0x00) != 0x00 )
		{
			if (++i > 25)
				return -1;
		}
	}

// ok, the flash contains only 0x00
// now we will erase the flash. if it failed,
// we will repeat it up to 1000 times

	i = 0;
	addr=0;

	while( i++ < 1000 )
	{
	
// instruction for erasing flash

		chip_put_byte( flash_hw, 0x20, 0x0000);
		chip_put_byte( flash_hw, 0x20, 0x0000);

		mdelay(10);

		if (!chip_erase_verify_cat28(flash_hw,&addr))
			return 0;
	}

return -2;
}

/* ------------------------------------------------------------------------ */
