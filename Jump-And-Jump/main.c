#include <stc15.h>
#include <string.h>
#include <math.h>
#include "LCD1602.H"

#define     MAIN_Fosc       11059200UL      //������ʱ��

#define     PWM_DUTY        221184          //����PWM�����ڣ���ֵΪʱ��������������ʹ��11.0592MHZ����Ƶ��PWMƵ��Ϊ20HZ��

#define     PWM_HIGH_MIN    32              //����PWM�������Сռ�ձȡ��û������޸ġ�
#define     PWM_HIGH_MAX    (PWM_DUTY-PWM_HIGH_MIN) //����PWM��������ռ�ձȡ��û������޸ġ�

typedef     unsigned char   u8;
typedef     unsigned int    u16;
typedef     unsigned long   u32;

sbit    P_PWM = P3^5;       //����PWM������š�
sbit    key = P3^2;
u16     pwm;                //����PWM����ߵ�ƽ��ʱ��ı������û�����PWM�ı�����

u16     PWM_high,PWM_low;   //�м�������û������޸ġ�

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

void port_mode()            // �˿�ģʽ
{
	P0M1=0x00; P0M0=0x00;
	P1M1=0x00; P1M0=0x00;
	P2M1=0x00; P2M0=0x00;
	P3M1 &= ~(1 << 5);  P3M0 |=  (1 << 5); //P3.5 ����Ϊ�������
	P4M1=0x00; P4M0=0x00;
	P5M1=0x00; P5M0=0x00;
	P6M1=0x00; P6M0=0x00;
	P7M1=0x00; P7M0=0x00; 
}

void main()
{
	/************���/AD�ĵı���************/
	char a[11];  
	char a1[11];  
	char a2[11];  
	unsigned char adc_status; 
    long temp_buf1=0;
	long temp_buf2=0;
	long T1=0;
    long T2=0;
    long time=0;
	
	/************���������ı���************/
    unsigned  char  length;  
	unsigned  char  ecc  = 0;          
    unsigned  int i=0; 
	unsigned char ta[11];
	               
	char text[] = {"\0"};
	
	/************LCD�ı���************/
	unsigned char xPos,yPos;		 // X���ꡢY����
	unsigned char *s="Step=";
	unsigned char DispBuf[9];            // ���4��������ASCII��
	unsigned long TestDat=0;     // ��ʱ����
	
	/************�ǲ�����step************/
    unsigned long step = 0;
	
	
	port_mode();	      // IO�ڹ���ģʽ����
	/************ LCD��ʼ�� ***********/
	delay_ms(100);                    // �ȴ�LCD1602�ϵ�ʱ�ڲ���λ
	LCD1602_Init();
	SetCur(CurFlash);     // �������ʾ����˸��NoCur��������ʾ�޹��,NoDisp��������ʾ��	
                          // CurNoFlash�����й�굫����˸	��CurFlash�����й������˸
	xPos=0;	              // xPos��ʾˮƽ�����ַ���(0-15)
	yPos=1;	              // yPos��ʾ��ֱ�����ַ���(0-1)
	WriteString(0,0,"JUMP-AND-JUMP");	  // X���ꡢY���ꡢ�ַ�������Ļ���Ͻ�Ϊ����ԭ�㣬ˮƽ��0-15����ֱ��0-1
	WriteString(xPos,yPos,s);	  // X���ꡢY���ꡢ�ַ�������Ļ���Ͻ�Ϊ����ԭ��	
    xPos=5;	               
	yPos=1;	
	Long_Str(TestDat,DispBuf);			  // ͬ��4��8��"���ô��ڵ��Գ���"
	WriteString(xPos,yPos,DispBuf);	
	
	/************ PWM/���ڳ�ʼ�� ***********/
	UartInit();	
	UartInit3();	
	length = strlen(text); 
    P_PWM = 0;
	
    TR0 = 0;        //ֹͣ����
    ET0 = 1;        //�����ж�
    PT0 = 1;        //�����ȼ��ж�
    TMOD &= ~0x03;  //����ģʽ,0: 16λ�Զ���װ
    AUXR |=  0x80;  //1T
    TMOD &= ~0x04;  //��ʱ
    INT_CLKO |=  0x01;  //���ʱ��

    TH0 = 0;
    TL0 = 0;
    TR0 = 1;    //��ʼ����

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
				ADC_CONTR=0x80;              //��AD��Դ
				delay(10);
				ADC_CONTR=0xe0;              //����ת���ٶȺ�ͨ��1
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
				ADC_CONTR=0x80;				//��AD��Դ
				delay(10);
				ADC_CONTR=0xe1;				//����ת���ٶȺ�ͨ��2
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
				LoadPWM(pwm); 			//�������
				delay_ms(time);
				delay_ms(5);
				pwm = PWM_DUTY / 25;	
				LoadPWM(pwm);  		    //���̧��
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
// ����: void  delay_ms(unsigned char ms)
// ����: ��ʱ������
// ����: ms,Ҫ��ʱ��ms��, ����ֻ֧��1~255ms. �Զ���Ӧ��ʱ��.
// ����: none.
// �汾: VER1.0
// ����: 2013-4-1
// ��ע: 
//========================================================================

void delay_ms(unsigned int  ms)     
{
	unsigned int i;  
    do{  
        i=MAIN_Fosc/9600;  
        while(--i);     //96T per loop  
    }while(--ms);       //--ms  ms=ms-1  
}  
/**************** ����PWM��װֵ���� *******************/
void    LoadPWM(u16 i)
{
    u16 j;

    if(i > PWM_HIGH_MAX)        i = PWM_HIGH_MAX;   //���д��������ռ�ձ����ݣ���ǿ��Ϊ���ռ�ձȡ�
    if(i < PWM_HIGH_MIN)        i = PWM_HIGH_MIN;   //���д��С����Сռ�ձ����ݣ���ǿ��Ϊ��Сռ�ձȡ�
    j = 65536UL - PWM_DUTY + i; //����PWM�͵�ƽʱ��
    i = 65536UL - i;            //����PWM�ߵ�ƽʱ��
    EA = 0;
    PWM_high = i;   //װ��PWM�ߵ�ƽʱ��
    PWM_low  = j;   //װ��PWM�͵�ƽʱ��
    EA = 1;
}

/********************* Timer0�жϺ���************************/
void timer0_int (void) interrupt 1
{
    if(P_PWM)
    {
        TH0 = (u8)(PWM_low >> 8);   //���������ߵ�ƽ����װ�ص͵�ƽʱ�䡣
        TL0 = (u8)PWM_low;
    }
    else
    {
        TH0 = (u8)(PWM_high >> 8);  //���������͵�ƽ����װ�ظߵ�ƽʱ�䡣
        TL0 = (u8)PWM_high;
    }
}

