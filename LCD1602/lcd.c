#include<reg51.h>
sbit RS=P2^0; //��������ѡ��˿�
sbit RW=P2^1; //д������ѡ��
sbit E=P2^2;  //LCDʹ�ܶ˿�

void delay_ms(unsigned char z)
{
	unsigned char i,j;
	for(i=0;i<z;i++)
		for(j=0;j<125;j++);
}

unsigned char lcd_read_state() //��״̬��ȷ��LCD�Ƿ�æ
{
	unsigned char state;
	RS=0;
	RW=1;
	E=1; //LCDʹ��
	delay_ms(2);
	state=P0&0X80;
	E=0;//�ر�ʹ��
	return(state);
}

void lcd_write_cmd(unsigned cmd)
{
	while(lcd_read_state());//�ȴ���æ����
	RS=0;//��ʾд�����ָ��
	RW=0;//д��״̬
	P0=cmd;
	E=1;//ʹ��LCD
	delay_ms(2);
	E=0;//�������ϣ��ر�ʹ��
}

void lcd_write_dat(unsigned char dat)
{
	while(lcd_read_state());//�ȴ���æ����
	RS=1;//��ʾд���������
	RW=0;//д��״̬
	P0=dat;
	E=1;//ʹ��LCD
	delay_ms(2);
	E=0;//���ݷ�����ϣ��ر�ʹ��
}

void lcd_init()
{
	lcd_write_cmd(0x38);//5x7��ʾ��8λ�������ߣ�������ʾ
	lcd_write_cmd(0x0c);//����ʾ����겻��ʾ
	lcd_write_cmd(0x06);//������ƣ��������ƶ�
	lcd_write_cmd(0x01);//����
	delay_ms(10);
}

void cursor_set(unsigned char x, unsigned char y)
{
	unsigned char addr=0;
	if (x==0)//������һ��
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
	//���ַ�����ʾ���ض���λ��
	char i=0;
	cursor_set(x,y);
	while(str[i]!='\0')
	{
		lcd_write_dat(str[i]);
		i++;
	}
}
