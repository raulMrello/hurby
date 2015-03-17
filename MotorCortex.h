/*************************************************************************************
		The MotorCortex module is in charged of controlling eyes, ears, feet 
		and mouth movements. It receives information from behavioural states
		and then it selects the most up to date and powerful information to
		move Hurby muscles (eyes, ears, feet and mouth) accordingly.
 *************************************************************************************/


#ifndef _MotorCortex_H
#define _MotorCortex_H

#include "mbed.h"
#include "HurbyEyes.h"
#include "HurbyEars.h"
#include "HurbyFeet.h"
#include "HurbyMouth.h"
#include "HurbySpeech.h"
#include "brain_types.h"
#include "tts_types.h"



class MotorCortex{
    public:
		// constructor
        MotorCortex(SPI *spil, DigitalOut *csl, DigitalOut *cdl, DigitalOut *rstl,DigitalOut *bll, 
					SPI * spir, DigitalOut *csr, DigitalOut *cdr, DigitalOut *rstr,DigitalOut *blr, 
					Servo *lear, Servo *rear, 
					Servo *lfoot, Servo *rfoot,
					Servo *mouth, HurbySpeech * speech);
		// destructor
        ~MotorCortex(void);
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial, int8_t level);
		// adds a new action
        int8_t AddActionList(ACTION_T** actions);
		// get selected action
		void GetAction(ACTION_T *a){
			memcpy(a, &this->selectedAction, sizeof(ACTION_T));
		}
		// public const events for moving action result
 		static const uint32_t EV_MOVE_COMPLETED  = 0x00000020;
		// public const error codes
		static const int8_t ERR_ACTION_LIST_FULL = -1;
		
    private:
		// max action list entries
		static const uint8_t ACTION_LIST_SIZE = 8;
		// internal async task
		void AsyncTask(void);
		// actuator control
		uint8_t MoveEyes(void);
		void MoveEars(uint8_t eye_pos);
		void MoveFeet(uint8_t eye_pos);
		void MoveMouth(int8_t isNewAction);
		void Talk(void);
		double GetRnd(float min, float max);
		// actuators
		HurbyEyes* eyes;
		HurbyEars* ears;
		HurbyFeet* feet;
		HurbyMouth* mouth;
		HurbySpeech* speech;
		// ticker timer for async operations (move)
        Ticker eyesTmr, earsTmr, feetTmr, mouthTmr, repeatTmr;
		// action list, count and index
		ACTION_T actionList[ACTION_LIST_SIZE];
		ACTION_T selectedAction;
		uint8_t actionCount;
		// status flag
		uint8_t isWaiting;
		uint8_t isVoiceReady;
		// serial debugger
		Serial * serial;
		// common event handling routine
		static void HandleEyesEvents(void *scope, void *args){
			//MotorCortex *self = static_cast<MotorCortex*>(scope);
		}			
		
		// common event handling routine
		static void HandleEarsEvents(void *scope, void *args){
			//MotorCortex *self = static_cast<MotorCortex*>(scope);
		}			
		
		// common event handling routine
		static void HandleFeetEvents(void *scope, void *args){
			//MotorCortex *self = static_cast<MotorCortex*>(scope);
		}			
		
		// common event handling routine
		static void HandleMouthEvents(void *scope, void *args){
			MotorCortex *self = static_cast<MotorCortex*>(scope);
			double delay;
			// checks if talk must be repeated
			if(self->selectedAction.mouth_sound_repeat_range[1]>0){
				self->isVoiceReady = 0;
				// if initial range
				if(self->selectedAction.mouth_sound_repeat_range[0]>0){
					delay = self->GetRnd(self->selectedAction.mouth_sound_repeat_range[0], self->selectedAction.mouth_sound_repeat_range[1]);
				}
				// else 
				else{
					delay = self->selectedAction.mouth_sound_repeat_range[1];
				}
				self->mouthTmr.attach(self, &MotorCortex::Talk, delay);	
			}	
			else{
				self->isVoiceReady = 1;
			}
		}			
};

#endif
