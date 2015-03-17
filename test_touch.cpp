/*************************************************************************************
		This file includes test cases for: 
			- Eyes blinking
			- Eyes movement
			- Eyes reloading
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;


/** Callback invoked after eyes movement trajectory is completed *********************/
static void Touched(){
	uint8_t stat,evt;
	hurby->GetTouchStat(&stat, &evt);
	if((stat & HurbyTouch::ANGRY_CHANGED)!=0 && (evt & HurbyTouch::ANGRY_CHANGED)!=0){
		hurby->StartAngry((BITMAP_FLASH**)eyes_types[8]);
	}
	else if((stat & HurbyTouch::ANGRY_CHANGED)==0 && (evt & HurbyTouch::ANGRY_CHANGED)!=0){
		hurby->StopAngry();
	}
	else if((stat & HurbyTouch::TUMMY_CHANGED)!=0 && (evt & HurbyTouch::TUMMY_CHANGED)!=0){
		hurby->StartTickle((BITMAP_FLASH**)eyes_types[5]);
	}
	else if((stat & HurbyTouch::TUMMY_CHANGED)==0 && (evt & HurbyTouch::TUMMY_CHANGED)!=0){
		hurby->StopTickle();
	}
}

void TestTouch(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// register touch event listener
	hurby->SubscribeTouch(Touched);
}

