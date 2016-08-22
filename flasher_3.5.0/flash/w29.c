/*
 * w29.c: algorithm for WINBOND W29C01x
 *
 * Dirk Pfau 06/2002 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 *
 */


#include "flash.h"

/* ------------------------------------------------------------------------ */

void chip_reset_w29(struct flash_hw *flash_hw)
{
	mdelay(flash_hw->_twc);

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0xF0, 0x5555 );

	mdelay(flash_hw->_twc);
return;
}

/* ------------------------------------------------------------------------ */

static __u16 _autoselect(struct flash_hw *flash_hw, __u32 offset)
{
__u16 id;

	chip_reset_w29(flash_hw);

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x80, 0x5555 );
	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x60, 0x5555 );
	
	mdelay(flash_hw->_twc);

	id = (chip_get_byte(flash_hw,offset+0x00) << 8) | chip_get_byte(flash_hw,offset+0x01);

// leave autoselect mode

	chip_reset_w29(flash_hw);

return id;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_w29(struct flash_hw *flash_hw) {return _autoselect(flash_hw,0);};

/* ------------------------------------------------------------------------ */

int verify_id_w29(struct flash_hw *flash_hw, __u16 id)
{
	chip_reset_w29(flash_hw);

	if ( id != ((chip_get_byte(flash_hw, 0x0000) << 8) | chip_get_byte(flash_hw, 0x0001)))
		return VERIFIED;

return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

int chip_erase_w29(struct flash_hw *flash_hw)
{
// first we will reset the chip

	chip_reset_w29(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

// instruction for erasing flash

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x80, 0x5555 );
	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x10, 0x5555 );

	mdelay(flash_hw->flash->c_erase_timeout);

return 0;
}

/* ------------------------------------------------------------------------ */

int program_byte_w29(struct flash_hw *flash_hw,__u32 addr,__u8 * byte)
{
__u32 flags;
__u32 i;

// the instructions below are really time critical
// therefore we will do all this commands without a break from the system

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif 

// instruction for writing a sector

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0xA0, 0x5555 );

// put all the data into the sector

	for (i=0; i < flash_hw->flash->sa_size; i++)
		chip_put_byte( flash_hw, byte[i], addr+i );

// we allow interrupts

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif

// we will wait 10ms, before we test the end of programming

	mdelay(flash_hw->_twc);

// look for finish of programming

	if (toggle_bit_algorithm(flash_hw,flash_hw->flash->prg_timeout))
		return -1;

// we test all the sector, if the wrote bytes are correct

	for (i=0; i < flash_hw->flash->sa_size; i++)
		if ( chip_get_byte(flash_hw,addr+i) != byte[i] )
		{
			printk(KERN_ERR "%s: prg-error: 0x%0lX\n", MODULE_NAME, addr+i);
			return -2;
		};
// all ok

return 0;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_w29(struct flash_hw *flash_hw,struct file * file,
	const char * buf, size_t count, loff_t *ppos)
{
size_t addr;
__u8 byte[flash_hw->flash->sa_size];

      if (access_ok(VERIFY_READ, (void *)buf, count)==0)
                return -EINVAL;

	addr = (count > flash_hw->flash->sa_size ? flash_hw->flash->sa_size : count);

	memset(byte, 0xFF, flash_hw->flash->sa_size);

// erase flash
	if (!flash_hw->curr_pos)
		if (flash_erase(flash_hw)) return -EFAULT;

	if(copy_from_user(byte, (void *)buf, addr))
                        return -EFAULT;

	if (addr+flash_hw->curr_pos > flash_hw->usable_romsize)
		return -ENOSPC;

	if ( program_byte_w29(flash_hw,flash_hw->curr_pos, byte) )
		return -ENXIO;

	flash_hw->curr_pos += addr;

return addr;
}

/*--------------------------------------------------------------------------*/

int sdp_enable_w29(struct flash_hw *flash_hw, char * secret)
{
__u32 flags;
int i=0;

// the instructions below are really time critical
// therefore we will do all this commands without a break from the system

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif 

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0xA0, 0x5555 );

	for (i=0; i < flash_hw->flash->secret_size; i++)
		chip_put_byte( flash_hw, secret[i], i );

// we allow interrupts

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_irq_restore(flags);
#else
    restore_flags(flags);
#endif

// we will wait 10ms, before we test the end of programming

	mdelay(flash_hw->_twc);

// look for finish of programming

	if (toggle_bit_algorithm(flash_hw,flash_hw->flash->prg_timeout))
		return -1;

return 0;
}

/*--------------------------------------------------------------------------*/

int sdp_disable_w29(struct flash_hw *flash_hw, char * secret)
{
__u32 flags;
int i=0;

// the instructions below are really time critical
// therefore we will do all this commands without a break from the system

#if  (LINUX_VERSION_CODE >= 0x020608)
    local_save_flags(flags);
    local_irq_disable();
#else
    save_flags(flags);
    cli();
#endif 

	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x80, 0x5555 );
	chip_put_byte( flash_hw, 0xAA, 0x5555 );
	chip_put_byte( flash_hw, 0x55, 0x2AAA );
	chip_put_byte( flash_hw, 0x20, 0x5555 );

	for (i=0; i < flash_hw->flash->secret_size; i++)
		chip_put_byte( flash_hw, secret[i], i );

// we allow interrupts

#if  (LINUX_VERSION_CODE >= 0x020608) 
    local_irq_restore(flags);   
#else  
	restore_flags(flags);
#endif

// we will wait 10ms, before we test the end of programming

	mdelay(flash_hw->_twc);

// look for finish of programming

	if (toggle_bit_algorithm(flash_hw,flash_hw->flash->prg_timeout))
		return -1;

return 0;
}

/*--------------------------------------------------------------------------*/
