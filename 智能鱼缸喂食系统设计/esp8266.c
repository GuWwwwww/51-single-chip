#include <reg51.h>
#include <string.h>
sbit led=P2^0;
extern void delay_ms(unsigned char z);
extern void motor_forward_circle();
extern bit feed_flag;
extern unsigned char portion;

void delay_10us(unsigned int ten_us)
{
	while(ten_us--);	
}
 
void UART_Send(char dat) 
{
  SBUF=dat;
	while(TI==0);//等待发送完毕
	TI=0;//为下一次发送做准备
}

//ESP8266-WIFI模块发送命令 
void ESP8266_SendCmd(unsigned char *pbuf)
{
	while(*pbuf!='\0') //遇到空格跳出循环	
	{
		UART_Send(*pbuf);
		delay_10us(1000);
		pbuf++;	
	}
}

//ESP8266-WIFI模块工作模式初始化
void ESP8266_ModeInit()
{
	ESP8266_SendCmd("AT+CIPMUX=1\r\n");	//开启多连接模式，允许多个各客户端接入
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
	ESP8266_SendCmd("AT+CIPSERVER=1,8080\r\n");	//启动TCP/IP 端口为8080实现基于网络控制	
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
}

void UART_Receive() interrupt 4 
{
	char received;
	if(RI==1) 
	{
		RI=0; //清除接收标志
		received=SBUF; //读取接收数据
		if(strcmp(&received, "w")==0) 
		{
			motor_forward_circle();   // 执行喂食任务
			feed_flag=1;
			ESP8266_SendCmd("AT+CIPSEND=0,18\r\n"); // 发送给 ID 为 0 的连接，数据长度为 18
			ESP8266_SendCmd("Feeding completed\r\n");

		}
		else if(strcmp(&received, "s")==0)//调整为小份
		{
			portion=0;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:S\r\n");
		}
		else if(strcmp(&received, "m")==0)//调整为中份
		{
			portion=1;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:M\r\n");
		}
		else if(strcmp(&received, "l")==0)//调整为大份
		{
			portion=2;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:L\r\n");
		}
	}
}
