#include<reg51.h>
#include<intrins.h>
extern void lcd_init();
extern void cursor_set(unsigned char x, unsigned char y); 
extern void lcd_show_str(unsigned char x, unsigned char y, char str[]);
extern void lcd_write_dat(unsigned char dat);
extern void lcd_ShowNum(unsigned char x,unsigned char y,unsigned int number,unsigned char length);
extern char key_scan();
extern void lcd_write_cmd(unsigned cmd);
extern void lcd_clear_line(unsigned char x);
extern unsigned char DS1302_Time[];
extern void DS1302_Init();
extern void DS1302_SetTime();
extern void DS1302_ReadTime();
extern void motor_forward_circle();
extern void motor_reverse_circle();
extern void cur_sta_update();
extern void ESP8266_ModeInit();
unsigned char key_value=0;
unsigned char TimeSetSelect=0;
unsigned char timerselect=0;
bit feed_mode=0; //喂食的模式，手动/自动，默认为自动
bit feed_flag=0;//电机的开关状态
bit timer_flag=1;//定时投喂功能开关，默认为打开
bit flash_flag=0;//调整时间闪烁标志位
sbit beep=P2^5;
unsigned char portion=0;
unsigned char adj_mode=0;
unsigned int t=0;
char timer_hour=6;//默认自动喂食情况6小时一次
char timer_min=0;
char timer_sec=0;
char timer[3]={6,0,0};//存放设置好的定时时间 

void delay_ms(unsigned char z)
{
	unsigned char i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void time_set()//设置时间
{
	if(key_value==2)
	{
		TimeSetSelect++;//时间设置选择位加1
		TimeSetSelect%=7;//清零
	}
	else if(key_value==3)
	{
		DS1302_Time[TimeSetSelect]++;//时间设置位数值加1
		if(DS1302_Time[0]>99){DS1302_Time[0]=0;}//年越界判断
		if(DS1302_Time[1]>12){DS1302_Time[1]=1;}//月越界判断
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//日越界判断
		{
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}//大月
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}//小月
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}//闰年2月
			}
			else
			{
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}//平年2月
			}
		}
		if(DS1302_Time[3]>23){DS1302_Time[3]=0;}//时越界判断
		if(DS1302_Time[4]>59){DS1302_Time[4]=0;}//分越界判断
		if(DS1302_Time[5]>59){DS1302_Time[5]=0;}//秒越界判断
		if(DS1302_Time[6]>7){DS1302_Time[5]=1;}//星期越界判断
	}
	
	//更新显示，根据TimeSetSelect和flash_flag判断可完成闪烁功能
	lcd_show_str(0,1,"  -  -  ");
	lcd_show_str(1,1,"  :  :  ");
	if(TimeSetSelect==0 && flash_flag==1) lcd_show_str(0,1,"  ");
	else lcd_ShowNum(0,1,DS1302_Time[0],2);
	if(TimeSetSelect==1 && flash_flag==1) lcd_show_str(0,4,"  ");
	else lcd_ShowNum(0,4,DS1302_Time[1],2);
	if(TimeSetSelect==2 && flash_flag==1) lcd_show_str(0,7,"  ");
	else lcd_ShowNum(0,7,DS1302_Time[2],2);
	if(TimeSetSelect==3 && flash_flag==1) lcd_show_str(1,1,"  ");
	else lcd_ShowNum(1,1,DS1302_Time[3],2);
	if(TimeSetSelect==4 && flash_flag==1) lcd_show_str(1,4,"  ");
	else lcd_ShowNum(1,4,DS1302_Time[4],2);
	if(TimeSetSelect==5 && flash_flag==1) lcd_show_str(1,7,"  ");
	else lcd_ShowNum(1,7,DS1302_Time[5],2);
	if(TimeSetSelect==6 && flash_flag==1) lcd_show_str(1,11,"  ");
	else lcd_ShowNum(1,11,DS1302_Time[6],2);
	
	DS1302_SetTime();
}

void timer_set()//设置定时时间
{
	if (timerselect==1)
	{	
		timer_hour++;
		timer_hour%=11;
		timer[0]=timer_hour;
	}
	else if(timerselect==2)
	{	
		timer_min++;
		timer_min%=60;
		timer[1]=timer_min;
	}	
	else if(timerselect==3)
	{
		timer_sec++;
		timer_sec%=60;
		timer[2]=timer_sec;
	}
}

void key_action() //按键模块
{
	if(key_value==1)
	{
		//可调整模式，模式0下可以调整喂食相关事项，
		//模式1下可以调整时钟
		//模式2下可以设置自动定时投喂时间间隔，也可以看距离下次投喂时间
		adj_mode++;
		adj_mode%=3;
		TimeSetSelect=0;
		timerselect=0;
		lcd_write_cmd(0x01);//清屏
	}
	else if(key_value==2)
	{
		if(adj_mode==0)  //设置小中大三种喂食份量
		{
			portion++;
			portion%=3;
		}
		else if(adj_mode==2)
		{
			timerselect++;//定时时间设置选择位加1
			timerselect%=4;
		}
	}
	else if(key_value==3)
	{
		if(adj_mode==0)
		{	
			feed_mode=!feed_mode;//切换喂食模式，自动/手动
			if (feed_mode==1)
			{
				timer_flag=0;
				timer_hour=timer[0];//定时投喂一次后，回到设置的定时时间
				timer_min=timer[1];
				timer_sec=timer[2];
			}
			else 
				timer_flag=1;
		}
		else if (adj_mode==2)
		{
			timer_set();//定时设置
		}
	}
	else if(key_value==4)
	{
		//手动喂食按钮
		if(feed_mode)
		{
			motor_forward_circle();
			feed_flag=1;
		}
	}
}

void lcd_show() //lcd显示模块
{
	if(adj_mode==2) //设置定时模式
	{
		lcd_show_str(1,4,"  :  :  ");//设置时分秒格式
		lcd_show_str(0,1,"Set Timer:");
		lcd_ShowNum(1,4,timer_hour,2);
		lcd_ShowNum(1,7,timer_min,2);
		lcd_ShowNum(1,10,timer_sec,2);
	}
	else
	{
		lcd_show_str(0,1,"  -  -  ");//设置年月日格式
		lcd_show_str(1,1,"  :  :  ");//设置时分秒格式
		DS1302_ReadTime();//读取内部时间
		lcd_ShowNum(0,1,DS1302_Time[0],2);//显示年
		lcd_ShowNum(0,4,DS1302_Time[1],2);//显示月
		lcd_ShowNum(0,7,DS1302_Time[2],2);//显示日
		lcd_ShowNum(1,1,DS1302_Time[3],2);//显示时
		lcd_ShowNum(1,4,DS1302_Time[4],2);//显示分
		lcd_ShowNum(1,7,DS1302_Time[5],2);//显示秒
		lcd_ShowNum(1,11,DS1302_Time[6],1);//显示星期
		
		// LCD上显示自动/手动模式
		if(!feed_mode)
			lcd_show_str(0,11,"AUTO");
		else
			lcd_show_str(0,11,"HAND");
		
		// LCD上显示自动投喂的份量
		if(portion==0)
			lcd_show_str(1,14,"S");
		else if(portion==1)
			lcd_show_str(1,14,"M");
		else if(portion==2)
			lcd_show_str(1,14,"L");
		
	}
}

void timer_config()//定时器初始化 
{
	SCON=0x50;  // 串口模式1，8位数据，1位停止位
	TMOD=0x21; //定时器工作方式为1
	//定时计数器的初始值设置
	TH0=63536>>8;  //(65536-x)>>8
	TL0=63536;  //(65536-x)%256
	TH1=0xFD;   // 波特率9600
  	TL1=0xFD;
	SM0 = 0;    // 串口模式1
  	SM1 = 1;
  	REN = 1;   // 允许接收
	EA=1;  //中断总允许
	ET0=1; //定时器1中断分允许
	ES=1; //串口定时器分允许
	TR0=1; //打开定时计数器0
	TR1=1; //打开定时计数器1
}

void main()
{
	timer_config();//定时器初始化
	ESP8266_ModeInit();//WIFI连接配置初始化
	lcd_init();//lcd初始化
	DS1302_Init();//DS1302初始化
	lcd_show_str(0,1,"  -  -  ");//设置年月日格式
	lcd_show_str(1,1,"  :  :  ");//设置时分秒格式
	DS1302_SetTime();//设置时间，通过数组进行设置
	while(1)
	{
		key_value=key_scan();
		key_action();
		if(adj_mode==1)//调整时间模式 
			time_set();
		else
			lcd_show();
	}
}

void timer0_2ms() interrupt 1
{
	static unsigned char timer_count=0;
	static unsigned char count=0;
	static unsigned int flash_count=0;
	TH0=63536>>8;  //重新设置初始值
	TL0=63536;
	cur_sta_update();
	flash_count++;
	if(flash_count==250)//每0.5s进入一次
	{
		flash_count=0;
		flash_flag=!flash_flag;//闪烁标志位取反
	
		if(feed_flag) //份量时间计算，小份2s，中份3s，大份4s
		{
			count++;
			if((portion==0 && count==4)||(portion==1 && count==6)||(portion==2 && count==8))
			{
				count=0;
				feed_flag=0;
				motor_reverse_circle();
				lcd_write_cmd(0x01);//清屏
			}
		}
	
		if(timer_flag)//定时时间递减
		{
			timer_count++;
			if (timer_count==2)
			{
				timer_count=0;
				timer_sec--;
				if(timer_sec<0)
				{
					timer_sec=59;
					timer_min--;
					if (timer_min<0)
					{
						timer_min=59;
						timer_hour--;
						if(timer_hour<0)
						{
							timer_hour=timer[0];//定时投喂一次后，回到设置的定时时间
							timer_min=timer[1];
							timer_sec=timer[2];
							motor_forward_circle();
							feed_flag=1;
						}
					}
				}
			}
		}
	}
}
