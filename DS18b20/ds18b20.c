#include<reg51.h>
#define Real_temp -1  // ����һ����ʾ��ȡʧ�ܵ��¶�ֵ
sbit RS=P2^6; //��������ѡ��˿�
sbit RW=P2^4; //д������ѡ��
sbit E=P2^7;  //LCDʹ�ܶ˿�
sbit DQ=P3^7;	//����DS18b20�Ĺܽ�

void delay(unsigned int t)//��ʱ�ӳ���
{
	for(;t>0;t--);
}


unsigned char Init_DS18B20()//��ʼ��DS18B20
{
  unsigned char x=0;
  DQ = 1;      //DQ��λ
  delay(8);    //������ʱ
  DQ = 0;      //��Ƭ����DQ����
  delay(80);   //��ȷ��ʱ������480us
  DQ = 1;      //��������
  delay(8);
  x = DQ;      //������ʱ�����x=0���ʼ���ɹ���x=1���ʼ��ʧ��
  delay(4);
  return x;
}

unsigned char ReadOneChar()//��һ���ֽ�
{
  unsigned char i=0;
  unsigned char dat = 0;
  for (i=8;i>0;i--)
  {
    DQ = 0;     // �������ź�
    dat>>=1;
    DQ = 1;     // �������ź�
    if(DQ)
    	dat|=0x80;
	delay(4);
  }
  return(dat);
}

/*****дһ���ֽ�*****/
void WriteOneChar(unsigned char dat)
{
  unsigned char i=0;
  for (i=8; i>0; i--)
  {
    DQ = 0;
    DQ = dat&0x01;
	delay(4);
    DQ = 1;
    dat>>=1;
  }
  delay(4);
}

/*****��ȡ�¶�*****/
int ReadTemperature()
{
  	unsigned char a=0;
  	unsigned char b=0;
  	unsigned int t=0;
  	t=Init_DS18B20();
  	if(t) return Real_temp;
  	WriteOneChar(0xCC);  //����������кŵĲ���
  	WriteOneChar(0x44);  //�����¶�ת��
  	t=Init_DS18B20();
  	if(t) return Real_temp;
  	WriteOneChar(0xCC);  //����������кŵĲ���
  	WriteOneChar(0xBE);  //��ȡ�¶ȼĴ���
  	a=ReadOneChar();     //����8λ
  	b=ReadOneChar();     //����8λ
  	t=b;
  	t<<=8;
  	t=t|a;
  	if(t<=0||t>0x900) 
	return Real_temp;
	t=t*0.625+0.5;
  	return(t);
}

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

int Pow(int x,int y)
{
	unsigned char i;
	int result = 1;
	for(i = 0; i < y; i++)
	{
		result *= x;
	}
	return result;
}

void lcd_ShowNum(unsigned char x,unsigned char y,unsigned int number,unsigned char length)
{
	unsigned char i;
	unsigned char temp;
	cursor_set(x,y);
	for(i=length;i>0;i--)
	{
		temp=number/Pow(10,i-1)%10+'0';	//ѭ����ÿһλ����ȡ������ת��Ϊ�ַ�
		lcd_write_dat(temp);
	}
}

void main()
{
	unsigned char t;
	while(Init_DS18B20());
	lcd_init();
	while(1)
	{
		t=ReadTemperature();
		lcd_ShowNum(0,5,t,2);
	}
}
