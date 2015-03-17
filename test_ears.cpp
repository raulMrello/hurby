/*************************************************************************************
		This file includes test cases for: 
			- 0: Ear meve back slow
			- 1: Ear move front fast
			- 2: Ear move center one shot
			- 3: Ear move back normal
			- 4: Ear move front fast
		Left ear will start from first test and right ear will start from last one.
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;


/** Callback invoked after eyes movement trajectory is completed *********************/
static void EarsMoved(){
	// test case direction
	static uint8_t increasing=1;
	static uint8_t i=0;
	static uint8_t j=4;
	
	// fixed position and speeds for test
	const uint8_t positions[]={Ear::MOVE_BACK, Ear::MOVE_FRONT, Ear::MOVE_CENTER, Ear::MOVE_BACK,   Ear::MOVE_FRONT};
	const uint8_t speeds[]={   Ear::MOVE_SLOW, Ear::MOVE_FAST,  Ear::MOVE_SHOT,   Ear::MOVE_NORMAL, Ear::MOVE_FAST};
	
	// set ears to a position and installs itself as callback caller to be notified after movement completion
	hurby->MoveEar(Ear::IS_LEFT, positions[i], speeds[i], EarsMoved);
	hurby->MoveEar(Ear::IS_RIGHT, positions[j], speeds[j], EarsMoved);

	// set next state
	if(increasing){
		if(i<4){
			i++;
			j--;
		}
		else{
			increasing=0;
			i--;
			j++;
		}
	}
	else{
		if(i>0){
			i--;
			j++;
		}
		else{
			increasing=1;
			i++;
			j--;
		}
	}
}

void TestEars(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// invoke movement routine
	EarsMoved();
}

