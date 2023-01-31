
#include "LCD1602.H"
/****************《51单片机轻松入门-基于STC15W4K系列》配套例程 *************
 ★★★★★★★★★★★★★★★★★★★★★★★★
 《51单片机轻松入门-基于STC15W4K系列》 一书已经由北航出版社正式出版发行。
  作者亲手创作的与教材配套的51双核实验板(2个MCU)对程序下载、调试、仿真方便，不需要外部
  仿真器与编程器，这种设计方式彻底解决了系统中多个最高优先级谁也不能让谁的中断竞争问题。
  淘宝店地址：https://shop117387413.taobao.com
  QQ群：STC51-STM32(3) ：515624099 或 STC51-STM32(2)：99794374。
        验证信息：STC15单片机
  邮箱：xgliyouquan@126.com
  ★★★★★★★★★★★★★★★★★★★★★★★★*/

void delay2uS ()  // 22.1184MHz
{
    unsigned char t=9;
	while(--t);
}

//	正常读写操作之前检测LCD控制器状态
//  读状态时序：RS=0，RW=1，E=1，判断忙完毕后释放总线
void WaitIdle()
{
	unsigned char tmp;
	RS=0;			  // 命令
	RW=1;			  // 读取
	DPORT=0xff;		  // 为接收数据作准备
	_nop_();          // 短暂延时
	E=1; 	          // 使能LCD1602 
	delay2uS();		  // LCD1602在E为高电平区间输出数据到端口
	for(;;)
	{
		tmp=DPORT;	   // 将数据端口上的值赋给tmp
		tmp&=0x80;	   // 最高位为1时表示液晶模块正忙，不能对其进行操作
		if(	tmp==0)	   // 其余6位表示内部当前显示地址，无实际用途。
			break;
	}
	E=0;			   // 释放总线
} 
	
// 向LCD1602液晶写入一字节数据，dat-待写入数据值 
// 写数据时序：RS=1，RW=0，D7——D0 = 数据，E=正脉冲，液晶在脉冲下降沿采样数据。
void LcdWriteDat(unsigned char dat)
{
    WaitIdle();			// 等待LCD1602空闲
	RS=1;				// 数据
	RW=0;				// 写
	DPORT=dat;			// 将待写数据送到数据端口
	_nop_();            // 短暂延时
	E=1;				// 使能LCD1602
	delay2uS();			// LCD1602在E为高电平区间读取数据端口上的值
	E=0;			    // 关闭LCD1602使能,释放总线	   
}

// 向LCD1602液晶写入一字节命令，cmd-待写入命令值 
// 写命令时序：RS=0，RW=0，D7——D0 = 数据，E=正脉冲，液晶在脉冲下降沿采样数据。
void LcdWriteCmd(unsigned char cmd)
{
    WaitIdle();			// 等待LCD1602空闲
	RS=0;				// 命令
	RW=0;				// 写
	DPORT=cmd;			// 将命令码输出在数据端口上
	_nop_();            // 短暂延时
	E=1;				// 使能LCD1602
	delay2uS();			// LCD1602在E为高电平区间读取数据端口上的值
	E=0; 				// 关闭LCD1602使能,释放总线
}

// 清屏命令：清除显示内容，将1602内部RAM全部填入空白的ASCII码20H
//			 光标归位，将光标撤回到屏幕左上角的坐标原点
//           将1602内部显示地址设为0  
void ClrLcd()	      
{	
	LcdWriteCmd(0x01);
} 

// 内部函数用于设置显示字符起始坐标
void LcdPos(unsigned char xPos,unsigned char yPos)	 
{
	unsigned char tmp;
	xPos&=0x0f;		      // x位置范围是0~15
	yPos&=0x01;		      // y位置范围是0~1
	if(yPos==0)	   	      // 显示第一行
		tmp=xPos;		  // 第一行字符地址从 0x00 开始
	else
		tmp=xPos+0x40;	  // 第二行字符地址从 0x40 开始
	tmp|=0x80;			  // 设置 RAM 地址
	LcdWriteCmd(tmp);
}

void SetCur(unsigned char Para)	    // 设置光标
{
	switch(Para)
	{	
		case 0:
		{	
			LcdWriteCmd(0x08);	break;     	 // 关显示
		}
		case 1:
		{	
			LcdWriteCmd(0x0c);	break;	     // 开显示但无光标
		}
		case 2:
		{
			LcdWriteCmd(0x0e);	break;	     // 开显示有光标但不闪烁
		}
		case 3:
		{
			LcdWriteCmd(0x0f);	break;	     // 开显示有光标且闪烁
		}
		default:
			break;
	}
}

//在指定的行与列显示指定的字符，xpos:行，ypos:列，c:待显示字符
void WriteChar(unsigned char xPos,unsigned char yPos,unsigned char Dat)
{
	LcdPos(xPos,yPos);
	LcdWriteDat(Dat);
}

// 在液晶上显示字符串，xpos:行坐标，ypos:列坐标，str-字符串指针
void WriteString(unsigned char xPos,unsigned char yPos,unsigned char *s)
{	
	unsigned char i=0;
	LcdPos(xPos,yPos);		  // 起始坐标
	while(s[i])
	{
		LcdWriteDat(s[i]);
		i++;
		if (i>=16)  break;	  // 超出16个字符外的数据丢弃
	}
}

// LCD 1602初始化
void LCD1602_Init()	    
{
	LcdWriteCmd(0x38);	     // 显示模式设置，设置16×2显示，5×7点阵，8位数据接口
	LcdWriteCmd(0x08);	     // 显示关闭，不显示光标、光标不闪烁
	LcdWriteCmd(0x01);	     // 显示清屏
	LcdWriteCmd(0x06);	     // 显示光标移动位置
	LcdWriteCmd(0x0c);	     // 显示开及光标设置
}