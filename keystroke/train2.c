#include<reg51.h>
#define N 8
sbit key1 = P3^1;
sbit beep = P2^5;
sbit led = P2^0;
unsigned char cur_sta=1;
unsigned char pre_sta=1;
unsigned char clock_sta = 1; //1表示时钟运行，0表示暂停 
unsigned char key_value=0;
unsigned char beep_on=0;  //1表示闹钟响，0表示闹钟关闭

unsigned char seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
												0x7f,0x6f};
unsigned char disp_code[8]={0x3f,0x3f,0x40,0x3f,0x3f,0x40,0x3f,0x3f};

void delay_ms(unsigned int z) //延时函数
{
	unsigned int j;
	unsigned int i;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void seg_led_scan() //数码管扫描函数
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

void disp_code_update() //数码管更新函数
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
	
	if ((hour==0)&&(min==0)&&(sec==30))
		beep_on=1;
	
}

void alarm_on() //蜂鸣器设置闹钟
{
	static unsigned beep_count = 0;
	beep = ~beep;//产生一定频率的脉冲信号
	beep_count++;
	if (beep_count==600)
	{
		beep_count=0;
		beep_on=0;
	}
}

void cur_sta_update() //按键状态更新函数
{
	static unsigned char buff=0xff;
	unsigned char i=0;
	//消抖
	buff=(buff<<1)|key1;//把新抽样的值加到buff中
	if (buff==0xff)//8次采样全为1
		cur_sta=1;
	else if (buff==0x00)//全为0
		cur_sta=0;
	
}

void timer1_config() //定时器函数
{
	TR1=1; //打开定时计数器1
	TMOD=0x10; //定时器工作方式为1
	//定时计数器的初始值设置
	TH1=64536>>8;  //(65536-x)>>8
	TL1=64536;  //(65536-x)%256
	EA=1;  //中断总允许
	ET1=1; //定时器1中断分允许
}

void timer1_1ms() interrupt 3 //中断函数
{
	static unsigned int count=0;
	TH1=64536>>8;  //重新设置初始值
	TL1=64536;
	cur_sta_update();
	if (clock_sta==1)
	{
		count++;
		if (count==1000)
		{
			count=0;
			disp_code_update();
		}
	}
	//if (beep_on) 
		//alarm_on();
		
}

void key_scan() //键值扫描函数
{
	
	unsigned char i=0;
	if(cur_sta!=pre_sta)
	{
		if(pre_sta)
		{
			key_value=1;
		}
		pre_sta=cur_sta;
	}
}

void key_action() //按键行为函数
{
	if (key_value==1) 
	{	
		clock_sta = !clock_sta;
		key_value=0;
	}
	
	
}

void main()
{
	timer1_config(); //配置定时器1
	while(1)
	{
		seg_led_scan(); //数码管刷新函数
		key_scan();
		key_action();
	}
}