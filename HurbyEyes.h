/*************************************************************************************
		This file is the header file of the Hurby's Eyes class
 *************************************************************************************/


#ifndef HURBY_EYES_H
#define HURBY_EYES_H

#include "mbed.h"
#include "Eye.h"

class HurbyEyes{
    public:
		// constructor
        HurbyEyes(SPI *spil, DigitalOut *csl, DigitalOut *cdl, DigitalOut *rstl,DigitalOut *bll, SPI * spir, DigitalOut *csr, DigitalOut *cdr, DigitalOut *rstr,DigitalOut *blr);
		// destructor
        ~HurbyEyes(void);
		// loads an image and install event handlers
        void LoadImage(BITMAP_FLASH *img[], void (*EventCallback)(void*, void*), void *scope);
		// move eyes to a predefined position
		void Move(uint8_t mcode, uint8_t moveSpeed = Eye::MOVE_FAST, uint8_t blinkSpeed = Eye::BLINK_FAST, float percent=1.0);
		// closes eyes
		void Close(void);
		// get target position
		uint8_t GetPosition(void);	
		// public const events for blinking and moving action result
 		static const uint32_t EV_BLINK_COMPLETED = 0x00000001;
 		static const uint32_t EV_MOVE_COMPLETED  = 0x00000002;
        
    private:
		// eyes references
        Eye *leftEye;
        Eye *rightEye;
		// event pending flags
		uint32_t evflags;
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;
		// private const events
		static const uint32_t EV_LEFT_EYE_BLINK_END  = 0x00000001;
 		static const uint32_t EV_RIGHT_EYE_BLINK_END = 0x00000002;
 		static const uint32_t EV_BOTH_EYES_BLINK_END = 0x00000003;
 		static const uint32_t EV_LEFT_EYE_MOVE_END   = 0x00000004;
 		static const uint32_t EV_RIGHT_EYE_MOVE_END  = 0x00000008;
 		static const uint32_t EV_BOTH_EYES_MOVE_END  = 0x0000000C;
		
		// static callback routin to propagate blinking events to external caller
		static void BlinkCallback(void *scope, void *args){
			// <scope> has an own reference
			HurbyEyes *self = static_cast<HurbyEyes*>(scope);
			// <args> has a reference of the Eye which raised the event
			Eye* eye = (Eye*)args;
			// depending on the notifier object, and eventflag is raised
			if(eye == self->leftEye){
				self->evflags |= EV_LEFT_EYE_BLINK_END;
			}
			else if(eye == self->rightEye){
				self->evflags |= EV_RIGHT_EYE_BLINK_END;
			}
			// if both eyes completed a blinking action, the flags are cleared and caller is notified
			if((self->evflags & EV_BOTH_EYES_BLINK_END) == EV_BOTH_EYES_BLINK_END){
				self->evflags &= ~EV_BOTH_EYES_BLINK_END;
				self->OnEventCallback(self->eventCaller, (void*)HurbyEyes::EV_BLINK_COMPLETED);
			}
		}
		
		// static callback routin to propagate movement events to external caller
		static void MoveCallback(void *scope, void *args){
			// <scope> has an own reference
			HurbyEyes *self = static_cast<HurbyEyes*>(scope);
			// <args> has a reference of the Eye which raised the event
			Eye* eye = (Eye*)args;
			// depending on the notifier object, and eventflag is raised
			if(eye == self->leftEye){
				self->evflags |= EV_LEFT_EYE_MOVE_END;
			}
			else if(eye == self->rightEye){
				self->evflags |= EV_RIGHT_EYE_MOVE_END;
			}
			// if both eyes completed a movement action, the flags are cleared and caller is notified
			if((self->evflags & EV_BOTH_EYES_MOVE_END) == EV_BOTH_EYES_MOVE_END){
				self->evflags &= ~EV_BOTH_EYES_MOVE_END;
				self->OnEventCallback(self->eventCaller, (void*)HurbyEyes::EV_MOVE_COMPLETED);
			}
		}
};

#endif
