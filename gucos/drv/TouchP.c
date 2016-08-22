/***************************************************************
 * file: TouchP.c
 * project: smart phone
 * description: Touch panel driver.
 **************************************************************/
/* pins assigned:
		touch panel		---		SC6600 CPU 
	------------------------------------------------------------
			PC0			---		GPI07(pin28)
			PC1			---		GPI08(pin29)
			PC2			---		GPIO2(pin21)
			PC3			---		GPIO6(pin27)
			A/D			---		AD2,AD3
			INT			---		GPIO35/GPIO17
*/

#include "bios1700.h"
#include "arm_reg.h"
#include "timer.h"
#include "adc.h"
#include "typedef.h"
#include "lcd.h"
#include "pen.h"

#define X_BEGIN	1
#define X_END	2
#define Y_BEGIN	3
#define Y_END	0

#define TP_CTRL_INIT			0
#define TP_CTRL_SAMPLING_X		1
#define TP_CTRL_SAMPLING_Y		2
#define TP_CTRL_RELEASE			3

#define TP_PEN_STATE_DOWN		0
#define TP_PEN_STATE_UP			1

#define BIT_PC0					BIT_7
#define BIT_PC1					BIT_8
#define BIT_PC2					BIT_2
#define BIT_PC3					BIT_6

void PenSaveStatus(void);
void PenLoadStatus(void);

extern void delay10us(uint  microsecond);
extern void OsTPpointProc(u_16 X, u_16 Y, u_8 status);
extern void BiosActivateHISR(u_8 hisr);
extern char IsTpIsr(void);
extern void tp_timer_isr_handler(void);
extern void SetGPIO(char GPIO, char enable);
extern void KeyOpenIsr(void);
extern void KeyCloseIsr(void);
extern void BiosSleepEnable(uint8 enable);
extern void BiosSleepDisable(uint8 enable);

static u_8 sample_step;
static u_8 tp_pen_state;	//0,pen down; 1,pen up(from OS)
static u_16 touchX,touchY;
static u_32 TP_INT_GPIO_BIT,TP_INT_GPIO_BASE;
void TpgpioIsrEnable(char enable);
static char TpLevelIsLow(void);
static void KeyOpenIsrA(void);
static long tp_up_tick_count =0;
static long tp_pressed = 0;
static unsigned long key_opened=0;

extern U16_T gp_ratio[4];
extern RECT_T gp_lcd_range;
extern uint8 gp_flip_x,gp_flip_y;

#define KEY_OPEN_DELAY_COUNT	6
#define KEY_OPENED_FLAG			0xffaa55

//开关TP中断
static void TpInterruptControl(char enable)
{
	if(enable)//笔抬起
	{
		tp_pressed = 0;
		TpgpioIsrEnable(1);
		SetGPIO(32,1);
		if(key_opened!=KEY_OPENED_FLAG)KeyOpenIsr();
	}
	else//笔按下
	{
		BiosSleepDisable(SLEEP_TP_CTL_BIT);

		TpgpioIsrEnable(0);
		KeyCloseIsr();
		SetGPIO(32,0);
		tp_pressed = 1;
		key_opened = 0;
	}
}
static void KeyOpenIsrA(void)
{	
	if(key_opened==KEY_OPENED_FLAG)return;
	if(key_opened<KEY_OPEN_DELAY_COUNT)
		key_opened ++;
	else
	{
		KeyOpenIsr();
		key_opened = KEY_OPENED_FLAG;
	}
}

char IsTpIsr(void)
{
	if(tp_pressed) return FALSE;
	if(TpLevelIsLow()) return TRUE;
	return FALSE;
}

/***********************************************************
 * get touch panel down-status
 * Input:	void
 * Output:	void
***********************************************************/
static char TpLevelIsLow(void)
{
	volatile gpio_ctl_s *ptrGpio;

	delay10us(10);
	
	ptrGpio = (volatile gpio_ctl_s *)TP_INT_GPIO_BASE;
	if(ptrGpio->data & TP_INT_GPIO_BIT)
		return 0;  //笔已经抬起
	else
		return 1;  //笔按下
}
/***********************************************************
 * control touch panel gpio interrupt
 * Input:	void
 * Output:	void
***********************************************************/
void TpgpioIsrEnable(char enable)
{
	volatile gpio_ctl_s *ptrGpio;
	
	ptrGpio = (volatile gpio_ctl_s *)TP_INT_GPIO_BASE;
	ptrGpio->ic |= (TP_INT_GPIO_BIT);
	
	if(enable)
		ptrGpio->ie |= (TP_INT_GPIO_BIT);
	else
		ptrGpio->ie &= ~(TP_INT_GPIO_BIT);
}

/***********************************************************
 * Touch panel control pin output
 * Input:	void
 * Output:	void
 在Sleep Thread被调用
***********************************************************/
void TouchPanelControl(u_8 type)
{
	volatile gpio_ctl_s *ptrGpio;

	ptrGpio = (volatile gpio_ctl_s *)GPIO_PG0_BASE;
	
	switch(type)
	{
		case TP_CTRL_INIT:			//PC0,PC2,PC3=1; PC1=0;
			ptrGpio->data |= (BIT_PC0|BIT_PC2|BIT_PC3);
			ptrGpio->data &= ~(BIT_PC1);
			delay10us(40);
			break;
		case TP_CTRL_SAMPLING_X:	//PC0,PC3=0; PC1,PC2=1;
			ptrGpio->data &= ~(BIT_PC0|BIT_PC3);
			ptrGpio->data |= (BIT_PC1|BIT_PC2);
			delay10us(10);
			break;
		case TP_CTRL_SAMPLING_Y:	//PC0,PC3=0; PC1,PC2=1;
			ptrGpio->data |= (BIT_PC0|BIT_PC3);
			ptrGpio->data &= ~(BIT_PC1|BIT_PC2);
			delay10us(10);
			break;
		case TP_CTRL_RELEASE:			
			ptrGpio->data |= (BIT_PC0|BIT_PC2);
			ptrGpio->data &= ~(BIT_PC1);
			ptrGpio->data &= ~(BIT_PC3);
			delay10us(40);
			break;
			
		default:	//not defined.
			break;
	}
}

/***********************************************************
 * Init touch panel 
 * Input:	void
 * Output:	void
***********************************************************/
void TouchPanelInit(void)
{
	volatile gpio_ctl_s *ptrGpio;
	volatile uint32 *ptr;
	
    // Disable IRQ before setup GPIO.
    *(volatile uint32 *)(INT_IRQ_EN) &= ~(INTCTL_GPIO_IRQ);
    
	ptr = (volatile uint32 *)CPC_GPIO_RF;
	*ptr |= 0x000C;		// GPIO2

	ptr = (volatile uint32 *)CPC_GPIO_PD;
	*ptr |= 0x000C;		// GPIO6-8

	ptrGpio = (volatile gpio_ctl_s *)GPIO_PG0_BASE;
	ptrGpio->dir |= 0x01C4;		// GPI02,6-8: output R/W
	ptrGpio->dmsk |= 0x01C4;

#if TP_INT_PORT==9
	ptr = (volatile uint32 *)CPC_GPIO_PD;
	*ptr |= BIT_4|BIT_5;		// select pin as GPIO9

	TP_INT_GPIO_BIT = BIT_GPIO09;
	TP_INT_GPIO_BASE = GPIO_PG0_BASE;

#elif TP_INT_PORT==35
	ptr = (volatile uint32 *)CPC_GPIO_ZD;
	*ptr |= 0x00C0;		//select pin as GPIO35 function
	
	TP_INT_GPIO_BIT = BIT_GPIO35;
	TP_INT_GPIO_BASE = GPIO_PG2_BASE;

#elif TP_INT_PORT==17
	ptr = (volatile uint32 *)CPC_GPIO_UART_KEY;
	*ptr |= 0x0c00;		//select pin as GPIO17 function
	
	TP_INT_GPIO_BIT = BIT_GPIO17;
	TP_INT_GPIO_BASE = GPIO_PG1_BASE;

#endif
	ptrGpio = (volatile gpio_ctl_s *)TP_INT_GPIO_BASE;
	ptrGpio->dir &= ~(TP_INT_GPIO_BIT);
	ptrGpio->dmsk |= TP_INT_GPIO_BIT;

    // enable GPIO17 IRQ.
	ptrGpio->ic |= TP_INT_GPIO_BIT;		//avoid RIS leave behind
	ptrGpio->ie &= ~TP_INT_GPIO_BIT;	//falling edge interrupt(all is default)

	TouchPanelControl(TP_CTRL_INIT);//TP is ready

	//打开能触发TP的中断(GPIO或KEY)
	TpInterruptControl(1);

	*(volatile uint32 *)(INT_IRQ_EN) |= (INTCTL_GPIO_IRQ);

	tp_pen_state = TP_PEN_STATE_UP;
	sample_step	= Y_END;

}

/***********************************************************
 * ISR for touch panel 
 * Input:	void
 * Output:	void
***********************************************************/
void tp_isr_handler(void)
{
	if(tp_pressed)
{
		
		//关闭TP中断
		TpInterruptControl(0);	
		return;
	}
		
	//关闭TP中断
	TpInterruptControl(0);	

	tp_timer_isr_handler();//响应更及时
	
	BiosOpenTimer(BIOS_TIMER_TP);

}

/***********************************************************
 * Sampling position to OS
 * Input:	void
 * Output:	void
***********************************************************/
void TPPositionToOS(void)
{
	OsTPpointProc(touchX, touchY, tp_pen_state);
}
/***********************************************************
 * ISR for touch panel A/D sampling result
 * Input:	void
 * Output:	void
***********************************************************/
void adc_tp_sample_x(void)
{
#define _SAMPLE_X	4
	static uint32 sumx=0,countx=0;
	
	sumx += AD_Result();
	
	countx++;
	if(countx<_SAMPLE_X)
	{
		AD_Open(ADC_TP_X);
		return;
	}
	touchX = sumx / _SAMPLE_X;
	sumx = 0;
	countx = 0;

	//	sample_step = X_END;//sampling...

	sample_step = Y_BEGIN;//sampling...
	TouchPanelControl(TP_CTRL_SAMPLING_Y);
	AD_Open(ADC_TP_Y);
}

/***********************************************************
 * ISR for touch panel A/D sampling result
 * Input:	void
 * Output:	void
***********************************************************/
void adc_tp_sample_y(void)
{
#define _SAMPLE_Y	2
	static uint32 sumy=0,county=0;
	
	sumy += AD_Result();
	
	county++;
	if(county<_SAMPLE_Y)
	{
		AD_Open(ADC_TP_Y);
		return;
	}
	
	touchY = sumy / _SAMPLE_Y;
	sumy = 0;
	county = 0;

	TouchPanelControl(TP_CTRL_INIT);

	if(!TpLevelIsLow())//笔已经抬起
	{
		//关闭timer;
		//tp_timer_opened = 0;
		//BiosCloseTimer(BIOS_TIMER_TP);
		tp_pen_state = TP_PEN_STATE_UP;
		BiosActivateHISR(HISR_TYPE_TP);

		//打开TP中断,等待下次笔点
		TpInterruptControl(1);
	}
	else
	{
		//释放TouchPannel,省电
		//TouchPanelControl(TP_CTRL_RELEASE);
			
		tp_pen_state = TP_PEN_STATE_DOWN;
		BiosActivateHISR(HISR_TYPE_TP);
	}

	sample_step = Y_END;
}

/***********************************************************
 * Check pen up in timer.
 * Input:	void
 * Output:	void
***********************************************************/
void tp_timer_isr_handler(void)
{

	//FIX:笔已经抬起后两秒终内不能进入睡眠
	if(!tp_pressed)
	{
		tp_up_tick_count++;
		//if(tp_up_tick_count>100)//1s
		//if(tp_up_tick_count>=0)//0s
		{
			BiosCloseTimer(BIOS_TIMER_TP);
			BiosSleepEnable(SLEEP_TP_CTL_BIT);
			tp_up_tick_count = 0;
		}
		return;
	}

	//FIX:打开KEY中断(解决笔按下时不能响应KEY的问题)
	KeyOpenIsrA();

	//一次采样结束
	if(sample_step==Y_END)
	{
		if(AD_IsBusy())return;

		sample_step = X_BEGIN;//sampling...
		TouchPanelControl(TP_CTRL_SAMPLING_X);
		AD_Open(ADC_TP_X);
		return;
	}

/*	if(sample_step==X_END)
	{
		if(AD_IsBusy())return;

		sample_step = Y_BEGIN;//sampling...
		TouchPanelControl(TP_CTRL_SAMPLING_Y);
		AD_Open(ADC_TP_Y);
		return;
	}
*/
}


/*关机时，保存笔点校验的全局变量*/
void PenSaveStatus(void)
{
	nSysStsRam.n_ratio[0] = gp_ratio[0];
	nSysStsRam.n_ratio[1] = gp_ratio[1];
	nSysStsRam.n_ratio[2] = gp_ratio[2];
	nSysStsRam.n_ratio[3] = gp_ratio[3];
	nSysStsRam.n_lcd_range.left = gp_lcd_range.x1;
	nSysStsRam.n_lcd_range.top = gp_lcd_range.y1;
	nSysStsRam.n_lcd_range.right = gp_lcd_range.x2;
	nSysStsRam.n_lcd_range.bottom = gp_lcd_range.y2;
	nSysStsRam.gp_flip_x = gp_flip_x;
	nSysStsRam.gp_flip_y = gp_flip_y;
#if 0
	TRACE("save n_ratio[]=0x%x,0x%x,0x%x,0x%x\n",nSysStsRam.n_ratio[0],nSysStsRam.n_ratio[1],nSysStsRam.n_ratio[2],nSysStsRam.n_ratio[3]);
	TRACE("save gp_lcd_range[]=0x%x,0x%x,0x%x,0x%x\n",gp_lcd_range.x1,gp_lcd_range.y1,gp_lcd_range.x2,gp_lcd_range.y2);
	TRACE("save gp_flip_x=0x%x,gp_flip_y=0x%x\n",gp_flip_x,gp_flip_y);
#endif
}

/*开机时，加载笔点校验的全局变量*/
void PenLoadStatus(void)
{
	gp_ratio[0] = nSysStsRam.n_ratio[0];
	gp_ratio[1] = nSysStsRam.n_ratio[1];
	gp_ratio[2] = nSysStsRam.n_ratio[2];
	gp_ratio[3] = nSysStsRam.n_ratio[3];
	gp_lcd_range.x1 = nSysStsRam.n_lcd_range.left;
	gp_lcd_range.y1 = nSysStsRam.n_lcd_range.top;
	gp_lcd_range.x2 = nSysStsRam.n_lcd_range.right;
	gp_lcd_range.y2 = nSysStsRam.n_lcd_range.bottom;
	gp_flip_x = nSysStsRam.gp_flip_x;
	gp_flip_y = nSysStsRam.gp_flip_y;
#if 0
	TRACE("load n_ratio[]=0x%x,0x%x,0x%x,0x%x\n",nSysStsRam.n_ratio[0],nSysStsRam.n_ratio[1],nSysStsRam.n_ratio[2],nSysStsRam.n_ratio[3]);
	TRACE("load gp_lcd_range[]=0x%x,0x%x,0x%x,0x%x\n",gp_lcd_range.x1,gp_lcd_range.y1,gp_lcd_range.x2,gp_lcd_range.y2);
	TRACE("load gp_flip_x=0x%x,gp_flip_y=0x%x\n",gp_flip_x,gp_flip_y);
#endif
}


/*========================TEST SAMPLE===========================*/
#ifdef _DEBUG_BIOS
extern void OsTPpointProcA(u_16 x, u_16 y, u_8 status);
void OsTPpointProc(u_16 x, u_16 y, u_8 status)	//status:0,pen down; 1,pen up.
{
	static u_16 x0=0,y0=0;
	//if(status)
//	m_trace4("TP: x=0x%x y=0x%x delta_x=%03d  delta_y= %03d\r\n",(int)x,(int)y,(int)x-(int)x0,(int)y-(int)y0);
//	x0 = x;
//	y0 = y;
	OsTPpointProcA(x,y,status);
}
#endif
