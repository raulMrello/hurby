/*************************************************************************************
		This file includes test cases for balancing body side to side and back to front 
		at different speed paces, from slow to one_shot.
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;


/** Callback invoked after eyes movement trajectory is completed *********************/
static void FeetMoved(){
	static uint8_t i=0;
	static uint8_t j=0;
	// set positions
	const uint8_t positions[]={	HurbyFeet::BALANCE_LEFT, 
								HurbyFeet::BALANCE_CENTER,
								HurbyFeet::BALANCE_RIGHT, 
								HurbyFeet::BALANCE_CENTER,
								HurbyFeet::BALANCE_BACK,
								HurbyFeet::BALANCE_FRONT,
								HurbyFeet::BALANCE_CENTER};
	// move hurby							
	hurby->MoveFoot(Foot::BOTH, positions[i], Foot::MOVE_NORMAL, FeetMoved);
	// next state
	i++;
	if(i>=7){
		i=0;
		j++;
		if(j >= 2){
			j=0;
		}
	}
}

void TestFeet(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// invoke movement routine
	FeetMoved();
}

