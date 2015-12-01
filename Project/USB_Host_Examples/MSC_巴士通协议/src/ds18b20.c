#include "ds18b20.h"

void DS18B20_IO_IN(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

void DS18B20_IO_OUT(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC,&GPIO_InitStructure);
}

u8 DS18B20_Read_Byte(void)
{
	u8 i=0,TempData=0;
	for(i=0;i<8;i++)
	{
		TempData>>=1;
		DS18B20_IO_OUT();//���
		DS18B20_DQ_Low;	 //����
		delay_us(4);//��ʱ4΢��
		DS18B20_DQ_High;
		delay_us(10);//��ʱ10΢��
		DS18B20_IO_IN();

		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==1)
		{
		   TempData|=0x80;//������ �ӵ�λ��ʼ
		}

		delay_us(45);//��ʱ45΢��
	}

	return TempData;	
}

void DS18B20_Write_Byte(u8 dat)
{
	u8 i=0;
	DS18B20_IO_OUT();//���

	for(i=0;i<8;i++)
	{
		DS18B20_DQ_Low;	 //����
		delay_us(15);//��ʱ15΢��
		
		if((dat&0x01)==1)
		{
			DS18B20_DQ_High;
		}
		else
		{
			DS18B20_DQ_Low;
		}
		delay_us(60);//��ʱ60΢��
		DS18B20_DQ_High;
		
		dat>>=1;//׼����һλ���ݵķ���	
	}
}

void DS18B20_Reset(void)
{
	DS18B20_IO_OUT();//���
	DS18B20_DQ_Low;
	delay_us(480);	
	DS18B20_DQ_High;
	delay_us(480);//��ʱ480΢��	
}

void DS18B20_Init(void)
{
	DS18B20_Reset();
}

double DS18B20_Get_wd(void)
{
	u8 TL=0,TH=0;
	u16 temp=0;
	double wd=0;

	DS18B20_Reset();
	DS18B20_Write_Byte(0xCC); //����ROM����
	DS18B20_Write_Byte(0x44); //�¶�ת������

	delay_ms(800);//��ʱ800����
	DS18B20_Reset();//��λ
	DS18B20_Write_Byte(0xCC); //����ROM����
	DS18B20_Write_Byte(0xBE); //���¶�����

	TL=DS18B20_Read_Byte();//LSB
	TH=DS18B20_Read_Byte();//MSB

	temp=TH;
	temp=(temp<<8)+TL;

	if((temp&0xF800)==0xF800)
	{
		temp=~temp;
		temp=temp+1;
		wd=temp*(-0.0625);
	}
	else
	{
		wd=temp*0.0625;	
	}

	return wd;
}

