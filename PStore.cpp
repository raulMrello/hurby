/**
 *****************************************************************************
 * File Name    : _24LCXXX.cpp
 *
 * Title        : I2C EEPROM 24LCXXX Claass Source File
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
 
/* Includes ----------------------------------------------------------------- */
#include "PStore.h"
 
#define M24C04_MEMSIZE	512


/** Constructor **********************************************************************/
PStore::PStore(I2C *i2c, const uint8_t address): _i2c_address(address<<1), _i2c(i2c), _pc(NULL), _debug(false){
}
 
PStore::PStore(I2C *i2c, Serial *pc, const uint8_t address): _i2c_address(address<<1), _i2c(i2c), _pc(pc), _debug(true){
}

/** Check integrity ******************************************************************/
int8_t PStore::IsOK(void){
	uint8_t value = 0;
	this->Read(0,&value, 1);
	if(value == 0x55){
		return 1;
	}
	return 0;
}

/** Lock during write ****************************************************************/
int8_t PStore::Lock(void){
	uint8_t lock = 0xAA;
	return this->Write(0, &lock, 1);
}

/** Lock during write ****************************************************************/
int8_t PStore::Unlock(void){
	uint8_t lock = 0x55;
	return this->Write(0, &lock, 1);
}

/** Write to PS **********************************************************************/
int8_t PStore::Write( uint8_t mem_addr, void *data, uint8_t size ){
    int8_t  res;
    char *buf;
	// if size exceeded
	if (size > 16 || (mem_addr + size) >= M24C04_MEMSIZE)
		return -1;
	buf = (char*)malloc(size+1);
    buf[0] = (char)mem_addr;
	memcpy(&buf[1], data, size);
    res = _i2c->write(_i2c_address, buf, (size+1), false);
	if(_debug){
		if(res==0){
			_pc->printf("Success! N-Byte Data Write. \n");
		}
		else{
			_pc->printf("Failed! N-Byte Data Write %d.\n", res);
		}
	} 
	wait_ms(5);         // 5mS 
    return res;
}

/** Read from PS ********************************************************************/
int8_t PStore::Read( uint8_t mem_addr, void *data, uint8_t size ){
    int8_t res;
    char buf;
    char *p;
	// if size exceeded
	if ((mem_addr + size) >= M24C04_MEMSIZE)
		return -1;
	
	p = (char *)data;
    buf = mem_addr;
    res = _i2c->write(_i2c_address, &buf, 1, true);
    if(_debug){
        if(res==0){
            _pc->printf("Success! nbyte read address send. \n");
        }
        else{
            _pc->printf("Failed! nbyte read address send %d.\n", res);
        }
    }
	wait_ms(5);
	res = _i2c->read(_i2c_address, p, size, false);		
	wait_ms(5);
	if(_debug){
		if(res==0){
			_pc->printf("Success! nbyte read address send. \n");
		}
		else{
			_pc->printf("Failed! nbyte read address send %d.\n", res);
		}
	}
    return res;
}
 
