#include<reg51.h>
#define N 8
sbit key1 = P3^1;
sbit beep = P2^5;
sbit led = P2^0;
unsigned char cur_sta=1;
unsigned char pre_sta=1;
unsigned char clock_sta = 1; //1��ʾʱ�����У�0��ʾ��ͣ 
unsigned char key_value=0;
unsigned char beep_on=0;  //1��ʾ�����죬0��ʾ���ӹر�

unsigned char seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
												0x7f,0x6f};
unsigned char disp_code[8]={0x3f,0x3f,0x40,0x3f,0x3f,0x40,0x3f,0x3f};

void delay_ms(unsigned int z) //��ʱ����
{
	unsigned int j;
	unsigned int i;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void seg_led_scan() //�����ɨ�躯��
{
	static unsigned int i;
	unsigned char bit_mode[8]={0xff,0xfa,0xf7,0xf3,0xef,0xea,0xe7,0xe3};
	
	P2=bit_mode[i];  //λѡ�źţ������ĸ��������
	P0=disp_code[i]; //��ѡ�źţ�������ʲô
	delay_ms(1); //����ˢ����
	//P0=0x00;  //������Ӱ
	i++;
	if (i==N) i=0;
}

void disp_code_update() //����ܸ��º���
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

void alarm_on() //��������������
{
	static unsigned beep_count = 0;
	beep = ~beep;//����һ��Ƶ�ʵ������ź�
	beep_count++;
	if (beep_count==600)
	{
		beep_count=0;
		beep_on=0;
	}
}

void cur_sta_update() //����״̬���º���
{
	static unsigned char buff=0xff;
	unsigned char i=0;
	//����
	buff=(buff<<1)|key1;//���³�����ֵ�ӵ�buff��
	if (buff==0xff)//8�β���ȫΪ1
		cur_sta=1;
	else if (buff==0x00)//ȫΪ0
		cur_sta=0;
	
}

void timer1_config() //��ʱ������
{
	TR1=1; //�򿪶�ʱ������1
	TMOD=0x10; //��ʱ��������ʽΪ1
	//��ʱ�������ĳ�ʼֵ����
	TH1=64536>>8;  //(65536-x)>>8
	TL1=64536;  //(65536-x)%256
	EA=1;  //�ж�������
	ET1=1; //��ʱ��1�жϷ�����
}

void timer1_1ms() interrupt 3 //�жϺ���
{
	static unsigned int count=0;
	TH1=64536>>8;  //�������ó�ʼֵ
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

void key_scan() //��ֵɨ�躯��
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

void key_action() //������Ϊ����
{
	if (key_value==1) 
	{	
		clock_sta = !clock_sta;
		key_value=0;
	}
	
	
}

void main()
{
	timer1_config(); //���ö�ʱ��1
	while(1)
	{
		seg_led_scan(); //�����ˢ�º���
		key_scan();
		key_action();
	}
}