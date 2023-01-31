#include <stc15.h>
#include <string.h>
#include <math.h>
#include "LCD1602.H"

#define     MAIN_Fosc       11059200UL      //定义主时钟

#define     PWM_DUTY        221184          //定义PWM的周期，数值为时钟周期数，假如使用11.0592MHZ的主频，PWM频率为20HZ。

#define     PWM_HIGH_MIN    32              //限制PWM输出的最小占空比。用户请勿修改。
#define     PWM_HIGH_MAX    (PWM_DUTY-PWM_HIGH_MIN) //限制PWM输出的最大占空比。用户请勿修改。

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

sbit    P_PWM = P3^5;       //定义PWM输出引脚。
sbit    key = P3^2;
u16     pwm;                //定义PWM输出高电平的时间的变量。用户操作PWM的变量。

u16     PWM_high,PWM_low;   //中间变量，用户请勿修改。

void delay_ms(unsigned int  ms);
void delay(unsigned long mycnt);

void LoadPWM(u16 i);
void PWM_Init(void);

void UartInit(void);
void UartInit3(void);

void UART_Send_Byte(unsigned char dat);
void UART_Send_Buff(const char* src);

void  voice(char  *text);

unsigned char Long_Str(long dat,unsigned char *str);

void port_mode()            // 端口模式
{
	P0M1=0x00; P0M0=0x00;
	P1M1=0x00; P1M0=0x00;
	P2M1=0x00; P2M0=0x00;
	P3M1 &= ~(1 << 5);  P3M0 |=  (1 << 5); //P3.5 设置为推挽输出
	P4M1=0x00; P4M0=0x00;
	P5M1=0x00; P5M0=0x00;
	P6M1=0x00; P6M0=0x00;
	P7M1=0x00; P7M0=0x00; 
}

void main()
{
	/************舵机/AD的的变量************/
	char a[11];  
	char a1[11];  
	char a2[11];  
	unsigned char adc_status; 
    long temp_buf1=0;
	long temp_buf2=0;
	long T1=0;
    long T2=0;
    long time=0;
	
	/************语音函数的变量************/
    unsigned  char  length;  
	unsigned  char  ecc  = 0;          
    unsigned  int i=0; 
	unsigned char ta[11];
	               
	char text[] = {"\0"};
	
	/************LCD的变量************/
	unsigned char xPos,yPos;		 // X坐标、Y坐标
	unsigned char *s="Step=";
	unsigned char DispBuf[9];            // 存放4个待发送ASCII码
	unsigned long TestDat=0;     // 临时变量
	
	/************记步变量step************/
    unsigned long step = 0;
	
	
	port_mode();	      // IO口工作模式设置
	/************ LCD初始化 ***********/
	delay_ms(100);                    // 等待LCD1602上电时内部复位
	LCD1602_Init();
	SetCur(CurFlash);     // 开光标显示、闪烁，NoCur——有显示无光标,NoDisp——无显示，	
                          // CurNoFlash——有光标但不闪烁	，CurFlash——有光标且闪烁
	xPos=0;	              // xPos表示水平右移字符数(0-15)
	yPos=1;	              // yPos表示垂直下移字符数(0-1)
	WriteString(0,0,"JUMP-AND-JUMP");	  // X坐标、Y坐标、字符串，屏幕左上角为坐标原点，水平：0-15，垂直：0-1
	WriteString(xPos,yPos,s);	  // X坐标、Y坐标、字符串，屏幕左上角为坐标原点	
    xPos=5;	               
	yPos=1;	
	Long_Str(TestDat,DispBuf);			  // 同第4章8节"利用串口调试程序"
	WriteString(xPos,yPos,DispBuf);	
	
	/************ PWM/串口初始化 ***********/
	UartInit();	
	UartInit3();	
	length = strlen(text); 
    P_PWM = 0;
	
    TR0 = 0;        //停止计数
    ET0 = 1;        //允许中断
    PT0 = 1;        //高优先级中断
    TMOD &= ~0x03;  //工作模式,0: 16位自动重装
    AUXR |=  0x80;  //1T
    TMOD &= ~0x04;  //定时
    INT_CLKO |=  0x01;  //输出时钟

    TH0 = 0;
    TL0 = 0;
    TR0 = 1;    //开始运行

    EA = 1;
	pwm = PWM_DUTY / 25;
	LoadPWM(pwm);
	key = 1; 

/************************************************/	

	while(1)
	{
		if(!key)
		{
			delay(20);
			if(!key)
			{
				
				P1ASF=0x01;
				ADC_CONTR=0x80;              //打开AD电源
				delay(10);
				ADC_CONTR=0xe0;              //设置转换速度和通道1
				CLK_DIV|=0x20;
				ADC_CONTR|=0x08;
				adc_status=0;
				while(adc_status==0)
				{
					adc_status=ADC_CONTR&0x10;
				}
				ADC_CONTR=ADC_CONTR&0xe7;
				temp_buf1=(ADC_RES<<8)+ADC_RESL;
				delay(10);
				
				
				
				P1ASF=0x02;
				ADC_CONTR=0x80;				//打开AD电源
				delay(10);
				ADC_CONTR=0xe1;				//设置转换速度和通道2
				CLK_DIV|=0x20;
				ADC_CONTR|=0x08;
				adc_status=0;
				while(adc_status==0)
				{
					adc_status=ADC_CONTR&0x10;
				}
				ADC_CONTR=ADC_CONTR&0xe7;
				temp_buf2=(ADC_RES<<8)+ADC_RESL;
				delay(10);

				time =temp_buf1-temp_buf2;
				if(time>0)
				{
					time = 1.8*time;
				}
				if(time<0)
				{
					time = -time;
					time = 2.2*time;
				}
				Long_Str(time,a);
				Long_Str(temp_buf1,a1);
				Long_Str(temp_buf2,a2);
				
				UART_Send_Buff(a1);
				UART_Send_Buff(".");
				UART_Send_Buff(a2);
				UART_Send_Buff(".");
				UART_Send_Buff(a);
				UART_Send_Buff(".");
				
				pwm = PWM_DUTY / 35;    
				LoadPWM(pwm); 			//舵机按下
				delay_ms(time);
				delay_ms(5);
				pwm = PWM_DUTY / 25;	
				LoadPWM(pwm);  		    //舵机抬起
				delay(20);
				
				xPos=5;	               
				yPos=1;	
				step++;
				TestDat = step;
				Long_Str(TestDat,DispBuf);	
				WriteString(xPos,yPos,DispBuf);	
				
				Long_Str(step,ta);
				for(i=0;i<11;i++)
				{
					text[i]=ta[i];
				}
				voice(text);

				while(!key);
			}
		}
	}
}

void delay(unsigned long mycnt)
{
	while(mycnt--);
}

//========================================================================
// 函数: void  delay_ms(unsigned char ms)
// 描述: 延时函数。
// 参数: ms,要延时的ms数, 这里只支持1~255ms. 自动适应主时钟.
// 返回: none.
// 版本: VER1.0
// 日期: 2013-4-1
// 备注: 
//========================================================================

void delay_ms(unsigned int  ms)     
{
	unsigned int i;  
    do{  
        i=MAIN_Fosc/9600;  
        while(--i);     //96T per loop  
    }while(--ms);       //--ms  ms=ms-1  
}  
/**************** 计算PWM重装值函数 *******************/
void    LoadPWM(u16 i)
{
    u16 j;

    if(i > PWM_HIGH_MAX)        i = PWM_HIGH_MAX;   //如果写入大于最大占空比数据，则强制为最大占空比。
    if(i < PWM_HIGH_MIN)        i = PWM_HIGH_MIN;   //如果写入小于最小占空比数据，则强制为最小占空比。
    j = 65536UL - PWM_DUTY + i; //计算PWM低电平时间
    i = 65536UL - i;            //计算PWM高电平时间
    EA = 0;
    PWM_high = i;   //装载PWM高电平时间
    PWM_low  = j;   //装载PWM低电平时间
    EA = 1;
}

/********************* Timer0中断函数************************/
void timer0_int (void) interrupt 1
{
    if(P_PWM)
    {
        TH0 = (u8)(PWM_low >> 8);   //如果是输出高电平，则装载低电平时间。
        TL0 = (u8)PWM_low;
    }
    else
    {
        TH0 = (u8)(PWM_high >> 8);  //如果是输出低电平，则装载高电平时间。
        TL0 = (u8)PWM_high;
    }
}

