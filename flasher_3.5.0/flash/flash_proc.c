/*
 * flash_proc.c: i'm very unhappy about it....
 *
 *
 * Dirk Pfau 07/1998 - 06/2002 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * Supported devices:
 *
 * version 3.x:
 *
 * /proc/.../data:		programming/reading the flash
 * /proc/.../info:		get informations about the hardware
 * /proc/.../erase:		only erase the flash
 *
 *
 * History:
 *
 *
 */

/* ------------------------------------------------------------------------ */
#include <linux/module.h>
#include <linux/proc_fs.h>

#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include "global.h"

#define  flash_io MAGIC_KEY_FLASH_A     

extern struct flash_shm flash_io;
static struct flash_hw lowlevel_hw[Flash_Max_Devices];
static struct file_operations flash_erase_fops[Flash_Max_Devices];
static struct file_operations flash_info_fops[Flash_Max_Devices];
static struct file_operations flash_data_fops[Flash_Max_Devices];

int hw_info(struct flashdat * flash, char * jpm , int len );

static int flash_used=0;
static int dev=0;
static int used_dev=0;
static const char *protection[]={"unknown", "yes", "no", ""};
Trust;

/* ------------------------------------------------------------------------ */

static int flash_open(struct inode * inode, struct file * filp);
static int flash_release(struct inode *inode, struct file *filep);
static ssize_t flash_data_read(struct file *, char *, size_t, loff_t *);
static ssize_t flash_data_write(struct file *, const char *, size_t, loff_t *);
static ssize_t flash_info_read(struct file *, char *, size_t , loff_t *);
static ssize_t flash_erase_read(struct file *, char *, size_t , loff_t *);

/* ------------------------------------------------------------------------ */
/* ------------------------------------------------------------------------ */

static int get_path_procfs(struct proc_dir_entry *proc_curr, int count, char **name)
{
// we add the size of every name/part of path (+1 is for "/")

	count += strlen(proc_curr->name) + 1;

// do we have arrived the root?

	if (proc_curr != proc_curr->parent)

// no! we must go deeper.

		get_path_procfs(proc_curr->parent, count, name);
	else
	{
// yes! now we have the size of full path. but at the end we need a "\0"

		count++;
		*name = kmalloc(count, GFP_KERNEL);
		memset(*name, 0, count);
	}
// copy the part of path at the end of *name

	strcat(*name, proc_curr->name);
	strcat(*name, "/");

return 0;
}

/* ------------------------------------------------------------------------ */

static int get_path_dentry(struct dentry *dentry_curr, int count, char **name)
{
// we add the size of every name/part of path (+1 is for "/")

	count += strlen(dentry_curr->d_name.name) + 1;

// do we have arrived the root?

	if (dentry_curr != dentry_curr->d_parent)

// no! we must go deeper.

		get_path_dentry(dentry_curr->d_parent, count, name);
	else
	{
// yes! now we have the size of full path. but at the end we need a "\0",
// at beginning a "/proc"

		count += 6;
		*name = kmalloc(count, GFP_KERNEL);
		memset(*name, 0, count);
		strcat(*name, "/proc/");
		return 0;
	}

// copy the part of path at the end of *name

	strcat(*name, dentry_curr->d_name.name);
	strcat(*name, "/");

return 0;
}

/* ------------------------------------------------------------------------ */

static int flash_open(struct inode * inode, struct file * filp)
{
char *path_name;

// is it in use?

	if (flash_used)
		return -EBUSY;

	flash_used = 1;

// we need the full path of opened file (multiple files use the same routine)

	get_path_dentry(filp->f_dentry->d_parent, 0, &path_name);

printk(KERN_ERR "flash_open: path=\"%s\"\n", path_name);

// with help of full path name we look for the corresponding device number

	for (used_dev=0; lowlevel_hw[used_dev].path; used_dev++)
		if (!strcmp(lowlevel_hw[used_dev].path,path_name)) break;

// path_name not needed anymore

	kfree(path_name);

printk(KERN_ERR "flash_open: used_dev=%d\n", used_dev);

// is the flash module loaded?

///////	flash_io = (struct flash_shm *)symbol_get(flash_io_pointer);

// no!
/////////	if (!flash_io)
////////////		goto flash_not_loaded;	

// let us look, which flashprom is used

	(flash_io.flash_open)(&lowlevel_hw[used_dev]);

// we don't read more bytes, than the hardware supports.

	lowlevel_hw[used_dev].usable_romsize = lowlevel_hw[used_dev].flash->rom_size;
	if ((lowlevel_hw[used_dev].flash->rom_size==0)||(lowlevel_hw[used_dev].flash->rom_size > lowlevel_hw[used_dev].max_romsize))
	lowlevel_hw[used_dev].usable_romsize = lowlevel_hw[used_dev].max_romsize;

return 0;

//flash_not_loaded:
//	flash_used = 0;
//	printk(KERN_ERR "flash: main module not loaded\n");
//	return -EBUSY;
}

/* ------------------------------------------------------------------------ */

static int flash_release( struct inode *inode, struct file *filp )
{
	flash_used = 0;
////////	symbol_put(flash_io);

return 0;
}

/* ------------------------------------------------------------------------ */

static ssize_t flash_data_read(struct file *filp, char *buf, size_t count, \
                               loff_t *ppos)
{
return (flash_io.flash_read)(&lowlevel_hw[used_dev], filp, buf, count, ppos);
}

/* ------------------------------------------------------------------------ */

static ssize_t flash_data_write(struct file *filp, const char *buf, size_t count, \
                                loff_t *ppos)
{
return (flash_io.flash_write)(&lowlevel_hw[used_dev], filp, buf, count, ppos);
}

/* ------------------------------------------------------------------------ */

static ssize_t flash_info_read(struct file *filp, char *buf, size_t count, \
                               loff_t *ppos)
{
char data[MAX_INFO_SIZE];
int nr = used_dev;
int len;

// be careful of a loop

        if (lowlevel_hw[nr].curr_pos)
                return 0;

	len = snprintf(data,MAX_INFO_SIZE,
				"%s"
				"manufacturer:   %s\n"
				"flashtype:      %s %s\n"
				"protected:      %s\n",
				lowlevel_hw[nr].info,
				lowlevel_hw[nr].flash->manufacturer,
				lowlevel_hw[nr].flash->type,
				reliability[lowlevel_hw[nr].trust],
				protection[lowlevel_hw[nr].protect]
				);
	if(len>0&&len<MAX_INFO_SIZE)
		len+=1+snprintf(data+len,MAX_INFO_SIZE-len,
        (lowlevel_hw[nr].flash->rom_size > lowlevel_hw[nr].max_romsize)?
			"romsize reduced to %ld kbytes because of hardware restrictions\n":
			"usable romsize: %ld kbytes\n",
		lowlevel_hw[nr].usable_romsize/1024);

	if(len>0&&len<MAX_INFO_SIZE)
		len+=hw_info(lowlevel_hw[nr].flash,data+len-1,MAX_INFO_SIZE-len);

	lowlevel_hw[nr].curr_pos=len;

    if ((ssize_t)count < lowlevel_hw[nr].curr_pos)  
		return -EFAULT;

	if (access_ok(VERIFY_WRITE, buf, lowlevel_hw[nr].curr_pos)==0)
		return -EFAULT;

	if(copy_to_user((void *)buf, data, lowlevel_hw[nr].curr_pos))
		return -EFAULT;

return lowlevel_hw[nr].curr_pos;
}

/* ------------------------------------------------------------------------ */

static ssize_t flash_erase_read(struct file *filp, char *buf, size_t count, \
                                loff_t *ppos)
{
char data[MAX_INFO_SIZE];
int nr = used_dev;

// be careful of a loop

        if (lowlevel_hw[nr].curr_pos)
                return 0;

	if ((flash_io.flash_erase)(&lowlevel_hw[nr]))
        	lowlevel_hw[nr].curr_pos = sprintf(data, "chip not erased\n");
	else
        	lowlevel_hw[nr].curr_pos = sprintf(data, "chip erased\n");

	if ((ssize_t)count < lowlevel_hw[nr].curr_pos)
		return -EFAULT;

	if (access_ok(VERIFY_WRITE, buf, lowlevel_hw[nr].curr_pos)==0)
		return -EFAULT;

	if(copy_to_user((void *)buf, data, lowlevel_hw[nr].curr_pos))
		return -EFAULT;

return lowlevel_hw[nr].curr_pos;
}

/* ------------------------------------------------------------------------ */

struct flash_hw * init_flash_proc(const char * name, struct proc_dir_entry *proc_curr, \
                          __u8 (*flash_get)(__u32 addr, int dev), \
                          void (*flash_put)(__u8 byte,__u32 addr, int dev), \
                          char * info, __u32 max_romsize)
{
struct proc_dir_entry *proc;

	if (dev >= Flash_Max_Devices)
		goto error_max_devices;
		
	if ( !(lowlevel_hw[dev].flash = kmalloc(sizeof(struct flashdat),GFP_KERNEL)) )
		goto error_kmalloc_flash;

	memset(lowlevel_hw[dev].flash,0,sizeof(struct flashdat));

	if ( !(lowlevel_hw[dev].info = kmalloc(strlen(info)+1,GFP_KERNEL)) )
		goto error_kmalloc_info;

	memset(lowlevel_hw[dev].info,0,strlen(info)+1);

	if ( !(lowlevel_hw[dev].flash_name = kmalloc(strlen(name)+1,GFP_KERNEL)) )
		goto error_kmalloc_flash_name;

	memset(lowlevel_hw[dev].flash_name,0,strlen(name)+1);

	strcpy(lowlevel_hw[dev].info, info);
	strcpy(lowlevel_hw[dev].flash_name, name);
	lowlevel_hw[dev].max_romsize = max_romsize;
	lowlevel_hw[dev].flash_put = flash_put;
	lowlevel_hw[dev].flash_get = flash_get;
	lowlevel_hw[dev].basic_proc = proc_curr;
	lowlevel_hw[dev].dev = dev;

// set fops for device /proc/../info

	flash_info_fops[dev].owner = THIS_MODULE;
	flash_info_fops[dev].open = flash_open;
	flash_info_fops[dev].read = flash_info_read;
	flash_info_fops[dev].release = flash_release;

// set fops for device /proc/../data

	flash_data_fops[dev].owner = THIS_MODULE;
	flash_data_fops[dev].open = flash_open;
	flash_data_fops[dev].read = flash_data_read;
	flash_data_fops[dev].write = flash_data_write;
	flash_data_fops[dev].release = flash_release;

// set fops for device /proc/../erase

	flash_erase_fops[dev].owner = THIS_MODULE;
	flash_erase_fops[dev].open = flash_open;
	flash_erase_fops[dev].read = flash_erase_read;
	flash_erase_fops[dev].release = flash_release;


// create directory

        if(!(lowlevel_hw[dev].flash_proc=create_proc_entry(name, \
	     S_IFDIR | S_IRUGO | S_IXUGO, proc_curr)))
                goto error_flash_dir;

#if LINUX_VERSION_CODE > 0x020400
        lowlevel_hw[dev].flash_proc->owner = THIS_MODULE;
#endif

//create file "info"

	if (!(proc=create_proc_entry("info", S_IFREG | S_IRUSR, lowlevel_hw[dev].flash_proc)))
		goto error_info;
	proc->proc_fops = &flash_info_fops[dev];

//create file "data"

	if (!(proc=create_proc_entry("data", S_IFREG | S_IRUSR, lowlevel_hw[dev].flash_proc)))
		goto error_data;
	proc->proc_fops = &flash_data_fops[dev];

// create file "erase"

	if (!(proc=create_proc_entry("erase", S_IFREG | S_IRUSR, lowlevel_hw[dev].flash_proc)))
		goto error_erase;
	proc->proc_fops = &flash_erase_fops[dev];

// get and save the path of used directory

	get_path_procfs(lowlevel_hw[dev].flash_proc, 0, &lowlevel_hw[dev].path);

printk(KERN_ERR "init_flash_proc %s: path=\"%s\"\n", name, lowlevel_hw[dev].path);

	dev++;

return &lowlevel_hw[dev-1];

error_erase:
        remove_proc_entry( "data", lowlevel_hw[dev].flash_proc );
error_info:
        remove_proc_entry( "info", lowlevel_hw[dev].flash_proc );
error_data:
        remove_proc_entry( name, proc_curr );
error_flash_dir:
        printk(KERN_ERR "%s: unable to register proc entry!\n", name);

	kfree(lowlevel_hw[dev].flash_name);
error_kmalloc_flash_name:
	kfree(lowlevel_hw[dev].info);
error_kmalloc_info:
	kfree(lowlevel_hw[dev].flash);
error_kmalloc_flash:
	return NULL;
error_max_devices:
	printk(KERN_ERR "%s: max. device count arrived.\n", name );
	return NULL;
}

/* ------------------------------------------------------------------------ */
// freeing all

void release_flash_proc(void)
{ 
	while(dev)
	{
		dev--;
        	remove_proc_entry( "data", lowlevel_hw[dev].flash_proc );
        	remove_proc_entry( "info", lowlevel_hw[dev].flash_proc );
        	remove_proc_entry( "erase", lowlevel_hw[dev].flash_proc );
        	remove_proc_entry( lowlevel_hw[dev].flash_name, lowlevel_hw[dev].basic_proc );
		kfree(lowlevel_hw[dev].flash_name);
		kfree(lowlevel_hw[dev].flash);
		kfree(lowlevel_hw[dev].info);
		kfree(lowlevel_hw[dev].path);
	}

return;
}

/* ------------------------------------------------------------------------ */
