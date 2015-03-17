/*************************************************************************************
		This file is the header file of the Eye class
 *************************************************************************************/


#ifndef _EYE_H
#define _EYE_H

#include "mbed.h"
#include "LCDDriver.h"

class Eye{
    public:
		// constructor
        Eye(uint8_t id, SPI * spi_port, DigitalOut *cs_pin, DigitalOut *cd_pin, DigitalOut *rst_pin,DigitalOut *bl_pin);
		// destructor
        ~Eye(void);
		// load an image array and blink/move callbacks
        void Load(BITMAP_FLASH *imgs[], void (*OnBlinkCallback)(void*,void*), void* blinkScope, void (*OnMoveCallback)(void*,void*), void *moveScope);
		// switch off eye
        void Close(void);
		// starts a blinking cycle
        void Blink(void);
		// starts a movement action to a predefined location
        void Move(uint8_t mcode, uint8_t moveSpeed = 2, uint8_t blinkSpeed = 2, float percent=1.0);
		// get target position
		uint8_t GetPosition(void);
		// public const predefined locations
        static const uint8_t MOVE_NW = 0;
        static const uint8_t MOVE_NC = 1;
        static const uint8_t MOVE_NE = 2;
        static const uint8_t MOVE_CW = 3;
        static const uint8_t MOVE_CC = 4;
        static const uint8_t MOVE_CE = 5;
        static const uint8_t MOVE_SW = 6;
        static const uint8_t MOVE_SC = 7;
        static const uint8_t MOVE_SE = 8;
		//public const predefined eye types
		static const uint8_t IS_RIGHT = 0;
		static const uint8_t IS_LEFT = 1;	
		//public const predefined eye movement speed
		static const uint8_t MOVE_SLOW    = 0;
		static const uint8_t MOVE_NORMAL  = 1;
		static const uint8_t MOVE_FAST    = 2;
		static const uint8_t BLINK_SLOW   = 0;
		static const uint8_t BLINK_NORMAL = 1;
		static const uint8_t BLINK_FAST   = 2;

    private:
		// internal callback during blinking cycle
        void OnBlinkCallback(void);
		// internal callback during movement action
        void OnMoveCallback(void);
		// installed blink cycle completion callback from external caller
		void (*OnBlinkEnd)(void *scope, void *args);
		// external blink caller reference
		void *blinkCaller;
		// installed move action completion callback from external caller
		void (*OnMoveEnd)(void *scope, void *args);
		// external move caller reference
		void *moveCaller;
		// time for blinking operation
        float blinkStep;
		// time for blinking delay since wide open eye
        double blinkDelay;
		// time for move action since wide open eye
        double moveDelay;
		// blinking state
        uint8_t blinkStat;
		// move action state
        uint8_t moveStat;
		// low level driver reference
        LCDDriver *lcd;
		// ticker timer for async operations (blink/move)
        Ticker tmr;
		// installed image array
        BITMAP_FLASH *img[5];
		// active image
        BITMAP_FLASH *activeImg;
		// contrast level of backlight
        uint8_t contrast;
		// XY path during movement action
        uint8_t x[4];
        uint8_t y[4];
		// XY origin of loaded image
        uint8_t left;
        uint8_t top;
		// position target
		uint8_t targetPosition;
		// eye type (left/right)
		uint8_t type;
};

#endif
