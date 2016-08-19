/*
 *
 *
 */

#include <asm/cacheflush.h>
#include <linux/fdtable.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/list.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/nsproxy.h>
#include <linux/poll.h>
#include <linux/proc_fs.h>
#include <linux/rbtree.h>
#include <linux/sched.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>


typedef int (*yyprint)(const char*, ...);

static int binder_init(void)
{
	int ret;
	yyprint myp;

	//printk("I am yyko, printk at:%p\n", printk);		//c1561f96
	//printk("%x\n", *((unsigned int *)0xc179718c));	//__ksymtab_printk);
	myp = (yyprint)0xffffffff81644453;
	myp("A=%d\n", 123);
	ret =0;
	return ret;
}

static void binder_exit(void)
{
	printk("Exit yy\n");
}


module_init(binder_init);
module_exit(binder_exit);

MODULE_LICENSE("GPL v2");
