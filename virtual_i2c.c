// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  *******************************************************************************
  * @file		virtual_i2c.c
  * @author		Yoki
  * @version		V1.0.0	
  * @date		2017-4-16
  * @brief		Use GPIO virtual i2c bus protocol
  ********************************************************************************
  */


#include "stm32f1xx_hal.h"
#include "virtual_i2c.h"

#define VI2C_SCL_HIGH(_VIRTUAL_I2C_)		(_VIRTUAL_I2C_.gpioPort->BSRR |= _VIRTUAL_I2C_.i2cSclPin)
#define VI2C_SCL_LOW(_VIRTUAL_I2C_)			(_VIRTUAL_I2C_.gpioPort->BRR |= _VIRTUAL_I2C_.i2cSclPin)

#define VI2C_SDA_HIGH(_VIRTUAL_I2C_)		(_VIRTUAL_I2C_.gpioPort->BSRR |= _VIRTUAL_I2C_.i2cSdaPin)
#define VI2C_SDA_LOW(_VIRTUAL_I2C_)			(_VIRTUAL_I2C_.gpioPort->BRR |= _VIRTUAL_I2C_.i2cSdaPin)

#define VI2C_SDA_READ(_VIRTUAL_I2C_)		((_VIRTUAL_I2C_.gpioPort->ODR & _VIRTUAL_I2C_.i2cSdaPin) ? 1 : 0)
#define VI2C_SCL_READ(_VIRTUAL_I2C_)		((_VIRTUAL_I2C_.gpioPort->ODR & _VIRTUAL_I2C_.i2cSclPin) ? 1 : 0)

/**
  * @fn			static void i2cDelay
	*
  * @brief	I2C bus delay, on the standard mode, i2c clock speed is 100KHz
	*					while on the fast mode, the clock speed up to 400KHz
	*
	* input parameters
	*
  * @param	None
	*
	* output parameters
	*
  * @retval	None
	*
  */
void i2cDelay(I2cSpeed_e i2cSpeed)
{
	uint8_t i = (uint8_t)i2cSpeed;
	for (; i > 0; i--);		/*!< When STM32 clock is 72MHz, run at the flash and MDK no optimize set */
												/*!< When cycle count is 10, the i2c clock freqency is 205KHz */
												/*!< When cycle count is 7, the i2c clock freqency is 347KHz, SCL high level is 1.5us and low level is 2.87us */
												/*!< When cycle count is 5, the i2c clock freqency is 421KHz, SCL high level is 1.25us and low level is 2.375us */
}

/**
  * @fn			void i2cStart
	*
  * @brief	Send a start signal to the i2c bus.
	*
	* input parameters
	*
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	None
	*
  */
void i2cStart(VirtualI2C_s sI2C)
{
	VI2C_SDA_HIGH(sI2C.sI2cPortPin);
	for(;;)
	{
		VI2C_SCL_HIGH(sI2C.sI2cPortPin);
		if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
			break;
	}
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SDA_LOW(sI2C.sI2cPortPin);
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SCL_LOW(sI2C.sI2cPortPin);
	i2cDelay(sI2C.eI2cSpeed);
}

/**
  * @fn			void i2cStop
	*
  * @brief	Send a stop signal to the i2c bus.
	*
	* input parameters
  * @param	sI2C: sI2C: specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	None
	*
  */
void i2cStop(VirtualI2C_s sI2C)
{
	VI2C_SDA_LOW(sI2C.sI2cPortPin);
	for(;;)
	{
		VI2C_SCL_HIGH(sI2C.sI2cPortPin);
		if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
			break;
	}
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SDA_HIGH(sI2C.sI2cPortPin);
}

/**
  * @fn			void i2cSendByte
	*
  * @brief	Send a byte data to i2c device.
	*
	* input parameters
	*
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
	* @param	ucByte - the data that what to send.
	*
  * @retval	None
	*
  */
void i2cSendByte(VirtualI2C_s sI2C, uint8_t ucByte)
{
	uint8_t i;
	
	for (i = 0; i < 8; i++)
	{
		if (ucByte & 0x80)	VI2C_SDA_HIGH(sI2C.sI2cPortPin);
		else	VI2C_SDA_LOW(sI2C.sI2cPortPin);

		i2cDelay(sI2C.eI2cSpeed);
		for(;;)
		{
			VI2C_SCL_HIGH(sI2C.sI2cPortPin);		// sampling data
			if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
				break;
		}		
		//VI2C_SCL_HIGH(sI2C.sI2cPortPin);		// sampling data
		sI2C.I2C_Delay(sI2C.eI2cSpeed);
		VI2C_SCL_LOW(sI2C.sI2cPortPin);
		if (i >= 7) VI2C_SDA_HIGH(sI2C.sI2cPortPin);		// realese i2c bus
		ucByte <<= 1;
		sI2C.I2C_Delay(sI2C.eI2cSpeed);				// optional comment
	}
}

/**
  * @fn			uint8_t i2cReadByte
	*
  * @brief	read a byte data from i2c device.
	*
	* input parameters
	*
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	value - a byte data from i2c device.
	*
  */
uint8_t i2cReadByte(VirtualI2C_s sI2C)
{
	uint8_t i, value = 0;
	//uint8_t readValue;

	for (i = 0; i < 8; i++)
	{
		value <<= 1;
		//VI2C_SCL_HIGH(sI2C.sI2cPortPin);
		for(;;)
		{
			VI2C_SCL_HIGH(sI2C.sI2cPortPin);
			if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
				break;
		}		
		sI2C.I2C_Delay(sI2C.eI2cSpeed);
		//readValue = VI2C_SDA_READ(sI2C.sI2cPortPin);
		if ( VI2C_SDA_READ(sI2C.sI2cPortPin) == 1 )	value++;
		VI2C_SCL_LOW(sI2C.sI2cPortPin);
		sI2C.I2C_Delay(sI2C.eI2cSpeed);
	}

	return value;
}

/**
  * @fn			uint8_t i2cWaitAck
	*
  * @brief	Generate a i2c clock and then read a ACK signal.
	*
	* input parameters
	*
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	ret - 0: correct ACK signal, 1: no device acknowledge.
	*
  */
uint8_t i2cWaitAck(VirtualI2C_s sI2C)
{
	uint8_t ret = 0;

	VI2C_SDA_HIGH(sI2C.sI2cPortPin);			// release i2c bus
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
//	VI2C_SCL_HIGH(sI2C.sI2cPortPin);			// Pull high SCL then SDA return ACK signal
	for(;;)
	{
		VI2C_SCL_HIGH(sI2C.sI2cPortPin);		// Pull high SCL then SDA return ACK signal
		if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
			break;
	}
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	if ( VI2C_SDA_READ(sI2C.sI2cPortPin) == 1)	ret = 1;	// sampling data
	VI2C_SCL_LOW(sI2C.sI2cPortPin);
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	return ret;
}

/**
  * @fn			i2cAck
	*
  * @brief	Generate a ACK singal.
	*
	* input parameters
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	None
	*
  */
void i2cAck(VirtualI2C_s sI2C)
{
	VI2C_SDA_LOW(sI2C.sI2cPortPin);
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
//	VI2C_SCL_HIGH(sI2C.sI2cPortPin);	// generate a i2c clock
	for(;;)
	{
		VI2C_SCL_HIGH(sI2C.sI2cPortPin);	// generate a i2c clcok
		if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
			break;
	}			
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SCL_LOW(sI2C.sI2cPortPin);
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SDA_HIGH(sI2C.sI2cPortPin);	// release i2c bus
}

/**
  * @fn			void i2cNAck
	*
  * @brief	Generate a NACK signal.
	*
	* input parameters
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	None
	*
  */
void i2cNAck(VirtualI2C_s sI2C)
{
	VI2C_SDA_HIGH(sI2C.sI2cPortPin);	// release i2c bus
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
//	VI2C_SCL_HIGH(sI2C.sI2cPortPin);	// generate a i2c clcok
	for(;;)
	{
		VI2C_SCL_HIGH(sI2C.sI2cPortPin);	// generate a i2c clcok
		if ( VI2C_SCL_READ(sI2C.sI2cPortPin) == 1 )
			break;
	}		
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
	VI2C_SCL_LOW(sI2C.sI2cPortPin);
	sI2C.I2C_Delay(sI2C.eI2cSpeed);
}

/**
  * @fn			uint8_t i2cCheckDevice
	*
  * @brief	Check if exist i2c device on the i2c bus, the i2c master send a address of 
	*					slave to bus, then judgement the corresponding slave device whether or not
						exist by read ACK signal on i2c bus.
	*
	* input parameters
  * @param	sI2C - specifies a struct of virtual i2c bus configuration.
	*
	* output parameters
	*
  * @retval	ucAck - 0: device exist, 1: no device exist.
	*
  */
uint8_t i2cCheckDevice(VirtualI2C_s sI2C)
{
	uint8_t ucAck;

	i2cStart(sI2C);
	i2cSendByte(sI2C, (I2C_WR | sI2C.I2cAddr_un.i2c7BitAddr));	/*!< Send device address and R/W access control bit: 0 - W, 1 - R*/
	ucAck = i2cWaitAck(sI2C);		// wati slave ACK signal
	i2cStop(sI2C);

	return ucAck;
}

/**************************************** Copyright (C) 2019 Yoki **************************************/
