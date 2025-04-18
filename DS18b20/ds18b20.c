#include<reg51.h>
#define Real_temp -1  // 定义一个表示读取失败的温度值
sbit RS=P2^6; //数据命令选择端口
sbit RW=P2^4; //写入或读出选择
sbit E=P2^7;  //LCD使能端口
sbit DQ=P3^7;	//定义DS18b20的管脚

void delay(unsigned int t)//延时子程序
{
	for(;t>0;t--);
}


unsigned char Init_DS18B20()//初始化DS18B20
{
  unsigned char x=0;
  DQ = 1;      //DQ复位
  delay(8);    //稍做延时
  DQ = 0;      //单片机将DQ拉低
  delay(80);   //精确延时，大于480us
  DQ = 1;      //拉高总线
  delay(8);
  x = DQ;      //稍做延时后，如果x=0则初始化成功，x=1则初始化失败
  delay(4);
  return x;
}

unsigned char ReadOneChar()//读一个字节
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // 给脉冲信号
    dat>>=1;
    DQ = 1;     // 给脉冲信号
    if(DQ)
    	dat|=0x80;
	delay(4);
  }
  return(dat);
}

/*****写一个字节*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
	delay(4);
    DQ = 1;
    dat>>=1;
  }
  delay(4);
}

/*****读取温度*****/
int ReadTemperature()
{
  	unsigned char a=0;
  	unsigned char b=0;
  	unsigned int t=0;
  	t=Init_DS18B20();
  	if(t) return Real_temp;
  	WriteOneChar(0xCC);  //跳过读序号列号的操作
  	WriteOneChar(0x44);  //启动温度转换
  	t=Init_DS18B20();
  	if(t) return Real_temp;
  	WriteOneChar(0xCC);  //跳过读序号列号的操作
  	WriteOneChar(0xBE);  //读取温度寄存器
  	a=ReadOneChar();     //读低8位
  	b=ReadOneChar();     //读高8位
  	t=b;
  	t<<=8;
  	t=t|a;
  	if(t<=0||t>0x900) 
	return Real_temp;
	t=t*0.625+0.5;
  	return(t);
}

void delay_ms(unsigned char z)
{
	unsigned char i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

unsigned char lcd_read_state() //读状态，确定LCD是否繁忙
{
	unsigned char state;
	RS=0;
	RW=1;
	E=1; //LCD使能
	delay_ms(2);
	state=P0&0X80;
	E=0;//关闭使能
	return(state);
}

void lcd_write_cmd(unsigned cmd)
{
	while(lcd_read_state());//等待繁忙结束
	RS=0;//表示写入的是指令
	RW=0;//写入状态
	P0=cmd;
	E=1;//使能LCD
	delay_ms(2);
	E=0;//命令发送完毕，关闭使能
}

void lcd_write_dat(unsigned char dat)
{
	while(lcd_read_state());//等待繁忙结束
	RS=1;//表示写入的是数据
	RW=0;//写入状态
	P0=dat;
	E=1;//使能LCD
	delay_ms(2);
	E=0;//数据发送完毕，关闭使能
}

void lcd_init()
{
	lcd_write_cmd(0x38);//5x7显示，8位数据总线，两行显示
	lcd_write_cmd(0x0c);//打开显示，光标不显示
	lcd_write_cmd(0x06);//光标右移，整屏不移动
	lcd_write_cmd(0x01);//清屏
	delay_ms(10);
}

void cursor_set(unsigned char x, unsigned char y)
{
	unsigned char addr=0;
	if (x==0)//表明第一行
	{
		addr=0x80+y;
	}
	else if (x==1)
	{
		addr=0x80+y+0x40;
	}
	lcd_write_cmd(addr);
}

int Pow(int x,int y)
{
	unsigned char i;
	int result = 1;
	for(i = 0; i < y; i++)
	{
		result *= x;
	}
	return result;
}

void lcd_ShowNum(unsigned char x,unsigned char y,unsigned int number,unsigned char length)
{
	unsigned char i;
	unsigned char temp;
	cursor_set(x,y);
	for(i=length;i>0;i--)
	{
		temp=number/Pow(10,i-1)%10+'0';	//循环将每一位都提取出来并转换为字符
		lcd_write_dat(temp);
	}
}

void main()
{
	unsigned char t;
	while(Init_DS18B20());
	lcd_init();
	while(1)
	{
		t=ReadTemperature();
		lcd_ShowNum(0,5,t,2);
	}
}
