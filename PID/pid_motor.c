#include<reg51.h>
sbit IN1=P2^3;
sbit IN2=P2^4;
sbit ENA_pwm=P2^5;
bit sec_flag=0;
unsigned int ratio=700;
unsigned int pulse=0;
unsigned int cur_pulse=0;
unsigned int target_pulse=350;
extern char key_scan(); 
extern void cur_sta_update();
extern void lcd_init();
extern disp(unsigned char x, unsigned char y,unsigned int speed);

void pid_ctr()
{
	float kp=2.0;
	float ki=0.9;
	float kd=0.16;
	float delta_u=0;
	static int e=0,e1=0,e2=0;
	e=target_pulse-cur_pulse;
	delta_u=kp*(e-e1)+ki*e+kd*(e-2*e1+e2);
	ratio=ratio+(int) delta_u;
	e1=e;
	e2=e1;
}

void system_config()
{
	TR1=1;
	TR0=1;
	TMOD=0x12;
	TH1=63536>>8;
	TL1=63536;
	TH0=156;
	TL0=156;
	EA=1;
	ET1=1;
	ET0=1;
	EX0=1;
	IT0=1;
}

void Ex0_int() interrupt 0
{
	pulse++;
}

void timer0_100us() interrupt 1
{
	static unsigned int cnt1=0;
	if (cnt1<ratio)
		ENA_pwm=1;
	else
		ENA_pwm=0;
	cnt1++;
	if (cnt1==1000) cnt1=0;
}

void timer1_2ms() interrupt 3
{
	static int count1=0;
	TH1=63536>>8;  //重新设置初始值
	TL1=63536;
	cur_sta_update();
	count1++;
	if (count1==500)
	{
		count1=0;
		cur_pulse=pulse;
		pulse=0;
		sec_flag=1;
		pid_ctr();
	}
}

void key_action(unsigned char key_value)
{
	if (key_value==0)
	{
		IN1=~IN1;
	}
	if (key_value==1)
	{
		target_pulse+=50;
		disp(1,2,target_pulse);
	}
	else if (key_value==2)
	{
		target_pulse-=50;
		disp(1,2,target_pulse);
	}
}

void main()
{
	char key_value=-1;
	system_config();
	lcd_init();
	IN1=0;
	IN2=0;
	ENA_pwm=1;
	disp(1,2,target_pulse);
	while(1)
	{
		key_value=key_scan();
		key_action(key_value);
		if(sec_flag)
		{
			disp(0,2,cur_pulse);
			sec_flag=0;
		}
	}
}