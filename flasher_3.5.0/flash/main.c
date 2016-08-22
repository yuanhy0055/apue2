/*
 *  main.c: main module for reading and programming of flashproms under linux
 *
 *  Dirk Pfau 07/1998 - 11/2003 dpfau@gmx.de
 *
 *  This program is under the terms of the BSD License.
 *
 */

/* ------------------------------------------------------------------------ */

#include <linux/module.h>
#include "flash.h"
#include "flash_data.h"
#include "flashcalls.h"

#include <linux/proc_fs.h>
#define  flash_io MAGIC_KEY_FLASH_A

struct proc_dir_entry *flash_proc;

static int flash_version_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data);
static int flash_open( struct flash_hw *flash_hw );
static ssize_t flash_read_data(struct flash_hw * flash_hw, struct file * filp, char * buf,
        size_t count, loff_t *ppos);
static ssize_t flash_write(struct flash_hw * flash_hw, struct file * filp, const char * buf,
        size_t count, loff_t *ppos);

struct flash_shm flash_io = {
				flash_open,
				flash_read_data,
				flash_write,
				flash_erase, };

#if LINUX_VERSION_CODE > 0x020600
EXPORT_SYMBOL(flash_io);
#else


#endif

#define MAXFLASH	(sizeof(autoselect)/sizeof(*autoselect))-1

static int flash_identify(struct flash_hw * flash_hw);

static int flash_types_open( struct inode *inode, struct file *filp );
static int flash_types_release( struct inode *inode, struct file *filp );
ssize_t flash_types_read(struct file * filp, char * buf, size_t count, loff_t *ppos);
static struct file_operations flash_types_fops;
static int flash_types_curr_pos=0;
static int flash_types_used=0;
static int debug;

Trust;

/* ------------------------------------------------------------------------ */

MODULE_AUTHOR("Dirk Pfau <dpfau@gmx.de>");
MODULE_SUPPORTED_DEVICE("flash");
MODULE_DESCRIPTION("Flasher for Linux");
#if LINUX_VERSION_CODE > 0x02040a
MODULE_LICENSE("Dual BSD/GPL");
#endif

#if LINUX_VERSION_CODE > 0x020600
module_param(debug,int,0);
#else
MODULE_PARM(debug, "i");
MODULE_PARM_DESC(debug, "Debug enabled or not");
#endif

/* ------------------------------------------------------------------------ */

int data_polling_algorithm(struct flash_hw * flash_hw,__u32 addr,__u8 byte, __u32 wait)
{
__u8 data=0;
unsigned long jiffies_old=jiffies;

	wait=(wait*HZ)/700+1;

	while(1)
	{
		data = chip_get_byte(flash_hw,addr);
		if (DQ7(data) == DQ7(byte)) return chip_get_byte(flash_hw,addr);

/* after wait*10msec we will finish */

		if ((jiffies-jiffies_old) > wait) break;

/* give other processes a chance! */

		schedule();
	}
	
	printk(KERN_ERR "%s: data_polling_algorithm -> error\n", MODULE_NAME );
printk(KERN_ERR "%s: addr: 0x%8.8lx wr:0x%2.2x rd:0x%2.2x to: %ld time:%ld \n", MODULE_NAME,addr,byte,data,wait,jiffies-jiffies_old );

return -1;
}

/* ------------------------------------------------------------------------ */

int toggle_bit_algorithm(struct flash_hw * flash_hw,__u32 wait)
{
__u8 tog_prev, tog_cur=0;
unsigned long jiffies_old=jiffies;

	wait=(wait*HZ)/700+1;

	tog_prev = chip_get_byte(flash_hw,1);

	while(1)
	{
		tog_cur = chip_get_byte(flash_hw,1);
		if (DQ6(tog_prev) == DQ6(tog_cur)) return 0;
		tog_prev = tog_cur;

/* after wait*10msec we will finish */

		if ((jiffies-jiffies_old) > wait) break;

/* give other processes a chance! */

		schedule();
	}

	printk(KERN_ERR "%s: toggle_bit_algorithm -> error\n", MODULE_NAME );
	printk(KERN_ERR "%s: rd:%2.2x to: %ld time:%ld \n", MODULE_NAME,tog_cur,wait,jiffies-jiffies_old );

return -1;
}

/* ------------------------------------------------------------------------ */

int flash_open( struct flash_hw *flash_hw )
{

//	MOD_INC_USE_COUNT;

	memset(flash_hw->flash, 0, sizeof(struct flashdat));

	flash_hw->_twc = 20;

	flash_hw->trust = NOT_SURE;

// which type of flash is inside the socket?

	flash_hw->trust = flash_identify(flash_hw) & 0x03;

	flash_hw->curr_pos = 0;

// we set the right timeout

	flash_hw->_twc = flash_hw->flash->twc;

return 0;
}

/* ------------------------------------------------------------------------ */

int chip_erase_check(struct flash_hw * flash_hw)
{
__u32 addr=0;

	(*chip_reset[flash_hw->flash->rule])(flash_hw);

// is the chip erased?

	while ( addr < flash_hw->usable_romsize )
	{
		if ( chip_get_byte(flash_hw,addr) != 0xFF )
		{
			printk(KERN_DEBUG "%s: flash not erased.\n", MODULE_NAME );
			return -EFAULT;
		};

// after 128 bytes we will give other processes a chance
// (problems with dma timeout)

		if (!(++addr & 0x7F))
			schedule();
	}

	printk(KERN_DEBUG "%s: flash erased.\n", MODULE_NAME );
return 0;
}

/* ------------------------------------------------------------------------ */

int flash_erase(struct flash_hw * flash_hw)
{
// first we will reset the chip

	(*chip_reset[flash_hw->flash->rule])(flash_hw);

// is the flash clean?

	if (!chip_erase_check(flash_hw)) return 0;

// erase the flash

	if ((*chip_erase[flash_hw->flash->rule])(flash_hw)) return -EFAULT;

// is it really clean?

	if (chip_erase_check(flash_hw)) return -EFAULT;

return 0;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_read_data(struct flash_hw * flash_hw,struct file * filp, char * buf,
	size_t count, loff_t *ppos)
{
size_t a;
size_t size;
char data[1024];

// at the beginning we reset the chip

	if (!flash_hw->curr_pos)
		(*chip_reset[flash_hw->flash->rule])(flash_hw);

       if (access_ok(VERIFY_WRITE, buf, count)==0)
                return -EFAULT;

	size = (count > sizeof(data) ? sizeof(data) : count);

	if ((flash_hw->curr_pos+size) > flash_hw->usable_romsize)
		size = flash_hw->usable_romsize-flash_hw->curr_pos;

	for (a=0; a < size; a++)
		data[a] = chip_get_byte(flash_hw,a+flash_hw->curr_pos);

	if(copy_to_user((void *)buf, data, a))
                return -EFAULT;


	flash_hw->curr_pos += a;

return (ssize_t)a;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_write(struct flash_hw *flash_hw, struct file * filp,
	const char * buf, size_t count, loff_t *ppos)
{
return (*flash_write_data[flash_hw->flash->rule])(flash_hw, filp, buf, count, ppos);
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static int __init flash_init(void)
#else
int init_module(void)
#endif
{
struct proc_dir_entry *proc;

	if(!(flash_proc=create_proc_entry(MODULE_NAME, S_IFDIR | S_IRUGO | S_IXUGO, &proc_root)))
		goto error_flash_dir;

#if LINUX_VERSION_CODE > 0x020400
	flash_proc->owner = THIS_MODULE;
#endif
	if (!(proc=create_proc_entry("version", S_IFREG | S_IRUSR, flash_proc)))
		goto error_version;
	proc->read_proc = flash_version_proc_read;
#if LINUX_VERSION_CODE > 0x020400
	proc->owner = THIS_MODULE;
#endif

// set fops for device /proc/flashprom/supported_types
 
        flash_types_fops.owner = THIS_MODULE;
        flash_types_fops.open = flash_types_open;
        flash_types_fops.read = flash_types_read;
        flash_types_fops.release = flash_types_release;

// create proc entry "supported_types"

	if (!(proc=create_proc_entry("supported_types", S_IFREG | S_IRUSR, flash_proc)))
		goto error_supported_types;

	proc->proc_fops = &flash_types_fops;

// register this module (flash routines are online)

//inter_module_register(MAGIC_KEY_FLASH, THIS_MODULE, &flash_io);

	printk(KERN_DEBUG "%s: Module installed.\n", MODULE_NAME );
return 0;

error_supported_types:
	remove_proc_entry( "version", flash_proc );
error_version:
	remove_proc_entry( MODULE_NAME, &proc_root );
error_flash_dir:
	printk(KERN_ERR "flash: unable to register proc entry!\n");
return -EIO;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
static void __exit flash_cleanup(void)
#else
void cleanup_module(void)
#endif
{ 
//	inter_module_unregister(MAGIC_KEY_FLASH);
	remove_proc_entry( "version", flash_proc );
	remove_proc_entry( "supported_types", flash_proc );
        remove_proc_entry(MODULE_NAME, &proc_root);

	printk(KERN_DEBUG "%s: cleanup_module successfull.\n", MODULE_NAME );

return;
}

/* ------------------------------------------------------------------------ */

#if (LINUX_VERSION_CODE >= 0x020300)
module_init(flash_init);
module_exit(flash_cleanup);
#endif

/* ------------------------------------------------------------------------ */

static int flash_identify(struct flash_hw * flash_hw)
{
int tmp;
int i=0;
int a=0;
__u16 id=0;
	

	for (i=MAXFLASH; i; i--)
	{
	    id = (*autoselect[i])(flash_hw);
        if(debug)printk("%s: id=0x%04x\n", MODULE_NAME, id);
	    for (a=0; Flash[a].id; a++)
	    {
		if ((Flash[a].id == id) && (i == Flash[a].rule))
		{
		    *(flash_hw->flash) = Flash[a];
		    tmp = (*verify_id[i])(flash_hw,id);
		    if ( tmp > 0)
		    {
		        printk(KERN_DEBUG "%s: flashprom found: %s %s\n", \
			    MODULE_NAME, Flash[a].type, reliability[tmp]);
			flash_hw->protect = (*write_protect_check[i])(flash_hw);
			return tmp;
		    }
		}
	    }
	}

	flash_hw->protect = 0;
	*(flash_hw->flash) = Flash[a];
	printk(KERN_DEBUG "%s: manufacturer unknown\n", MODULE_NAME);

return -1;
}

/*--------------------------------------------------------------------------*/

void chip_put_byte(struct flash_hw * flash_hw,__u8 byte,__u32 addr)
{
  	(*hwmod_put_byte[flash_hw->flash->rule])(flash_hw,byte,addr);
};


/*--------------------------------------------------------------------------*/

__u8 chip_get_byte(struct flash_hw * flash_hw,__u32 addr)
{
	return (*hwmod_get_byte[flash_hw->flash->rule])(flash_hw,addr);
};

/*--------------------------------------------------------------------------*/

static int flash_version_proc_read(char *page, char **start, off_t off, \
       int count, int *eof, void *data)
{
int len = 0;

	len += sprintf(page + len, "flash module version: %s\n", FLASH_VERSION);

return len;
}

/*--------------------------------------------------------------------------*/

static int flash_types_open(struct inode * inode, struct file * filp)
{
	if (flash_types_used)
		return -EBUSY;

	flash_types_used = 1;
	flash_types_curr_pos = 0;

return 0;
}

/* ------------------------------------------------------------------------ */

static int flash_types_release( struct inode *inode, struct file *filp )
{
	flash_types_used = 0;
 
return 0;
}

/* ------------------------------------------------------------------------ */

ssize_t flash_types_read(struct file * filp, char * buf, size_t count, loff_t *ppos)
{
int i;
ssize_t nbytes;
char data[80];

// are we at the end of list?

	if (!Flash[flash_types_curr_pos].id)
		return 0;

      if (access_ok(VERIFY_WRITE, buf, count)==0)
                return -EFAULT;

	i = sprintf(data, "%s\n", Flash[flash_types_curr_pos].type);

	nbytes = (i > (int)count ? (ssize_t)count : i);

	if(copy_to_user((void *)buf, data, nbytes))
		return -EFAULT;

	flash_types_curr_pos++;

return nbytes;
}

/*--------------------------------------------------------------------------*/
