#include<reg51.h>
#include<intrins.h>
#define N 16
sbit DS1=P2^2;
sbit rck1=P2^1;
sbit sck1=P2^0;
sbit DS2=P3^2;
sbit rck2=P3^1;
sbit sck2=P3^0;
unsigned char disp_code1[16]={0x00,0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x02,0x04,0x08,0x10,0x20,0x40,0x80,0x00};
unsigned char disp_code2[16]={0x00,0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x40,0x20,0x10,0x08,0x04,0x02,0x01,0x00};

void delay_ms(unsigned char z)
{
  unsigned int i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void write_74HC595_row(unsigned char Dat)
{
	unsigned char i=0;
	for(i=0;i<8;i++)
	{
		DS1 = (Dat >> 7);
	  Dat <<= 1;
		sck1=0;
		_nop_();
		_nop_();
		sck1=1;
	}
	rck1=0;
	_nop_();
	_nop_();
	rck1=1;
}

void write_74HC595_column(unsigned char Dat)
{
	unsigned char i=0;
	for(i=0;i<8;i++)
	{
		DS2 = (Dat >> 7);
	  Dat <<= 1;
		sck2=0;
		_nop_();
		_nop_();
		sck2=1;
	}
	rck2=0;
	_nop_();
	_nop_();
	rck2=1;
}

void oled_scan()
{
	static unsigned char i=0;
	static unsigned char j=0;
	unsigned char temp;
  temp=~(0x0001<<i);//控制哪列灯亮
	write_74HC595_column(temp);
	write_74HC595_row(disp_code2[j]);
	write_74HC595_row(disp_code1[j]);
	delay_ms(1);//控制刷新率
	i++;
	j++;
	if(i==N) i=0;
	if(j==16) j=0;
}

void main()
{
	while(1)
	{
		oled_scan();
	}
}