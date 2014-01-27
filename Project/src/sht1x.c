/*
 * =====================================================================================
 *
 *       Filename:  sht1x.c
 *
 *    Description:  SHT1x温湿度传感器操作
 *
 *        Version:  
 *        Created:  
 *       Revision:  
 *       Compiler:  
 *
 *         Author:  zhangyuxiang
 *   Organization:  
 *
 * =====================================================================================
 */

#include "stm32f10x.h"
#include "common.h"
#include "systick.h"
#include "sht1x.h"

#define CMD_MEASURE_TEMP 0x03
#define CMD_MEASURE_HUMI 0x05

const float T_d1 = -39.6, T_d2 = 0.01;
const float H_c1 = -2.0468, H_c2 = 0.0367, H_c3 = -1.5955E-6;
const float H_t1 = 0.01, H_t2 = 0.00008;

void SHT1x_Config()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_GPIOClockCmd(SHT_PORT, ENABLE); 

	GPIO_InitStructure.GPIO_Pin = SHT_DATA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SHT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SHT_SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SHT_PORT, &GPIO_InitStructure);
}

static void SHT1xWriteByte(uint8_t val)
{
	uint8_t i;
	for(i=0x80; i>0; i>>=1){
		GPIO_WriteBit(SHT_PORT, SHT_DATA, (i&val) ? Bit_SET : Bit_RESET);
		GPIO_SetBits(SHT_PORT, SHT_SCK);
		Delay_us(3);
		GPIO_ResetBits(SHT_PORT, SHT_SCK);
		Delay_us(1);
	}
	GPIO_SetBits(SHT_PORT, SHT_DATA|SHT_SCK);
	Delay_us(3);
	GPIO_ResetBits(SHT_PORT, SHT_SCK);
	Delay_us(1);
}

static uint8_t SHT1xReadByte(uint8_t ack)
{
	uint8_t i, ret = 0;
	GPIO_SetBits(SHT_PORT, SHT_DATA); //开漏模式下释放数据线 
	for(i=0x80; i>0; i>>=1){
		GPIO_SetBits(SHT_PORT, SHT_SCK);
		Delay_us(3);
		if(GPIO_ReadInputDataBit(SHT_PORT, SHT_DATA))
			ret |= i;
		GPIO_ResetBits(SHT_PORT, SHT_SCK);
		Delay_us(1);
	}
	GPIO_WriteBit(SHT_PORT, SHT_DATA, ack ? Bit_RESET : Bit_SET);
	GPIO_SetBits(SHT_PORT, SHT_SCK);
	Delay_us(3);
	GPIO_ResetBits(SHT_PORT, SHT_SCK);
	Delay_us(3);
	GPIO_SetBits(SHT_PORT, SHT_DATA); //开漏模式下释放数据线 

	return ret;
}

static void SHT1xStartTrans(void)
{
	GPIO_SetBits(SHT_PORT, SHT_DATA); //开漏模式下释放数据线 
	GPIO_ResetBits(SHT_PORT, SHT_SCK);
	Delay_us(2);
	GPIO_SetBits(SHT_PORT, SHT_SCK);
	Delay_us(2);
	GPIO_ResetBits(SHT_PORT, SHT_DATA);
	Delay_us(2);
	GPIO_ResetBits(SHT_PORT, SHT_SCK);
	Delay_us(3);
	GPIO_SetBits(SHT_PORT, SHT_SCK);
	Delay_us(2);
	GPIO_SetBits(SHT_PORT, SHT_DATA);
	Delay_us(2);
	GPIO_ResetBits(SHT_PORT, SHT_SCK);
	Delay_us(2);
}

static void SHT1xResetConn(void)
{
	uint8_t i;
	GPIO_SetBits(SHT_PORT, SHT_DATA); //开漏模式下释放数据线 
	GPIO_ResetBits(SHT_PORT, SHT_SCK);

	for(i=0; i<9; i++)
	{
		GPIO_SetBits(SHT_PORT, SHT_SCK);
		Delay_us(3);
		GPIO_ResetBits(SHT_PORT, SHT_SCK);
		Delay_us(1);
	}

	SHT1xStartTrans();
}

int SHT1x_MeasureTemp(float *temperature)
{
	uint16_t val;

	SHT1xStartTrans();
	SHT1xWriteByte(CMD_MEASURE_TEMP);
	Delay_ms(320);
	if(GPIO_ReadInputDataBit(SHT_PORT, SHT_DATA))
	{
		//温度测量超时
		SHT1xResetConn();
		return -1;
	}

	val = SHT1xReadByte(1);
	val = ((val << 8)|SHT1xReadByte(1));
	SHT1xReadByte(0); // checksum

	*temperature = T_d1 + T_d2*val;

	return 0;
}

int SHT1x_MeasureHumi(float *humidity, const float temp_comp)
{
	uint16_t val;

	SHT1xStartTrans();
	SHT1xWriteByte(CMD_MEASURE_HUMI);
	Delay_ms(80);
	if(GPIO_ReadInputDataBit(SHT_PORT, SHT_DATA))
	{
		//湿度测量超时
		SHT1xResetConn();
		return -1;
	}

	val = SHT1xReadByte(1);
	val = ((val << 8)|SHT1xReadByte(1));
	SHT1xReadByte(0); // checksum

	*humidity = H_c1 + H_c2*val + H_c3*val*val;
	*humidity = (temp_comp - 25)*(H_t1 + H_t2*val) + *humidity;

	return 0;
}
