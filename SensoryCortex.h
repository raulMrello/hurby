/*************************************************************************************
	Sensory Cortex module handles touch events detected by the internal module
	HurbyTouch (a periodic touch event scanner). This modules builds input data
	for Behaviour Subsumption Architecture module. This input data is formed by
	touch event flags, TSLP and PTT variables. For timestamp get/set it needs both
	time calendar api and ps backup
 *************************************************************************************/


#ifndef _SensoryCortex_H
#define _SensoryCortex_H

#include "mbed.h"
#include "PStore.h"
#include "HurbyTouch.h"
#include "HurbyDetect.h"
#include "HurbySpeech.h"
#include "brain_types.h"
#include "tts_types.h"

class SensoryCortex{
    public:
		// constructor
        SensoryCortex(DigitalIn *head, DigitalIn *tummy, DigitalIn *angry, DigitalIn *presence, Serial *vr, DigitalOut * vr_rst, I2C *i2c, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~SensoryCortex(void);
		// adds a new action
        void Start(void);
		// enables/disables presence detection
		void EnDisPresenceDetection(uint8_t endis);
		// enables/disables debug log capability
		void EnDisDebugging(Serial * serial, int8_t level);
		// get speech scanner reference
		HurbySpeech * GetSpeechScanner(void);
		// public const touch events
		static const uint32_t SCAN_TIME = 20;
 		static const uint8_t EV_HEAD  = HurbyTouch::HEAD_CHANGED;
 		static const uint8_t EV_TUMMY = HurbyTouch::TUMMY_CHANGED;
 		static const uint8_t EV_EARS  = HurbyTouch::ANGRY_CHANGED;
 		static const uint8_t EV_DETECT  = HurbyDetect::DETECTED;
 		static const uint8_t EV_SPEECH  = HurbySpeech::RECOGNISED;
		
    private:
		INPUT_T inpData;
		time_t firstTouch;
		time_t lastTouch;
		time_t lastLift;
		time_t lastDetect;
		Ticker tmr;
		uint8_t evtFlags;
		uint8_t isWaiting;
		HurbyTouch * touchScanner;
		HurbyDetect * detectScanner;
		HurbySpeech * speechScanner;
		float scanTime;
		Serial * serial;
		PStore * ps;
		// internal async task
		void AsyncTask(void);
		// callback installed by an external caller to be invoked when input data ready
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;

		// common event handling routine
		static void HandleTouchEvents(void *scope, void *args){
			// <scope> has an own reference
			SensoryCortex *self = static_cast<SensoryCortex*>(scope);
			// stop timeout timer
			self->tmr.detach();
			// get touch/detect event
			self->evtFlags = (uint8_t)(uint32_t)args;
			// invoke state machine
			self->AsyncTask();
		}
	
};

#endif
