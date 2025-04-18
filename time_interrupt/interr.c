#include<reg51.h>
#define N 8

unsigned char seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
												0x7f,0x6f};
unsigned char disp_code[8]={0x3f,0x3f,0x40,0x3f,0x3f,0x40,0x3f,0x3f};

void delay_ms(unsigned int z)
{
	unsigned int j;
	unsigned int i;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void seg_led_scan()
{
	static unsigned int i;
	unsigned char bit_mode[8]={0xff,0xfa,0xf7,0xf3,0xef,0xea,0xe7,0xe3};
	
	P2=bit_mode[i];  //位选信号，控制哪个数码管亮
	P0=disp_code[i]; //段选信号，控制亮什么
	delay_ms(1); //控制刷新率
	//P0=0x00;  //消除鬼影
	i++;
	if (i==N) i=0;
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

}

void main()
{
	timer1_config(); //配置定时器1
	while(1)
	{
		seg_led_scan(); //数码管刷新函数
	}

}