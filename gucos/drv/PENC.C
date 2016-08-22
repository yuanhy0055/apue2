#include "base.h"
#include "nucleus.h"
#include "drv_head.h"
#include "pen.h"
#include "queuemng.h"

#define MAX_DELTA_X              120
#define MAX_DELTA_Y              120
#define MAX_DELTA_DELTA_X        60
#define MAX_DELTA_DELTA_Y        60

#define MIN_PENMOVE_X    	 4
#define MIN_PENMOVE_Y    	 4

#define FILTER_POINT		1
#define PEN_NO_MERGE

static U32_T	gp_sample_count;
static S16_T	gp_down_x,gp_down_y;
static S16_T	gp_move_x,gp_move_y;
static S16_T	gp_lastpannel_x,gp_lastpannel_y;
static U16_T	gp_lastdelta_x,gp_lastdelta_y;
static U8_T		gp_down_flag;
U8_T gp_flip_x=0,gp_flip_y=0;
static U8_T		gp_up_flag;
static U8_T		gp_work_mode;

U16_T	gp_ratio[4] = {0xffff,0xffff,0xffff,0xffff};
RECT_T	gp_lcd_range;
static PENMSG_T	gp_msg_buf[PEN_MSG_MAX];
static U32_T		gp_msghead;
static U32_T		gp_msgtail;
static U32_T	keep_count=0;

//local
U16_T	PenGetMsgType(void);
U8_T	PenPointMoveValid(S16_T x,S16_T y);
U16_T	ABS(S16_T x,S16_T y);
U8_T	PointInRange(S16_T x,S16_T y,RECT_T *rect);
U8_T	PenPointFilter(S16_T x,S16_T y);
void	PenPannel2LcdPoint(S16_T x,S16_T y,S16_T *lcd_x,S16_T *lcd_y);
void	PenInsertMessage(PENMSG_T *msg);
void	PenClearMessage(void);
U32_T	PenNextMessage(U32_T msg_no);
U32_T	PenPreMessage(U32_T msg_no);
static char m_tracebuf[200];
extern void output_msg(char *p);

extern void SavePowerVars(void);

/*****************************************************************
功能：检查笔点校验的合法性
输入：没有
返回：没有
******************************************************************/
BOOL GetPenCorrectStatus(void)
{
	if(gp_ratio[0] == 0xffff)	return FALSE;
	if(gp_ratio[1] == 0xffff)	return FALSE;
	if(gp_ratio[2] == 0xffff)	return FALSE;
	if(gp_ratio[3] == 0xffff)	return FALSE;
	return TRUE;
}

/*****************************************************************
功能：笔点类的初始化
输入：没有
返回：没有
******************************************************************/
void PenInitial(void)
{
	gp_sample_count =0;
	gp_down_flag = 0;
	gp_up_flag = 0;
	PenClearMessage();
	PenSetWorkMode(PEN_MODE_NORMAL);
}

/*****************************************************************
功能：设置笔点类的工作模式
输入：没有
返回：没有
******************************************************************/
void PenSetWorkMode(U8_T mod)
{
	gp_work_mode = mod;
}

/*****************************************************************
功能：获得笔点消息接口
输入：笔点消息指针
返回：TRUE -- 有笔点消息
	  FALSE--没有笔点消息
说明：供application调用
******************************************************************/
U32_T PenGetMessage(PENMSG_T *msg)
{
	if(gp_msgtail==gp_msghead) return FALSE;

	memcpy((U8_T*)msg,(U8_T*)(&gp_msg_buf[gp_msghead]),sizeof(PENMSG_T));

	gp_msghead = PenNextMessage(gp_msghead);

//	TRACE("got pen message, kind %d ,head %d ,tail %d \n",msg->name,gp_msghead,gp_msgtail);
	
	return TRUE;
}

//next pen msg
U32_T PenNextMessage(U32_T msg_no)
{
	if(msg_no+1>=PEN_MSG_MAX)
		return (U32_T)0;

	return (U32_T)(msg_no+1);
}

//previous pen msg
U32_T PenPreMessage(U32_T msg_no)
{
	if(msg_no)
		return (U32_T)(msg_no-1);

	return (U32_T)(PEN_MSG_MAX-1);
}
/*****************************************************************
功能：设置笔点校验数据的程序接口
输入：笔点坐标对应范围
返回：TRUE --校验数据设置成功
      FALSE --校验数据不正确
说明：由笔点校验函数调用
******************************************************************/
U8_T PenSetCollateData(RECT_T *lcd_rect,RECT_T *pannel_rect)
{
	S16_T pnl_x,pnl_y;
	S16_T lcd_x,lcd_y;

	if(!(gp_ratio[0] = ABS(pannel_rect->x1,pannel_rect->x2)))return FALSE;
	if(!(gp_ratio[1] = ABS(pannel_rect->y1,pannel_rect->y2)))return FALSE;
	if(!(gp_ratio[2] = ABS(lcd_rect->x1,lcd_rect->x2)))return FALSE;
	if(!(gp_ratio[3] = ABS(lcd_rect->y1,lcd_rect->y2)))return FALSE;

	pnl_x = (pannel_rect->x1+pannel_rect->x2)/2;
	pnl_y = (pannel_rect->y1+pannel_rect->y2)/2;
	lcd_x = (lcd_rect->x1+lcd_rect->x2)/2;
	lcd_y = (lcd_rect->y1+lcd_rect->y2)/2;
	
	gp_flip_x=0;gp_flip_y=0;
	if(pannel_rect->x1>pannel_rect->x2) gp_flip_x=1;
	if(pannel_rect->y1>pannel_rect->y2) gp_flip_y=1;
#if 0
	m_trace1("\r\n====================================",gp_flip_y);
	m_trace1("\r\npannel range x1=0x%x ",(unsigned int)pannel_rect->x1);
	m_trace1("\r\npannel range y1=0x%x ",(unsigned int)pannel_rect->y1);
	m_trace1("\r\npannel range x2=0x%x ",(unsigned int)pannel_rect->x2);
	m_trace1("\r\npannel range y2=0x%x ",(unsigned int)pannel_rect->y2);
	m_trace1("\r\nflip x =0x%x ",gp_flip_x);
	m_trace1("\r\nflip y =0x%x ",gp_flip_y);
	m_trace1("\r\n====================================\r\n",gp_flip_y);
#endif
	gp_lcd_range.x1 = pnl_x - lcd_x * gp_ratio[0] / gp_ratio[2];
	gp_lcd_range.x2 = pnl_x + lcd_x * gp_ratio[0] / gp_ratio[2];
	gp_lcd_range.y1 = pnl_y - lcd_y * gp_ratio[1] / gp_ratio[3];
	gp_lcd_range.y2 = pnl_y + lcd_y * gp_ratio[1] / gp_ratio[3];

	return TRUE;

}


// pen interrupt main process
/*****************************************************************
从Touch Pannel 取得原始笔点数据后，要进行下列处理：
	1、笔点数据的滤波处理
	2．笔点数据变换成LCD座标
	3．笔点信息变换成笔点消息或标签键消息
	4、如果处于笔校验模式下，直接发送原始数据
******************************************************************/
void OsTPpointProc(S16_T x,S16_T y,U8_T up_flag)
{
	PENMSG_T msg;
	U8_T ret;
	
	if(up_flag)
	{
		gp_up_flag = TRUE;	// pen up
		gp_sample_count = 0;
	}
	else
	{
		gp_up_flag = FALSE;
		ret = PenPointFilter(x,y);
		gp_lastpannel_x = x;
		gp_lastpannel_y = y;
		if (!ret)
		{
			keep_count = 0;
			return;
		}

		if(!PenPointMoveValid(x,y))return;
	}
	keep_count =0;
	msg.name = PenGetMsgType();

	switch ( msg.name)
	{
	case PENDOWN:
		gp_down_x = x;
		gp_down_y = y;
	case PENMOVE:
		gp_move_x = x;
		gp_move_y = y;
		break;
	case PENUP:
		x = gp_move_x;
		y = gp_move_y;
		break;
	default:
		return;
	}
	msg.x = x;
	msg.y = y;

	switch (gp_work_mode)
	{
	case PEN_MODE_SETCORE:
		PenInsertMessage(&msg);
		break;

	case PEN_MODE_NORMAL:
	case PEN_MODE_COMPARE:
		PenPannel2LcdPoint(x,y,&(msg.x),&(msg.y));
		PenInsertMessage(&msg);
		break;
	default :
		break;
	}

}

/*获取PenDown时实际的TouchPanel点*/
void PenGetRawPoint(S16_T *x,S16_T *y)
{
	*x = gp_down_x;
	*y = gp_down_y;
}

/*获取PenDown时实际的TouchPanel点*/
void PenDownGetRawPoint(S16_T *x,S16_T *y)
{
	*x = gp_down_x;
	*y = gp_down_y;
}
/*获取PenMove时实际的TouchPanel点*/
void PenMoveGetRawPoint(S16_T *x,S16_T *y)
{
	*x = gp_move_x;
	*y = gp_move_y;
}

/*****************************************************************
功能：判断笔点消息类型
输入：没有
返回：笔点消息类型
******************************************************************/
U16_T PenGetMsgType(void)
{

	if(gp_down_flag)
	{
		if(!gp_up_flag) return PENMOVE;
		gp_down_flag =0;
		return PENUP;
	}

	if(gp_up_flag) return PENNULL;

	gp_down_flag =1;
	return PENDOWN;
}


/*****************************************************************
功能：判断笔点move消息的有效性
输入：没有
返回：笔点消息类型
******************************************************************/
U8_T PenPointMoveValid(S16_T x,S16_T y)
{
#if !FILTER_POINT
	return TRUE;
#endif 
	if(!gp_down_flag) return TRUE;

	if(ABS(x,gp_move_x) > MIN_PENMOVE_X) return TRUE;
	if(ABS(y,gp_move_y) > MIN_PENMOVE_Y) return TRUE;
	keep_count++;
	if(keep_count>12)	return TRUE;	//send move msg percent 120ms 

	return FALSE;

}
//求绝对值
U16_T ABS(S16_T x,S16_T y)
{
	if(x>y) return x-y;
	return y-x;
}

/*****************************************************************
功能：判断笔点是否在预定的范围内
输入：x,y-----笔点坐标
	  rect--- 预定范围
返回：TRUE or FALSE
******************************************************************/
U8_T PointInRange(S16_T x,S16_T y,RECT_T *rect)
{
	if(x<rect->x1) return FALSE;
	if(x>rect->x2) return FALSE;
	if(y<rect->y1) return FALSE;
	if(y>rect->y2) return FALSE;
	
	SavePowerVars();	// 把year，month，day保存到注册表中
	
	return TRUE;
}

/*****************************************************************
功能：滤除掉原始笔点数据中离散点
输入：笔点坐标
返回：TRUE --有效值
      FALSE --无效
******************************************************************/
U8_T PenPointFilter(S16_T x,S16_T y)
{
	U16_T delta_x,delta_y;
	U16_T delta_delta_x,delta_delta_y;
#if !FILTER_POINT
	return TRUE;
#endif 

	if(gp_sample_count==0)
	{
		gp_sample_count++;
		return FALSE;
	}

	if(gp_sample_count ==1)
	{
		gp_sample_count++;
		gp_lastdelta_x = ABS(x,gp_lastpannel_x);
		if(gp_lastdelta_x > MAX_DELTA_X)
		{
		gp_sample_count=0;
		return FALSE;
		}
		gp_lastdelta_y = ABS(y,gp_lastpannel_y);
		if(gp_lastdelta_y > MAX_DELTA_Y)
		{
		gp_sample_count=0;
		return FALSE;
		}
		return FALSE;
	}
	gp_sample_count++;

	delta_x = ABS(x,gp_lastpannel_x);
	if(delta_x > MAX_DELTA_X)
	{
		gp_sample_count=0;
		return FALSE;
	}
	delta_delta_x = ABS(delta_x,gp_lastdelta_x);
	if(delta_delta_x > MAX_DELTA_DELTA_X)
	{
		gp_sample_count=0;
		return FALSE;
	}
	gp_lastdelta_x = delta_x;

	delta_y = ABS(y,gp_lastpannel_y);
	if(delta_y > MAX_DELTA_Y)
	{
		gp_sample_count=0;
		return FALSE;
	}
	delta_delta_y = ABS(delta_y,gp_lastdelta_y);
	if(delta_delta_y > MAX_DELTA_DELTA_Y)
	{
		gp_sample_count=0;
		return FALSE;
	}
	gp_lastdelta_y = delta_y;

	return TRUE;

}


/*****************************************************************
功能：笔点坐标转化成LCD坐标
输入：笔点坐标
返回： LCD坐标
******************************************************************/
void PenPannel2LcdPoint(S16_T x,S16_T y,S16_T *lcd_x,S16_T *lcd_y)
{
	if(gp_flip_x)	
		*lcd_x = (gp_lcd_range.x2-x)*gp_ratio[2]/gp_ratio[0];
	else
		*lcd_x = (x-gp_lcd_range.x1)*gp_ratio[2]/gp_ratio[0];

	if(gp_flip_y)
		*lcd_y = (gp_lcd_range.y2-y)*gp_ratio[3]/gp_ratio[1];
	else
		*lcd_y = (y-gp_lcd_range.y1)*gp_ratio[3]/gp_ratio[1];

}


/*****************************************************************
功能：把笔点消息队列清空
输入：没有
返回：没有
******************************************************************/
void PenClearMessage(void)
{
	gp_msghead = 0;
	gp_msgtail = 0;

}
U32_T PenMessageIsFull(void)
{
	if(gp_msgtail<gp_msghead)
	{
		if((gp_msghead-gp_msgtail)<10) return 1;
		return 0;
	}
	else
	{
		if((PEN_MSG_MAX+gp_msghead-gp_msgtail)<10) return 1;
		return 0;
	}
}

/*****************************************************************
功能：把笔点消息放进队列
输入：笔点消息
返回：没有
******************************************************************/
void PenInsertMessage(PENMSG_T *msg)
{

	U32_T new_tail= gp_msgtail,i;
	U32_T CombineFlag = 0;

#ifdef PEN_NO_MERGE
	if(PenMessageIsFull())
	{
		switch(msg->name)
		{
		case PENMOVE:
			return ;
			//gp_msgtail = PenPreMessage(gp_msgtail);
			break;
		case PENDOWN:
			//gp_msghead = PenNextMessage(gp_msghead);
			break;
		case PENUP:
			//gp_msghead = PenNextMessage(gp_msghead);
			break;
		}
	}
	goto pen_com_insert;	
#endif

	if(gp_msgtail==gp_msghead)
	{
		goto pen_com_insert;
	}

	i = PenPreMessage(gp_msgtail);

	if(msg->name==PENDOWN)
	{
		while(i!=gp_msghead)
		{
		if(gp_msg_buf[i].name == PENDOWN)
		{
			 new_tail = i;
			 break;
		}
		i=PenPreMessage(i);
		}
	}
	else if(msg->name ==PENMOVE)
	{
		if(gp_msg_buf[i].name == PENMOVE)
		{
			new_tail = i;
			CombineFlag = 1;
		}
	}
pen_com_insert:
	memcpy((U8_T*)(&gp_msg_buf[new_tail]),(U8_T*)msg,sizeof(PENMSG_T));
	gp_msgtail = PenNextMessage(new_tail);

//	TRACE("send pen message, kind %d ,head %d ,tail %d \n",msg->name,gp_msghead,gp_msgtail);

	if(msg->name == PENUP)
	{
		DrvMsgSemaphInc();
	}

	if(!CombineFlag)
	{
		DrvMsgSemaphInc();
	}

	
	return;
}


