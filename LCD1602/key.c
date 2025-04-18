#include<reg51.h>
sbit k1=P3^0;
sbit k2=P3^1;
sbit k3=P3^2;
sbit k4=P3^3;
sbit o1=P3^4;
sbit o2=P3^5;
sbit o3=P3^6;
sbit o4=P3^7;
sbit o5=P1^0;
unsigned char cur_sta[4][5]={{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};
unsigned char pre_sta[4][5]={{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1},{1,1,1,1,1}};

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