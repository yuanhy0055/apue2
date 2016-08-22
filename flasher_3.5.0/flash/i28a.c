/*
 * i28.c: algorithm for INTEL 28F00x
 *
 * Dirk Pfau 12/2001 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * prepared for v2.0
 *
 */


#include "flash.h"

/*--------------------------------------------------------------------------*/
void hwmod_put_byte_i28a(struct flash_hw * flash_hw,__u8 byte,__u32 addr)
{
    addr|=0x20000;
// don't use addresses greater than supported flashrom size
// we cut all address bits out of the limit
        addr &= (flash_hw->max_romsize-1);
        (*flash_hw->flash_put)(byte,addr,flash_hw->dev);
};

/*--------------------------------------------------------------------------*/

__u8 hwmod_get_byte_i28a(struct flash_hw * flash_hw,__u32 addr)
{
    addr|=0x20000;
        // don't use addresses greater than supported flashrom size
        // we cut all address bits out of the limit
        addr &= (flash_hw->max_romsize-1);
        return (*flash_hw->flash_get)(addr,flash_hw->dev);
}
  

/* ------------------------------------------------------------------------ */

void chip_reset_i28a(struct flash_hw *flash_hw)
{
	chip_put_byte( flash_hw, 0xFF, 0x20000 );
	chip_put_byte( flash_hw, 0xFF, 0x20000 );
return;
}

/* ------------------------------------------------------------------------ */

static __u8 read_status_register(struct flash_hw *flash_hw, __u32 offset)
{
	chip_put_byte( flash_hw, 0x70, offset );
return chip_get_byte( flash_hw,offset);
}
	
/* ------------------------------------------------------------------------ */

static void clear_status_register(struct flash_hw *flash_hw)
{
	chip_put_byte( flash_hw, 0x50, 0x0000 );
return;
}
	
/* ------------------------------------------------------------------------ */

static __u16 _autoselect(struct flash_hw *flash_hw, __u32 offset)
{
__u16 id;

	chip_reset_i28a(flash_hw);

	chip_put_byte( flash_hw, 0x90, 0x20000|offset );
	
	id = (chip_get_byte(flash_hw,offset+0x20000) << 8) | chip_get_byte(flash_hw,offset+0x20001);

	chip_reset_i28a(flash_hw);

return id;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_i28a(struct flash_hw *flash_hw) {return _autoselect(flash_hw,0);};

/* ------------------------------------------------------------------------ */

int verify_id_i28a(struct flash_hw *flash_hw,__u16 id)
{
__u32 testaddr=0;

        chip_reset_i28a(flash_hw);

        while (testaddr < flash_hw->usable_romsize)
        {
            if ( id != ((chip_get_byte(flash_hw,testaddr+0x00) << 8) | chip_get_byte(flash_hw,testaddr+0x01)))
            {
                if ( id != _autoselect(flash_hw,testaddr) )
                        return NOT_VERIFIED;
                else
                        return VERIFIED;
            }

            if ( id != ((read_status_register(flash_hw,testaddr+0x00) << 8) |
                         read_status_register(flash_hw,testaddr+0x01)))
                                return VERIFIED;

            testaddr += 0x02;
        }

return NOT_SURE;
}


/* ------------------------------------------------------------------------ */
static int check_status_programming(struct flash_hw *flash_hw,__u32 addr)
{
__u8 srd;
	srd = chip_get_byte( flash_hw,0x00000);
	if (DQ7(srd))
	{
	    clear_status_register(flash_hw);

	    if (DQ3(srd))
	    {
		printk(KERN_ERR "%s: Vpp out of range\n", MODULE_NAME );
		return -1;
	    }
	    if (DQ4(srd))
	    {
		printk(KERN_ERR "%s: error while programming byte\n", MODULE_NAME );
		return -2;
	    }
	    return chip_get_byte(flash_hw,addr);
	}
return -3;
}

/* ------------------------------------------------------------------------ */

int program_byte_i28a(struct flash_hw *flash_hw,__u32 addr,__u8 byte)
{
int i;
__u32 wait=(flash_hw->flash->prg_timeout*HZ)/700 + 1;
unsigned long jiffies_old=jiffies;

	clear_status_register(flash_hw);

// instruction for programming a byte

	chip_put_byte( flash_hw, 0x40, addr );
	chip_put_byte( flash_hw, byte, addr );

// look for finish of programming

	while(jiffies-jiffies_old < wait)
	{
		i = check_status_programming(flash_hw,addr);
		if (i > -3 ) return i;

/* give other processes a chance! */

		schedule();
	}

	i = check_status_programming(flash_hw,addr);
	if (i > -3 ) return i;


	printk(KERN_ERR "%s: timeout while programming byte\n", MODULE_NAME );

	clear_status_register(flash_hw);

return -3;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_i28a(struct flash_hw *flash_hw, struct file * file,
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

		if ( program_byte_i28a(flash_hw, addr+flash_hw->curr_pos, byte) != byte )
			return -ENXIO;
	}

	flash_hw->curr_pos += addr;

return (ssize_t)addr;
}

/* ------------------------------------------------------------------------ */

static int check_srd_erasing(struct flash_hw *flash_hw)
{
__u8 srd=chip_get_byte(flash_hw,0x00000);

	if (DQ7(srd))
	{
	    chip_reset_i28a(flash_hw);
	    clear_status_register(flash_hw);

	    if (DQ3(srd))
	    {
		printk(KERN_ERR "%s: Vpp out of range\n", MODULE_NAME );
		return -1;
	    }
	    if (DQ4(srd) && DQ5(srd))
	    {
		printk(KERN_ERR "%s: command sequence error\n", MODULE_NAME );
		return -2;
	    }
	    if (DQ5(srd))
	    {
		printk(KERN_ERR "%s: block erase error\n", MODULE_NAME );
		return -3;
	    }
	    return 0;
	}
return -4;
}

/* ------------------------------------------------------------------------ */

static int check_status_erasing(struct flash_hw *flash_hw)
{
int i;
__u32 wait=(flash_hw->flash->s_erase_timeout*HZ)/700 + 1;
unsigned long jiffies_old=jiffies;

    while(jiffies-jiffies_old < wait)
    {
	i = check_srd_erasing(flash_hw);
	if (i > -4) return i;

/* give other processes a chance! */

	schedule();
    }

    i = check_srd_erasing(flash_hw);
    if (i > -4) return i;

    printk(KERN_ERR "%s: timeout while erasing chip\n", MODULE_NAME );

    chip_reset_i28a(flash_hw);
    clear_status_register(flash_hw);

return -4;
}

/* ------------------------------------------------------------------------ */

int chip_erase_i28a(struct flash_hw *flash_hw)
{
int i;

// first we will reset the chip

	chip_reset_i28a(flash_hw);

	printk(KERN_DEBUG "%s: erasing flash.\n", MODULE_NAME );

// instruction for erasing flash

	for (i=0; flash_hw->flash->block_address[i] >= 0 ; i++)
	{
	    clear_status_register(flash_hw);
	    chip_reset_i28a(flash_hw);

	    chip_put_byte( flash_hw, 0x20, (__u32)flash_hw->flash->block_address[i] );
	    chip_put_byte( flash_hw, 0xD0, (__u32)flash_hw->flash->block_address[i] );

	    if (check_status_erasing(flash_hw))
		return -EFAULT;
	}

return 0;
}

/* ------------------------------------------------------------------------ */
