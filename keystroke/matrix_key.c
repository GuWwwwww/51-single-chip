/*
计算器实现了连加、负数运算、小数运算。
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
char sign=1; //定义符号位
char count=0; //记录小数点后面的位数
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
	
	P2=bit_mode[i];  //位选信号，控制哪个数码管亮
	P0=disp_code[i]; //段选信号，控制亮什么
	delay_ms(1); //控制刷新率
	//P0=0x00;  //消除鬼影
	i++;
	if (i==N) i=0;
}

void cur_sta_update()
{
	static unsigned char buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
	unsigned char i=0;
	static unsigned char j=0; 
	//消抖							
	switch(j)
	{
		case 0: o1=0;o2=1;o3=1;o4=1;break;
		case 1: o1=1;o2=0;o3=1;o4=1;break;
		case 2: o1=1;o2=1;o3=0;o4=1;break;
		case 3: o1=1;o2=1;o3=1;o4=0;break;
		default: break;
	}
	buff[0][j]=(buff[0][j]<<1)|k1;//把新抽样的值加到buff中
	buff[1][j]=(buff[1][j]<<1)|k2;//把新抽样的值加到buff中
	buff[2][j]=(buff[2][j]<<1)|k3;//把新抽样的值加到buff中
	buff[3][j]=(buff[3][j]<<1)|k4;//把新抽样的值加到buff中
	for(i=0;i<4;i++)
	{
		if (buff[i][j]==0xff)//8次采样全为1
			cur_sta[i][j]=1;
		else if (buff[i][j]==0x00)//全为0
			cur_sta[i][j]=0;
	}
	j++;
	if(j==4) j=0;
}

void timer1_config()
{
	TR1=1; //打开定时计数器1
	TMOD=0x10; //定时器工作方式为1
	//定时计数器的初始值设置
	TH1=63536>>8;  //(65536-x)>>8
	TL1=63536;  //(65536-x)%256
	EA=1;  //中断总允许
	ET1=1; //定时器1中断分允许
}

void timer1_50ms() interrupt 3
{
	TH1=63536>>8;  //重新设置初始值
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
	return(key_value);//返回按键值
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
	
	//分离整数和小数部分
  temp_integer = (unsigned int)show_value; //整数部分
  temp_decimal = (unsigned int)(round((show_value - temp_integer) * pow(10, count)));//小数部分
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
		if (sign==-1)  //如果符号是负号，最前面显示负号
		{	
			disp_code[7-k]=0x40;
			k++;
		}
		if (flag_p==1) //如果是小数，在个位数上与0x80相或显示小数点
			disp_code[7-count]=disp_code[7-count]|0x80;
	}
	for(i=0;i<(8-k);i++)
		disp_code[i]=0x00; //清除之前的显示
}

void key_action(unsigned char key_value)
{
	static int cur_integer=0; //整数部分
	static int cur_point=0;  //小数部分
	static float cur_value_real=0;
	static float cur_value_unsigned=0;
	static float result=0;
	static float result_unsigned=0;
	static float add1=0;
	static unsigned char op=0;
	static unsigned char pre_key_value=0;
	if (key_value>=0 && key_value<=9) //按下数字
	{
		if (flag_p==1)//小数点标志位为1，按下数字为小数
		{
		  cur_point=cur_point*10+key_value;
			count++;
		}
		else
		{
			cur_integer=cur_integer*10+key_value;
		}
		cur_value_real=(cur_integer+cur_point*pow(10,-count))*sign;
		cur_value_unsigned=(cur_value_real < 0) ? -cur_value_real : cur_value_real; // 计算当前值的绝对值
		show(cur_value_unsigned);
	}
	else if (key_value>=10 && key_value<=12) //按下符号
	{
		//判断操作数为负的情况(第一个操作数为负，或者上一个键值为操作符，紧跟着负号的情况)
		if ((op==0 && cur_integer==0 && result==0 && key_value==11) || (pre_key_value>=10 && pre_key_value<=12 && key_value==11))
			sign=-1;
		else
		{
			sign=1;
			op=key_value;
		}
		if (flag==1) cur_value_real=result;//把上一步的结果存起来
		add1=cur_value_real; //把键入的第一个操作数存起来
		flag=0;
		flag_p=0;
		cur_integer=0;
		cur_point=0;
	}
	else if (key_value==13) //按下小数点
	{
		flag_p=1;
		count=0; //小数点位数重新开始记录
		cur_point = 0; // 重置小数部分
		show(cur_value_unsigned);
	}
	
	else if (key_value==14) //按下等号
	{
		flag_p=1; //小数点判断符置1，允许结果显示小数
		flag=1;
		switch(op)
		{
			case 10: result=cur_value_real+add1;break;
			case 11: result=add1-cur_value_real;break;
			case 12: result=add1*cur_value_real;break;
			case 13: result=add1/cur_value_real;break;
			default: break;
		}
		if (result<0) sign=-1;//判断结果的正负
		else sign=1;
		result_unsigned	= (result < 0) ? -result : result; // 计算结果的绝对值
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
	else if (key_value==15) //按下清除键
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
	timer1_config(); //配置定时器1
	while(1)
	{
		seg_led_scan();
		key_value=key_scan();
		key_action(key_value);
	}
}