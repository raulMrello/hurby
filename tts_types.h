/*************************************************************************************
		This file includes TTS common type declarations. A voice phrase is defined by
		a data structure named TTS_FLASH which consists of several properties:
			id: voice phrase ID to be found in voice-recognition's module memory
			seconds: duration of the phrase
			percent: an array of percent mouth positions for each syllable
			period: duration in seconds for each syllable
			count: number of talked silabes
		So, for example saying the phrase: "Hello, how are you", it can be decomposed
		in this way:
			id = 0
			seconds = 2.0
			Syllables:  He...llo...ho....w....are...you   .
			  percent= 1.0  0.5   0.75  0.1  0.75  0.25   0
			  period=  0.5  0.5   0.25  0.1  0.25  0.3    0.1
			  count=7
 *************************************************************************************/

#ifndef TTS_TYPES_H
#define TTS_TYPES_H

// Required for C/C++ mixed code
#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

		
// TTS id list
#define TTS_NO_VOICE	(uint8_t)0		
#define TTS_HELLO		(uint8_t)1
#define TTS_YUPPIE		(uint8_t)2
#define TTS_BYE			(uint8_t)3		
#define TTS_NOOO		(uint8_t)4
#define TTS_LETMEGO		(uint8_t)5
#define TTS_TOUCHME		(uint8_t)6
		
typedef struct{
    uint8_t id;			//!< tts text id
    float seconds;  	//!< tts text duration in seconds
	float *percent;		//!< array of percent values
	float *period;		//!< array of time values for each talk-period
	int8_t count;		//!< number of talk periods.
} TTS_FLASH;


// TTS array incluiding all compiled phrases.
extern volatile const TTS_FLASH *tts_list[19];

double rnd(float min, float max);

#ifdef __cplusplus
    }
#endif

#endif
