#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

static FILE *fp = NULL;

struct tcl_timeval {
long a;
long b;
};

struct tcl_input_event {
	struct tcl_timeval time;
	short type;
	short code;
	int value;
};

struct InputEvent{
	char type;
	struct input_event event;
};

int open_proc_interface(void)
{
	if(fp)
		return 0;

	if ((fp = fopen("/proc/motion/event", "wb")) == NULL)
	{
		printf("Cannot open file /proc/motion/event....\n");
		return -1;
	}
	else 
	{
		printf("Open file /proc/motion/event success.\n");
		return 0;
	}	
}

int input_event_in(struct InputEvent * eventbuf)//char *eventbuf)
{
	int error;

	printf("event_in 1.\n");
	if(open_proc_interface() < 0)
	{
		printf("event_in 2.\n");
		return -1;
	}
	printf("event_in 3.\n");
	error = fwrite(eventbuf,sizeof(char),strlen(eventbuf),fp);
	printf("len:%d",error);
	return error;

}

int TCL_StrToInt(char* str)
{
      int num = 0;
	char*  digit = str;
	char i = 0;

      if(digit != NULL)
       {    
		while(*digit)
		{
			 if(*digit >= '0' && *digit <= '9')
				 num = num * 10 + (*digit - '0');
			else if(i > 0)
				goto next_step;
			digit ++;
			i++;
		}
	}
next_step:
	if(*str == '-')
		num *= -1;
	//printk_TCLinput("strtoint i:%d\n",i);
	return num;
}

int main(int argc, char **argv)
{

char posBuffer[20];   
int x, y, z, k;
int error, char_size ,str_size;

if(argc < 4)
return -1;
//printf("argc:%d\n",argc);
struct InputEvent cmd;


x = TCL_StrToInt(argv[1]);
y = TCL_StrToInt(argv[2]);
z = TCL_StrToInt(argv[3]);
k = TCL_StrToInt(argv[4]);
printf("x:%d,y:%d,z:%d,k:%d.\n",x,y,z,k);

//sprintf(posBuffer,"42%04d,%04d,0,0",x,y);  
cmd.type = TCL_StrToInt(argv[1]);
cmd.event.type = TCL_StrToInt(argv[2]);
cmd.event.code = TCL_StrToInt(argv[3]);
cmd.event.value = TCL_StrToInt(argv[4]);

if(fp)
		return 0;

	if ((fp = fopen("/proc/motion/event", "wb")) == NULL)
	{
		printf("Cannot open file /proc/motion/event....\n");
		return -1;
	}
	else 
	{
		printf("Open file /proc/motion/event success.\n");
		//return 0;
	}	

char_size = sizeof(char);
//str_size = strlen(&cmd);
str_size = sizeof(struct InputEvent);
printf("event_in 3:%d,strsize:%d.\n",char_size ,str_size);
//error = fwrite(&cmd,sizeof(char),strlen(&cmd),fp);
error = fwrite(&cmd, char_size, str_size, fp);
	printf("len:%d",error);
//error = input_event_in(&cmd);
//saveCoordinates(posBuffer);
//error = input_event_in(posBuffer);
/*
if(error < 0)
	printf("error in %d.\n",error);
else 
	printf("ok in %d.\n",error);
*/

return 0;
}


