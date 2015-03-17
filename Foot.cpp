/*************************************************************************************
		This file is the implementation file of the Foot class
 *************************************************************************************/


#include "Foot.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Constructor **********************************************************************/
Foot::Foot(uint8_t id, Servo * pin, void (*OnMoveCallback)(void*, void*), void *moveScope){
	// setup default data
    this->servo = pin;
	this->type = id;
	this->OnMoveEnd = (OnMoveCallback)? OnMoveCallback : NULL_CALLBACK_2;
	this->moveCaller = moveScope;
	this->position = 0.5;
	for(uint8_t i=0;i<64;i++){
		this->degree[i] = 0.5;
	}
}

/** Destructor ***********************************************************************/
Foot::~Foot(void){
}

/** Movement service *****************************************************************/
void Foot::Move(uint8_t mcode, uint8_t speed, float percent){  
	// table for slow,normal,fast speeds values according with:
	//	SLOW   => rotates 180º in 3.0s => 3.0/180 = 0.01666
	//	NORMAL => rotates 180º in 1.5s => 1.5/180 = 0.00833
	//	FAST   => rotates 180º in 0.5s => 0.5/180 = 0.00277
	const float SECONDS_PER_DEGREE[3]= {0.01666, 0.00833, 0.00277};
	float destination;

	// intializes movement variables
    this->tmr.detach();
	// setup final location depending on predefined code
    switch(mcode){
        case MOVE_CENTER:{
            destination = 0.5;
            break;
        }
        case MOVE_BACK:{
			if(this->type == IS_RIGHT){
				destination = (0.5*percent) + 0.5;
			}
			else{
				destination = 0.5 - (0.5*percent);
			}
            break;
        }
        case MOVE_FRONT:{
			if(this->type == IS_RIGHT){
				destination = 0.5 - (0.5*percent);
			}
			else{
				destination = (0.5*percent) + 0.5;
			}
            break;
        }
    }	
	// limit range according to speed. For FAST and SHOT values, range is [0.25..0.75]
	if(speed >= Foot::MOVE_FAST){
		if(destination < 0.25){
			destination = 0.25;
		}
		if(destination > 0.75){
			destination = 0.75;
		}
	}
	// if one shot movement, then apply inmediately
	if(speed == Foot::MOVE_SHOT){
		this->degree[0] = destination;
		this->degree[1] = destination;
		this->numSteps = 2;
	}
	// else... calculate degree variation
	else{
		float diff = destination - this->position;
		// calculate num of steps according with selected speed (step pace is 50ms = 0.05)
		this->numSteps = (uint8_t)(((180*abs(diff))*SECONDS_PER_DEGREE[speed])/0.05)+1;
		// if same position, then exits
		if(this->numSteps <= 2){
			this->degree[0] = destination;
			this->degree[1] = destination;
			this->numSteps = 2;
		}
		else{
			diff = diff/this->numSteps;
			// first step is the actual position
			this->degree[0] = this->position;
			// last step is the destination position
			this->degree[this->numSteps-1] = destination;
			// generate intermediate steps
			for(uint8_t i=1; i < this->numSteps-1; i++){
				this->degree[i] = this->degree[i-1] + diff;
			}    
		}
	}
    // starts movement
	this->idStep = 0;
    this->OnMoveCallback();
}

/** Movement callback ****************************************************************/
void Foot::OnMoveCallback(void){
	// servo will update each 50ms
	const float UPDATE_PACE = 0.05; 
	// load position
	this->position = this->degree[this->idStep];
	this->servo->write(this->position);
	// if more steps pending, then re-schedule
	if(++this->idStep < this->numSteps){
		this->tmr.attach(this, &Foot::OnMoveCallback, UPDATE_PACE);
		return;
	}
	// else... terminates and notify
	this->tmr.detach();
    this->numSteps = 0;
	this->idStep = 0;
	// notify movement completion event to external caller
	this->OnMoveEnd(this->moveCaller, this);    
}
