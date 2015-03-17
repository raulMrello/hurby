/*************************************************************************************
	Boredom module is a motive-based behaviour. This motive is active and modulated by
	Hurby's boredom level. This boredom depends on different inputs: TSLP (time since
	last play) or TSLD (time since last detect).
 *************************************************************************************/


#ifndef _Boredom_H
#define _Boredom_H

#include "mbed.h"
#include "brain_types.h"
#include "eyes_img.h"
#include "tts_types.h"
#include "Subsumption.h"
#include "Fuzzy.h"



class Boredom{
    public:
		// constructor
        Boredom(uint8_t prio);
		// destructor
        ~Boredom(void);
		// Get behaviour
        BEHAVIOUR_T* GetBehaviour(void);		
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial, int8_t level);
		
    private:
		// Fuzzy Inference module
		Fuzzy *fuzzy;
		// behaviour 
		BEHAVIOUR_T bhv;
		// inputs
		uint32_t tslp;
		uint32_t tsld;
		// output
		ACTION_T action;
		// level of priority
		uint8_t priority;
		// boredom value (actual and old)
		float boredom;
		// boredom variation
		float lastModulation;	
		// goal value
		float activity;
		float lastActivity;
		// serial debugger
		Serial * serial;
		// internal implementation
		void Run(float modulation);
		// sets action
		void SetAction(float modulation);
		// behaviour interface delegation
		static void SetInput(void* obj, void* args){
			Boredom *self = static_cast<Boredom*>(obj);
			INPUT_T *inp = (INPUT_T*)args;
			self->tslp = inp->tslp;
			self->tsld = inp->tsld;
		}			
		static void SetInputParam(void* obj, void* args, uint8_t argId){
			// <scope> has a copy of this reference
			Boredom *self = static_cast<Boredom*>(obj);
			switch(argId){
				case 0:{
					self->tslp = *(uint32_t*)args;	
					break;
				}
				case 1:{
					self->tsld = *(uint32_t*)args;	
					break;
				}
			}
		}			
		static void RunBehaviour(void* obj, float modulation, void (*SetInputParam)(void* obj, void* args, uint8_t argId), void* extObj){
			// <scope> has a copy of this reference
			Boredom *self = static_cast<Boredom*>(obj);
			BEHAVIOUR_T *b = (BEHAVIOUR_T*)extObj;
			self->Run(modulation);
			SetInputParam(b->obj, &self->boredom, INPUT_TYPE_BOREDOM);
		}			
		static void GetOutput(void* obj, ACTION_T** action){
			// <scope> has a copy of this reference
			Boredom *self = static_cast<Boredom*>(obj);
			*action = &self->action;
		}			
};

#endif
