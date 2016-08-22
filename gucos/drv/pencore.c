#include "bios1700.h"
#include "lcd.h"
#include "pen.h"

//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
// pen correct

//#define _PEN_DEBUG

#define  COR_X1     20
#define  COR_Y1     20
#define  COR_X2     (LCD_X_MAX-20)
#define  COR_Y2     (LCD_Y_MAX-20)
#define  COR_X3     (LCD_X_MAX/2)
#define  COR_Y3     (LCD_Y_MAX/2)

const unsigned short pen_hint_pos[5][2] =
{
        {COR_X1,COR_Y1},

        {COR_X2,COR_Y1},
        {COR_X1,COR_Y2},

        {COR_X2,COR_Y2},
        {COR_X3,COR_Y3}
};
const unsigned short pen_final_range[4] = {COR_X3-10,COR_Y3-10, COR_X3+10, COR_Y3+10};
void DrawLine(short x1,short y1,short x2,short y2);

void SetRectA(rect* rc,short x1,short y1,short x2,short y2)
{
	rc->x1 = x1;
	rc->y1 = y1;
	rc->x2 = x2;
	rc->y2 = y2;
}
extern void m_trace1(char *str,long value);
#define trace m_trace1

static short lcd_g_buf[LCD_X_MAX*LCD_Y_MAX];
void PutupCross(short x, short y)
{
	short i;
	
	LCDClearScreen(0, 0x001f);	
	for(i = 0; i < 20; i++)
	{
		LCDDrawPixel(x - 10 + i, y, 0xF800);
	}
	
	for(i = 0; i < 20; i++)
	{
		LCDDrawPixel(x, y - 10 + i, 0xF800);
	}

}


/****************************************************************
功能：笔点校验程序入口
输入：没有
返回：没有
*****************************************************************/
void PenCorrect(void)
{
	unsigned short	x[6],y[6];
	unsigned char	step=1;
	PENMSG_T	msg;
	rect	lcd_rect,pannel_rect;
	
	OpenLcd(0);
	PenInitial();

#ifdef _PEN_DEBUG	

	SetRectA(&lcd_rect,0,0,LCD_X_MAX,LCD_Y_MAX);
	memset(lcd_g_buf,0,LCD_X_MAX*LCD_Y_MAX*2);
	RefreshLCD(0,&lcd_rect,lcd_g_buf);
	delay(200);
	memset(lcd_g_buf,0xf800,LCD_X_MAX*LCD_Y_MAX*2);
	RefreshLCD(0,&lcd_rect,lcd_g_buf);
	delay(200);
	memset(lcd_g_buf,0x1f,LCD_X_MAX*LCD_Y_MAX*2);
	RefreshLCD(0,&lcd_rect,lcd_g_buf);
	delay(200);
	memset(lcd_g_buf,0x7e0,LCD_X_MAX*LCD_Y_MAX*2);
	RefreshLCD(0,&lcd_rect,lcd_g_buf);
	delay(200);
	
	SetRectA(&pannel_rect,0xa0,0xbf,0x2fe,0x350);
	SetRectA(&lcd_rect,COR_X1,COR_Y1,COR_X2,COR_Y2);
	PenSetCollateData(&lcd_rect,&pannel_rect);
	step = 0;
#endif
	while(step)
	{
		PutupCross(pen_hint_pos[step-1][0], pen_hint_pos[step-1][1]);
		//DisplayStr("test tp",0,0);

		PenSetWorkMode(PEN_MODE_SETCORE);
		if(step==5) PenSetWorkMode(PEN_MODE_COMPARE);

		while(!(PenGetMessage(&msg)&&(msg.name == PENUP)));

		x[step-1] = msg.x;
		y[step-1] = msg.y;
		step++;
//		m_trace1("x=0x%x  ",msg.x);	
//		m_trace1("y=0x%x\n",msg.y);	

		if((step==2)||(step==3))step=4; //for 3 points

		if(step<5)continue;

		if(step==5)
		{
		//SetRectA(&pannel_rect,(x[0]+x[2])/2,(y[0]+y[1])/2,(x[1]+x[3])/2,(y[2]+y[3])/2);

		SetRectA(&pannel_rect,x[0],y[0],x[3],y[3]);

		SetRectA(&lcd_rect,COR_X1,COR_Y1,COR_X2,COR_Y2);

		if(!PenSetCollateData(&lcd_rect,&pannel_rect))step = 1;
		continue;
		}
		if(step ==6)
		{
		SetRectA(&lcd_rect,pen_final_range[0],pen_final_range[1],pen_final_range[2],pen_final_range[3]);

		if(PointInRange(msg.x,msg.y,&lcd_rect))break;
		step =1;
		}
	}
}

char PenTest(void)
{
	extern const RECT fail_rect;
	extern const RECT pass_rect;
	PENMSG_T	msg;
	short x,y;

	// pen correct pass, test now...
	LCDClearScreen(0, 0x01f);	
	
	for(x=10;x<=118;x++)
	for(y=10;y<=120;y++)
		LCDDrawPixel(x,y,0xffff);
	
	PutString("[FAIL]",fail_rect.left,fail_rect.top,1);
	PutString("[PASS]",pass_rect.left,pass_rect.top,1);

	//PenSetWorkMode(PEN_MODE_SETCORE);
	PenSetWorkMode(PEN_MODE_NORMAL);
	PenClearMessage();
	while(1)
	{
		static short x,y;
		
		while(!PenGetMessage(&msg));
		switch(msg.name)
		{
			case PENDOWN:
				x = msg.x;
				y = msg.y;
				DrawLine(x,y,msg.x,msg.y);
				break;
			case PENMOVE:
				DrawLine(x,y,msg.x,msg.y);	//line
//				DrawLine(msg.x,msg.y,msg.x,msg.y);	//dot
				x = msg.x;
				y = msg.y;
				break;
			case PENUP:
				if(PointInRange(msg.x,msg.y,&fail_rect)) return 0;
				if(PointInRange(msg.x,msg.y,&pass_rect)) return 1;

				break;
		}
	}

}
void WaitPenDown(void)
{ 
	PENMSG_T msg;
	
	PenClearMessage();
	while(!PenGetMessage(&msg));
}

void LCDDrawPixel2(short x,short y, int color)
{
	if(x+3>LCD_MAIN_XSIZE)return;
	if(y+3>LCD_MAIN_YSIZE)return;
	LCDDrawPixel(x, y, color);
//	LCDDrawPixel(x+1, y+1, color);
//	LCDDrawPixel(x, y+1, color);
//	LCDDrawPixel(x+1, y, color);
	
}
void DrawLine(short x1,short y1,short x2,short y2)
{
	unsigned char bit,penSize;
	short i,j,x,y,px,py;
	short ex,ey,dx,dy,offset;

	bit = 0;
	x = y = 0;
	x1<=x2 ? (dx=x2-x1,ex=1) : (dx=x1-x2,ex=-1);
	y1<=y2 ? (dy=y2-y1,ey=1) : (dy=y1-y2,ey=-1);
	penSize = 1;
	{
		if(dx >= dy)
		{
			offset = dy-dx+dy;
			for(i=0;i<=dx;i++)
			{
				if(penSize>1)
				{
					py = y+y1-((penSize+1)>>1);
					for(j=0;j<penSize;j++)
						LCDDrawPixel2(x+x1,py+j,0xf800);
				}
				else if(1)
				{
						LCDDrawPixel2(x+x1,y+y1,0xf800);
					bit++;
				}

				if(offset >= 0)
				{
					offset += (dy-dx)<<1;
					y += ey;
				}
				else
					offset += dy<<1;
				x += ex;
			}
		}
		else /* dx < dy */
		{
			offset = dx-dy+dx;
			for(i=0;i<=dy;i++)
			{
				if(penSize>1)
				{
					px = x+x1-((penSize+1)>>1);
					for(j=0;j<penSize;j++)
						LCDDrawPixel2(px+j,y+y1,0xf800);
				}
				else if(1)
				{
						LCDDrawPixel2(x+x1,y+y1,0xf800);
					bit++;
				}

				if(offset >= 0)
				{
					offset += (dx-dy)<<1;
					x += ex;
				}
				else
					offset += dx<<1;
				y += ey;
			}
		}
	}

}
