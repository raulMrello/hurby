/* Copyright (c) 2011 Pololu Corporation.  For more information, see
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
 
#ifndef __GyroDriver_H
#define __GyroDriver_H
 
#include "mbed.h"
 
/** Clave para activar/desactivar la medida en los gyros */ 
//#define GyroDriver_ENABLE_READINGS	


// register addresses
#define L3G4200D_WHO_AM_I      0x0F
 
#define L3G4200D_CTRL_REG1     0x20
#define L3G4200D_CTRL_REG2     0x21
#define L3G4200D_CTRL_REG3     0x22
#define L3G4200D_CTRL_REG4     0x23
#define L3G4200D_CTRL_REG5     0x24
#define L3G4200D_REFERENCE     0x25
#define L3G4200D_OUT_TEMP      0x26
#define L3G4200D_STATUS_REG    0x27
 
#define L3G4200D_OUT_X_L       0x28
#define L3G4200D_OUT_X_H       0x29
#define L3G4200D_OUT_Y_L       0x2A
#define L3G4200D_OUT_Y_H       0x2B
#define L3G4200D_OUT_Z_L       0x2C
#define L3G4200D_OUT_Z_H       0x2D
 
#define L3G4200D_FIFO_CTRL_REG 0x2E
#define L3G4200D_FIFO_SRC_REG  0x2F
 
#define L3G4200D_INT1_CFG      0x30
#define L3G4200D_INT1_SRC      0x31
#define L3G4200D_INT1_THS_XH   0x32
#define L3G4200D_INT1_THS_XL   0x33
#define L3G4200D_INT1_THS_YH   0x34
#define L3G4200D_INT1_THS_YL   0x35
#define L3G4200D_INT1_THS_ZH   0x36
#define L3G4200D_INT1_THS_ZL   0x37
#define L3G4200D_INT1_DURATION 0x38

#define GyroDriver_EV_READY		(uint32_t)1
#define GyroDriver_EV_CAPTURE	(uint32_t)2
 
typedef char byte;
 
/** Interface library for the ST L3G4200D 3-axis gyro
 *
 * Ported from Pololu L3G4200D library for Arduino by
 * Michael Shimniok http://bot-thoughts.com
 *
 * @code
 * #include "mbed.h"
 * #include "GyroDriver.h"
 * GyroDriver gyro(p28, p27);
 * ...
 * int g[3];
 * gyro.read(g);
 * @endcode
 */
class GyroDriver
{
    public:
        /** Create a new GyroDriver I2C interface
         * @param i2c I2C channel
         * @param addr Device's address
         */
        GyroDriver(I2C * i2c, uint8_t dev_addr=0xD0);
        	
		/** Read gyro angular displacement. 
         * @param x degree displacement
         * @param y degree displacement
         * @param z degree displacement
         */
        void getAngles(int32_t *x, int32_t *y, int32_t *z);
        	        	
		/** Read gyro stat 
         * @return True if ready
         */
        uint8_t isReady(void);
	
		/** Starts continuous sensor readings for calibration
		 * @return 1 if started
         */
		void calibrate(void);
	
		/** Starts continuous sensor readings for calibration
		 * @param OnEvent Callback for event notification
         */
		void start(void (*OnEvent)(uint32_t event)=0);
	
		/** Stop continuous sensor readings 
         */
		void stop();
	
		/** Test module
         */
		void test(uint32_t testCode);


        
    private:
		Ticker timer;
		uint8_t stat;
		uint8_t devAddr;
		int32_t xMax, xZero, xMin, xThr, yMax, yZero, yMin, yThr, zMax, zZero, zMin, zThr;
		uint16_t numSamples;
		double dx, dy, dz;
		int g[3];
        I2C * _device;
        void writeReg(uint8_t addr, byte reg, byte value);
        byte readReg(uint8_t addr, byte reg);
		void read(void);
		void (*OnEvent)(uint32_t event);
};
 
#endif
            
