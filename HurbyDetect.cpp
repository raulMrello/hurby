/*************************************************************************************
		This file is the implementation file of the HurbyDetect class
 *************************************************************************************/



#include "HurbyDetect.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Main Constructor *****************************************************************/
HurbyDetect::HurbyDetect(DigitalIn *presence, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	// initializes status
	this->lastStat = 0;
	this->stat = this->lastStat;
	this->presence = presence;
}

/** Main Destructor *****************************************************************/
HurbyDetect::~HurbyDetect(void){
}

/** Starts sensor scanning **********************************************************/
void HurbyDetect::Start(void){
	// reads status
	this->stat = 0;	
	if(this->presence->read()){
		this->stat |= DETECTED;
	}
	// starts periodic scanning
	tmr.attach(this, &HurbyDetect::AsyncTask, 0.1);
}

/** Stops sensor scanning ***********************************************************/
void HurbyDetect::Stop(void){
	this->tmr.detach();
	this->lastStat = this->stat;
}

/** Periodo Async-Task **************************************************************/
void HurbyDetect::AsyncTask(void){
	// save last stat
	this->lastStat = this->stat;
	// updates stat
	this->stat = 0;
	if(this->presence->read()){
		this->stat |= DETECTED;
	}
	// notify asserted/deasserted states
	if( ((this->lastStat & DETECTED)==0 && (this->stat & DETECTED)!=0) ||
		((this->lastStat & DETECTED)!=0 && (this->stat & DETECTED)==0)){	
		uint8_t evt = this->stat ^ this->lastStat;
		if(evt != 0){
			this->OnEventCallback(this->eventCaller, (void*)(uint32_t)evt);
		}
	}
}

/** Get stat and active events ******************************************************/
void HurbyDetect::GetStat(uint8_t *stat, uint8_t *evt){
	*stat = this->stat;
	*evt = this->stat ^ this->lastStat;
}



