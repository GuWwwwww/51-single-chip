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
bit feed_mode=0; //ιʳ��ģʽ���ֶ�/�Զ���Ĭ��Ϊ�Զ�
bit feed_flag=0;//����Ŀ���״̬
bit timer_flag=1;//��ʱͶι���ܿ��أ�Ĭ��Ϊ��
bit flash_flag=0;//����ʱ����˸��־λ
sbit beep=P2^5;
unsigned char portion=0;
unsigned char adj_mode=0;
unsigned int t=0;
char timer_hour=6;//Ĭ���Զ�ιʳ���6Сʱһ��
char timer_min=0;
char timer_sec=0;
char timer[3]={6,0,0};//������úõĶ�ʱʱ�� 

void delay_ms(unsigned char z)
{
	unsigned char i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

void time_set()//����ʱ��
{
	if(key_value==2)
	{
		TimeSetSelect++;//ʱ������ѡ��λ��1
		TimeSetSelect%=7;//����
	}
	else if(key_value==3)
	{
		DS1302_Time[TimeSetSelect]++;//ʱ������λ��ֵ��1
		if(DS1302_Time[0]>99){DS1302_Time[0]=0;}//��Խ���ж�
		if(DS1302_Time[1]>12){DS1302_Time[1]=1;}//��Խ���ж�
		if( DS1302_Time[1]==1 || DS1302_Time[1]==3 || DS1302_Time[1]==5 || DS1302_Time[1]==7 || 
			DS1302_Time[1]==8 || DS1302_Time[1]==10 || DS1302_Time[1]==12)//��Խ���ж�
		{
			if(DS1302_Time[2]>31){DS1302_Time[2]=1;}//����
		}
		else if(DS1302_Time[1]==4 || DS1302_Time[1]==6 || DS1302_Time[1]==9 || DS1302_Time[1]==11)
		{
			if(DS1302_Time[2]>30){DS1302_Time[2]=1;}//С��
		}
		else if(DS1302_Time[1]==2)
		{
			if(DS1302_Time[0]%4==0)
			{
				if(DS1302_Time[2]>29){DS1302_Time[2]=1;}//����2��
			}
			else
			{
				if(DS1302_Time[2]>28){DS1302_Time[2]=1;}//ƽ��2��
			}
		}
		if(DS1302_Time[3]>23){DS1302_Time[3]=0;}//ʱԽ���ж�
		if(DS1302_Time[4]>59){DS1302_Time[4]=0;}//��Խ���ж�
		if(DS1302_Time[5]>59){DS1302_Time[5]=0;}//��Խ���ж�
		if(DS1302_Time[6]>7){DS1302_Time[5]=1;}//����Խ���ж�
	}
	
	//������ʾ������TimeSetSelect��flash_flag�жϿ������˸����
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

void timer_set()//���ö�ʱʱ��
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

void key_action() //����ģ��
{
	if(key_value==1)
	{
		//�ɵ���ģʽ��ģʽ0�¿��Ե���ιʳ������
		//ģʽ1�¿��Ե���ʱ��
		//ģʽ2�¿��������Զ���ʱͶιʱ������Ҳ���Կ������´�Ͷιʱ��
		adj_mode++;
		adj_mode%=3;
		TimeSetSelect=0;
		timerselect=0;
		lcd_write_cmd(0x01);//����
	}
	else if(key_value==2)
	{
		if(adj_mode==0)  //����С�д�����ιʳ����
		{
			portion++;
			portion%=3;
		}
		else if(adj_mode==2)
		{
			timerselect++;//��ʱʱ������ѡ��λ��1
			timerselect%=4;
		}
	}
	else if(key_value==3)
	{
		if(adj_mode==0)
		{	
			feed_mode=!feed_mode;//�л�ιʳģʽ���Զ�/�ֶ�
			if (feed_mode==1)
			{
				timer_flag=0;
				timer_hour=timer[0];//��ʱͶιһ�κ󣬻ص����õĶ�ʱʱ��
				timer_min=timer[1];
				timer_sec=timer[2];
			}
			else 
				timer_flag=1;
		}
		else if (adj_mode==2)
		{
			timer_set();//��ʱ����
		}
	}
	else if(key_value==4)
	{
		//�ֶ�ιʳ��ť
		if(feed_mode)
		{
			motor_forward_circle();
			feed_flag=1;
		}
	}
}

void lcd_show() //lcd��ʾģ��
{
	if(adj_mode==2) //���ö�ʱģʽ
	{
		lcd_show_str(1,4,"  :  :  ");//����ʱ�����ʽ
		lcd_show_str(0,1,"Set Timer:");
		lcd_ShowNum(1,4,timer_hour,2);
		lcd_ShowNum(1,7,timer_min,2);
		lcd_ShowNum(1,10,timer_sec,2);
	}
	else
	{
		lcd_show_str(0,1,"  -  -  ");//���������ո�ʽ
		lcd_show_str(1,1,"  :  :  ");//����ʱ�����ʽ
		DS1302_ReadTime();//��ȡ�ڲ�ʱ��
		lcd_ShowNum(0,1,DS1302_Time[0],2);//��ʾ��
		lcd_ShowNum(0,4,DS1302_Time[1],2);//��ʾ��
		lcd_ShowNum(0,7,DS1302_Time[2],2);//��ʾ��
		lcd_ShowNum(1,1,DS1302_Time[3],2);//��ʾʱ
		lcd_ShowNum(1,4,DS1302_Time[4],2);//��ʾ��
		lcd_ShowNum(1,7,DS1302_Time[5],2);//��ʾ��
		lcd_ShowNum(1,11,DS1302_Time[6],1);//��ʾ����
		
		// LCD����ʾ�Զ�/�ֶ�ģʽ
		if(!feed_mode)
			lcd_show_str(0,11,"AUTO");
		else
			lcd_show_str(0,11,"HAND");
		
		// LCD����ʾ�Զ�Ͷι�ķ���
		if(portion==0)
			lcd_show_str(1,14,"S");
		else if(portion==1)
			lcd_show_str(1,14,"M");
		else if(portion==2)
			lcd_show_str(1,14,"L");
		
	}
}

void timer_config()//��ʱ����ʼ�� 
{
	SCON=0x50;  // ����ģʽ1��8λ���ݣ�1λֹͣλ
	TMOD=0x21; //��ʱ��������ʽΪ1
	//��ʱ�������ĳ�ʼֵ����
	TH0=63536>>8;  //(65536-x)>>8
	TL0=63536;  //(65536-x)%256
	TH1=0xFD;   // ������9600
  	TL1=0xFD;
	SM0 = 0;    // ����ģʽ1
  	SM1 = 1;
  	REN = 1;   // �������
	EA=1;  //�ж�������
	ET0=1; //��ʱ��1�жϷ�����
	ES=1; //���ڶ�ʱ��������
	TR0=1; //�򿪶�ʱ������0
	TR1=1; //�򿪶�ʱ������1
}

void main()
{
	timer_config();//��ʱ����ʼ��
	ESP8266_ModeInit();//WIFI�������ó�ʼ��
	lcd_init();//lcd��ʼ��
	DS1302_Init();//DS1302��ʼ��
	lcd_show_str(0,1,"  -  -  ");//���������ո�ʽ
	lcd_show_str(1,1,"  :  :  ");//����ʱ�����ʽ
	DS1302_SetTime();//����ʱ�䣬ͨ�������������
	while(1)
	{
		key_value=key_scan();
		key_action();
		if(adj_mode==1)//����ʱ��ģʽ 
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
	TH0=63536>>8;  //�������ó�ʼֵ
	TL0=63536;
	cur_sta_update();
	flash_count++;
	if(flash_count==250)//ÿ0.5s����һ��
	{
		flash_count=0;
		flash_flag=!flash_flag;//��˸��־λȡ��
	
		if(feed_flag) //����ʱ����㣬С��2s���з�3s�����4s
		{
			count++;
			if((portion==0 && count==4)||(portion==1 && count==6)||(portion==2 && count==8))
			{
				count=0;
				feed_flag=0;
				motor_reverse_circle();
				lcd_write_cmd(0x01);//����
			}
		}
	
		if(timer_flag)//��ʱʱ��ݼ�
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
							timer_hour=timer[0];//��ʱͶιһ�κ󣬻ص����õĶ�ʱʱ��
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
