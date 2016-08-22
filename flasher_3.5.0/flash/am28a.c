/*
 * am28a.c: algorithm for AMD AM28FxxxA
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

void chip_reset_am28a(struct flash_hw *flash_hw)
{
	chip_put_byte( flash_hw, 0xFF, 0x0000 );
	chip_put_byte( flash_hw, 0xFF, 0x0000 );
return;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_am28a(struct flash_hw *flash_hw)
{
__u16 id;

	chip_reset_am28a(flash_hw);

	chip_put_byte( flash_hw, 0x80, 0x0000 );
	
	id = (chip_get_byte(flash_hw,0x0000) << 8) | chip_get_byte(flash_hw,0x0001);

return id;
}

/* ------------------------------------------------------------------------ */

int verify_id_am28a(struct flash_hw *flash_hw,__u16 id)
{
        chip_reset_am28a(flash_hw);

        if ( id != ((chip_get_byte(flash_hw,0x00000) << 8) | chip_get_byte(flash_hw,0x00001)))
                return VERIFIED;

return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

int program_byte_am28a(struct flash_hw *flash_hw,__u32 addr,__u8 byte)
{
// instruction for programming a byte

	chip_put_byte( flash_hw, 0x10, addr );
	chip_put_byte( flash_hw, byte, addr );

// look for finish of programming

return data_polling_algorithm(flash_hw,addr,byte,flash_hw->flash->prg_timeout);

}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_am28a(struct flash_hw *flash_hw,struct file * file,
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
                        return -EINVAL;

		if ( program_byte_am28a(flash_hw,addr+flash_hw->curr_pos, byte) != byte )
			return -ENXIO;
	}

	flash_hw->curr_pos += addr;

return (ssize_t)addr;
}

/* ------------------------------------------------------------------------ */

int chip_erase_am28a(struct flash_hw *flash_hw)
{
// first we will reset the chip

	chip_reset_am28a(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

// instruction for erasing flash
// (embedded command; all bytes will be write first to 0x00)

	chip_put_byte( flash_hw, 0x30, 0x0000);
	chip_put_byte( flash_hw, 0x30, 0x0000);

// look for finish of erasing

	if (toggle_bit_algorithm(flash_hw,flash_hw->flash->c_erase_timeout))
	{
		printk(KERN_ERR "%s: error while erasing flash\n", MODULE_NAME );
		return -EFAULT;
	}


return 0;
}

/* ------------------------------------------------------------------------ */
