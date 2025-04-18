#include<reg51.h>
sbit DS1302_SCLK=P3^6;
sbit DS1302_IO=P3^4;
sbit DS1302_CE=P3^5;
//对应寄存器位置定义
#define DS1302_SECOND	0X80
#define DS1302_MINUTE 0X82
#define DS1302_HOUR		0X84
#define DS1302_DATE   0X86
#define DS1302_MONTH	0X88
#define DS1302_DAY    0X8A
#define DS1302_YEAR   0X8C
#define DS1302_WP     0X8E

//定义数组用于存放时间
unsigned char DS1302_Time[]={24,12,26,15,0,59,4};//顺序：年月日时分秒星期

void DS1302_Init()
{
	DS1302_CE=0;//将使能位置0
	DS1302_SCLK=0;//将时钟位置0
}

void DS1302_WriteBety(unsigned char command,Data)//单字节写入函数
{
	unsigned char i;
	DS1302_CE=1;//将使能位置高电平
	for(i=0;i<8;i++)//控制寄存器的数据逐位写入
	{
		DS1302_IO=command&(0x01<<i);
		DS1302_SCLK=1;
		DS1302_SCLK=0;
	}
	for(i=0;i<8;i++)//将数据写入
	{
		DS1302_IO=Data&(0x01<<i);
		DS1302_SCLK=1;
		DS1302_SCLK=0;
	}
	DS1302_CE=0;	
}

unsigned char DS1302_ReadBety(unsigned char command)//单字节读出函数
{
	unsigned i,Data=0X00;
	command|=0X01;//写入指令与读出指令只在最后一位相差1
  DS1302_CE=1;//将使能位置高电平
	for(i=0;i<8;i++)
	{
		DS1302_IO=command&(0x01<<i);
		DS1302_SCLK=0;
		DS1302_SCLK=1;
	}
	DS1302_IO=0;
	for(i=0;i<8;i++)//数据读出
	{
		DS1302_SCLK=1;
		DS1302_SCLK=0;			
		if(DS1302_IO)  
			Data|=(0X01<<i);
	}
	DS1302_CE=0;
	return Data;
}

void DS1302_SetTime()//向DS1302内设定时间
{
	DS1302_WriteBety(DS1302_WP,0x00);//关闭写保护，不然指令无法进入控制寄存器；
	DS1302_WriteBety(DS1302_YEAR, DS1302_Time[0]/10*16+DS1302_Time[0]%10);
	DS1302_WriteBety(DS1302_MONTH, DS1302_Time[1]/10*16+DS1302_Time[1]%10);
	DS1302_WriteBety(DS1302_DATE, DS1302_Time[2]/10*16+DS1302_Time[2]%10);
	DS1302_WriteBety(DS1302_HOUR, DS1302_Time[3]/10*16+DS1302_Time[3]%10);
	DS1302_WriteBety(DS1302_MINUTE, DS1302_Time[4]/10*16+DS1302_Time[4]%10);
	DS1302_WriteBety(DS1302_SECOND, DS1302_Time[5]/10*16+DS1302_Time[5]%10);
	DS1302_WriteBety(DS1302_DAY, DS1302_Time[6]/10*16+DS1302_Time[6]%10);
	DS1302_WriteBety( DS1302_WP,0x80);//写入结束，开启写保护；
}

void DS1302_ReadTime()//读取DS1302内时间
{
	unsigned char Temp;//定义变量，用于暂时存储BCD码
	Temp=DS1302_ReadBety(DS1302_YEAR);//读取年BCD码；
	DS1302_Time[0]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_MONTH);//读取月BCD码；
	DS1302_Time[1]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_DATE);//读取日BCD码；
	DS1302_Time[2]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_HOUR);//读取小时BCD码；
	DS1302_Time[3]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_MINUTE);//读取分钟BCD码；
	DS1302_Time[4]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_SECOND);//读取秒BCD码；
	DS1302_Time[5]=Temp/16*10+Temp%16;//BCD码转十进制；
  Temp=DS1302_ReadBety(DS1302_DAY);//读取星期BCD码；
	DS1302_Time[6]=Temp/16*10+Temp%16;//BCD码转十进制；
}



