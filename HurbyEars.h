/*************************************************************************************
		This file is the header file of the Hurby's Ears class
 *************************************************************************************/


#ifndef HURBY_EARS_H
#define HURBY_EARS_H

#include "mbed.h"
#include "Ear.h"

class HurbyEars{
    public:
		// constructor
        HurbyEars(Servo *l, Servo *r, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~HurbyEars(void);
		// move an ear to a predefined position and with a speed
		void Move(uint8_t earId, uint8_t mcode, uint8_t speed, float percent=1.0);
		// public const events for moving action result
 		static const uint32_t EV_MOVE_COMPLETED  = 0x00000004;
        
    private:
		// eyes references
        Ear *leftEar;
        Ear *rightEar;
		// event pendig flags
		uint32_t evPending;
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;
		// private const mask actions
 		static const uint32_t EV_LEFT_EAR_PENDING   = 0x00000001;
 		static const uint32_t EV_RIGHT_EAR_PENDING  = 0x00000002;
		
		// static callback routin to propagate movement events to external caller
		static void MoveCallback(void *scope, void *args){
			// <scope> has an own reference
			HurbyEars *self = static_cast<HurbyEars*>(scope);
			// <args> has a reference of the Eye which raised the event
			Ear* ear = (Ear*)args;
			// depending on the notifier object, and eventflag is raised
			if(ear == self->leftEar){
				self->evPending &= ~EV_LEFT_EAR_PENDING;
			}
			else if(ear == self->rightEar){
				self->evPending &= ~EV_RIGHT_EAR_PENDING;
			}
			else{
				return;
			}
			// if no pending actions, then clear flags and notify caller
			if(self->evPending == 0){
				self->OnEventCallback(self->eventCaller, (void*)HurbyEars::EV_MOVE_COMPLETED);	
			}
		}
};

#endif
