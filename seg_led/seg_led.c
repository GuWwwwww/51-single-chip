#include<reg51.h>
void delay_ms(unsigned int z)
{
	unsigned int j;
	unsigned int i;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

sbit led = P2^7;

void main()
{
	unsigned char seg_mode[16]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
												0x7f,0x6f};
	// ,0x77,0x7c,0x39,0x5e,0x79,0x71
	unsigned char i=0;

	while(1)
	{
		unsigned char j;
		P2=0xff;
		P0=seg_mode[i];
		//P0=seg_mode[i%10];
		
		if(i>=5 && i<=10)
		{
			for(j=0;j<20;j++)
			{
				led=~led;
				delay_ms(50);
			}
		}
		else 
			delay_ms(1000);
		i++;
		if (i==10)
			i=0;
	}
}