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


/** Callback invoked after eyes blinking cycle is completed **************************/
static void EyesBlinked(void){
	static uint8_t mcode = 0;	
	// change movement position (see Eye.h for mcode values)
	mcode = (mcode < 8)? (mcode+1) : 0;
	// move eyes
	hurby->MoveEyes(mcode);		
}

/** Callback invoked after eyes movement trajectory is completed *********************/
static void EyesMoved(){
	static uint8_t img = 0;
	// change eyes image group (see eyes_img.h for image values)
	img = (img < 9)? (img+1) : 0;
	// change image and starts a new blinking cycle
	hurby->SetEyes((BITMAP_FLASH**)eyes_types[img], EyesBlinked, EyesMoved);
}

void TestEyes(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// setup eyes image and install blink and move callbacks.
	hurby->SetEyes((BITMAP_FLASH**)eyes_types[0], EyesBlinked, EyesMoved);
}

