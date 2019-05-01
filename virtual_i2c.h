// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  *******************************************************************************
  * @file			virtual_i2c.h
  * @author		Yoli
  * @version	V1.0.0
  * @date			2017-4-16
  * @brief		Use GPIO virtual i2c bus protocol
  ********************************************************************************
  */

#ifndef VIRTUAL_I2C_H
#define VIRTUAL_I2C_H

#ifdef __cplusplus
extern "C"	{
#endif

#include "stm32f1xx_hal_def.h"
#include "common.h"

#define I2C_WR		0
#define I2C_RD		1

#define __VIRTUAL_I2C_SET_SLAVE_7ADDR(__HANDLE__, __VALUE__)		st((__HANDLE__).i2c7BitAddr = (__VALUE__))
#define __VIRTUAL_I2C_SET_SLAVE_10ADDR(__HANDLE__, __VALUE__)		st((__HANDLE__).i2c7BitAddr = (__VALUE__))

typedef enum _I2CSPEED 
{
	I2C_FAST_400KHz = 5,
	I2C_STANDARD_100KHz = 20
} I2cSpeed_e;

typedef struct _VirtualI2cProtPin 
{
	GPIO_TypeDef* gpioPort;
	uint16_t	i2cSclPin;
	uint16_t	i2cSdaPin;
}	VirtualI2cPortPin_s;

typedef struct _VirtualI2C 
{
	VirtualI2cPortPin_s sI2cPortPin;
	I2cSpeed_e eI2cSpeed;		// scl clock speed
	
	union 
	{
		uint16_t	i2c10BitAddr;
		uint8_t i2c7BitAddr;
	} I2cAddr_un;
	
	// function pointer for i2c delay
	void (*I2C_Delay) (I2cSpeed_e i2cSpeed);
} VirtualI2C_s;

void i2cDelay(I2cSpeed_e i2cSpeed);
void i2cStart(VirtualI2C_s sVirtualI2c);
void i2cStop(VirtualI2C_s sVirtualI2c);
void i2cSendByte(VirtualI2C_s sVirtualI2c, uint8_t ucByte);
uint8_t i2cReadByte(VirtualI2C_s sVirtualI2c);
uint8_t i2cWaitAck(VirtualI2C_s sVirtualI2c);
void i2cAck(VirtualI2C_s sVirtualI2c);
void i2cNAck(VirtualI2C_s sVirtualI2c);
uint8_t i2cCheckDevice(VirtualI2C_s sVirtualI2c);

#ifdef __cplusplus
}
#endif

#endif

/**************************************** Copyright (C) 2019 Yoki **************************************/
