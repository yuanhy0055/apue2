#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#define MOTION_PROC_DIR "motion"
#define MOTION_PROC_FILE "event"
#define SENSOR_DATA_LEN 12
static char sensor_data[SENSOR_DATA_LEN];
static struct input_dev *motion_mouse;
static struct input_dev *motion_keyboard;
static struct proc_dir_entry *proc_dir;
static ssize_t event_write(struct file *, const char __user *, size_t, loff_t *);
static ssize_t event_read(struct file *, char __user *, size_t, loff_t *);
static struct file_operations event_fops = {.write = event_write,.read = event_read};
//static short lastx = 640;
//static short lasty = 360;
static int motion_diff =10000;
static int sim_function = 3;
module_param(motion_diff, int, 0644);
module_param(sim_function, int, 0644);
struct InputEvent{
	char type;
	struct input_event event;
};
DEFINE_SEMAPHORE(event_sem);
DEFINE_SEMAPHORE(sensor_rw_sem);
static ssize_t event_write(struct file *f, const char __user *buffer, size_t size, loff_t *off)
{
	//int reInputEventport_key = 0;
	
	struct InputEvent cmd;

	down(&event_sem);
   //	printk(KERN_ERR "hello\n");
	
	if (copy_from_user(&cmd, buffer, sizeof(struct InputEvent)))
	{
		printk(KERN_ERR "copy_from_user error.\n");
		return -1;
	}
//	printk("event_write,type=%d,code=%d,value=%d\n", cmd.type, cmd.event.code, cmd.event.value);
	//return size;
	
	if (sim_function & 0x02) { 
		if(cmd.type == 1)//mouse
		{
			//printk("cmd->type == EV_REL size = %d\n", size);
			if(cmd.event.type == EV_REL)
			input_report_rel(motion_mouse, cmd.event.code, cmd.event.value);
			else if(cmd.event.type == EV_KEY)
			input_report_key(motion_mouse, cmd.event.code, cmd.event.value);
			//if(cmd.code == REL_Y)
			input_sync(motion_mouse);
		}else if(cmd.type == 2)//sensor
		{	
			down(&sensor_rw_sem);				
			memcpy(sensor_data, &(cmd.event), SENSOR_DATA_LEN);
			up(&sensor_rw_sem);
		}	
		else if(cmd.type == 0)//key
		{					
			//input_report_key(motion_keyboard, cmd.event.code, 1);
			input_report_key(motion_keyboard, cmd.event.code, cmd.event.value);
			input_sync(motion_keyboard);
		}
		
		
		
	}

	up(&event_sem);

	return size;
}
static ssize_t event_read(struct file *f, char __user *buffer, size_t size, loff_t *off)
{
	char temp[SENSOR_DATA_LEN];
	memcpy(temp, sensor_data, SENSOR_DATA_LEN);
	down(&sensor_rw_sem);				
	copy_to_user(buffer, temp, SENSOR_DATA_LEN);
	up(&sensor_rw_sem);
	return SENSOR_DATA_LEN;
}
static int __init motion_input_init(void)
{
	int err = 0;
	int key = 0;
	struct proc_dir_entry *proc_file;
//	struct proc_dir_entry *test_file;

	motion_mouse = input_allocate_device();
	if (!motion_mouse) {
		err = -ENOMEM;
		goto fail;
	}
	motion_keyboard = input_allocate_device();
	if (!motion_keyboard) {
		err = -ENOMEM;
		goto fail;
	}

	motion_keyboard->name = "sim-kb";
	motion_keyboard->id.vendor = 0x0019;
    motion_keyboard->id.product = 0x0001;
	motion_keyboard->evbit[0] = BIT_MASK(EV_KEY);
//	__set_bit(KEY_REPLY, motion_keyboard->keybit);
//	__set_bit(KEY_UP, motion_keyboard->keybit);
//	__set_bit(KEY_DOWN, motion_keyboard->keybit);
//	__set_bit(KEY_LEFT, motion_keyboard->keybit);
//	__set_bit(KEY_RIGHT, motion_keyboard->keybit);
//	__set_bit(KEY_ESC, motion_keyboard->keybit);
//	__set_bit(KEY_HOME, motion_keyboard->keybit);
//	__set_bit(KEY_POWER, motion_keyboard->keybit);
//	__set_bit(KEY_MENU, motion_keyboard->keybit);
	//change by zhangxiongbo
	for(key = KEY_ESC;key <= KEY_UNKNOWN; key++)
	__set_bit(key, motion_keyboard->keybit);
	//for(key = KEY_TCL_VGR_LEFT;key <= KEY_TCL_BLUE; key++)
	for(key = 681;key <= 703; key++)
	__set_bit(key, motion_keyboard->keybit);
/*
	for(key = KEY_TCL_VGR_LEFT;key <= KEY_TCL_BODY_SENSOR; key++)
	__set_bit(key, motion_keyboard->keybit);
	for(key = KEY_TCL_SOURCE;key <= KEY_TCL_BLUE; key++)
	__set_bit(key, motion_keyboard->keybit);
*/

	motion_keyboard->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);

	err = input_register_device(motion_keyboard);
	if (err) {
		dev_err(&motion_keyboard->dev, "failed to register input device simulate keyboard\n");
		goto fail;
	}

	motion_mouse->name = "sim-mouse";
	motion_mouse->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REL);
	motion_mouse->relbit[0] = BIT_MASK(REL_X) | BIT_MASK(REL_Y) | BIT_MASK(REL_WHEEL);
//	motion_mouse->keybit[BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT) |
//		BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);
	motion_mouse->keybit[BIT_WORD(BTN_MOUSE)] = BIT_MASK(BTN_LEFT) |
		BIT_MASK(BTN_MIDDLE) | BIT_MASK(BTN_RIGHT);


		
	err = input_register_device(motion_mouse);
	if (err) {
		dev_err(&motion_mouse->dev, "failed to register input device simulate mouse\n");
		goto mouse_fail;
	}

	proc_dir = proc_mkdir(MOTION_PROC_DIR, NULL);
	if (!proc_dir) {
		err = -ENOMEM;
		goto no_proc_dir;
	}
	proc_file = create_proc_entry(MOTION_PROC_FILE, 0666, proc_dir);
	if (!proc_file) {
		err = -ENOMEM;
		goto no_proc_file;
	}
	proc_file->proc_fops = &event_fops;

	printk("sim_function:%d\n", sim_function);
	//printk("motion_input_init\n");

	return 0;

no_proc_file:
	remove_proc_entry(MOTION_PROC_DIR, NULL);
no_proc_dir:
	input_unregister_device(motion_mouse);
	input_unregister_device(motion_keyboard);
mouse_fail:
	input_unregister_device(motion_keyboard);
fail:
	return err;

}

static void __exit motion_input_exit(void)
{
	if (proc_dir) {
		remove_proc_entry(MOTION_PROC_FILE, proc_dir);
		remove_proc_entry(MOTION_PROC_DIR, NULL);
	}

	if (motion_mouse) {
		input_unregister_device(motion_mouse);
		input_free_device(motion_mouse);
	}
	if (motion_keyboard) {
		input_unregister_device(motion_keyboard);
		input_free_device(motion_keyboard);
	}

}
module_init(motion_input_init);
module_exit(motion_input_exit);
MODULE_AUTHOR("Jackson Liao <liaojinshun@tcl.com>");
MODULE_DESCRIPTION("TCL Simulate Input Driver");
