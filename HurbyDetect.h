/*************************************************************************************
		This file is the header file of the Hurby's Detect class.
		This Async-Task will evaluate presence sensor each 100ms continuosly 
		while activated. Presence sensor is connected to a DigitalIn pin 
		(pull-down by default) and will fire an event callback each time it detects 
		a change from deasserted-to-asserted or viceversa transition.
 *************************************************************************************/


#ifndef HURBY_DETECT_H
#define HURBY_DETECT_H

#include "mbed.h"

class HurbyDetect{
    public:
		// constructor
        HurbyDetect(DigitalIn *presence, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~HurbyDetect(void);
		// starts periodic scan
		void Start(void);
		// stops periodic scan
		void Stop(void);
		// get stat
		void GetStat(uint8_t *stat, uint8_t *evt);
		// status flags
		uint8_t stat;
		// public const event
		static const uint8_t DETECTED  = 0x08;

        
    private:
		// previous stat
		uint8_t lastStat;
		// Sensor references
        DigitalIn *presence;
		// Periodic timer and async task
		Ticker tmr;
		void AsyncTask(void);
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;

};

#endif
