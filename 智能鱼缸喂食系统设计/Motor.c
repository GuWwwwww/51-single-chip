#include <reg51.h>
sbit IN1 = P1^0;
sbit IN2 = P1^1;
sbit IN3 = P1^2;
sbit IN4 = P1^3;
extern void delay_ms(unsigned char z);

//半步激励模式的步进序列
void step_forward()
{
	//顺时针转动
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 0;  //第1步
	delay_ms(2);
	IN1 = 1; IN2 = 1; IN3 = 0; IN4 = 0;  //第2步
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 0; IN4 = 0;  //第3步
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 1; IN4 = 0;  //第4步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 0;  //第5步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 1;  //第6步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 1;  //第7步
	delay_ms(2);
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 1;  //第8步
	delay_ms(2);
}

void step_reverse()
{
	//逆时针转动
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 1;  // 第8步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 0; IN4 = 1;  // 第7步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 1;  // 第6步
	delay_ms(2);
	IN1 = 0; IN2 = 0; IN3 = 1; IN4 = 0;  // 第5步
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 1; IN4 = 0;  // 第4步
	delay_ms(2);
	IN1 = 0; IN2 = 1; IN3 = 0; IN4 = 0;  // 第3步
	delay_ms(2);
	IN1 = 1; IN2 = 1; IN3 = 0; IN4 = 0;  // 第2步
	delay_ms(2);
	IN1 = 1; IN2 = 0; IN3 = 0; IN4 = 0;  // 第1步
	delay_ms(2);
}

void motor_forward_circle()
{
  //顺时针转动半圈（512步=360°）
  int i=0;
  for (i=0;i<256;i++) 
	step_forward();
}

void motor_reverse_circle()
{
	//逆时针转动半圈
	int i=0;
	for (i=0;i<256;i++)
		step_reverse();
}
