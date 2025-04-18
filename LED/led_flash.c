#include<reg51.h>
#include<intrins.h>
void delay_ms(unsigned char z);

sbit led=P2^0;

void main()
{
	unsigned char n,i,m;
	n=20;
	m=1;
	while(1)
	{
		for(i=0;i<n;i++)
		{
			led=1;
			delay_ms(50*m);
			led=0;
			delay_ms(50*m);
			
		}
		if (m==4) m=1;
		else m=m*2;
		if (n==5) n=20;
		else n=n/2;
	
	}
}

void delay_ms(unsigned char z)
{
	//延迟z毫秒，最大只能到255ms
	unsigned int i=0;
	unsigned char j=0;
	for(j=0;j<z;j++)
		{
			for(i=0;i<125;i++);
		}
}