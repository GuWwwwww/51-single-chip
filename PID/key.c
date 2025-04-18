#include<reg51.h>
sbit k1=P1^0;
sbit k2=P1^1;
sbit k3=P1^2;
unsigned char cur_sta[3]={1,1,1};
unsigned char pre_sta[3]={1,1,1};
void cur_sta_update()
{
	static unsigned char buff[3]={0xff,0xff,0xff};
	unsigned char i=0;	
	buff[0]=(buff[0]<<1)|k1;//把新抽样的值加到buff中
	buff[1]=(buff[1]<<1)|k2;//把新抽样的值加到buff中
	buff[2]=(buff[2]<<1)|k3;//把新抽样的值加到buff中
	for(i=0;i<3;i++)
	{
		if (buff[i]==0xff)//8次采样全为1
			cur_sta[i]=1;
		else if (buff[i]==0x00)//全为0
			cur_sta[i]=0;
	}
}

char key_scan()
{
	unsigned char key_value=-1;
	unsigned char i=0;
	for(i=0;i<3;i++)
	{
		{
			if(cur_sta[i]!=pre_sta[i])
			{
				if(pre_sta[i])
				{
					key_value=i;
				}
				pre_sta[i]=cur_sta[i];
			}
		}
	}
	return(key_value);//返回按键值
}
