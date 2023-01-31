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

/**************************************************
;连线图:
;DB0---DPROT.0  DB4---DPROT.4    RS-------------P1.3
;DB1---DPROT.1  DB5---DPROT.5    RW-------------P1.2
;DB2---DPROT.2  DB6---DPROT.6    E--------------P4.7
;DB3---DPROT.3  DB7---DPROT.7    
;STC15f2k6060S2的晶振频率为22.1184MHz 
;**************************************************/ 
#include "stc15.h"
#include "intrins.h"		 // _nop_()函数需要	 
sbit	RS	=	P4^1;		 // 根据实际硬件连接修改
sbit	RW	=	P4^2;		 // 根据实际硬件连接修改
sbit	E	=	P4^4;		 // 根据实际硬件连接修改
#define DPORT	P2			 // 根据实际硬件连接修改

#define NoDisp 0 		     // 无显示
#define NoCur 1 	 	     // 有显示无光标
#define CurNoFlash 2	     // 有光标但不闪烁
#define CurFlash 3     	     // 有光标且闪烁

void LCD1602_Init();		 // 初始化
void ClrLcd();           	 // 清屏命令
void SetCur(unsigned char Para);	//设置光标
void WriteChar(unsigned char xPos,unsigned char yPos,unsigned char Dat);  // 写1个字符
void WriteString(unsigned char xPos,unsigned char yPos,unsigned char *s); // 写字符串