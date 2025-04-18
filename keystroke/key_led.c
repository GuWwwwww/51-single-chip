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
	
	P2=bit_mode[i];  //λѡ�źţ������ĸ��������
	P0=disp_code[i]; //��ѡ�źţ�������ʲô
	delay_ms(1); //����ˢ����
	//P0=0x00;  //������Ӱ
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
	//����
	buff[0]=(buff[0]<<1)|key1;//���³�����ֵ�ӵ�buff��
	buff[1]=(buff[1]<<1)|key2;//���³�����ֵ�ӵ�buff��
	for(i=0;i<2;i++)
	{
		if (buff[i]==0xff)//8�β���ȫΪ1
			cur_sta[i]=1;
		else if (buff[i]==0x00)//ȫΪ0
			cur_sta[i]=0;
	}
}

void timer1_config()
{
	TR1=1; //�򿪶�ʱ������1
	TMOD=0x10; //��ʱ��������ʽΪ1
	//��ʱ�������ĳ�ʼֵ����
	TH1=64536>>8;  //(65536-x)>>8
	TL1=64536;  //(65536-x)%256
	EA=1;  //�ж�������
	ET1=1; //��ʱ��1�жϷ�����
}

void timer1_50ms() interrupt 3
{
	TH1=64536>>8;  //�������ó�ʼֵ
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
	return(key_value);//���ذ���ֵ
}

void main()
{
	unsigned char key_value=0;
	timer1_config(); //���ö�ʱ��1
	while(1)
	{
		seg_led_scan(); //�����ˢ�º���
		key_value=key_scan();
		disp_code_update(key_value);
	}
}