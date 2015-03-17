/*************************************************************************************
	Frustation module is a motive-based behaviour. This motive is active and modulated by
	Hurby's anger level. This frustation emotion depends on two inputs: TLU (time lifted
	up), it is the time elapsed since it was lifted by an ear, and TSLL (time since last
	lift).
 *************************************************************************************/


#ifndef _Frustation_H
#define _Frustation_H

#include "mbed.h"
#include "brain_types.h"
#include "eyes_img.h"
#include "tts_types.h"
#include "Subsumption.h"
#include "Fuzzy.h"



class Frustation{
    public:
		// constructor
        Frustation(uint8_t prio);
		// destructor
        ~Frustation(void);
		// Get behaviour
        BEHAVIOUR_T* GetBehaviour(void);
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial, int8_t level);
		
    private:
		// behaviour 
		BEHAVIOUR_T bhv;
		// inputs
		uint32_t tlu;
		uint32_t tsll;
		// output
		ACTION_T action;
		// level of priority
		uint8_t priority;
		// frustation value
		float frustation;
		// frustation variation
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
			Frustation *self = static_cast<Frustation*>(obj);
			INPUT_T *inp = (INPUT_T*)args;
			self->tsll = inp->tsll;
			self->tlu = inp->tlu;
		}			
		static void SetInputParam(void* obj, void* args, uint8_t argId){
			// <scope> has a copy of this reference
			Frustation *self = static_cast<Frustation*>(obj);
			switch(argId){
				case INPUT_TYPE_TSLL:{
					self->tsll = *(uint32_t*)args;	
					break;
				}
				case INPUT_TYPE_TLU:{
					self->tlu = *(uint32_t*)args;	
					break;
				}
			}
		}			
		static void RunBehaviour(void* obj, float modulation, void (*SetInputParam)(void* obj, void* args, uint8_t argId), void* extObj){
			// <scope> has a copy of this reference
			Frustation *self = static_cast<Frustation*>(obj);
			BEHAVIOUR_T *b = (BEHAVIOUR_T*)extObj;
			self->Run(modulation);
			SetInputParam(b->obj, &self->frustation, INPUT_TYPE_FRUSTATION);
		}			
		static void GetOutput(void* obj, ACTION_T** action){
			// <scope> has a copy of this reference
			Frustation *self = static_cast<Frustation*>(obj);
			*action = &self->action;
		}			
};

#endif
