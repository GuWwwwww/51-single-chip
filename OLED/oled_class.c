#include<reg51.h>
#define N 8
sbit DS=P3^4;
sbit rck=P3^5;
sbit sck=P3^6;
unsigned char disp_code[8]={0x00,0xFA,0x8A,0x8A,0x8F,0x8A,0x8A,0xFA};
void delay_ms(unsigned char z)
{
  unsigned int i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void write_74HC595(unsigned char Dat)
{
	unsigned char i=0;
	for(i=0;i<8;i++)
	{
		DS=(Dat&(0x01<<i));
		sck=0;
		sck=1;
	}
	rck=0;
	rck=1;
}

void oled_scan()
{
	static unsigned char i=0;
  P0=~(0x80>>i);//控制哪列灯亮
	write_74HC595(disp_code[i]);
	delay_ms(2);//控制刷新率
	i++;
	if(i==N) i=0;
}

void main()
{
	while(1)
	{
		oled_scan();
	}
}