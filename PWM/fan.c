#include<reg51.h>
#define N 4 
#define Wx_port P2
#define Dx_port P0
sbit key1=P1^0;
sbit key2=P1^1;
sbit key3=P1^2;
sbit fan_IO=P3^0;
bit T_minus_mode=0;
char min=0;
unsigned char CoundDownHour=0;
unsigned char TH1_Low=0;
unsigned char TL1_Low=0;
unsigned char TH1_High=0;
unsigned char TL1_High=0;	
unsigned char cur_sta[3]={1,1,1};
unsigned char pre_sta[3]={1,1,1};
unsigned char code led_mode[11]={0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x00};//0~9????
unsigned char Disp_Code[4]={0x00,0x00,0x00,0x00};
void PWM_config(unsigned char ratio, unsigned int period)
{
	//ratio占空比，period周期以微秒为单位
	unsigned int HighvoltTime,LowvoltTime;
	HighvoltTime=period/100*ratio;
	LowvoltTime=period-HighvoltTime;
	TH1_Low=(65536-LowvoltTime)>>8;
	TL1_Low=(65536-LowvoltTime)%256;
	TH1_High=(65536-HighvoltTime)>>8;
	TL1_High=(65536-HighvoltTime)%256;
}

void delay_ms(unsigned char z)
{
  unsigned int i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void  SegLed_Scan()
{
	static unsigned char i=0;
  Wx_port=(0xfe<<i)|(0xfe>>(8-i));//????
  Dx_port=Disp_Code[i];
	delay_ms(1);//?????
	Dx_port=0x00;//??
	i++;
	if(i==N) i=0;
}

void timer1_config()
{
  TMOD=0x11; 
	TR0=1;//?????1
	TR1=0;//?????1
  TH0=63536>>8;//???????
	TL0=63536%256;
	
	EA=1;//?????
	ET0=1;//???1?????
	ET1=1;
}
void Disp_Code_update()
{
	min--;
	if(min<0)
	{
	  min=59;
		CoundDownHour=CoundDownHour-1;
	}
    Disp_Code[3]=led_mode[min%10];
	  Disp_Code[2]=led_mode[min/10];
	  Disp_Code[1]=led_mode[CoundDownHour%10];
	  Disp_Code[0]=led_mode[CoundDownHour/10];
	if (min==0 && CoundDownHour==0)
	{
		fan_IO=0;
		T_minus_mode=0;
	}

}

unsigned char key_scan()
{ unsigned char i=0;
	unsigned char key_value=0;
	for(i=0;i<3;i++)
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
	return(key_value);
}
void cur_sta_update()
{
	unsigned char i=0;
	static unsigned char buff[3]={0xff,0xff,0xff};
  buff[0]=(buff[0]<<1)|key1;
  buff[1]=(buff[1]<<1)|key2;
  buff[2]=(buff[2]<<1)|key3;
	for(i=0;i<3;i++)
	{
		if(buff[i]==0xff)
			cur_sta[i]=1;
		else if(buff[i]==0x00)
			cur_sta[i]=0;
  }
}
void key_action(unsigned char key_value)
{
	static unsigned char level=1;
	 if(key_value==1) //???????
	 {
			TR1=~TR1;
		 if(TR1==0)
			 fan_IO=0;
	 }
	 else if(key_value==2)
	 {
		 if(TR1) //???????
		 {
			 level++;
			 if(level==4) level=1;
			 PWM_config(20*level,10000);
		 }
	 }
	 else if(key_value==3)
	 {
		 T_minus_mode=1;
	   CoundDownHour+=2;
		 if(CoundDownHour>8)
		 {
			 CoundDownHour=0;
			 T_minus_mode=0;
		 }
		 if(T_minus_mode)
		 {
		   Disp_Code[3]=led_mode[min%10];
	     Disp_Code[2]=led_mode[min/10];
	     Disp_Code[1]=led_mode[CoundDownHour%10];
	     Disp_Code[0]=led_mode[CoundDownHour/10];
		 }
		 else
		 {
			 Disp_Code[3]=0x00;
			 Disp_Code[2]=0x00;
			 Disp_Code[1]=0x00;
	     Disp_Code[0]=0x00;
			 min=0;
			 CoundDownHour=0;
		 }
	 }
}

void main()
{
	unsigned char key_value=0;
	timer1_config();//配置定时器
	fan_IO=0;// 初始状态风扇关闭
  PWM_config(20,10000);
	while(1)
  {
		SegLed_Scan();
		key_value=key_scan();
	  key_action(key_value);

  }
	 
}

void timer0_2ms() interrupt 1
{
  static unsigned int count=0;
	TH0=63536>>8;
	TL0=63536%256;
  cur_sta_update();
	if(T_minus_mode)
	{
	  count++;
		if(count==500) 
		{
		  count=0;
			Disp_Code_update();
		}
	}
}

void timer1_pwm() interrupt 3
{
	if(fan_IO)
	 {
     TH1=TH1_Low;
	   TL1=TL1_Low;	  
	 }
	 else
	 {
	   TH1=TH1_High;
	   TL1=TL1_High;		 
	 }
	 fan_IO=~fan_IO;
}