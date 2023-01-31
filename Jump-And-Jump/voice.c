
#include <stc15.h>        
#include <string.h> 
 
void  voice(char  *text) 
{    
/****************需要发送的文本**********************************/ 
         unsigned  char  headOfFrame[5];
         unsigned  char  length;  
         unsigned  char  ecc  = 0;              //定义校验字节
         unsigned  int i=0; 
         length = strlen(text);                 //需要发送文本的长度
 
/****************串口的初始化************************************/ 

	/*com2串口的初始化
	S2CON = 0x50;	
	AUXR |= 0x04;	
	T2L = 0xE0;		
	T2H = 0xFE;		
	AUXR |= 0x10;		

/*	//com1
	SCON = 0x50;        //8位数据,可变波特率
    AUXR |= 0x40;        //定时器1时钟为Fosc,即1T
    AUXR &= 0xFE;        //串口1选择定时器1为波特率发生器
    TMOD &= 0x0F;        //设定定时器1为16位自动重装方式
    TL1 = 0xE0;        //设定定时初值
    TH1 = 0xFE;        //设定定时初值
    ET1 = 0;           //禁止定时器1中断
    TR1 = 1;           //启动定时器1
*/
/*****************发送过程**************************************/           
         headOfFrame[0] = 0xFD ;             //构造帧头FD
         headOfFrame[1] = 0x00 ;             //构造数据区长度的高字节
         headOfFrame[2] = length + 3;         //构造数据区长度的低字节
         headOfFrame[3] = 0x01 ;             //构造命令字：合成播放命令
/*****************带背景音乐**************************************/           
         headOfFrame[4] = 0x21 ;             //构造命令参数：编码格式为GBK

/*****************不带背景音乐**************************************/           
        //headOfFrame[4] = 0x01 ;             //构造命令参数：编码格式为GBK

         for(i = 0; i<5; i++)                   //依次发送构造好的5个帧头字节
         {  
             ecc=ecc^(headOfFrame[i]);         //对发送的字节进行异或校验    
             S3BUF = headOfFrame[i]; 
             while (!(S3CON & 0x02)) {;}               //等待发送中断标志位置位
             S3CON &= 0xFD;                        //发送中断标志位清零
         }

            for(i = 0; i<length; i++)           //依次发送待合成的文本数据
         {  
             ecc=ecc^(text[i]);                 //对发送的字节进行异或校验    
             S3BUF = text[i]; 
             while (!(S3CON & 0x02)) {;} 
             S3CON &= 0xFD;                
         }

           S3BUF=ecc;                            //最后发送校验字节
           while (!(S3CON & 0x02)) {;} 
		   S3CON &= 0xFD;   
        
}
