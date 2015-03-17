/*************************************************************************************
		This file is the implementation file of the HurbyEars class
 *************************************************************************************/



#include "HurbyEars.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Main Constructor *****************************************************************/
HurbyEars::HurbyEars(Servo *l, Servo *r, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	this->evPending = 0;

    // Initiates both ears and installs intself as caller
    this->leftEar = new Ear(Ear::IS_LEFT, l, HurbyEars::MoveCallback, this);
    this->rightEar = new Ear(Ear::IS_RIGHT, r, HurbyEars::MoveCallback, this);
    // sets center position
	this->Move(Ear::BOTH, Ear::MOVE_CENTER, Ear::MOVE_NORMAL);

}

/** Main Destructor *****************************************************************/
HurbyEars::~HurbyEars(void){
	// destroy allocated objects
    if(this->leftEar){
        delete(this->leftEar);
    }
    if(this->rightEar){
        delete(this->rightEar);
    }
}

/** Move ear with a move_code ********************************************************/
void HurbyEars::Move(uint8_t earId, uint8_t mcode, uint8_t speed, float percent){
	// move ear to position with speed
	switch(earId){
		case Ear::IS_LEFT:{
			this->evPending |= HurbyEars::EV_LEFT_EAR_PENDING;
			this->leftEar->Move(mcode, speed, percent);
			break;
		}
		case Ear::IS_RIGHT:{
			this->evPending |= HurbyEars::EV_RIGHT_EAR_PENDING;
			this->rightEar->Move(mcode, speed, percent);
			break;
		}
		case Ear::BOTH:{
			this->evPending |= HurbyEars::EV_LEFT_EAR_PENDING;
			this->evPending |= HurbyEars::EV_RIGHT_EAR_PENDING;
			this->leftEar->Move(mcode, speed, percent);
			this->rightEar->Move(mcode, speed, percent);
			break;
		}
	}
}

