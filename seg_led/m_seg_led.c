#include<reg51.h>

unsigned char seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
												0x7f,0x6f};
unsigned char disp_code[8]={0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f,0x3f};

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
	delay_ms(1);
	//P0=0x00;  //消除鬼影
	i++;
	if (i==8) i=0;
}

void disp_code_update()
{
	static unsigned char count=0;
	count++;
	if(count==100) count=0;
	disp_code[7]=seg_mode[count%10];
	disp_code[6]=seg_mode[count/10];
}

void timer1_config()
{
	TR1=1; //打开定时计数器
	TMOD=0x10; //定时器工作方式为1
	TH0=0x3c;  //定时计数器的初始值设置
	TL0=0xb0;
}

void main()
{	
	unsigned char count=0;
	//配置定时计数器的特殊功能寄存器
	timer1_config();
	while(1)
	{
		seg_led_scan(); //数码管刷新函数
		if (TF1==1)
		{
			TF1=0;
			TH0=0x3c;  //定时计数器的初始值设置
			TL0=0xb0;
			count++;  //记录溢出的次数，每次是50毫秒
		}
		if (count==20)  //1s到了
		{
			count=0;
			disp_code_update();
		}	
	}

}