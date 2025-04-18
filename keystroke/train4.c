/*
该程序实现了计算器的加减乘除运算，连加连减连乘连除；
可以对负数运算，与电脑计算器相同，有+/-按钮可以实现符号转换，如果是第一个操作数，
也可以直接按负号输入负数；
可以输入并显示小数，并做运算，但结果只能保留整数（还未实现小数的具体操作）；
加入了平方按钮，先按下数字，再按平方按钮可直接显示平方结果；
加入了删除按钮，可以删除上一个输入的位，如234——>23;
*/

#include<reg51.h>
#include<math.h>
#define N 8 
#define Wx_port P2
#define Dx_port P0
sbit k1=P3^0;
sbit k2=P3^1;
sbit k3=P3^2;
sbit k4=P3^3;
sbit o1=P3^4;
sbit o2=P3^5;
sbit o3=P3^6;
sbit o4=P3^7;
sbit o5=P1^0;
bit flag=0;
bit flag_p=0;
char sign=1; //定义符号位
unsigned char count=0; //记录小数点后面的位数
unsigned char disp_code[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f};
unsigned char code seg_mode[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};
unsigned char cur_sta[4][5]={{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
unsigned char pre_sta[4][5]={{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
unsigned int cur_decimal=0; //定义小数部分
unsigned int cur_integer=0; //定义整数部分

void delay_ms(unsigned char z)
{
  unsigned int i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void  seg_led_scan()
{
	static unsigned char i=0;
  Wx_port=(0xfe<<i)|(0xfe>>(8-i));//位选信号，控制哪个数码管亮
  Dx_port=disp_code[i]; //段选信号，控制亮什么
	delay_ms(1);//控制刷新率
	Dx_port=0x00;//消影
	i++;
	if(i==N) i=0;
}

void cur_sta_update()
{
	static unsigned char buff[4][5]={{0xff,0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff,0xff},
																	{0xff,0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff,0xff}};
	unsigned char i=0;
	static unsigned char j=0; 
	//消抖							
	switch(j)
	{
		case 0: o1=0;o2=1;o3=1;o4=1;o5=1;break;
		case 1: o1=1;o2=0;o3=1;o4=1;o5=1;break;
		case 2: o1=1;o2=1;o3=0;o4=1;o5=1;break;
		case 3: o1=1;o2=1;o3=1;o4=0;o5=1;break;
		case 4: o1=1;o2=1;o3=1;o4=1;o5=0;break;
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
	if(j==5) j=0;
}

char key_scan()
{
	unsigned char key_value=-1;
	unsigned char i=0;
	unsigned char j=0;
	for (j=0;j<5;j++)
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





void show(float show_value)
{
	unsigned char i=0;
	unsigned char k=0;
	
	cur_integer = (unsigned int)show_value; //整数部分
  cur_decimal = (unsigned int)((show_value - cur_integer) * pow(10, count));//小数部分
	
	if (show_value==0)
	{
		disp_code[7]=seg_mode[0];
		k++;
	}
	else
	{
		if (flag_p==1) //如果有小数，则先显示小数
		{
			while (cur_decimal)
			{
				disp_code[7-k]=seg_mode[cur_decimal%10];
				cur_decimal/=10;
				k++;
			}
		}
		while(cur_integer) //显示整数
		{
			disp_code[7-k]=seg_mode[cur_integer%10];
			cur_integer/=10;
			k++;
		}
		if (sign==-1)  //如果符号是负号，最前面显示负号
		{	
			disp_code[7-k]=0x40;
			k++;
		}
		if (flag_p==1)  //如果是小数，在个位数上与0x80相或，显示小数点
			disp_code[7-count]= disp_code[7-count]|0x80;
	}
	for(i=0;i<(8-k);i++)
		disp_code[i]=0x00; //清除之前的显示
}

void key_action(unsigned char key_value)
{
	static float cur_value=0;   //记录按键输入的数字
	static float cur_value_real=0;
	static int result=0;
	static unsigned int result_unsigned=0;
	static int add1=0;
	static unsigned char op=0;
	if (key_value>=0 && key_value<=9) //按下数字
	{
		if (flag_p==1)
		{
			count++;
			cur_value+=key_value*pow(10,-count);
		}
		else
			cur_value=cur_value*10+key_value;
		cur_value_real=cur_value*sign; //加上符号，计算真实值
		show(cur_value);
	}
	else if (key_value==10) //按下小数点
	{
		flag_p=1;
		count=0;
		show(cur_value);
	}
	else if (key_value==11) //平方键，对操作数进行平方运算
	{
		cur_value_real*=cur_value_real;
		sign=1;
		show(cur_value_real);
	}
	else if (key_value>=12 && key_value<=15) //按下符号
	{
		if(op==0 && cur_value==0 && result==0 && key_value==13)//当第一个按下减号也可以当作负号
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
		cur_value=0;
		cur_decimal=0;
		cur_integer=0;
		sign=1;
	}
	else if (key_value==16) //按下删除键，删除上一个按下的数字
	{
		if (flag_p==1) //如果有小数，则删除小数位
		{
			cur_decimal/=10;
			count--;
			cur_value=cur_integer+cur_decimal*pow(10,-count);
		}
		else
			cur_value/=10;
		cur_value_real=cur_value*sign;
		show(cur_value);
	}
	else if (key_value==17) //按下正负号键
	{
		sign=-sign;
		cur_value_real=cur_value*sign;
		show(cur_value);
	}
	else if (key_value==18) //按下等号
	{
		flag=1;
		flag_p=1;
		count=2;
		switch(op)
		{
			case 12: result=add1+cur_value_real;break;
			case 13: result=add1-cur_value_real;break;
			case 14: result=add1*cur_value_real;break;
			case 15: result=add1/cur_value_real;break;
			default: break;
		}
		if (result<0) sign=-1;//判断结果的正负
		else sign=1;
		result_unsigned	= (result < 0) ? -result : result; // 计算结果的绝对值
		show(result_unsigned);
		flag_p=0;
		cur_value=0;
		cur_value_real=0;
	}
	else if (key_value==19) //按下清除键
	{
		cur_decimal=0;
		cur_integer=0;
		cur_value=0;
		cur_value_real=0;
		result=0;
		result_unsigned=0;
		op=0;
		flag=0;
		flag_p=0;
		count=0;
		sign=1;
		show(0);
	}
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

