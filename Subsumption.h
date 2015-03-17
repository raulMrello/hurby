/*************************************************************************************
	Sensory Cortex module handles touch events detected by the internal module
	HurbyTouch (a periodic touch event scanner). This modules builds input data
	for Behaviour Subsumption Architecture module. This input data is formed by
	touch event flags, TSLP and PTT variables. For timestamp get/set it needs both
	time calendar api and ps backup
 *************************************************************************************/


#ifndef _Subsumption_H
#define _Subsumption_H

#include <stdint.h>
#include <stdlib.h>
#include "brain_types.h"



class Subsumption{
    public:
		// constructor
        Subsumption(uint8_t numLevels, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~Subsumption(void);
		// Add a behaviour
        void AddBehaviour(BEHAVIOUR_T *b);
		// Executes behaviour with new input data
		void Execute(INPUT_T * inpData);
		
    private:
		// behaviour levels and actions
		BEHAVIOUR_T **level;
		ACTION_T **actions;
		uint8_t numLevels;
		// callback installed by an external caller to be invoked when input data ready
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;		
};

#endif
