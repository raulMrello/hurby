/**
 * Copyright (c) 2011 Pololu Corporation.  For more information, see
 * 
 * http://www.pololu.com/
 * http://forum.pololu.com/
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
 
#include "mbed.h"
#include "GyroDriver.h"
#include <math.h>
 
// Defines ////////////////////////////////////////////////////////////////
 
// The Arduino two-wire interface uses a 7-bit number for the address, 
// and sets the last bit correctly based on reads and writes
// mbed I2C libraries take the 7-bit address shifted left 1 bit
// #define GYR_ADDRESS (0xD2 >> 1)
#define GYR_ADDRESS 	0xD0
//#define GYR_ADDRESS 	0xD2

#define STAT_STOP		(uint8_t)0
#define STAT_CALIBRATE	(uint8_t)1
#define STAT_READY		(uint8_t)2
#define STAT_CAPTURE	(uint8_t)3

 
// Private Methods //////////////////////////////////////////////////////////////

#if defined(GyroDriver_ENABLE_READINGS)
static void NULL_CALLBACK(uint32_t nullEvt){}
#endif
	
// Writes a gyro register
void GyroDriver::writeReg(uint8_t addr, byte reg, byte value){
	byte data[2];
    data[0] = reg;
    data[1] = value;
    _device->write(addr, data, 2);
}
 
// Reads a gyro register
byte GyroDriver::readReg(uint8_t addr, byte reg){
    byte value = 0;
    _device->write(addr, &reg, 1);
    _device->read(addr, &value, 1);
    return value;
}
// Public Methods //////////////////////////////////////////////////////////////
 
// Constructor
GyroDriver::GyroDriver(I2C * i2c, uint8_t dev_addr){
    // Turns on the L3G4200D's gyros and places it in normal mode.
    // 0x0F = 0b00001111
    // Normal power mode, all axes enabled
	this->_device = i2c;
	this->devAddr = dev_addr;
	this->stat = STAT_STOP;
	this->dx = 0;
	this->dy = 0;
	this->dz = 0;
}
 
// Starts gyro captures
void GyroDriver::start(void (*OnEvent)(uint32_t event)){
	#if defined(GyroDriver_ENABLE_READINGS)
	#warning "GyroDriver_ENABLE_READINGS activada";
 	this->OnEvent = (OnEvent)? OnEvent : NULL_CALLBACK;
 	this->calibrate();
	#endif
}
 
// Starts gyro captures
void GyroDriver::stop(void){
	this->timer.detach();
	this->stat = STAT_STOP;
}
 
// Starts gyro calibration (1second)
void GyroDriver::calibrate(void){
    writeReg(this->devAddr, L3G4200D_CTRL_REG1, 0x0F);
    writeReg(this->devAddr, L3G4200D_CTRL_REG4, 0x20); // 2000 dps full scale	
	this->stat = STAT_CALIBRATE;
	this->numSamples = 0;
	this->xMax = -32700;
	this->yMax = -32700;
	this->zMax = -32700;
	this->xMin = 32700;
	this->yMin = 32700;
	this->zMin = 32700;
	this->xZero = 0;
	this->yZero = 0;
	this->zZero = 0;	
	this->timer.attach(this, &GyroDriver::read, 0.01);
}

 
// Reads the 3 gyro channels and stores them in vector g
void GyroDriver::read(void){
	const double sensitivity = 0.07;
	const double samplingPeriod = 0.01; // 100Hz	
	byte data[6];
    // assert the MSB of the address to get the gyro 
    // to do slave-transmit subaddress updating.
    data[0] = L3G4200D_OUT_X_L | (1 << 7);
    _device->write(this->devAddr, data, 1); 
    _device->read(this->devAddr, data, 6); 
 
	this->g[0] = (short) (data[1] << 8 | data[0]);
	this->g[1] = (short) (data[3] << 8 | data[2]);
	this->g[2] = (short) (data[5] << 8 | data[4]);

	this->OnEvent(GyroDriver_EV_CAPTURE);
	switch(this->stat){
		case STAT_CALIBRATE:{
			this->xZero += this->g[0];
			this->yZero += this->g[1];
			this->zZero += this->g[2];
			this->xMax = (this->g[0] > this->xMax)? this->g[0] : this->xMax;
			this->yMax = (this->g[1] > this->yMax)? this->g[1] : this->yMax;
			this->zMax = (this->g[2] > this->zMax)? this->g[2] : this->zMax;
			this->xMin = (this->g[0] < this->xMin)? this->g[0] : this->xMin;
			this->yMin = (this->g[1] < this->yMin)? this->g[1] : this->yMin;
			this->zMin = (this->g[2] < this->zMin)? this->g[2] : this->zMin;
			
			if(++this->numSamples >= 100){
				this->xZero = this->xZero/100;
				this->yZero = this->yZero/100;
				this->zZero = this->zZero/100;
				this->xMax = this->xMax - this->xZero;
				this->xMin = this->xMin - this->xZero;
				this->yMax = this->yMax - this->yZero;
				this->yMin = this->yMin - this->yZero;
				this->zMax = this->zMax - this->zZero;
				this->zMin = this->zMin - this->zZero;
				
				this->stat = STAT_READY;
				this->OnEvent(GyroDriver_EV_READY);
			}
			break;
		}
		case STAT_READY:{
			this->stat = STAT_CAPTURE;
			break;
		}
		case STAT_CAPTURE:{
			double rate;
			rate =  (double)(this->g[0] - this->xZero);
			if(rate > this->xMax || rate < this->xMin){
				this->dy += (rate * sensitivity * samplingPeriod);	
			}

			rate = (double)(this->g[1] - this->yZero);
			if(rate > this->yMax || rate < this->yMin){
				this->dx -= (rate * sensitivity * samplingPeriod);	
			}

			rate = (double)(this->g[2] - this->zZero);
			if(rate > this->zMax || rate < this->zMin){
				this->dz += (rate * sensitivity * samplingPeriod);	
			}
			
			break;
		}
		default:
			break;
	}
	this->timer.attach(this, &GyroDriver::read, 0.01);
}

 
// Reads the 3 gyro angles and stores them
void GyroDriver::getAngles(int32_t *x, int32_t *y, int32_t *z){	
	*x = (int32_t)this->dx;
	*y = (int32_t)this->dy;
	*z = (int32_t)this->dz;
}

 
 
/** Read gyro stat 
 * @return True if ready
 */
uint8_t GyroDriver::isReady(void){
	return (this->stat == STAT_READY)? 1 : 0;
}

	
/** Test module
 */
void GyroDriver::test(uint32_t testCode){
	switch(testCode){
		case 0:{
			const double sensitivity = 0.07;
			const double samplingPeriod = 0.01; // 100Hz
			double rate;
			uint16_t j = 0;
			const int32_t xraw[] = {4,15,17,17,17,17,9,13,13,13,9,0,-17,-18,
						-21,-14,-6,0,3,-2,-15,-32,-39,-57,-136,-161,
						-172,-185,-224,-230,-225,-200,-188,-212,-225,
						-236,-263,-265,-243,-224,-209,-197,-183,-166,
						-163,-173,-173,-182,-204,-195,-214,-220,-223,
						-229,-232,-245,-244,-249,-248,-261,-263,-278,
						-275,-289,-293,-294,-294,-290,-296,-295,-304,
						-287,-289,-281,-286,-281,-284,-289,-298,-281,
						-273,-282,-286,-278,-286,-297,-292,-302,-295,
						-305,-325,-318,-299,-283,-269,-239,-233,-246,
						-236,-240,-241,-230,-243,-241,-269,-264,-257,
						-267,-273,-281,-282,-294,-320,-357,-385,-382,
						-404,-447,-465,-456,-467,-480,-469,-466,-484,
						-482,-467,-442,-441,-402,-413,-384,-371,-387,
						-358,-354,-350,-364,-371,-333,-345,-373,-372,
						-383,-382,-380,-375,-389,-380,-395,-407,-361,
						-373,-391,-379,-337,-307,-305,-301,-304,-283,
						-267,-275,-263,-250,-237,-236,-225,-211,-200,
						-157,-165,-185,-169,-155,-148,-155,-162,-148,
						-149,-139,-132,-131,-120,-90,-94,-108,-109,-103,
						-49,-55,-60,-53,-46,-44,-32,-20,49,82,17,15,65536};
			this->xMax = 10;
			this->xMin = -10;
			this->xZero = 10;
			this->dx = 0;
			
			// TEST CAPTURE
			while(xraw[j] != 65536){
				rate = (double)(xraw[j] - this->xZero);
				if(rate < this->xMax && rate > this->xMin){
					rate = 0;
				}
				else{
					rate *= sensitivity * samplingPeriod;
				}
				this->dy -= rate;
				j++;
			}
			break;
		}
	}
}

            

