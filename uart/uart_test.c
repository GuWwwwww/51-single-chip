#include<reg51.h>

void delay(unsigned char z)
{
	unsigned char i,j;
	for (i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void uart_init()
{
	TMOD=0x20; //��ʱ��1 ������ʽ2
	TR1=1;
	TH1=0xfa;
	TL1=0xfa; //���ò�����Ϊ4800
	SCON=0x50;//���ô��ڹ�����ʽΪ1
	
	EA=1;
	ES=1;
}

void uart_send(unsigned char dat)
{
	SBUF=dat;
	while(TI==0);//�ȴ��������
	TI=0;//Ϊ��һ�η�����׼��
}

void main()
{
	uart_init();
	while(1)
	{
		uart_send(0x80);
		delay(250);
		delay(250);
		delay(250);
		delay(250);
	}
}

void uart_re() interrupt 4
{
	P0=SBUF;
	RI=0;
}