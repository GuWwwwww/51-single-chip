#include<reg51.h>
#define N 8 //数码管个数
#define M 8 //流水灯个数
#define Wx_port P3
#define Dx_port P2
#define led_port P1
unsigned char seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char disp_code[8]={0x3f,0x3f,0x40,0x3f,0x3f,0x40,0x3f,0x3f};
unsigned char dir=0; //定义流水灯的方向

void delay_ms(unsigned int z)
{
	unsigned int j;
	unsigned int i;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void  seg_led_scan()
{
	static unsigned char i=0;
  Wx_port=(0xfe<<i)|(0xfe>>(8-i)); //位选信号，控制哪个数码管亮
  Dx_port=disp_code[i];   //段选信号，控制亮什么
	delay_ms(1); //控制刷新率
	P2=0x00; //消影
	i++;
	if(i==N) i=0;
}

void disp_code_update()
{
	static unsigned char sec=0;
	static unsigned char min=0;
	static unsigned char hour=0;
	sec++;
	if(sec==60) 
	{
		sec=0;
		min++;
		if(min==60)
		{
			hour++;
			if(hour==24) hour=0;
		}
	}
	disp_code[7]=seg_mode[sec%10];
	disp_code[6]=seg_mode[sec/10];
	disp_code[4]=seg_mode[min%10];
	disp_code[3]=seg_mode[min/10];
	disp_code[1]=seg_mode[hour%10];
	disp_code[0]=seg_mode[hour/10];
	
	if ((hour==0)&&(min==0)&&(sec==30)) dir=1; //在00-00-30时，流水灯开始正向流动
	if ((hour==0)&&(min==1)&&(sec==0))  dir=2; //在00-01-00时，流水灯开始反向流动
	if ((hour==0)&&(min==1)&&(sec==30)) dir=3; //在00-01-30时，流水灯开始正反交替
	if ((hour==0)&&(min==2)&&(sec==0))  dir=0; //在00-02-00时，流水灯停止工作

}

void led_running(unsigned int d)
{
	static char i=0;
	static int flag=0;
	if (d==1) //流动方向为正向
		led_port=~(0x01<<i);
	else if (d==2) //流动方向为反向
		led_port=~(0x80>>(8-M+i));
	else if (d==3) //流动方向正反交替
	{
	if (flag==0) 
		led_port=~(0x01<<i);
	else
		led_port=~(0x80>>(8-M+i));
	if (i==(M-1)) flag^=1;
  }
	//delay_ms(50);每中断1次50ms亮一个灯，不再需要delay
	i++;
	if (i==M) i=0;
}

void timer1_config()
{
	TR1=1; //打开定时计数器1
	TMOD=0x10; //定时器工作方式为1
	//定时计数器的初始值设置
	TH1=0x3c;  //(65536-x)>>8
	TL1=0xb0;  //(65536-x)%256
	EA=1;  //中断总允许
	ET1=1; //定时器1中断分允许
}

void timer1_50ms() interrupt 3
{
	static unsigned char count=0;
	TH1=0x3c;  //重新设置初始值
	TL1=0xb0;
	count++;
	if (count==20)
	{
		count=0;
		disp_code_update(); //更新控制刷新管显示的数字
	}
	led_running(dir); //每50ms亮其中一个灯,该方法的速率只能控制在50ms的整数倍
}

void main()
{
	timer1_config(); //配置定时器1
	while(1)
	{
		seg_led_scan(); //数码管刷新函数
		
	}

}
