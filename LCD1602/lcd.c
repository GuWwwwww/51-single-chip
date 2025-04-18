#include<reg51.h>
sbit RS=P2^0; //数据命令选择端口
sbit RW=P2^1; //写入或读出选择
sbit E=P2^2;  //LCD使能端口

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


void lcd_show_str(unsigned char x, unsigned char y, char str[])
{
	//把字符串显示到特定的位置
	char i=0;
	cursor_set(x,y);
	while(str[i]!='\0')
	{
		lcd_write_dat(str[i]);
		i++;
	}
}
