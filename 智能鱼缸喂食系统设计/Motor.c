#include <reg51.h>
sbit IN1 = P1^0;
sbit IN2 = P1^1;
sbit IN3 = P1^2;
sbit IN4 = P1^3;
extern void delay_ms(unsigned char z);

//�벽����ģʽ�Ĳ�������
void step_forward()
{
	//˳ʱ��ת��
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 0;  //��1��
	delay_ms(2);
	IN1 = 1; IN2 = 1; IN3 = 0; IN4 = 0;  //��2��
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 0; IN4 = 0;  //��3��
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 1; IN4 = 0;  //��4��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 0;  //��5��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 1;  //��6��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 1;  //��7��
	delay_ms(2);
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 1;  //��8��
	delay_ms(2);
}

void step_reverse()
{
	//��ʱ��ת��
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 1;  // ��8��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 1;  // ��7��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 1;  // ��6��
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 0;  // ��5��
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 1; IN4 = 0;  // ��4��
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 0; IN4 = 0;  // ��3��
	delay_ms(2);
	IN1 = 1; IN2 = 1; IN3 = 0; IN4 = 0;  // ��2��
	delay_ms(2);
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 0;  // ��1��
	delay_ms(2);
}

void motor_forward_circle()
{
  //˳ʱ��ת����Ȧ��512��=360�㣩
  int i=0;
  for (i=0;i<256;i++) 
	step_forward();
}

void motor_reverse_circle()
{
	//��ʱ��ת����Ȧ
	int i=0;
	for (i=0;i<256;i++)
		step_reverse();
}
