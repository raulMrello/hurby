/*************************************************************************************
		This file is the header file of the HurbyMouth class
 *************************************************************************************/


#ifndef _HURBYMOUTH_H
#define _HURBYMOUTH_H

#include "mbed.h"
#include "Servo.h"
#include "tts_types.h"

class HurbyMouth{
    public:
		// constructor
        HurbyMouth(Servo *pin, void (*OnMoveCallback)(void*, void*), void *moveScope);
		// destructor
        ~HurbyMouth(void);
		// starts a talk movement during several seconds
        int8_t Talk(uint8_t tts_id);
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial);
		// public const events for moving action result
 		static const uint32_t EV_MOVE_COMPLETED  = 0x00000020;
		
    private:		
		// internal async task
		void AsyncTask(void);
		// event callback installed by an external caller
		void (*OnMoveCallback)(void *scope, void *args);
		// external caller reference
		void *moveCaller;
		// low level servo reference
        Servo *servo;
		// ticker timer for async operations (move)
        Ticker tmr;
		// TTS reference
		TTS_FLASH *tts;
		// TTS step
		int8_t tts_step;
		int8_t tts_numSteps;
		// movement amplitude
		float percent;
		// step movement
		float diff;
		// actual degree position
		float position;
		// logger
		Serial * serial;

};

#endif
