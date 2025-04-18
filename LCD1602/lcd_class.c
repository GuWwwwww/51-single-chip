#include<reg51.h>
#include<math.h>
extern void lcd_init();
extern void lcd_write_dat(unsigned char dat);
extern void cursor_set(unsigned char x, unsigned char y);
extern void delay_ms(unsigned char z);
extern void cur_sta_update();
extern char key_scan();
extern void timer1_config();
bit flag=0;
bit flag_p=0;
char sign=1; //定义符号位
unsigned char count=0; //记录小数点后面的位数
unsigned char count0=0; //记录小数点紧接着0的个数
unsigned int cur_decimal=0; //定义小数部分
unsigned int cur_integer=0; //定义整数部分

int round(float value) //定义四舍五入到整数函数
{
  return (int)(value + 0.5);
}

void show(float show_value)
{
	unsigned char i=0;
	unsigned char k=0;
	unsigned char c=count0;
	
	cur_integer = show_value; //整数部分
  cur_decimal = round((show_value - cur_integer) * pow(10,count));//小数部分
	cursor_set(1,15);
	if (show_value==0)
	{
		cursor_set(1,15);
		lcd_write_dat('0');
		k++;
	}
	else
	{
		if (flag_p==1) //如果有小数，则先显示小数
		{
			while(cur_decimal)
			{
				cursor_set(1,15-k);
				lcd_write_dat((cur_decimal%10)+'0');
				cur_decimal/=10;
				k++;
			}
			while(c)
			{
				cursor_set(1,15-k);
				lcd_write_dat('0');
				k++;
				c--;
			}
			cursor_set(1,15-k);
			lcd_write_dat('.');
			k++;
		}
		while(cur_integer) //显示整数
		{
			cursor_set(1,15-k);
			lcd_write_dat((cur_integer%10)+'0');
			cur_integer/=10;
			k++;
		}
		if (sign==-1)  //如果符号是负号，最前面显示负号
		{
			cursor_set(1,15-k);
			lcd_write_dat('-');
			k++;
		}
	}
	cursor_set(1,0);
	for(i=0;i<(16-k);i++)
			lcd_write_dat(' ');
}

void key_action(unsigned char key_value)
{
	static float cur_value=0;   //记录按键输入的数字
	static float cur_value_real=0;
	static float result=0;
	static float result_unsigned=0;
	static float add1=0;
	static unsigned char op=0;
	if (key_value>=0 && key_value<=9) //按下数字
	{
		if (flag_p==1)
		{
			if (key_value==0)
				count0++;
			count++;
			cur_value=cur_value+key_value*pow(0.1,count);
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
		count0=0;
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
		count0=0;
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
			cur_value=(unsigned int)(cur_value/10);
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
		count0=0;
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
		if (result_unsigned!=(int)result_unsigned) 
    {
			// 结果是小数，则计算小数的位数
      flag_p=1;
      count=0;
      cur_decimal=result_unsigned-(int)result_unsigned;
        
			while (cur_decimal>0)
			{
				 cur_decimal*=10;
				 cur_decimal-=(int)cur_decimal;
				 count++;
			}
    }
    else
    {
			flag_p = 0;
			count = 0;
    }
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
		count0=0;
		sign=1;
		show(0);
	}
}

void main()
{
	char key_value=-1;
	timer1_config(); // 配置定时器1
	lcd_init();
	while(1)
	{
		key_value=key_scan();
		key_action(key_value);
	}
}