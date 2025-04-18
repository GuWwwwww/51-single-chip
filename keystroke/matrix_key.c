/*
������ʵ�������ӡ��������㡢С�����㡣
*/
#include<reg51.h>
#include<math.h>
#define N 8
sbit k1=P1^7;
sbit k2=P1^6;
sbit k3=P1^5;
sbit k4=P1^4;
sbit o1=P1^3;
sbit o2=P1^2;
sbit o3=P1^1;
sbit o4=P1^0;
bit flag=0;
bit flag_p=0;
char sign=1; //�������λ
char count=0; //��¼С��������λ��
unsigned char cur_sta[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
unsigned char pre_sta[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
unsigned char code seg_mode[10]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};
unsigned char disp_code[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

void delay_ms(unsigned int z)
{
	unsigned int i,j;
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

void cur_sta_update()
{
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
	unsigned char i=0;
	static unsigned char j=0; 
	//����							
	switch(j)
	{
		case 0: o1=0;o2=1;o3=1;o4=1;break;
		case 1: o1=1;o2=0;o3=1;o4=1;break;
		case 2: o1=1;o2=1;o3=0;o4=1;break;
		case 3: o1=1;o2=1;o3=1;o4=0;break;
		default: break;
	}
	buff[0][j]=(buff[0][j]<<1)|k1;//���³�����ֵ�ӵ�buff��
	buff[1][j]=(buff[1][j]<<1)|k2;//���³�����ֵ�ӵ�buff��
	buff[2][j]=(buff[2][j]<<1)|k3;//���³�����ֵ�ӵ�buff��
	buff[3][j]=(buff[3][j]<<1)|k4;//���³�����ֵ�ӵ�buff��
	for(i=0;i<4;i++)
	{
		if (buff[i][j]==0xff)//8�β���ȫΪ1
			cur_sta[i][j]=1;
		else if (buff[i][j]==0x00)//ȫΪ0
			cur_sta[i][j]=0;
	}
	j++;
	if(j==4) j=0;
}

void timer1_config()
{
	TR1=1; //�򿪶�ʱ������1
	TMOD=0x10; //��ʱ��������ʽΪ1
	//��ʱ�������ĳ�ʼֵ����
	TH1=63536>>8;  //(65536-x)>>8
	TL1=63536;  //(65536-x)%256
	EA=1;  //�ж�������
	ET1=1; //��ʱ��1�жϷ�����
}

void timer1_50ms() interrupt 3
{
	TH1=63536>>8;  //�������ó�ʼֵ
	TL1=63536;
	cur_sta_update();
}

char key_scan()
{
	unsigned char key_value=-1;
	unsigned char i=0;
	unsigned char j=0;
	for (j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			{
				if(cur_sta[i][j]!=pre_sta[i][j])
				{
					if(pre_sta[i][j])
					{
						key_value=i+4*j;
					}
					pre_sta[i][j]=cur_sta[i][j];
				}
			}
		}
	}
	return(key_value);//���ذ���ֵ
}

int round(float value)
{
  return (int)(value + 0.5);
}

unsigned int temp_integer=0;
unsigned int temp_decimal=0;

void show(float show_value)
{
	unsigned char i=0;
	unsigned char k=0;
	
	//����������С������
  temp_integer = (unsigned int)show_value; //��������
  temp_decimal = (unsigned int)(round((show_value - temp_integer) * pow(10, count)));//С������
	if (show_value==0)
	{
		disp_code[7]=seg_mode[0];
		k++;
	}
	else
	{
		if (flag_p==1)
		{
			while(temp_decimal)
			{
				disp_code[7-k]=seg_mode[temp_decimal%10];
				temp_decimal/=10;
				k++;
			}
		}
		while(temp_integer)
		{
			disp_code[7-k]=seg_mode[temp_integer%10];
			temp_integer/=10;
			k++;
		}
		if (sign==-1)  //��������Ǹ��ţ���ǰ����ʾ����
		{	
			disp_code[7-k]=0x40;
			k++;
		}
		if (flag_p==1) //�����С�����ڸ�λ������0x80�����ʾС����
			disp_code[7-count]=disp_code[7-count]|0x80;
	}
	for(i=0;i<(8-k);i++)
		disp_code[i]=0x00; //���֮ǰ����ʾ
}

void key_action(unsigned char key_value)
{
	static int cur_integer=0; //��������
	static int cur_point=0;  //С������
	static float cur_value_real=0;
	static float cur_value_unsigned=0;
	static float result=0;
	static float result_unsigned=0;
	static float add1=0;
	static unsigned char op=0;
	static unsigned char pre_key_value=0;
	if (key_value>=0 && key_value<=9) //��������
	{
		if (flag_p==1)//С�����־λΪ1����������ΪС��
		{
		  cur_point=cur_point*10+key_value;
			count++;
		}
		else
		{
			cur_integer=cur_integer*10+key_value;
		}
		cur_value_real=(cur_integer+cur_point*pow(10,-count))*sign;
		cur_value_unsigned=(cur_value_real < 0) ? -cur_value_real : cur_value_real; // ���㵱ǰֵ�ľ���ֵ
		show(cur_value_unsigned);
	}
	else if (key_value>=10 && key_value<=12) //���·���
	{
		//�жϲ�����Ϊ�������(��һ��������Ϊ����������һ����ֵΪ�������������Ÿ��ŵ����)
		if ((op==0 && cur_integer==0 && result==0 && key_value==11) || (pre_key_value>=10 && pre_key_value<=12 && key_value==11))
			sign=-1;
		else
		{
			sign=1;
			op=key_value;
		}
		if (flag==1) cur_value_real=result;//����һ���Ľ��������
		add1=cur_value_real; //�Ѽ���ĵ�һ��������������
		flag=0;
		flag_p=0;
		cur_integer=0;
		cur_point=0;
	}
	else if (key_value==13) //����С����
	{
		flag_p=1;
		count=0; //С����λ�����¿�ʼ��¼
		cur_point = 0; // ����С������
		show(cur_value_unsigned);
	}
	
	else if (key_value==14) //���µȺ�
	{
		flag_p=1; //С�����жϷ���1����������ʾС��
		flag=1;
		switch(op)
		{
			case 10: result=cur_value_real+add1;break;
			case 11: result=add1-cur_value_real;break;
			case 12: result=add1*cur_value_real;break;
			case 13: result=add1/cur_value_real;break;
			default: break;
		}
		if (result<0) sign=-1;//�жϽ��������
		else sign=1;
		result_unsigned	= (result < 0) ? -result : result; // �������ľ���ֵ
		if (result != (int)result)
    {
        flag_p = 1;  // ???????
        count = 0;  // ?????????
        cur_point = result_unsigned - (int)result_unsigned;  // ??????
        
        // ???????????????(????4???)
        while (cur_point > 0)
        {
            cur_point *= 10;
            cur_point -= (int)cur_point;
            count++;
        }
    }
    else
    {
        flag_p = 0;  // ????????,???????
        count = 0;
    }
		show(result_unsigned);
		flag_p=0;
		cur_integer=0;
		cur_point=0;
		cur_value_real=0;
	}
	else if (key_value==15) //���������
	{
		cur_integer=0;
		cur_point=0;
		cur_value_real=0;
		cur_value_unsigned=0;
		pre_key_value=0;
		result=0;
		op=0;
		flag=0;
		flag_p=0;
		sign=1;
		show(0);
	}
	pre_key_value=key_value;
}

void main()
{
	char key_value=-1;
	timer1_config(); //���ö�ʱ��1
	while(1)
	{
		seg_led_scan();
		key_value=key_scan();
		key_action(key_value);
	}
}