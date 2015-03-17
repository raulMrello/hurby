/*************************************************************************************
		This file is the header file of the Hurby's Feet class
 *************************************************************************************/


#ifndef HURBY_FEET_H
#define HURBY_FEET_H

#include "mbed.h"
#include "Foot.h"

class HurbyFeet{
    public:
		// constructor
        HurbyFeet(Servo *l, Servo *r, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~HurbyFeet(void);
		// move an ear to a predefined position and with a speed
		void Move(uint8_t footId, uint8_t mcode, uint8_t speed, float percent=1.0);
		// public const events for moving action result
 		static const uint32_t EV_MOVE_COMPLETED  = 0x00000008;
		// public const body movements
		static const uint8_t BALANCE_CENTER = 0;
		static const uint8_t BALANCE_LEFT   = 1;
		static const uint8_t BALANCE_RIGHT  = 2;
		static const uint8_t BALANCE_BACK   = 3;
		static const uint8_t BALANCE_FRONT  = 4;
	
        
    private:
		// Feet references
        Foot *leftFoot;
        Foot *rightFoot;
		// event pendig flags
		uint32_t evPending;
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;
		// private const mask actions
 		static const uint32_t EV_LEFT_FOOT_PENDING   = 0x00000001;
 		static const uint32_t EV_RIGHT_FOOT_PENDING  = 0x00000002;
		
		// static callback routin to propagate movement events to external caller
		static void MoveCallback(void *scope, void *args){
			// <scope> has an own reference
			HurbyFeet *self = static_cast<HurbyFeet*>(scope);
			// <args> has a reference of the Eye which raised the event
			Foot* foot = (Foot*)args;
			// depending on the notifier object, and eventflag is raised
			if(foot == self->leftFoot){
				self->evPending &= ~EV_LEFT_FOOT_PENDING;
			}
			else if(foot == self->rightFoot){
				self->evPending &= ~EV_RIGHT_FOOT_PENDING;
			}
			else{
				return;
			}
			// if no pending actions, then clear flags and notify caller
			if(self->evPending == 0){
				self->OnEventCallback(self->eventCaller, (void*)HurbyFeet::EV_MOVE_COMPLETED);	
			}
		}
};

#endif
