/*************************************************************************************
		This file is the implementation file of the HurbyFeet class
 *************************************************************************************/



#include "HurbyFeet.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Main Constructor *****************************************************************/
HurbyFeet::HurbyFeet(Servo *l, Servo *r, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	this->evPending = 0;

    // Initiates both ears and installs intself as caller
    this->leftFoot = new Foot(Foot::IS_LEFT, l, HurbyFeet::MoveCallback, this);
    this->rightFoot = new Foot(Foot::IS_RIGHT, r, HurbyFeet::MoveCallback, this);
	this->Move(Foot::BOTH, BALANCE_CENTER, Foot::MOVE_NORMAL);
}

/** Main Destructor *****************************************************************/
HurbyFeet::~HurbyFeet(void){
	// destroy allocated objects
    if(this->leftFoot){
        delete(this->leftFoot);
    }
    if(this->rightFoot){
        delete(this->rightFoot);
    }
}

/** Move ear with a move_code ********************************************************/
void HurbyFeet::Move(uint8_t footId, uint8_t mcode, uint8_t speed, float percent){
	// move ear to position with speed
	switch(footId){
		case Foot::IS_LEFT:{
			this->evPending |= HurbyFeet::EV_LEFT_FOOT_PENDING;
			this->leftFoot->Move(mcode, speed, percent);
			break;
		}
		case Foot::IS_RIGHT:{
			this->evPending |= HurbyFeet::EV_RIGHT_FOOT_PENDING;
			this->rightFoot->Move(mcode, speed, percent);
			break;
		}
		case Foot::BOTH:{
			this->evPending |= HurbyFeet::EV_LEFT_FOOT_PENDING;
			this->evPending |= HurbyFeet::EV_RIGHT_FOOT_PENDING;
			if(mcode == BALANCE_CENTER){
				this->leftFoot->Move(Foot::MOVE_CENTER, speed);
				this->rightFoot->Move(Foot::MOVE_CENTER, speed);
				break;
			}
			if(mcode == BALANCE_LEFT){
				this->leftFoot->Move(Foot::MOVE_CENTER, speed);
				this->rightFoot->Move(Foot::MOVE_FRONT, speed, percent);
				break;
			}
			if(mcode == BALANCE_RIGHT){
				this->leftFoot->Move(Foot::MOVE_FRONT, speed, percent);
				this->rightFoot->Move(Foot::MOVE_CENTER, speed);
				break;
			}
			if(mcode == BALANCE_BACK){
				this->leftFoot->Move(Foot::MOVE_FRONT, speed, percent);
				this->rightFoot->Move(Foot::MOVE_FRONT, speed, percent);
				break;
			}
			if(mcode == BALANCE_FRONT){
				this->leftFoot->Move(Foot::MOVE_BACK, speed, percent);
				this->rightFoot->Move(Foot::MOVE_BACK, speed, percent);
				break;
			}
			break;
		}
	}
}


