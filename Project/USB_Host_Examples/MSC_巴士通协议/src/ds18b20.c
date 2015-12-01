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
		DS18B20_IO_OUT();//输出
		DS18B20_DQ_Low;	 //拉低
		delay_us(4);//延时4微妙
		DS18B20_DQ_High;
		delay_us(10);//延时10微妙
		DS18B20_IO_IN();

		if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_8)==1)
		{
		   TempData|=0x80;//读数据 从低位开始
		}

		delay_us(45);//延时45微妙
	}

	return TempData;	
}

void DS18B20_Write_Byte(u8 dat)
{
	u8 i=0;
	DS18B20_IO_OUT();//输出

	for(i=0;i<8;i++)
	{
		DS18B20_DQ_Low;	 //拉低
		delay_us(15);//延时15微妙
		
		if((dat&0x01)==1)
		{
			DS18B20_DQ_High;
		}
		else
		{
			DS18B20_DQ_Low;
		}
		delay_us(60);//延时60微妙
		DS18B20_DQ_High;
		
		dat>>=1;//准备下一位数据的发送	
	}
}

void DS18B20_Reset(void)
{
	DS18B20_IO_OUT();//输出
	DS18B20_DQ_Low;
	delay_us(480);	
	DS18B20_DQ_High;
	delay_us(480);//延时480微妙	
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
	DS18B20_Write_Byte(0xCC); //跳过ROM命令
	DS18B20_Write_Byte(0x44); //温度转换命令

	delay_ms(800);//延时800毫秒
	DS18B20_Reset();//复位
	DS18B20_Write_Byte(0xCC); //跳过ROM命令
	DS18B20_Write_Byte(0xBE); //读温度命令

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

