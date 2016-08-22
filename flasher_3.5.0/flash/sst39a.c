/*
 * sst39.c: algorithm for SST39VFxxx
 *
 * Dirk Pfau 01/2002 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * prepared for v2.0
 *
 */

#include "flash.h"


/* ------------------------------------------------------------------------ */

void chip_reset_sst39a(struct flash_hw *flash_hw)
{
	mdelay(flash_hw->_twc);

	chip_put_byte( flash_hw, 0xAA, 0x45555 );
	chip_put_byte( flash_hw, 0x55, 0x42AAA );
	chip_put_byte( flash_hw, 0xF0, 0x45555 );

	mdelay(flash_hw->_twc);
return;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_sst39a(struct flash_hw *flash_hw)
{
__u16 id;

	chip_reset_sst39a(flash_hw);

	chip_put_byte( flash_hw, 0xAA, 0x45555 );
	chip_put_byte( flash_hw, 0x55, 0x42AAA );
	chip_put_byte( flash_hw, 0x90, 0x45555 );

	mdelay(flash_hw->_twc);
	
	id = (chip_get_byte( flash_hw,0x40000) << 8) | chip_get_byte( flash_hw,0x40001);

// leave auto select mode

	chip_reset_sst39a(flash_hw);
	
return id;
}


/*--------------------------------------------------------------------------*/
void hwmod_put_byte_sst39a(struct flash_hw * flash_hw,__u8 byte,__u32 addr)
{
	addr|=0x40000;
// don't use addresses greater than supported flashrom size
// we cut all address bits out of the limit
        addr &= (flash_hw->max_romsize-1);
        (*flash_hw->flash_put)(byte,addr,flash_hw->dev);
};

/*--------------------------------------------------------------------------*/

__u8 hwmod_get_byte_sst39a(struct flash_hw * flash_hw,__u32 addr)
{
	addr|=0x40000;
        // don't use addresses greater than supported flashrom size
        // we cut all address bits out of the limit
        addr &= (flash_hw->max_romsize-1);
        return (*flash_hw->flash_get)(addr,flash_hw->dev);
}

/* ------------------------------------------------------------------------ */

int verify_id_sst39a(struct flash_hw *flash_hw,__u16 id)
{
	chip_reset_sst39a(flash_hw);

	if ( id != ((chip_get_byte( flash_hw,0x0) << 8) | chip_get_byte( flash_hw,0x1)))
		return VERIFIED;

return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

int chip_erase_sst39a(struct flash_hw *flash_hw)
{
// first we will reset the chip

	chip_reset_sst39a(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

// instruction for erasing flash

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x80, 0x5555 );
	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x10, 0x5555 );

// look for finish of erasing

	if (toggle_bit_algorithm(flash_hw,flash_hw->flash->c_erase_timeout))
	{
		printk(KERN_ERR "%s: error while erasing flash\n", MODULE_NAME );
		return -EFAULT;
	}

return 0;
}

/* ------------------------------------------------------------------------ */

int program_byte_sst39a(struct flash_hw *flash_hw,__u32 addr,__u8 byte)
{
// instruction for programming a byte

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0xA0, 0x5555 );

	chip_put_byte( flash_hw, byte, addr );

// look for finish of programming

return data_polling_algorithm(flash_hw,addr,byte,flash_hw->flash->prg_timeout);
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_sst39a(struct flash_hw *flash_hw,struct file * file,
	const char * buf, size_t count, loff_t *ppos)
{
size_t addr;
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

		if ( program_byte_sst39a(flash_hw,addr+flash_hw->curr_pos, byte) != byte )
			return -ENXIO;
	}

	flash_hw->curr_pos += addr;

return (ssize_t)addr;
}

/* ------------------------------------------------------------------------ */
