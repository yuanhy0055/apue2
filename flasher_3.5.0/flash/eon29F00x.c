/*
 * eon29F00x.c: algorithm for EON EN29F00x
 *
 * Dirk Pfau 11/2003 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 */

#include "flash.h"

/* ------------------------------------------------------------------------ */

int data_polling_algorithm_eon29F00x(struct flash_hw * flash_hw,__u32 addr,__u8 byte, __u32 wait)
{
__u8 data;
unsigned long jiffies_old=jiffies;

	wait=(wait*HZ)/700+1;

/* after wait*10msec we will finish */

	while(jiffies-jiffies_old < wait )
	{
		data = chip_get_byte(flash_hw,addr);
		if (DQ7(data) == DQ7(byte)) return chip_get_byte(flash_hw,addr);
		if (DQ5(data)) break;

/* give other processes a chance! */

		schedule();
	}
	if (DQ7(chip_get_byte(flash_hw,addr)) == DQ7(byte)) return chip_get_byte(flash_hw,addr);
	
	printk(KERN_ERR "%s: data_polling_algorithm -> error\n", MODULE_NAME );

return -1;
}

/* ------------------------------------------------------------------------ */

int toggle_bit_algorithm_eon29F00x(struct flash_hw * flash_hw,__u32 wait)
{
__u8 tog_prev, tog_cur;
unsigned long jiffies_old=jiffies;

	wait=(wait*HZ)/700+1;

	tog_prev = chip_get_byte(flash_hw,1);

/* after wait*10msec we will finish */

	while(jiffies-jiffies_old < wait )
	{
		tog_cur = chip_get_byte(flash_hw,1);
		if (DQ6(tog_prev) == DQ6(tog_cur)) return 0;
		tog_prev = tog_cur;
		if (DQ5(tog_cur)) break;

/* give other processes a chance! */

		schedule();
	}
	if (DQ6(tog_prev) == DQ6(chip_get_byte(flash_hw,1))) return 0;

	printk(KERN_ERR "%s: toggle_bit_algorithm -> error\n", MODULE_NAME );

return -1;
}

/* ------------------------------------------------------------------------ */

void chip_reset_eon29F00x(struct flash_hw *flash_hw)
{
	chip_put_byte( flash_hw, 0xF0, 0x0000 );
	chip_put_byte( flash_hw, 0xF0, 0x0000 );
return;
}

/* ------------------------------------------------------------------------ */

static __u32 _autoselect(struct flash_hw *flash_hw, __u32 offset)
{
__u32 id;

	chip_reset_eon29F00x(flash_hw);

	chip_put_byte( flash_hw, 0xAA, 0x555 );
	chip_put_byte( flash_hw, 0x55, 0xAAA );
	chip_put_byte( flash_hw, 0x90, 0x555 );
	
	id = (chip_get_byte(flash_hw,offset+0x100) << 8) | chip_get_byte(flash_hw,offset+0x101) |
	     (chip_get_byte(flash_hw,offset+0x02) << 16);

// leave auto select mode

	chip_reset_eon29F00x(flash_hw);
	
return id;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_eon29F00x(struct flash_hw *flash_hw)
{
return (__u16)(_autoselect(flash_hw,0) & 0xFFFF);
};

/* ------------------------------------------------------------------------ */

int write_protect_check_eon29F00x(struct flash_hw *flash_hw)
{
__u8 protected=0;
int i=0;

	chip_reset_eon29F00x(flash_hw);

	chip_put_byte( flash_hw, 0xAA, 0x555 );
	chip_put_byte( flash_hw, 0x55, 0xAAA );
	chip_put_byte( flash_hw, 0x90, 0x555 );
	
	if (flash_hw->flash->block_address[0] > -1)
	{
	__s32 sa;
	    while ((sa=flash_hw->flash->block_address[i++]) > -1)
		if ((protected=DQ0(chip_get_byte(flash_hw,sa+0x0002)))) break;
	}
	else
	{
	__u32 sa;
	    if(flash_hw->flash->sa_size)
                for (sa=0x0000; sa < flash_hw->usable_romsize; sa+=flash_hw->flash->sa_size)
                    if ((protected=DQ0(chip_get_byte(flash_hw,sa+0x0002)))) break;
	}

// leave auto select mode

	chip_reset_eon29F00x(flash_hw);
	
return (protected?1:2);
}

/* ------------------------------------------------------------------------ */
// can we be sure, not an eprom with unfortunally bytes at the right address
// is inside the socket?

int verify_id_eon29F00x(struct flash_hw *flash_hw, __u16 id)
{
__u32 testaddr=0;
__u32 id_lock;

	while (testaddr < flash_hw->usable_romsize)
	{
		id_lock = _autoselect(flash_hw, testaddr);

		if ( id != (__u16)(id_lock & 0xFFFF))
			return NOT_VERIFIED;

		if ( id != ((chip_get_byte(flash_hw,testaddr+0x00) << 8) | chip_get_byte(flash_hw,testaddr+0x01)))
			return VERIFIED;

		testaddr += 0x100;
	}
return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

int chip_erase_eon29F00x(struct flash_hw *flash_hw)
{
// first we will reset the chip

	chip_reset_eon29F00x(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

// instruction for erasing flash

	chip_put_byte( flash_hw, 0xAA, 0x555 );
	chip_put_byte( flash_hw, 0x55, 0xAAA );
	chip_put_byte( flash_hw, 0x80, 0x555 );
	chip_put_byte( flash_hw, 0xAA, 0x555 );
	chip_put_byte( flash_hw, 0x55, 0xAAA );
	chip_put_byte( flash_hw, 0x10, 0x555 );

// look for finish of erasing

	if (toggle_bit_algorithm_eon29F00x(flash_hw, flash_hw->flash->c_erase_timeout))
	{
		printk(KERN_ERR "%s: error while erasing flash\n", MODULE_NAME );
		return -EFAULT;
	}

return 0;
}

/* ------------------------------------------------------------------------ */

int program_byte_eon29F00x(struct flash_hw *flash_hw,__u32 addr,__u8 byte)
{
// instruction for programming a byte

	chip_put_byte( flash_hw, 0xAA, 0x555 );
	chip_put_byte( flash_hw, 0x55, 0xAAA );
	chip_put_byte( flash_hw, 0xA0, 0x555 );

	chip_put_byte( flash_hw, byte, addr );

// looking for end of programming

return data_polling_algorithm_eon29F00x(flash_hw,addr,byte,flash_hw->flash->prg_timeout);
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_eon29F00x(struct flash_hw *flash_hw,struct file * file,
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

		if ( program_byte_eon29F00x(flash_hw,addr+flash_hw->curr_pos, byte) != byte )
			return -ENXIO;
	}

	flash_hw->curr_pos += addr;

return (ssize_t)addr;
}

/* ------------------------------------------------------------------------ */
