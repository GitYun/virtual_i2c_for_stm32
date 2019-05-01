// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  *******************************************************************************
  * @file			hal_24cxx.c
  * @author		Yoki
  * @version	V1.0.0
  * @date			2017-4-21
  * @brief		AT24CXX device driver function implement file.
	*
  ********************************************************************************
  */

#include "hal_24cxx.h"

VirtualI2C_s g_at24cxx_I2C = {
	{GPIOB, GPIO_PIN_10, GPIO_PIN_11},
	I2C_STANDARD_100KHz,
	AT24CXX_SLAVE_ADDRESS,
	i2cDelay
};

/**
  * @fn						at24cxxReadByte
  *
  * @brief				Read a byte data from designated EEPROM address of at24cxx.
  *
  * input parameters
  *
  * @param				readAddr - The EEPROM address that start read data.
  *
  * output parameters
  *
  * @retval				A byte data read form EEPROM address of at24cxxx
  *
  */
uint8_t at24cxxReadByte(uint16_t readAddr)
{
	uint8_t recvData = 0;
	i2cStart(g_at24cxx_I2C);
	if (AT24CXX_TYPE > AT24C16)
	{
		// send write command
		i2cSendByte(g_at24cxx_I2C, (I2C_WR | g_at24cxx_I2C.I2cAddr_un.i2c7BitAddr) );
		i2cWaitAck(g_at24cxx_I2C);
		i2cSendByte( g_at24cxx_I2C ,(readAddr >> 8) );		// send high byte of address
	}
	else
	{
		// send device address, wirte data
		i2cSendByte( g_at24cxx_I2C, (g_at24cxx_I2C.I2cAddr_un.i2c7BitAddr + ((readAddr/256)<<1)) );
	}

	i2cWaitAck(g_at24cxx_I2C);
	i2cSendByte( g_at24cxx_I2C, (readAddr%256) );		// send low byte of address
	i2cWaitAck(g_at24cxx_I2C);

	i2cStart(g_at24cxx_I2C);
	// send read command
	i2cSendByte(g_at24cxx_I2C, (I2C_RD | g_at24cxx_I2C.I2cAddr_un.i2c7BitAddr));
	i2cWaitAck(g_at24cxx_I2C);
	recvData = i2cReadByte(g_at24cxx_I2C);
	i2cNAck(g_at24cxx_I2C);			// at24cxx read a byte data, SDA line no Ack signal
	i2cStop(g_at24cxx_I2C);

	return recvData;
}

/**
  * @fn						at24cxxWriteByte
  *
  * @brief				Write a byte data to designated EEPROM address of at24cxx.
  *
  * input parameters
  *
  * @param			writeAddr 	- The EEPROM address that start write data.
	* @param			data2Write 	- The data to write to EEPROM address of at24cxx.
  *
  * output parameters
  *
  * @retval			None
  *
  */
void at24cxxWriteByte(uint16_t writeAddr, uint8_t data2Write)
{
	i2cStart(g_at24cxx_I2C);
	if (AT24CXX_TYPE > AT24C16)
	{
		i2cSendByte(g_at24cxx_I2C ,(I2C_WR | g_at24cxx_I2C.I2cAddr_un.i2c7BitAddr));
		i2cWaitAck(g_at24cxx_I2C);
		i2cSendByte(g_at24cxx_I2C, (writeAddr>>8));	// send high byte of address where will be writen data
	}
	else
	{
		i2cSendByte( g_at24cxx_I2C, (g_at24cxx_I2C.I2cAddr_un.i2c7BitAddr + ((writeAddr/256)<<1)) );
	}

	i2cWaitAck(g_at24cxx_I2C);
	i2cSendByte(g_at24cxx_I2C ,(writeAddr%256));	// send low address where will be writen data
	i2cWaitAck(g_at24cxx_I2C);
	i2cSendByte(g_at24cxx_I2C ,data2Write);
	i2cWaitAck(g_at24cxx_I2C);
	i2cStop(g_at24cxx_I2C);
}

/**
  * @fn						at24cxxWriteTwoOrFourByte
  *
  * @brief				Write len length data to the EEPROM address of at24cxx, where the len
	*								value can be:  2, 4
  *
  * input parameters
  *
  * @param				writeAddr		- The EEPROM address that start write data.
	* @param				data2Write	- The a half word or a word data to write to EEPROM address of at24cxx.
	* @param				len					- where the len value can be: 2 or 4, that means a half word or a word.
  *
  * output parameters
  *
  * @retval				None
  *
  */
void at24cxxWriteTwoOrFourByte(uint16_t writeAddr, uint32_t data2Write, uint8_t len)
{
	uint8_t index = 0;
	for (index = 0; index < len; index++)
	{
		at24cxxWriteByte( (writeAddr+index), ((data2Write>>(8*index))&0xff) );
	}
}

/**
  * @fn						at24cxxReadTwoOrFourByte
  *
  * @brief				Read len length data from the EEPROM address of at24cxx, where the len
	*								value can be:  2, 4
  *
  * input parameters
  *
  * @param				readAddr	- The EEPROM address that start read data.
	* @param				len				-  where the len value can be: 2 or 4, that means a half word or a word.
  *
  * output parameters
  *
  * @retval				A half word or a word that read from EEPROM address of at24cxx.
  *
  */
uint32_t at24cxxReadTwoOrFourByte(uint16_t readAddr, uint8_t len)
{
	uint8_t index;
	uint32_t recvData = 0;
	for(index = 0; index < len; index++)
	{
		recvData <<= 8;
		at24cxxReadByte( (readAddr+len-index-1) );
	}

	return recvData;
}

/**
  * @fn					at24cxxCheck
  *
  * @brief			Checking whether the at24cxx is work done.
  *
  * input parameters
  *
  * @param			None
  *
  * output parameters
  *
  * @retval			0 - work done, 1 - don't work
  *
  */
uint8_t at24cxxCheck(void)
{
	uint8_t ret;

	ret = at24cxxReadByte(AT24CXX_CHECK_ADDRESS);
	if (ret == AT24CXX_CHECK_VALUE)
		return 0;
	else
	{
		at24cxxWriteByte(AT24CXX_CHECK_ADDRESS, AT24CXX_CHECK_VALUE);
		ret = at24cxxReadByte(AT24CXX_CHECK_ADDRESS);
		if (ret == AT24CXX_CHECK_VALUE)
			return 0;
	}

	return 1;
}

/**
  * @fn						at24cxxReadNbrByte
  *
  * @brief				Read len length data from EEPROM address of at24cxx,
	*								and store data to a buffer.
  *
  * input parameters
  *
  * @param				readAddr	- The EEPROM address that start read data.
	*	@param				pBuffer		- A pointer to a buffer that is used store data.
	* @param				len				- Length of data to write to at24cxx.
  *
  * output parameters
  *
  * @retval				None
  *
  */
void at24cxxReadNbrByte(uint16_t readAddr, uint8_t *pBuffer, uint16_t len)
{
	while(len)
	{
		*pBuffer++ = at24cxxReadByte(readAddr);
		readAddr++;
		len--;
	}
}

/**
  * @fn						at24cxxWriteNbrByte
  *
  * @brief				Write len length data to EEPROM address of at24cxx.
  *
  * input parameters
  *
  * @param				writeAddr	- The EEPROM address that start write data.
	* @param				pBuffer		- A pointer to a buffer that stores the data to be sent.
  * @param				len				- Length of data to write to at24cxx.
	*
  * output parameters
  *
  * @retval				None
  *
  */
void at24cxxWriteNbrByte(uint16_t writeAddr, uint8_t *pBuffer, uint16_t len)
{
	while(len)
	{
		at24cxxWriteByte(writeAddr, *pBuffer);
		writeAddr++;
		pBuffer++;
		len--;
	}
}

/**************************************** Copyright (C) 2019 **************************************/
