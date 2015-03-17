/*************************************************************************************
		This file is the header file of the Foot class
 *************************************************************************************/


#ifndef _FOOT_H
#define _FOOT_H

#include "mbed.h"
#include "Servo.h"

class Foot{
    public:
		// constructor
        Foot(uint8_t id, Servo *pin, void (*OnMoveCallback)(void*, void*), void *moveScope);
		// destructor
        ~Foot(void);
		// starts a movement action to a predefined location and with a speed
        void Move(uint8_t mcode, uint8_t speed, float percent=1.0);
		// public const predefined locations
        static const uint8_t MOVE_CENTER = 0;
        static const uint8_t MOVE_BACK   = 1;
        static const uint8_t MOVE_FRONT  = 2;
		// public const predefined speeds
        static const uint8_t MOVE_SLOW   = 0;	//!< 180º in 3.0s
        static const uint8_t MOVE_NORMAL = 1;	//!< 180º in 1.5s
        static const uint8_t MOVE_FAST   = 2;	//!< 180º in 0.5s
        static const uint8_t MOVE_SHOT	 = 3;	//!< 180º instantaneously		
		//public const predefined foot types
		static const uint8_t IS_RIGHT    = 0;
		static const uint8_t IS_LEFT     = 1;	
		static const uint8_t BOTH		 = 2;

    private:
		// internal callback during movement action
        void OnMoveCallback(void);
		// installed move action completion callback from external caller
		void (*OnMoveEnd)(void *scope, void *args);
		// external move caller reference
		void *moveCaller;
		// low level servo reference
        Servo *servo;
		// ticker timer for async operations (move)
        Ticker tmr;
		// path during movement action (slowest movement is 180º in 3sec --> 60 steps each 50ms, so
		// array of [64] is enough
        float degree[64];
		uint8_t numSteps;
		uint8_t idStep;
		// actual degree position
		float position;
		// ear type (left/right)
		uint8_t type;
};

#endif
