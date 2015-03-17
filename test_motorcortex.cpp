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
static ACTION_T a;

void TestMotorCortex(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// creates a test action
	a.weight = 1;
	a.repeat_range[0] = 2;
	a.repeat_range[1] = 4;
	a.eyes_speed = Eye::MOVE_FAST;
	a.eyes_imgs = (BITMAP_FLASH**)eyes_types[1];
	a.eyes_percent_range[0] = 0.8;
	a.eyes_percent_range[1] = 1;
	a.eyes_prob_ncs[0] = 0.5;
	a.eyes_prob_ncs[1] = 0.3;
	a.eyes_prob_ncs[2] = 0.2;
	a.ears_speed=Ear::MOVE_FAST;
	a.ears_percent_range[0] = 0.7;
	a.ears_percent_range[1] = 1;
	a.feet_speed=Foot::MOVE_FAST;
	a.feet_percent_range[0] = 0.7;
	a.feet_percent_range[1] = 1;
	a.mouth_sound_count= 0;
	//a.mouth_sound_list[16];
	//mouth_sound_repeat_range[2];
	
	hurby->SetBehaviour(&a);
}

