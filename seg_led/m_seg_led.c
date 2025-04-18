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
	
	P2=bit_mode[i];  //λѡ�źţ������ĸ��������
	P0=disp_code[i]; //��ѡ�źţ�������ʲô
	delay_ms(1);
	//P0=0x00;  //������Ӱ
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
	TR1=1; //�򿪶�ʱ������
	TMOD=0x10; //��ʱ��������ʽΪ1
	TH0=0x3c;  //��ʱ�������ĳ�ʼֵ����
	TL0=0xb0;
}

void main()
{	
	unsigned char count=0;
	//���ö�ʱ�����������⹦�ܼĴ���
	timer1_config();
	while(1)
	{
		seg_led_scan(); //�����ˢ�º���
		if (TF1==1)
		{
			TF1=0;
			TH0=0x3c;  //��ʱ�������ĳ�ʼֵ����
			TL0=0xb0;
			count++;  //��¼����Ĵ�����ÿ����50����
		}
		if (count==20)  //1s����
		{
			count=0;
			disp_code_update();
		}	
	}

}