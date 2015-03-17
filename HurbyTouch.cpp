/*************************************************************************************
		This file is the implementation file of the HurbyFeet class
 *************************************************************************************/



#include "HurbyTouch.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Main Constructor *****************************************************************/
HurbyTouch::HurbyTouch(DigitalIn *head, DigitalIn *tummy, DigitalIn *angry, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	// initializes status
	this->lastStat = 0;
	this->stat = this->lastStat;
	this->head = head;
	this->tummy = tummy;
	this->angry = angry;
}

/** Main Destructor *****************************************************************/
HurbyTouch::~HurbyTouch(void){
}

/** Starts sensor scanning **********************************************************/
void HurbyTouch::Start(void){
	// reads status
	this->stat = 0;	
	if(this->head->read()){
		this->stat |= HEAD_CHANGED;
	}
	if(this->tummy->read()){
		this->stat |= TUMMY_CHANGED;
	}
	if(this->angry->read()){
		this->stat |= ANGRY_CHANGED;
	}
	// starts periodic scanning
	tmr.attach(this, &HurbyTouch::AsyncTask, 0.1);
}

/** Stops sensor scanning ***********************************************************/
void HurbyTouch::Stop(void){
	this->tmr.detach();
	this->lastStat = this->stat;
}

/** Periodo Async-Task **************************************************************/
void HurbyTouch::AsyncTask(void){
	// save last stat
	this->lastStat = this->stat;
	// updates stat
	this->stat = 0;
	if(this->head->read()){
		this->stat |= HEAD_CHANGED;
	}
	if(this->tummy->read()){
		this->stat |= TUMMY_CHANGED;
	}
	if(this->angry->read()){
		this->stat |= ANGRY_CHANGED;
	}
	// notify asserted/deasserted states
	if( ((this->lastStat & HEAD_CHANGED)==0 && (this->stat & HEAD_CHANGED)!=0) ||
		((this->lastStat & TUMMY_CHANGED)==0 && (this->stat & TUMMY_CHANGED)!=0) ||
		((this->lastStat & ANGRY_CHANGED)==0 && (this->stat & ANGRY_CHANGED)!=0) ||
		((this->lastStat & HEAD_CHANGED)!=0 && (this->stat & HEAD_CHANGED)==0) ||
		((this->lastStat & TUMMY_CHANGED)!=0 && (this->stat & TUMMY_CHANGED)==0) ||
		((this->lastStat & ANGRY_CHANGED)!=0 && (this->stat & ANGRY_CHANGED)==0)){	
		uint8_t evt = this->stat ^ this->lastStat;
		if(evt != 0){
			this->OnEventCallback(this->eventCaller, (void*)(uint32_t)evt);
		}
	}
}

/** Get stat and active events ******************************************************/
void HurbyTouch::GetStat(uint8_t *stat, uint8_t *evt){
	*stat = this->stat;
	*evt = this->stat ^ this->lastStat;
}



