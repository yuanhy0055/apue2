/*
 * eeprom_proc.c: install devices for access to eeprom
 *
 *
 * Dirk Pfau 03/2005 dpfau@gmx.de
 *
 * This program is under the terms of the BSD License.
 *
 * Supported devices:
 *
 * version 3.x:
 *
 * /proc/.../data:		programming/reading the eeprom
 * /proc/.../info:		list register/flags configured in eeprom
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
#include "../../flash/global.h"

static struct flash_hw * lowlevel_hw[Flash_Max_Devices];

int eeprom_info(struct flash_hw * lowlevel_hw, char * jpm , int len );

static struct file_operations eeprom_info_fops[Flash_Max_Devices];
static struct file_operations eeprom_data_fops[Flash_Max_Devices];

static int eeprom_used=0;
static int dev=0;
static int eeprom_used_dev=0;

/* ------------------------------------------------------------------------ */

static int eeprom_open(struct inode * inode, struct file * filp);
static int eeprom_release(struct inode *inode, struct file *filep);
static ssize_t eeprom_data_read(struct file *, char *, size_t, loff_t *);
static ssize_t eeprom_data_write(struct file *, const char *, size_t, loff_t *);
static ssize_t eeprom_info_read(struct file *, char *, size_t , loff_t *);

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

static int eeprom_open(struct inode * inode, struct file * filp)
{
char *path_name;

// is it in use?

	if (eeprom_used)
		return -EBUSY;

	eeprom_used = 1;

// we need the full path of opened file (multiple files use the same routine)

	get_path_dentry(filp->f_dentry->d_parent, 0, &path_name);

printk(KERN_ERR "eeprom_open: path=\"%s\"\n", path_name);

// with help of full path name we look for the corresponding device number

	for (eeprom_used_dev=0; lowlevel_hw[eeprom_used_dev]->eeprom_path; eeprom_used_dev++)
		if (!strcmp(lowlevel_hw[eeprom_used_dev]->eeprom_path,path_name)) break;

// path_name not needed anymore

	kfree(path_name);

printk(KERN_ERR "eeprom_open: eeprom_used_dev=%d\n", eeprom_used_dev);


return 0;

}

/* ------------------------------------------------------------------------ */

static int eeprom_release( struct inode *inode, struct file *filp )
{
	eeprom_used = 0;

return 0;
}

/* ------------------------------------------------------------------------ */

static ssize_t eeprom_data_read(struct file *filp, char *buf, size_t count, \
                               loff_t *ppos)
{
return 0;
}

/* ------------------------------------------------------------------------ */

static ssize_t eeprom_data_write(struct file *filp, const char *buf, size_t count, \
                                loff_t *ppos)
{
return 0;
}

/* ------------------------------------------------------------------------ */

static ssize_t eeprom_info_read(struct file *filp, char *buf, size_t count, \
                               loff_t *ppos)
{
char data[MAX_INFO_SIZE];
int nr = eeprom_used_dev;
int len=0;

// be careful of a loop

        if (lowlevel_hw[nr]->curr_pos)
                return 0;

	len+=eeprom_info(lowlevel_hw[nr],data+len,MAX_INFO_SIZE-len);


	lowlevel_hw[nr]->curr_pos=len;

    if ((ssize_t)count < lowlevel_hw[nr]->curr_pos)  
		return -EFAULT;

       if (access_ok(VERIFY_WRITE, buf, lowlevel_hw[nr]->curr_pos)==0)
                return -EFAULT;

	if(copy_to_user((void *)buf, data, lowlevel_hw[nr]->curr_pos))
                return -EFAULT;

return lowlevel_hw[nr]->curr_pos;
}

		
/* ------------------------------------------------------------------------ */

int init_eeprom_proc(const char * name, struct flash_hw * curr_flash_hw, \
                          __u8 (*eeprom_get)(__u32 addr, int device_nr), \
                          void (*eeprom_put)(__u8 byte,__u32 addr, int device_nr))
{
struct proc_dir_entry *proc;

	lowlevel_hw[dev] = curr_flash_hw;

	if ( !(lowlevel_hw[dev]->eeprom_name = kmalloc(strlen(name)+1,GFP_KERNEL)) )
		goto error_kmalloc_eeprom_name;

	memset(lowlevel_hw[dev]->eeprom_name,0,strlen(name)+1);

	strcpy(lowlevel_hw[dev]->eeprom_name, name);

// set fops for device /proc/../eeprom/info

	eeprom_info_fops[dev].owner = THIS_MODULE;
	eeprom_info_fops[dev].open = eeprom_open;
	eeprom_info_fops[dev].read = eeprom_info_read;
	eeprom_info_fops[dev].release = eeprom_release;

// set fops for device /proc/../eeprom/data

	eeprom_data_fops[dev].owner = THIS_MODULE;
	eeprom_data_fops[dev].open = eeprom_open;
	eeprom_data_fops[dev].read = eeprom_data_read;
//	eeprom_data_fops[dev].write = eeprom_data_write;
	eeprom_data_fops[dev].release = eeprom_release;

// create directory

	if(!(lowlevel_hw[dev]->eeprom_proc=create_proc_entry(name, \
		S_IFDIR | S_IRUGO | S_IXUGO, lowlevel_hw[dev]->flash_proc)))
		goto error_eeprom_dir;

#if LINUX_VERSION_CODE > 0x020400
        lowlevel_hw[dev]->eeprom_proc->owner = THIS_MODULE;
#endif

//create file "info"

	if (!(proc=create_proc_entry("info", S_IFREG | S_IRUSR, lowlevel_hw[dev]->eeprom_proc)))
		goto error_info;
	proc->proc_fops = &eeprom_info_fops[dev];

//create file "data"

	if (!(proc=create_proc_entry("data", S_IFREG | S_IRUSR, lowlevel_hw[dev]->eeprom_proc)))
		goto error_data;
	proc->proc_fops = &eeprom_data_fops[dev];

// get and save the path of used directory

	get_path_procfs(lowlevel_hw[dev]->eeprom_proc, 0, &lowlevel_hw[dev]->eeprom_path);

printk(KERN_ERR "init_eeprom_proc %d %s: path=\"%s\"\n", dev, name, lowlevel_hw[dev]->eeprom_path);


	dev++;

return 0;

error_info:
        remove_proc_entry( "info", lowlevel_hw[dev]->eeprom_proc );
error_data:
        remove_proc_entry( name, lowlevel_hw[dev]->flash_proc );
error_eeprom_dir:
        printk(KERN_ERR "%s: unable to register proc entry!\n", name);
	kfree(lowlevel_hw[dev]->eeprom_name);
error_kmalloc_eeprom_name:
	return -EBUSY;
}

/* ------------------------------------------------------------------------ */
// freeing all

void release_eeprom_proc(void)
{ 
	while(dev)
	{
		dev--;
        	remove_proc_entry( "data", lowlevel_hw[dev]->eeprom_proc );
        	remove_proc_entry( "info", lowlevel_hw[dev]->eeprom_proc );
		remove_proc_entry( lowlevel_hw[dev]->eeprom_name, lowlevel_hw[dev]->flash_proc );
		kfree(lowlevel_hw[dev]->eeprom_path);
		kfree(lowlevel_hw[dev]->eeprom_name);
	}

return;
}

/* ------------------------------------------------------------------------ */
