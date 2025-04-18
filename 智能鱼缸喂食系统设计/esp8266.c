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
	while(TI==0);//�ȴ��������
	TI=0;//Ϊ��һ�η�����׼��
}

//ESP8266-WIFIģ�鷢������ 
void ESP8266_SendCmd(unsigned char *pbuf)
{
	while(*pbuf!='\0') //�����ո�����ѭ��	
	{
		UART_Send(*pbuf);
		delay_10us(1000);
		pbuf++;	
	}
}

//ESP8266-WIFIģ�鹤��ģʽ��ʼ��
void ESP8266_ModeInit()
{
	ESP8266_SendCmd("AT+CIPMUX=1\r\n");	//����������ģʽ�����������ͻ��˽���
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
	delay_ms(250);
	ESP8266_SendCmd("AT+CIPSERVER=1,8080\r\n");	//����TCP/IP �˿�Ϊ8080ʵ�ֻ����������	
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
		RI=0; //������ձ�־
		received=SBUF; //��ȡ��������
		if(strcmp(&received, "w")==0) 
		{
			motor_forward_circle();   // ִ��ιʳ����
			feed_flag=1;
			ESP8266_SendCmd("AT+CIPSEND=0,18\r\n"); // ���͸� ID Ϊ 0 �����ӣ����ݳ���Ϊ 18
			ESP8266_SendCmd("Feeding completed\r\n");

		}
		else if(strcmp(&received, "s")==0)//����ΪС��
		{
			portion=0;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:S\r\n");
		}
		else if(strcmp(&received, "m")==0)//����Ϊ�з�
		{
			portion=1;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:M\r\n");
		}
		else if(strcmp(&received, "l")==0)//����Ϊ���
		{
			portion=2;
			ESP8266_SendCmd("AT+CIPSEND=0,15\r\n");
			ESP8266_SendCmd("Feeding mode:L\r\n");
		}
	}
}
