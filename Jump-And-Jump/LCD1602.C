
#include "LCD1602.H"
/****************��51��Ƭ����������-����STC15W4Kϵ�С��������� *************
 �������������������������
 ��51��Ƭ����������-����STC15W4Kϵ�С� һ���Ѿ��ɱ�����������ʽ���淢�С�
  �������ִ�������̲����׵�51˫��ʵ���(2��MCU)�Գ������ء����ԡ����淽�㣬����Ҫ�ⲿ
  ��������������������Ʒ�ʽ���׽����ϵͳ�ж��������ȼ�˭Ҳ������˭���жϾ������⡣
  �Ա����ַ��https://shop117387413.taobao.com
  QQȺ��STC51-STM32(3) ��515624099 �� STC51-STM32(2)��99794374��
        ��֤��Ϣ��STC15��Ƭ��
  ���䣺xgliyouquan@126.com
  �������������������������*/

void delay2uS ()  // 22.1184MHz
{
    unsigned char t=9;
	while(--t);
}

//	������д����֮ǰ���LCD������״̬
//  ��״̬ʱ��RS=0��RW=1��E=1���ж�æ��Ϻ��ͷ�����
void WaitIdle()
{
	unsigned char tmp;
	RS=0;			  // ����
	RW=1;			  // ��ȡ
	DPORT=0xff;		  // Ϊ����������׼��
	_nop_();          // ������ʱ
	E=1; 	          // ʹ��LCD1602 
	delay2uS();		  // LCD1602��EΪ�ߵ�ƽ����������ݵ��˿�
	for(;;)
	{
		tmp=DPORT;	   // �����ݶ˿��ϵ�ֵ����tmp
		tmp&=0x80;	   // ���λΪ1ʱ��ʾҺ��ģ����æ�����ܶ�����в���
		if(	tmp==0)	   // ����6λ��ʾ�ڲ���ǰ��ʾ��ַ����ʵ����;��
			break;
	}
	E=0;			   // �ͷ�����
} 
	
// ��LCD1602Һ��д��һ�ֽ����ݣ�dat-��д������ֵ 
// д����ʱ��RS=1��RW=0��D7����D0 = ���ݣ�E=�����壬Һ���������½��ز������ݡ�
void LcdWriteDat(unsigned char dat)
{
    WaitIdle();			// �ȴ�LCD1602����
	RS=1;				// ����
	RW=0;				// д
	DPORT=dat;			// ����д�����͵����ݶ˿�
	_nop_();            // ������ʱ
	E=1;				// ʹ��LCD1602
	delay2uS();			// LCD1602��EΪ�ߵ�ƽ�����ȡ���ݶ˿��ϵ�ֵ
	E=0;			    // �ر�LCD1602ʹ��,�ͷ�����	   
}

// ��LCD1602Һ��д��һ�ֽ����cmd-��д������ֵ 
// д����ʱ��RS=0��RW=0��D7����D0 = ���ݣ�E=�����壬Һ���������½��ز������ݡ�
void LcdWriteCmd(unsigned char cmd)
{
    WaitIdle();			// �ȴ�LCD1602����
	RS=0;				// ����
	RW=0;				// д
	DPORT=cmd;			// ����������������ݶ˿���
	_nop_();            // ������ʱ
	E=1;				// ʹ��LCD1602
	delay2uS();			// LCD1602��EΪ�ߵ�ƽ�����ȡ���ݶ˿��ϵ�ֵ
	E=0; 				// �ر�LCD1602ʹ��,�ͷ�����
}

// ������������ʾ���ݣ���1602�ڲ�RAMȫ������հ׵�ASCII��20H
//			 ����λ������곷�ص���Ļ���Ͻǵ�����ԭ��
//           ��1602�ڲ���ʾ��ַ��Ϊ0  
void ClrLcd()	      
{	
	LcdWriteCmd(0x01);
} 

// �ڲ���������������ʾ�ַ���ʼ����
void LcdPos(unsigned char xPos,unsigned char yPos)	 
{
	unsigned char tmp;
	xPos&=0x0f;		      // xλ�÷�Χ��0~15
	yPos&=0x01;		      // yλ�÷�Χ��0~1
	if(yPos==0)	   	      // ��ʾ��һ��
		tmp=xPos;		  // ��һ���ַ���ַ�� 0x00 ��ʼ
	else
		tmp=xPos+0x40;	  // �ڶ����ַ���ַ�� 0x40 ��ʼ
	tmp|=0x80;			  // ���� RAM ��ַ
	LcdWriteCmd(tmp);
}

void SetCur(unsigned char Para)	    // ���ù��
{
	switch(Para)
	{	
		case 0:
		{	
			LcdWriteCmd(0x08);	break;     	 // ����ʾ
		}
		case 1:
		{	
			LcdWriteCmd(0x0c);	break;	     // ����ʾ���޹��
		}
		case 2:
		{
			LcdWriteCmd(0x0e);	break;	     // ����ʾ�й�굫����˸
		}
		case 3:
		{
			LcdWriteCmd(0x0f);	break;	     // ����ʾ�й������˸
		}
		default:
			break;
	}
}

//��ָ������������ʾָ�����ַ���xpos:�У�ypos:�У�c:����ʾ�ַ�
void WriteChar(unsigned char xPos,unsigned char yPos,unsigned char Dat)
{
	LcdPos(xPos,yPos);
	LcdWriteDat(Dat);
}

// ��Һ������ʾ�ַ�����xpos:�����꣬ypos:�����꣬str-�ַ���ָ��
void WriteString(unsigned char xPos,unsigned char yPos,unsigned char *s)
{	
	unsigned char i=0;
	LcdPos(xPos,yPos);		  // ��ʼ����
	while(s[i])
	{
		LcdWriteDat(s[i]);
		i++;
		if (i>=16)  break;	  // ����16���ַ�������ݶ���
	}
}

// LCD 1602��ʼ��
void LCD1602_Init()	    
{
	LcdWriteCmd(0x38);	     // ��ʾģʽ���ã�����16��2��ʾ��5��7����8λ���ݽӿ�
	LcdWriteCmd(0x08);	     // ��ʾ�رգ�����ʾ��ꡢ��겻��˸
	LcdWriteCmd(0x01);	     // ��ʾ����
	LcdWriteCmd(0x06);	     // ��ʾ����ƶ�λ��
	LcdWriteCmd(0x0c);	     // ��ʾ�����������
}