/*************************************************************************************
		This file includes test cases for dancing 
		1st: Dance slow for 10 seconds
		2nd: Dance normal for 10 seconds
		3rd: Dance fast for 10 seconds
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;
// local reference of ticker dance timer
static Ticker tmr;
// local status variable
static uint8_t stat = 0;

/** Callback invoked after eyes movement trajectory is completed *********************/
static void DanceUpdate(void){
	switch(stat){
		case 0:{
			// dance slow
			hurby->StartDance((BITMAP_FLASH**)eyes_types[0], Foot::MOVE_SLOW);
			stat++;
			break;
		}
		case 1:{
			// dance normal
			hurby->StartDance((BITMAP_FLASH**)eyes_types[1], Foot::MOVE_NORMAL);
			stat++;
			break;
		}
		case 2:{
			// dance fast
			hurby->StartDance((BITMAP_FLASH**)eyes_types[2], Foot::MOVE_FAST);
			stat=0;
			break;
		}			
	}
	tmr.attach(DanceUpdate, 10);			
}

/** Test case entry ******************************************************************/
void TestDance(Hurby *h){
	// stores the hurby reference
	hurby = h;
	// starts dancing
	hurby->StartDance((BITMAP_FLASH**)eyes_types[1], Foot::MOVE_NORMAL);
	//stat = 0;
	//DanceUpdate();	
}

