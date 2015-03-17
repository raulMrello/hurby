/*************************************************************************************
		This file includes test cases for: 
			- Eyes blinking
			- Eyes movement
			- Eyes reloading
 *************************************************************************************/

#include "mbed.h"
#include "main.h"
#include "Fuzzy.h"

static Fuzzy *fz;
static float buf_in[]={0,24,48,0,2,5};
static float buf_ou[]={0,50,100};

#define UNHAPPY		(uint8_t)0
#define NEUTRAL		(uint8_t)1
#define HAPPY		(uint8_t)2

static uint8_t rules[]={NEUTRAL, NEUTRAL, UNHAPPY,
						HAPPY,   NEUTRAL, NEUTRAL,
						HAPPY,   HAPPY,   NEUTRAL};

/** Callback invoked after eyes movement trajectory is completed *********************/
static void FuzzyEvent(void* caller, void* evt){
}

void TestFuzzy(void){
	fz = new Fuzzy(2, buf_in, 1, buf_ou, rules);
	float in[] = {0,0,24,0,48,0,0,2,24,2,48,2,0,5,24,5,48,5};
	float ou;
	uint8_t ix=0;
	while(ix <= 16){
		fz->Execute(&in[ix], &ou, FuzzyEvent, 0);
		ix += 2;		
	}
}

