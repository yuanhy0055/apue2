#ifndef PEN__H
#define PEN__H


#define LCD_X_MAX			176     // or 320 dot (lcd type)
#define LCD_Y_MAX			240     // or 240 dot

#define PENDOWN				1
#define PENMOVE				2
#define PENUP				3

#define PENNULL				0
#define PENLABEL			0

#define LABEL1				0x10
//...

#define PEN_MODE_NORMAL		0	//normal mode, send lcd point(x,y)
#define PEN_MODE_SETCORE	1	//set mode, send pannel point(x,y)
#define PEN_MODE_COMPARE	2	//verify mode,send lcd point(x,y)

#define PEN_MSG_MAX			500


typedef struct tagRECT
{
		S16_T	x1;
		S16_T	y1;
		S16_T	x2;
		S16_T	y2;
}RECT_T;

typedef struct tagLABELKEY
{
		U8_T	name;
		RECT_T	range;
}LABELKEY_T;

typedef struct tagPENMSG
{
		U16_T	name;
		S16_T	x;
		S16_T	y;

		//for debug
//		long	sample_count;
//		long	msg_count;
}PENMSG_T;

/*
//adc driver api
extern void adc_init(void);
extern char adc_is_busy(void);
extern void adc_enable_channel(char ChannelNo);

//pen driver api
extern void touch_init(void);
extern void touch_get_point(void);		// call by timer2
extern void touch_read_coordinate_x(int data);	// call by ADC
extern void touch_read_coordinate_y(int data);	// call by ADC
*/

//pen bios api
void OsTPpointProc(S16_T x,S16_T y,U8_T up_flag);

//pen user api
extern void PenInitial(void);
extern void PenSetWorkMode(U8_T mod);
extern U32_T PenGetMessage(PENMSG_T *msg);
extern U8_T PenSetCollateData(RECT_T *lcd_rect,RECT_T *pannel_rect);
extern void PenProcessPoint(S16_T x,S16_T y);
extern void PenCorrect(void);
extern void PenWait(void);

#endif

