/*************************************************************************************
		This file includes test cases for: 
			- Mouth talking simulation
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;
// local reference of tts item
TTS_FLASH tts;


/** Callback invoked after eyes movement trajectory is completed *********************/
static void Touched(){
	volatile float p = 0.3;
	hurby->MoveMouth(Touched, &tts, p);
}

void TestMouth(Hurby *h){
	// stores the hurby reference
	hurby = h;
	tts.id = 0;
	tts.seconds = 0.6; //only one mouth open
	// register touch event listener
	hurby->MoveMouth(Touched, &tts, 0.2);
}

