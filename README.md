### The is code snippet of I2C for STM32 use GPIO

**Note: All `VirtualI2C_s` structure parameters of the function can be modified to point to `VirtualI2C_s` has speeded up access and execution speed**

### Usage
- ***./test*** directory is a AT24XX EEPROM device access code

- Declare `VirtualI2C` variable and check the I2C bus if exist AT24XX device:
	```C
	VirtualI2C_s g_sVirtualI2c = {
		{GPIOB, 10, 11},
		I2C_STANDARD_100KHz,
		0,
		i2cDelay
	};

	at24cxxCheck();
	...
	```

- Please check for details the file ***./test/hal_24cxx.c***

### If you have any suggestions or questions, please send an email to youneihai@126.com or youneihai@gmail.com