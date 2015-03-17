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
static void ChangeBehaviour(void){
	switch(stat){
		case 0:{
			// set happy
			hurby->SetHappy((BITMAP_FLASH**)eyes_types[7]);
			stat++;
			break;
		}
		case 1:{
			// set unhappy
			hurby->SetUnhappy((BITMAP_FLASH**)eyes_types[0]);
			stat++;
			break;
		}
		case 2:{
			// start frightened
			hurby->StartFrightened((BITMAP_FLASH**)eyes_types[3]);
			stat++;
			break;
		}			
		case 3:{
			// dance normal
			hurby->StartDance((BITMAP_FLASH**)eyes_types[1], Foot::MOVE_NORMAL);
			stat++;
			break;
		}
		case 4:{
			// dance normal
			hurby->SetSleep();
			stat++;
			break;
		}
		default:
			tmr.detach();
			return;
	}
	tmr.attach(ChangeBehaviour, 10);			
}

/** Test case entry ******************************************************************/
void TestBehaviours(Hurby *h){
	// stores the hurby reference
	hurby = h;
	stat = 0;
	ChangeBehaviour();	
}
