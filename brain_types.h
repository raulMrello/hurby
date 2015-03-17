/*************************************************************************************
		This file includes BRAIN common type declarations. 
 *************************************************************************************/

#ifndef BRAIN_TYPES_H
#define BRAIN_TYPES_H

// Required for C/C++ mixed code
#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>


// public input data structures
		
#define INPUT_TYPE_TSLP		(uint8_t)0
#define INPUT_TYPE_TSLD		(uint8_t)1
#define INPUT_TYPE_PTT		(uint8_t)2		
#define INPUT_TYPE_TSLL		(uint8_t)3
#define INPUT_TYPE_TLU		(uint8_t)4		
typedef struct{
	uint32_t tslp;		//!< time since last play (tummy or head)
	uint32_t tsld;		//!< time since last detect (ir detector)
	uint32_t ptt;		//!< playing time trend
	uint32_t tsll;		//!< time since last lift up (by an ear)
	uint32_t tlu;		//!< time being lifted up (by an ear)
	uint32_t lcmd;		//!< last speech commands detected
	uint8_t touchEvt;	//!< type of touch event (tummy, head, ear)
	int8_t voiceCmd; 	//!< voice command detected
}INPUT_T;

#define INPUT_TYPE_FRUSTATION	(uint8_t)0
#define INPUT_TYPE_SATIATION	(uint8_t)1
#define INPUT_TYPE_BOREDOM		(uint8_t)2
typedef struct{
	float frustation;	//!< frustation percentage level
	float satiation;	//!< satiation percentage level
	float boredom;		//!< boreadom percentage level
}INPUT_BSF_T;


// public action data structure
typedef struct{
	float   weight;
	float   repeat_range[2];
	void    *eyes_imgs;
	uint8_t eyes_speed;
	float   eyes_percent_range[2];
	float   eyes_prob_ncs[3];
	uint8_t ears_speed;
	float   ears_percent_range[2];
	uint8_t feet_speed;
	float   feet_percent_range[2];
	uint8_t mouth_sound_count;
	uint8_t mouth_sound_list[16];
	float   mouth_sound_repeat_range[2];
	char    layerName[16];
}ACTION_T;


// public behaviour data structure
typedef struct{
	void * obj;
	void (*SetInput)(void* obj, void* args);
	void (*SetInputParam)(void* obj, void* args, uint8_t argId);
	void (*RunBehaviour)(void *obj, float modulation, void (*SetInputParam)(void* obj, void* args, uint8_t argId), void* extObj);
	void (*GetOutput)(void* obj, ACTION_T** action);
	uint8_t level;
}BEHAVIOUR_T;

typedef enum{
	SPEED_SLOW = 0,
	SPEED_NORMAL = 1,
	SPEED_FAST = 2
}SPEED_E;

// Ranges for behaviour variables (from ms to sec)
#define RANGE_48H	(float)172800
#define RANGE_10M	(float)600
#define RANGE_05M	(float)300
#define RANGE_02M	(float)120
#define RANGE_01M	(float)60
#define RANGE_10S	(float)10
#define TSLP_RANGE	RANGE_02M
#define TSLD_RANGE	RANGE_02M
#define TSLL_RANGE	RANGE_02M
#define TLU_RANGE	RANGE_10S
#define PTT_RANGE	RANGE_01M


#ifdef __cplusplus
    }
#endif

#endif
