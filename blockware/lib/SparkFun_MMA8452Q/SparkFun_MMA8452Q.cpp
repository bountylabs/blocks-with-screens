/******************************************************************************
SparkFun_MMA8452Q.cpp
SparkFun_MMA8452Q Library Source File
Jim Lindblom and Andrea DeVore @ SparkFun Electronics
Original Creation Date: June 3, 2014
https://github.com/sparkfun/MMA8452_Accelerometer

This file implements all functions of the MMA8452Q class. Functions here range
from higher level stuff, like reading/writing MMA8452Q registers to low-level,
hardware I2C reads and writes.

Development environment specifics:
	IDE: Arduino 1.0.5
	Hardware Platform: Arduino Uno

	**Updated for Arduino 1.8.5 2/2019**

This code is beerware; if you see me (or any other SparkFun employee) at the
local, and you've found our code helpful, please buy us a round!

Distributed as-is; no warranty is given.
******************************************************************************/

#include "SparkFun_MMA8452Q.h"
#include <Arduino.h>
#include <Wire.h>

// CONSTRUCTUR
//   This function, called when you initialize the class will simply write the
//   supplied address into a private variable for future use.
//   The variable addr should be either 0x1C or 0x1D, depending on which voltage
//   the SA0 pin is tied to (GND or 3.3V respectively).
MMA8452Q::MMA8452Q(byte addr)
{
	_deviceAddress = addr; // Store address into private variable
}

// BEGIN INITIALIZATION (New Implementation of Init)
// 	This will be used instead of init in future sketches
// 	to match Arudino guidelines. We will maintain init
// 	for backwards compatability purposes.
bool MMA8452Q::begin(TwoWire &wirePort, uint8_t deviceAddress)
{
	_deviceAddress = deviceAddress;
	_i2cPort = &wirePort;

	byte c = readRegister(WHO_AM_I); // Read WHO_AM_I register

	if (c != 0x2A) // WHO_AM_I should always be 0x2A
	{
		return false;
	}

	scale = SCALE_2G;
	odr = ODR_800;

	setScale(scale);  // Set up accelerometer scale
	setDataRate(odr); // Set up output data rate
	setupPL();		  // Set up portrait/landscape detection

	// Multiply parameter by 0.0625g to calculate threshold.
	setupTap(0x80, 0x80, 0x08); // Disable x, y, set z to 0.5g

	return true;
}

// INITIALIZATION
//	This function initializes the MMA8452Q. It sets up the scale (either 2, 4,
//	or 8g), output data rate, portrait/landscape detection and tap detection.
//	It also checks the WHO_AM_I register to make sure we can communicate with
//	the sensor. Returns a 0 if communication failed, 1 if successful.
byte MMA8452Q::init(MMA8452Q_Scale fsr, MMA8452Q_ODR odr)
{
	scale = fsr; // Haul fsr into our class variable, scale

	if (_i2cPort == NULL)
	{
		_i2cPort = &Wire;
	}

	_i2cPort->begin(); // Initialize I2C

	byte c = readRegister(WHO_AM_I); // Read WHO_AM_I register

	if (c != 0x2A) // WHO_AM_I should always be 0x2A
	{
		return 0;
	}

	standby(); // Must be in standby to change registers

	setScale(scale);  // Set up accelerometer scale
	setDataRate(odr); // Set up output data rate
	setupPL();		  // Set up portrait/landscape detection
	// Multiply parameter by 0.0625g to calculate threshold.
	setupTap(0x80, 0x80, 0x08); // Disable x, y, set z to 0.5g

	active(); // Set to active to start reading

	return 1;
}

byte MMA8452Q::readID()
{
	return readRegister(WHO_AM_I);
}

// GET FUNCTIONS FOR RAW ACCELERATION DATA
// Returns raw X acceleration data
short MMA8452Q::getX()
{
	byte rawData[2];
	readRegisters(OUT_X_MSB, rawData, 2); // Read the X data into a data array
	return ((short)(rawData[0] << 8 | rawData[1])) >> 4;
}

// Returns raw Y acceleration data
short MMA8452Q::getY()
{
	byte rawData[2];
	readRegisters(OUT_Y_MSB, rawData, 2); // Read the Y data into a data array
	return ((short)(rawData[0] << 8 | rawData[1])) >> 4;
}

// Returns raw Z acceleration data
short MMA8452Q::getZ()
{
	byte rawData[2];
	readRegisters(OUT_Z_MSB, rawData, 2); // Read the Z data into a data array
	return ((short)(rawData[0] << 8 | rawData[1])) >> 4;
}

// GET FUNCTIONS FOR CALCULATED ACCELERATION DATA
// Returns calculated X acceleration data
float MMA8452Q::getCalculatedX()
{
	x = getX();
	return (float)x / (float)(1 << 11) * (float)(scale);
}

// Returns calculated Y acceleration data
float MMA8452Q::getCalculatedY()
{
	y = getY();
	return (float)y / (float)(1 << 11) * (float)(scale);
}

// Returns calculated Z acceleration data
float MMA8452Q::getCalculatedZ()
{
	z = getZ();
	return (float)z / (float)(1 << 11) * (float)(scale);
}

// READ ACCELERATION DATA
//  This function will read the acceleration values from the MMA8452Q. After
//	reading, it will update two triplets of variables:
//		* int's x, y, and z will store the signed 12-bit values read out
//		  of the acceleromter.
//		* floats cx, cy, and cz will store the calculated acceleration from
//		  those 12-bit values. These variables are in units of g's.
void MMA8452Q::read()
{
	byte rawData[6]; // x/y/z accel register data stored here

	readRegisters(OUT_X_MSB, rawData, 6); // Read the six raw data registers into data array

	x = ((short)(rawData[0] << 8 | rawData[1])) >> 4;
	y = ((short)(rawData[2] << 8 | rawData[3])) >> 4;
	z = ((short)(rawData[4] << 8 | rawData[5])) >> 4;
	cx = (float)x / (float)(1 << 11) * (float)(scale);
	cy = (float)y / (float)(1 << 11) * (float)(scale);
	cz = (float)z / (float)(1 << 11) * (float)(scale);
}

// CHECK IF NEW DATA IS AVAILABLE
//	This function checks the status of the MMA8452Q to see if new data is availble.
//	returns 0 if no new data is present, or a 1 if new data is available.
byte MMA8452Q::available()
{
	return (readRegister(STATUS_MMA8452Q) & 0x08) >> 3;
}

// SET FULL-SCALE RANGE
//	This function sets the full-scale range of the x, y, and z axis accelerometers.
//	Possible values for the fsr variable are SCALE_2G, SCALE_4G, or SCALE_8G.
void MMA8452Q::setScale(MMA8452Q_Scale fsr)
{
	// Must be in standby mode to make changes!!!
	// Change to standby if currently in active state
	if (isActive() == true)
		standby();

	byte cfg = readRegister(XYZ_DATA_CFG);
	cfg &= 0xFC;	   // Mask out scale bits
	cfg |= (fsr >> 2); // Neat trick, see page 22. 00 = 2G, 01 = 4A, 10 = 8G
	writeRegister(XYZ_DATA_CFG, cfg);

	// Return to active state when done
	// Must be in active state to read data
	active();
}

// SET THE OUTPUT DATA RATE
//	This function sets the output data rate of the MMA8452Q.
//	Possible values for the odr parameter are: ODR_800, ODR_400, ODR_200,
//	ODR_100, ODR_50, ODR_12, ODR_6, or ODR_1
void MMA8452Q::setDataRate(MMA8452Q_ODR odr)
{
	// Must be in standby mode to make changes!!!
	// Change to standby if currently in active state
	if (isActive() == true)
		standby();

	byte ctrl = readRegister(CTRL_REG1);
	ctrl &= 0xC7; // Mask out data rate bits
	ctrl |= (odr << 3);
	writeRegister(CTRL_REG1, ctrl);

	// Return to active state when done
	// Must be in active state to read data
	active();
}

// SET UP TAP DETECTION
//	This function can set up tap detection on the x, y, and/or z axes.
//	The xThs, yThs, and zThs parameters serve two functions:
//		1. Enable tap detection on an axis. If the 7th bit is SET (0x80)
//			tap detection on that axis will be DISABLED.
//		2. Set tap g's threshold. The lower 7 bits will set the tap threshold
//			on that axis.
void MMA8452Q::setupTap(byte xThs, byte yThs, byte zThs)
{
	// Must be in standby mode to make changes!!!
	// Change to standby if currently in active state
	if (isActive() == true)
		standby();

	// Set up single and double tap - 5 steps:
	// for more info check out this app note:
	// http://cache.freescale.com/files/sensors/doc/app_note/AN4072.pdf
	// Set the threshold - minimum required acceleration to cause a tap.
	byte temp = 0;
	if (!(xThs & 0x80)) // If top bit ISN'T set
	{
		temp |= 0x3;					 // Enable taps on x
		writeRegister(PULSE_THSX, xThs); // x thresh
	}
	if (!(yThs & 0x80))
	{
		temp |= 0xC;					 // Enable taps on y
		writeRegister(PULSE_THSY, yThs); // y thresh
	}
	if (!(zThs & 0x80))
	{
		temp |= 0x30;					 // Enable taps on z
		writeRegister(PULSE_THSZ, zThs); // z thresh
	}
	// Set up single and/or double tap detection on each axis individually.
	writeRegister(PULSE_CFG, temp | 0x40);
	// Set the time limit - the maximum time that a tap can be above the thresh
	writeRegister(PULSE_TMLT, 0x30); // 30ms time limit at 800Hz odr
	// Set the pulse latency - the minimum required time between pulses
	writeRegister(PULSE_LTCY, 0xA0); // 200ms (at 800Hz odr) between taps min
	// Set the second pulse window - maximum allowed time between end of
	//	latency and start of second pulse
	writeRegister(PULSE_WIND, 0xFF); // 5. 318ms (max value) between taps max

	// Return to active state when done
	// Must be in active state to read data
	active();
}

// READ TAP STATUS
//	This function returns any taps read by the MMA8452Q. If the function
//	returns no new taps were detected. Otherwise the function will return the
//	lower 7 bits of the PULSE_SRC register.
byte MMA8452Q::readTap()
{
	byte tapStat = readRegister(PULSE_SRC);
	if (tapStat & 0x80) // Read EA bit to check if a interrupt was generated
	{
		return tapStat & 0x7F;
	}
	else
		return 0;
}

// SET UP PORTRAIT/LANDSCAPE DETECTION
//	This function sets up portrait and landscape detection.
void MMA8452Q::setupPL()
{
	// Must be in standby mode to make changes!!!
	// Change to standby if currently in active state
	if (isActive() == true)
		standby();

	// For more info check out this app note:
	//	http://cache.freescale.com/files/sensors/doc/app_note/AN4068.pdf
	// 1. Enable P/L
	writeRegister(PL_CFG, readRegister(PL_CFG) | 0x40); // Set PL_EN (enable)
	// 2. Set the debounce rate
	writeRegister(PL_COUNT, 0x50); // Debounce counter at 100ms (at 800 hz)

	// Return to active state when done
	// Must be in active state to read data
	active();
}

// READ PORTRAIT/LANDSCAPE STATUS
//	This function reads the portrait/landscape status register of the MMA8452Q.
//	It will return either PORTRAIT_U, PORTRAIT_D, LANDSCAPE_R, LANDSCAPE_L,
//	or LOCKOUT. LOCKOUT indicates that the sensor is in neither p or ls.
byte MMA8452Q::readPL()
{
	byte plStat = readRegister(PL_STATUS);

	if (plStat & 0x40) // Z-tilt lockout
		return LOCKOUT;
	else // Otherwise return LAPO status
		return (plStat & 0x6) >> 1;
}

// CHECK FOR ORIENTATION
bool MMA8452Q::isRight()
{
	if (readPL() == LANDSCAPE_R)
		return true;
	return false;
}

bool MMA8452Q::isLeft()
{
	if (readPL() == LANDSCAPE_L)
		return true;
	return false;
}

bool MMA8452Q::isUp()
{
	if (readPL() == PORTRAIT_U)
		return true;
	return false;
}

bool MMA8452Q::isDown()
{
	if (readPL() == PORTRAIT_D)
		return true;
	return false;
}

bool MMA8452Q::isFlat()
{
	if (readPL() == LOCKOUT)
		return true;
	return false;
}

// SET STANDBY MODE
//	Sets the MMA8452 to standby mode. It must be in standby to change most register settings
void MMA8452Q::standby()
{
	byte c = readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, c & ~(0x01)); //Clear the active bit to go into standby
}

// SET ACTIVE MODE
//	Sets the MMA8452 to active mode. Needs to be in this mode to output data
void MMA8452Q::active()
{
	byte c = readRegister(CTRL_REG1);
	writeRegister(CTRL_REG1, c | 0x01); //Set the active bit to begin detection
}

// CHECK STATE (ACTIVE or STANDBY)
//	Returns true if in Active State, otherwise return false
bool MMA8452Q::isActive()
{
	byte currentState = readRegister(SYSMOD);
	currentState &= 0b00000011;

	// Wake and Sleep are both active SYSMOD states (pg. 10 datasheet)
	if (currentState == SYSMOD_STANDBY)
		return false;
	return true;
}

// WRITE A SINGLE REGISTER
// 	Write a single byte of data to a register in the MMA8452Q.
void MMA8452Q::writeRegister(MMA8452Q_Register reg, byte data)
{
	writeRegisters(reg, &data, 1);
}

// WRITE MULTIPLE REGISTERS
//	Write an array of "len" bytes ("buffer"), starting at register "reg", and
//	auto-incrmenting to the next.
void MMA8452Q::writeRegisters(MMA8452Q_Register reg, byte *buffer, byte len)
{
	_i2cPort->beginTransmission(_deviceAddress);
	_i2cPort->write(reg);
	for (int x = 0; x < len; x++)
		_i2cPort->write(buffer[x]);
	_i2cPort->endTransmission(); //Stop transmitting
}

// READ A SINGLE REGISTER
//	Read a byte from the MMA8452Q register "reg".
byte MMA8452Q::readRegister(MMA8452Q_Register reg)
{
#ifdef _VARIANT_ARDUINO_DUE_X_
	_i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)1, (uint32_t)reg, (uint8_t)1, true);
#else
	_i2cPort->beginTransmission(_deviceAddress);
	_i2cPort->write(reg);
	_i2cPort->endTransmission(false); //endTransmission but keep the connection active

	_i2cPort->requestFrom(_deviceAddress, (byte)1); //Ask for 1 byte, once done, bus is released by default
#endif
	if (_i2cPort->available())
	{							 //Wait for the data to come back
		return _i2cPort->read(); //Return this one byte
	}
	else
	{
		return 0;
	}
}

// READ MULTIPLE REGISTERS
//	Read "len" bytes from the MMA8452Q, starting at register "reg". Bytes are stored
//	in "buffer" on exit.
void MMA8452Q::readRegisters(MMA8452Q_Register reg, byte *buffer, byte len)
{
#ifdef _VARIANT_ARDUINO_DUE_X_
	_i2cPort->requestFrom((uint8_t)_deviceAddress, (uint8_t)len, (uint32_t)reg, (uint8_t)1, true);
#else
	_i2cPort->beginTransmission(_deviceAddress);
	_i2cPort->write(reg);
	_i2cPort->endTransmission(false);			//endTransmission but keep the connection active
	_i2cPort->requestFrom(_deviceAddress, len); //Ask for bytes, once done, bus is released by default
#endif
	if (_i2cPort->available() == len)
	{
		for (int x = 0; x < len; x++)
			buffer[x] = _i2cPort->read();
	}
}
