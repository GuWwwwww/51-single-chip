#include<reg51.h>
#define N 8  //������ˮ�Ƶĸ���
#define v 100 //���������ٶ�
#define D 2  //������������
void delay_ms(unsigned char z);
void led_running(unsigned int d);

void main()
{
	while(1)
	{
		led_running(D);
	}
}

void led_running(unsigned int d)
{
	static char i=0;
	static int flag=0;
	
	if (d==0)  //��������Ϊ����
		P2=~(0x01<<i);
	else if (d==1)  //��������Ϊ����
		P2=~(0x80>>(8-N+i));
	else   //��������Ϊ��������
	{
		if (flag==0) 
			P2=~(0x01<<i);
		else
			P2=~(0x80>>(8-N+i));
		if (i==(N-1)) flag^=1;
	}
	
	delay_ms(v);
	i++;
	if (i==N) i=0;
}

void delay_ms(unsigned char z)
{
	//�ӳ�z���룬���ֻ�ܵ�255ms
	unsigned int i=0;
	unsigned char j=0;
	for(j=0;j<z;j++)
		{
			for(i=0;i<125;i++);
		}
}