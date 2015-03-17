/*************************************************************************************
	Happiness level depends on 3 inputs: Frustation, Satiation and Boredom. Each input
	is in range [0..100]. Happiness is calculated through the execution of 2 consecutive
	fuzzy inference systems: Frustation-Boredom which gives a FRBO level. This output is
	combined with the Satiation input to execute a second fuzzy system. The output result
	is a variation of happiness level with range [-20..+20]. Applying this variation to
	last happiness level, give us the final result of happiness which will be ranged into 
	[0..100].
 *************************************************************************************/


#ifndef _Happiness_H
#define _Happiness_H

#include "mbed.h"
#include "brain_types.h"
#include "eyes_img.h"
#include "tts_types.h"
#include "Subsumption.h"
#include "Fuzzy.h"



class Happiness{
    public:
		// constructor
        Happiness(uint8_t prio);
		// destructor
        ~Happiness(void);
		// Get behaviour
        BEHAVIOUR_T* GetBehaviour(void);
		// Get happiness
		float GetHappiness(void);
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial, int8_t level);
		
    private:
		// Fuzzy Inference module
		Fuzzy *frbo;
		Fuzzy *fbsa;
		// behaviour 
		BEHAVIOUR_T bhv;
		// inputs
		float boredom;
		float satiation;
		float frustation;
		// output
		ACTION_T action;
		// level of priority
		uint8_t priority;
		// happiness value (actual and old)
		float happiness;
		float lastHappiness;
		// serial debugger
		Serial * serial;
		// internal implementation
		void Run(void);
		// behaviour interface delegation
		static void SetInput(void* obj, void* args){
			// do nothing. Use last updated input data
		}			
		static void SetInputParam(void* obj, void* args, uint8_t argId){
			// <scope> has a copy of this reference
			Happiness *self = static_cast<Happiness*>(obj);
			switch(argId){
				case 0:{
					self->frustation = *(float*)args;	
					break;
				}
				case 1:{
					self->satiation = *(float*)args;	
					break;
				}
				case 2:{
					self->boredom = *(float*)args;	
					break;
				}
			}
		}			
		static void RunBehaviour(void* obj, float modulation, void (*SetInputParam)(void* obj, void* args, uint8_t argId), void* extObj){
			// <scope> has a copy of this reference
			Happiness *self = static_cast<Happiness*>(obj);
			self->Run();
		}			
		static void GetOutput(void* obj, ACTION_T** action){
			// <scope> has a copy of this reference
			Happiness *self = static_cast<Happiness*>(obj);
			*action = &self->action;
		}			
};

#endif
