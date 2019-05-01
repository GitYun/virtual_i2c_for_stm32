// Copyright (c) 2019 Yoki
// Licensed under the MIT License

/**
  *******************************************************************************
  * @file		hal_24cxx.h
  * @author		Yoki
  * @version	V1.0
  * @date		2014-4-21
  * @brief		AT24CXX device driver header file.
	*
  ********************************************************************************
  */

#ifndef HAL_24CXX_H
#define HAL_24CXX_H

#ifdef __cplusplus
extern "C"	{
#endif

#include "stm32f1xx_hal.h"
#include "common.h"
#include "virtual_i2c.h"

#define AT24C01					127
#define AT24C02					256
#define AT24C04					511
#define AT24C08					1023
#define AT24C16					2047
#define AT24C32					4095
#define AT24C64					8191
#define AT24C128				16383
#define AT24C256				32767

#define AT24CXX_TYPE		AT24C02

#define AT24CXX_SLAVE_ADDRESS		0xA0

#define AT24CXX_CHECK_ADDRESS		0xFF
#define	AT24CXX_CHECK_VALUE			0x55

extern VirtualI2C_s gAt24cxx_I2C;

uint8_t at24cxxReadByte(uint16_t readAddr);
void at24cxxWriteByte(uint16_t writeAddr, uint8_t ucData2Write);
void at24cxxWriteTwoOrFourByte(uint16_t writeAddr, uint32_t uwData2Write, uint8_t len);
uint32_t at24cxxReadTwoOrFourByte(uint16_t readAddr, uint8_t len);
uint8_t at24cxxCheck(void);
void at24cxxReadNbrByte(uint16_t readAddr, uint8_t *pBuffer, uint16_t len);
void at24cxxWriteNbrByte(uint16_t writeAddr, uint8_t *pBuffer, uint16_t len);


#ifdef __cplusplus
}
#endif

#endif
/**************************************** Copyright (C) 2019 **************************************/
