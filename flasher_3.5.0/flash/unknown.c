/*
 * unknown.c: for reading unknown types
 *
 * Dirk Pfau 05/2001 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * prepared for v2.0
 *
 */


#include "flash.h"

/*--------------------------------------------------------------------------*/

__u8 hwmod_get_byte_unknown(struct flash_hw * flash_hw,__u32 addr)
{
    // don't use addresses greater than supported flashrom size
    // we cut all address bits out of the limit
    addr &= (flash_hw->max_romsize-1);
    return (*flash_hw->flash_get)(addr,flash_hw->dev);
}
 
/*--------------------------------------------------------------------------*/

void hwmod_put_byte_unknown(struct flash_hw * flash_hw,__u8 byte,__u32 addr)
{
// don't use addresses greater than supported flashrom size
// we cut all address bits out of the limit
    addr &= (flash_hw->max_romsize-1);
    (*flash_hw->flash_put)(byte,addr,flash_hw->dev);
};      


/* ------------------------------------------------------------------------ */

int protect_check_unknown(struct flash_hw * flash_hw)
{
return 0;
}

/* ------------------------------------------------------------------------ */

void chip_reset_unknown(struct flash_hw *flash_hw)
{
return;
}

/* ------------------------------------------------------------------------ */

__u16 autoselect_unknown(struct flash_hw *flash_hw)
{
return 0xFFFF;
}

/* ------------------------------------------------------------------------ */

int verify_id_unknown(struct flash_hw *flash_hw, __u16 id)
{
return NOT_SURE;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write_data_unknown(struct flash_hw *flash_hw, struct file * file,
	const char * buf, size_t count, loff_t *ppos)
{
return -ENOSPC;
}

/* ------------------------------------------------------------------------ */

int chip_erase_unknown(struct flash_hw *flash_hw)
{
return -EIO;
}

/* ------------------------------------------------------------------------ */
