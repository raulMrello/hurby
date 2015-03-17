/*************************************************************************************
		This file is the header file of the Hurby class
 *************************************************************************************/


#ifndef HURBY_H
#define HURBY_H

#include "mbed.h"
#include "SensoryCortex.h"
#include "MotorCortex.h"
#include "Subsumption.h"
#include "Boredom.h"
#include "Frustation.h"
#include "Satiation.h"
#include "Happiness.h"

//#define ENABLE_SERIAL_LOG
//#define ENABLE_SERIAL_BRIDGE_RECORD
//#define ENABLE_SERIAL_BRIDGE_DOWNLOAD
class Hurby{
    public:
		// constructor
        Hurby(Serial *pc = 0);
		// destructor
        ~Hurby(void);
	
    private:
		// debugging
		void Debug(char* txt);
		static const int8_t LOGGER_LEVEL = 1;
		
		/***** BRAIN CORTEX MODULES *****/
        SensoryCortex *sensoryCortex;
        MotorCortex *motorCortex;
	
		/***** BEHAVIOUR MODULES *****/
        Subsumption *ssa;
		Boredom *boredom;
		Frustation *frustation;
		Satiation *satiation;
        Happiness *happiness;

		/***** EVENT HANDLING *****/	
	
		// Sensory Cortex event handling routine
		static void SensoryEvents(void *scope, void *args){
			// <scope> has a copy of hurby reference
			Hurby *self = static_cast<Hurby*>(scope);
			// send INPUT_T data to Subsumption architecture (ssa) module
			self->ssa->Execute((INPUT_T*)args);
		}					
		
		// Subsumption event handling routine
		static void SubsumptionEvents(void *scope, void *args){
			// <scope> has a copy of hurby reference
			Hurby *self = static_cast<Hurby*>(scope);
			// send ACTION_T * list to motor cortex
			self->motorCortex->AddActionList((ACTION_T **)args);
		}			
};

#endif
