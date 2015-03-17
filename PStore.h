/**
 *****************************************************************************
 * File Name    : _24LCXXX.h
 *
 * Title        : I2C EEPROM 24LCXXX Claass Header File
 * Revision     : 0.1
 * Notes        :
 * Target Board : mbed NXP LPC1768, mbed LPC1114FN28  etc
 * Tool Chain   : ????
 *
 * Revision History:
 * When         Who         Description of change
 * -----------  ----------- -----------------------
 * 2012/12/06   Hiroshi M   init
 *****************************************************************************
 *
 * Copyright (C) 2013 Hiroshi M, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/
 
#ifndef __PSTORE_H_
#define __PSTORE_H_
 
/* Includes ----------------------------------------------------------------- */
#include "mbed.h"
 
 
class PStore{ 
	public:
		PStore(I2C *i2c, const uint8_t address=0x50);
		PStore(I2C *i2c, Serial *pc, const uint8_t address=0x50);
		int8_t IsOK(void);
		int8_t Lock(void);
		int8_t Unlock(void);
		int8_t Write(uint8_t mem_addr, void *data, uint8_t size );
		int8_t Read( uint8_t mem_addr, void *data, uint8_t size );
		const static uint8_t BASE_ADDR = 1;
	
	private:
		uint8_t _i2c_address;
		I2C *_i2c;
		Serial *_pc;
		bool _debug;
};
 
#endif 

