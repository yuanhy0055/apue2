/*
 * Copyright (c) 1999 Greg Haerr <greg@censoft.com>
 *
 * DOS Mouse Driver, uses int 33h
 * Added fix for DJGPP 32 bit compilation
 */

 /* Pen.c mou_win.c is the driver codes, and pencore.c touchp.c is the codes from bios */

#include "device.h"
#include "windows.h"

//////  Device Status BIT //////////////
#define DEV_LIGHT_CLOSE	0x01	// for Backlight Status
#define DEV_LCD_SLEEP	0x02	// for LcdSleep Status
#define DEV_SCREEN_LOCK	0x04	// for ScreenLock Status

#define	SCALE	1	/* default scaling factor for acceleration WAS 3*/
#define	THRESH	0	/* default threshhold for acceleration WAS 0*/

static int  	MOU_Open(MOUSEDEVICE *pmd);
static void 	MOU_Close(void);
static int  	MOU_GetButtonInfo(void);
static void	MOU_GetDefaultAccel(int *pscale,int *pthresh);
static int  	MOU_Read(short *dx, short *dy, short *dz,int *bp);
static int	MOU_Poll(void);

MOUSEDEVICE mousedev = {
	MOU_Open,
	MOU_Close,
	MOU_GetButtonInfo,
	MOU_GetDefaultAccel,
	MOU_Read,
	MOU_Poll
};

#ifdef WIN32
extern int MOUSE_Status(void);
extern short MOUSE_Read(short*,short*,int*);
#else

typedef struct tagPENMSG
{
		unsigned short	name;
		short	x;
		short	y;

		//for debug
//		long	sample_count;
//		long	msg_count;
}PENMSG_T;

extern unsigned int PenGetMessage(PENMSG_T *msg);

int MOUSE_Status(void)
{
	return 1;
}

short MOUSE_Read(short *a,short *b,int *c)
{
	static short x,y;
	static char MouseUpFlag = 0,MouseNoneFlag=1;
	static char bDeviceWakeupProc = 0;
	PENMSG_T msg;
	extern void PlayPenTone(PENMSG_T *pen);
	extern void Device_Wakeup(void);
	extern DWORD BiosGetDeviceStatus(void);
	
	if(MouseUpFlag)		// for second semaphore
	{
		*a = x;
		*b = y;
		*c = -1;  // left button
		MouseUpFlag = 0;
		MouseNoneFlag =1;
		return 0;
	}
	if(!PenGetMessage(&msg))	// no mouse key
	{
		*a = x;
		*b = y;
		if(MouseNoneFlag)
		{
			*c = -1;  // left button
			return 0;
		}
		else
		{
			if(bDeviceWakeupProc)
			{
				bDeviceWakeupProc = 0;
				*c = -1;  // left button
				return 0;
			}

			*c = 0;
			return 2;
		}
	}
	
//	TRACE("read pen message, type %d \r\n",msg.name);

	if(BiosGetDeviceStatus()&DEV_LCD_SLEEP)
	{
		bDeviceWakeupProc = 1;
	}
	
	Device_Wakeup();

	if(msg.name == 3)	// penup
	{
		if(bDeviceWakeupProc)
		{
			bDeviceWakeupProc = 0;
			*c = -1;
			return 0;
		}
	
		MouseNoneFlag =0;
			
		*a = x;
		*b = y;
		*c = -1;  // left button
		MouseUpFlag = 1;
		return 2;
	}

	if(bDeviceWakeupProc)
	{
		*c = -1;
		return 0;
	}

	MouseNoneFlag =0;

	PlayPenTone(&msg);
	
	x = *a = msg.x;
	y = *b = msg.y;
	*c = 0;  // left button
	return 2;
}
#endif


/*
 * Open up the mouse device.
 */
static int
MOU_Open(MOUSEDEVICE *pmd)
{
	return 1;
}

/*
 * Close the mouse device.
 */
static void
MOU_Close(void)
{
}

/*
 * Get mouse buttons supported
 */
static int
MOU_GetButtonInfo(void)
{
	return MWBUTTON_L | MWBUTTON_R;
}

/*
 * Get default mouse acceleration settings
 */
static void
MOU_GetDefaultAccel(int *pscale,int *pthresh)
{
	*pscale = SCALE;
	*pthresh = THRESH;
}

/*
 * Attempt to read bytes from the mouse and interpret them.
 * Returns -1 on error, 0 if either no bytes were read or not enough
 * was read for a complete state, or 1 if the new state was read.
 * When a new state is read, the current buttons and x and y deltas
 * are returned.  This routine does not block.
 */
static int ButtonKey=0;

static int
MOU_Read(short *dx, short *dy, short *dz, int *bp)
{
	*dz = 0;
	*bp = 0;
#ifdef WIN32
	if(MOUSE_Read(dx,dy,bp)==0)
		return 0;
#else
	MOUSE_Read(dx,dy,bp);
#endif
	if(*bp==0) *bp=MWBUTTON_L;
	else if(*bp==1) *bp=MWBUTTON_R;
	else if(*bp==2) *bp=MWBUTTON_M;
	else *bp = 0;
#ifdef WIN32
	ButtonKey = *bp;
#endif
	return 2;
}

static int
MOU_Poll(void)
{
	int status;
	status = MOUSE_Status();
#ifdef WIN32
	return 1;//status | ButtonKey;
#else
	return 1;
#endif
}

