/*************************************************************************************
		This file is the header file of the Hurby's Touch class.
		This Async-Task will evaluate tummy, head and angry sensors
		each 100ms continuosly while activated. Each sensor is connected
		to a DigitalIn pin (pull-down by default) and will fire an event
		callback each time it detects a change from deasserted-to-asserted
		or viceversa transition.
 *************************************************************************************/


#ifndef HURBY_TOCUH_H
#define HURBY_TOUCH_H

#include "mbed.h"

class HurbyTouch{
    public:
		// constructor
        HurbyTouch(DigitalIn *head, DigitalIn *tummy, DigitalIn *angry, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~HurbyTouch(void);
		// starts periodic scan
		void Start(void);
		// stops periodic scan
		void Stop(void);
		// get stat
		void GetStat(uint8_t *stat, uint8_t *evt);
		// status flags
		uint8_t stat;
		// public const body movements
		static const uint8_t HEAD_CHANGED  = 0x01;
		static const uint8_t TUMMY_CHANGED = 0x02;
		static const uint8_t ANGRY_CHANGED = 0x04;
		
        
    private:
		// previous stat
		uint8_t lastStat;
		// Sensor references
        DigitalIn *head;
        DigitalIn *tummy;
		DigitalIn *angry;
		// Periodic timer and async task
		Ticker tmr;
		void AsyncTask(void);
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;

};

#endif
