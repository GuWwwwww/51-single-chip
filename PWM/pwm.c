#include<reg51.h>
sbit pwm_IO=P2^0;
unsigned int TH1_Low=0;
unsigned int TL1_Low=0;
unsigned int TH1_High=0;
unsigned int TL1_High=0;
unsigned char code level[]={95,75,55,35,5,35,55,75};
void PWM_config(unsigned char ratio, unsigned int period)
{
	//ratio占空比，period周期以微秒为单位
	unsigned int HighvoltTime,LowvoltTime;
	HighvoltTime=period/100*ratio;
	LowvoltTime=period-HighvoltTime;
	TH1_Low=(65536-LowvoltTime)>>8;
	TL1_Low=(65536-LowvoltTime)%256;
	TH1_High=(65536-HighvoltTime)>>8;
	TL1_High=(65536-HighvoltTime)%256;
}

void timer1_config()
{
	TR1=1; //打开定时计数器1
	TMOD=0x10; //定时器工作方式为1
	//定时计数器的初始值设置
	TH1=0;  //(65536-x)>>8
	TL1=0;  //(65536-x)%256
	EA=1;  //中断总允许
	ET1=1; //定时器1中断分允许
}

void timer1_ms() interrupt 3
{
	static unsigned int count=0;
	static unsigned char index=0;
	if (pwm_IO)
	{
		TH1=TH1_Low;  //重新设置初始值
		TL1=TL1_Low;
	}
	else
	{
		TH1=TH1_High;  //重新设置初始值
		TL1=TL1_High;
	}
	pwm_IO=~pwm_IO;
	count++;
	if(count==600)
	{
		
	}
}

void main()
{
	timer1_config();
	PWM_config(20,10000);
	while(1)
	{
		
	}
}