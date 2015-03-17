/*************************************************************************************
	Satiation module is a motive-based behaviour. This motive is active and modulated by
	Hurby's playing level. This emotion depends on two inputs: TSLP (time since last play)
	and PTT (playing time trend).
 *************************************************************************************/


#ifndef _Satiation_H
#define _Satiation_H

#include "mbed.h"
#include "brain_types.h"
#include "eyes_img.h"
#include "tts_types.h"
#include "Subsumption.h"
#include "Fuzzy.h"



class Satiation{
    public:
		// constructor
        Satiation(uint8_t prio);
		// destructor
        ~Satiation(void);
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
		uint32_t ptt;
		// output
		ACTION_T action;
		// level of priority
		uint8_t priority;
		// satiation value
		float satiation;
		// satiation variation
		float lastModulation;
		// goal value
		float activity;
		float lastActivity;
		// serial debugger
		Serial * serial;
		// internal implementation
		void Run(float modulation);
		// sets the action
		void SetAction(float modulation);
		// behaviour interface delegation
		static void SetInput(void* obj, void* args){
			Satiation *self = static_cast<Satiation*>(obj);
			INPUT_T *inp = (INPUT_T*)args;
			self->tslp = inp->tslp;
			self->ptt = inp->ptt;
		}			
		static void SetInputParam(void* obj, void* args, uint8_t argId){
			// <scope> has a copy of this reference
			Satiation *self = static_cast<Satiation*>(obj);
			switch(argId){
				case INPUT_TYPE_TSLP:{
					self->tslp = *(uint32_t*)args;	
					break;
				}
				case INPUT_TYPE_PTT:{
					self->ptt = *(uint32_t*)args;	
					break;
				}
			}
		}			
		static void RunBehaviour(void* obj, float modulation, void (*SetInputParam)(void* obj, void* args, uint8_t argId), void* extObj){
			// <scope> has a copy of this reference
			Satiation *self = static_cast<Satiation*>(obj);
			BEHAVIOUR_T *b = (BEHAVIOUR_T*)extObj;
			self->Run(modulation);
			SetInputParam(b->obj, &self->satiation, INPUT_TYPE_SATIATION);
		}			
		static void GetOutput(void* obj, ACTION_T** action){
			// <scope> has a copy of this reference
			Satiation *self = static_cast<Satiation*>(obj);
			*action = &self->action;
		}			
};

#endif
