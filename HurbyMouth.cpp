/*************************************************************************************
		This file is the implementation file of the HurbyMouth class
 *************************************************************************************/


#include "HurbyMouth.h"

#define MOUTH_STEP_TIME	(float)0.05

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Constructor **********************************************************************/
HurbyMouth::HurbyMouth(Servo * pin, void (*EventCallback)(void*, void*), void *moveScope){
	// setup default data
    this->servo = pin;
	this->OnMoveCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->moveCaller = moveScope;
	this->position = 0;
	this->tts = 0;
	this->tts_numSteps = 0;
	this->diff = 0;
	this->servo->write(this->position);
}

/** Destructor ***********************************************************************/
HurbyMouth::~HurbyMouth(void){
}

/** En/Dis logger ********************************************************************/
void HurbyMouth::EnDisDebugging(Serial * serial){
	this->serial = serial;
}

/** Movement service *****************************************************************/
int8_t HurbyMouth::Talk(uint8_t tts_id){
	// stops timer
    this->tmr.detach();
	// get reference
	this->tts = (TTS_FLASH*)tts_list[tts_id];
	// if no movements then exit
	if(!this->tts || this->tts->count <= 0)
		return -1;
	// restart step count
	this->tts_step = 0;
	// check OOB or Nan errors
	if(this->position >= 0.0 &&  this->position <= 1.0){		
		// get the number of steps if first path track
		this->tts_numSteps = (int8_t)(this->tts->period[this->tts_step]/MOUTH_STEP_TIME);
		// setup final location depending on actual position
		this->diff = (this->tts->percent[this->tts_step] - this->position)/(float)this->tts_numSteps;
		if(this->serial){
			this->serial->printf("\r\nMouth: ");
		}	
		// starts movement
		this->AsyncTask();
	}
	else{
		this->position = 0;
		this->diff = 0;
	}
	return 0;
}

/** Movement callback ****************************************************************/
void HurbyMouth::AsyncTask(void){
	// load next position and move to it
	if(this->serial){
		this->serial->printf("%f ",this->position);
	}		
	this->position = this->position + this->diff;
	this->servo->write(this->position);
	// if more steps pending, then re-schedule
	if(--this->tts_numSteps > 0){
		this->tmr.attach(this, &HurbyMouth::AsyncTask, MOUTH_STEP_TIME);
		return;
	}	
	// else check if more paths pending
	else if(++this->tts_step < this->tts->count){
		// get the number of steps
		this->tts_numSteps = (int8_t)(this->tts->period[this->tts_step]/MOUTH_STEP_TIME);
		// setup final location depending on actual position
		this->diff = (this->tts->percent[this->tts_step] - this->position)/(float)this->tts_numSteps;
		this->tmr.attach(this, &HurbyMouth::AsyncTask, MOUTH_STEP_TIME);
		return;
	}
	// else if terminated... stops and notify
	if(this->serial){
		this->serial->printf("[END]\r\n");
	}		
	this->tmr.detach();
	this->tts_numSteps = 0;
	this->tts_step = 0;
	// notify movement completion event to external caller
	this->OnMoveCallback(this->moveCaller, (void*)HurbyMouth::EV_MOVE_COMPLETED);    
}
