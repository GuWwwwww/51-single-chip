#include<reg51.h>
#define N 8

sbit key1 = P3^1;
sbit key2 = P3^0;
sbit key3 = P3^2;
sbit beep = P2^5;
sbit led = P2^0;
unsigned char cur_sta[2]={1,1};
unsigned char pre_sta[2]={1,1};

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

void disp_code_update(key_value)
{
	static unsigned char sec=0;
	static unsigned char min=0;
	static unsigned char hour=0;
	if (key_value==1)
		sec++;
	else if (key_value==2)
		sec--;
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

void cur_sta_update()
{
	static unsigned char buff[2]={0xff,0xff};
	unsigned char i=0;
	//消抖
	buff[0]=(buff[0]<<1)|key1;//把新抽样的值加到buff中
	buff[1]=(buff[1]<<1)|key2;//把新抽样的值加到buff中
	for(i=0;i<2;i++)
	{
		if (buff[i]==0xff)//8次采样全为1
			cur_sta[i]=1;
		else if (buff[i]==0x00)//全为0
			cur_sta[i]=0;
	}
}

void timer1_config()
{
	TR1=1; //打开定时计数器1
	TMOD=0x10; //定时器工作方式为1
	//定时计数器的初始值设置
	TH1=64536>>8;  //(65536-x)>>8
	TL1=64536;  //(65536-x)%256
	EA=1;  //中断总允许
	ET1=1; //定时器1中断分允许
}

void timer1_50ms() interrupt 3
{
	TH1=64536>>8;  //重新设置初始值
	TL1=64536;
	cur_sta_update();
}

unsigned char key_scan()
{
	unsigned char key_value=0;
	unsigned char i=0;
	for(i=0;i<2;i++)
	{
		if(cur_sta[i]!=pre_sta[i])
		{
			if(pre_sta[i])
			{
				key_value=i+1;
			}
			pre_sta[i]=cur_sta[i];
		}
	}
	return(key_value);//返回按键值
}

void main()
{
	unsigned char key_value=0;
	timer1_config(); //配置定时器1
	while(1)
	{
		seg_led_scan(); //数码管刷新函数
		key_value=key_scan();
		disp_code_update(key_value);
	}
}