/*************************************************************************************
		This file includes common definitions to compile one or other option like: 
		test cases, debug mode, or target release.
 *************************************************************************************/

#ifndef MAIN_H
#define MAIN_H

// Required for C/C++ mixed code
#ifdef __cplusplus
    extern "C" {
#endif

#include "Hurby.h"
						
		
//#define ENABLE_EYES_TEST		// Comment to disable		
//#define ENABLE_EARS_TEST		// Comment to disable
//#define ENABLE_FEET_TEST		// Comment to disable
//#define ENABLE_DANCE_TEST		// Comment to disable
//#define ENABLE_BEHAVIOURS_TEST	// Comment to disable
//#define ENABLE_TOUCH_TEST		// Comment to disable
//#define ENABLE_MOUTH_TEST		// Comment to disable
//#define ENABLE_VOICE_TEST		// Comment to disable		
//#define ENABLE_CORTEX_TEST		// Comment to disable		
//#define ENABLE_FUZZY_TEST		// Comment to disable		
		
		
#if defined(ENABLE_EYES_TEST)
extern void TestEyes(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_EARS_TEST)
extern void TestEars(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_FEET_TEST)
extern void TestFeet(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_DANCE_TEST)
extern void TestDance(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_BEHAVIOURS_TEST)
extern void TestBehaviours(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_TOUCH_TEST)
extern void TestTouch(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_MOUTH_TEST)
extern void TestMouth(Hurby* h);		
#endif		

		
		
#if defined(ENABLE_VOICE_TEST)
extern void TestVoice(Hurby* h);		
#endif		
	


#if defined(ENABLE_CORTEX_TEST)
extern void TestMotorCortex(Hurby *h);
#endif
	


#if defined(ENABLE_FUZZY_TEST)
extern void TestFuzzy(void);
#endif





#ifdef __cplusplus
    }
#endif

#endif
