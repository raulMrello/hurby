/*************************************************************************************
		This file is the implementation file of the Frustation class
 *************************************************************************************/

#include "Frustation.h"


// Modulation macro
#define MODULATE(a)	(float)((modulation * (a))/100)



/** Line calculator */
static float lineCalculation(float x1, float x2, float y1, float y2, float x){
	if((x2>x1 && x>x2) || (x2<x1 && x<x2))
		x=x2;
	if((x2>x1 && x<x1) || (x2<x1 && x>x1))
		x=x1;
	
	return (((y2-y1)*(x-x1)/(x2-x1))+y1);
}


/** Constructor **********************************************************************/
Frustation::Frustation(uint8_t prio){
	this->serial = 0;
	this->priority = prio+1;
	this->activity = 0;
	this->lastActivity = this->activity;
	this->lastModulation = -1;
	this->tsll = TSLL_RANGE;
	this->tlu = 0;
	// setup behaviour interface
	this->bhv.level = prio;
	this->bhv.obj = this;
	this->bhv.SetInput = Frustation::SetInput;
	this->bhv.SetInputParam = Frustation::SetInputParam;
	this->bhv.RunBehaviour = Frustation::RunBehaviour;
	this->bhv.GetOutput = Frustation::GetOutput;	
	strcpy(this->action.layerName, "frustation");
}

/** Destructor ***********************************************************************/
Frustation::~Frustation(void){
}

/** Gets  behaviour ******************************************************************/
BEHAVIOUR_T* Frustation::GetBehaviour(void){ 
	return(&this->bhv);
}

/** En/Dis logger ********************************************************************/
void Frustation::EnDisDebugging(Serial * serial, int8_t level){
	if(level <= 0){
		this->serial = 0;
		return;
	}
	this->serial = serial;
}

/** Updates its inputs **************************************************************/
void Frustation::Run(float modulation){
	if(this->serial){
		this->serial->printf("F62\r\n");
	}
	this->lastActivity = this->activity;
	this->activity = lineCalculation(0, TLU_RANGE, 0, 100, (float)this->tlu);
	this->frustation = lineCalculation(0, TSLL_RANGE, 50, 0, (float)this->tsll);
	this->frustation += (this->activity/2);
	if(this->frustation > 100)
		this->frustation = 100;
	this->activity = this->frustation;
	// fill the action to be published to the subsumption architecture. It is application
	// dependent and it can be modified according with the function to be done.
	this->SetAction(modulation);
	// debug log
	if(this->serial){
		this->serial->printf("\r\nFRUSTATION_RUN\r\n");
		this->serial->printf("    tlu=%dsec\r\n", this->tlu);
		this->serial->printf("    tsll=%dsec\r\n", this->tsll);
		this->serial->printf("    modul=%f\r\n", modulation);
		this->serial->printf("    weight=%f\r\n", this->action.weight);
		this->serial->printf("    level=%f\r\n", this->frustation);
		this->serial->printf("    talk=%d\r\n", this->action.mouth_sound_count);
	}
}

/** Sets the action **************************************************************
 * FillingAction defines the action to be carried out according with behaviour's target.
 * In this case, as target is percentage of Boredom, it will react moving actuators 
 * accordingly (faster as happier). For voice/sound generation, as it is already undefined, I've done
 * an example of use, I've added 3 (suppose existing) sounds, that should be defined in "tts_types.c"
 * file. Each has an id and a duration, and according with that Id, VR module can play them. The
 * repetition time of each sound (I've decided) that must be  between a range. For example when
 * happiest, action cycle is between 3s-5s, and here voice sound playback will occur between
 * 3s/2=1.5s and 3*5s=15s. [1.5s -> 15s].
 */
void Frustation::SetAction(float modulation){
	if(this->serial){
		this->serial->printf("F98\r\n");
	}
	if(this->lastModulation == -1){
		this->lastModulation = modulation;
	}
	this->action.weight = this->activity;
	this->action.repeat_range[0] = (this->activity < 50)? lineCalculation(0,50,10,6,(float)this->activity) : lineCalculation(50,100,6,3,(float)this->activity);
	this->action.repeat_range[1] = (this->activity < 50)? lineCalculation(0,50,15,8,(float)this->activity) : lineCalculation(50,100,8,5,(float)this->activity);
	// Eyes types according with happiness modulation state from (unhappy to happy)
	uint8_t eyeId[] = {8,3,1}; 
	// changes eye each 33.4% of happiness modulation
	for(uint8_t i=0;i<3;i++){
		if(modulation < ((float)33.4*(1+i))){
			this->action.eyes_imgs = &eyes_types[eyeId[i]];
			break;
		}
	}
	float modulatedActivity = MODULATE(this->activity);
	this->action.eyes_speed = (uint8_t)((modulatedActivity < 33)? SPEED_SLOW : ((modulatedActivity < 66)? SPEED_NORMAL : SPEED_FAST));
	this->action.eyes_percent_range[0] = 0;
	this->action.eyes_percent_range[1] = 1;
	this->action.eyes_prob_ncs[0] = (modulatedActivity < 50)? lineCalculation(0,50,0.2,0.3,modulatedActivity) : lineCalculation(50,100,0.3,0.6,modulatedActivity);
	this->action.eyes_prob_ncs[1] = (modulatedActivity < 50)? lineCalculation(0,50,0.3,0.4,modulatedActivity) : lineCalculation(50,100,0.4,0.2,modulatedActivity);
	this->action.eyes_prob_ncs[2] = (modulatedActivity < 50)? lineCalculation(0,50,0.5,0.3,modulatedActivity) : lineCalculation(50,100,0.3,0.2,modulatedActivity);
	this->action.ears_speed = this->action.eyes_speed;
	this->action.ears_percent_range[0] = 0;
	this->action.ears_percent_range[1] = 1;
	this->action.feet_speed = this->action.eyes_speed;
	this->action.feet_percent_range[0] = 0;
	this->action.feet_percent_range[1] = 1;
	
	this->action.mouth_sound_count = 0; 
	this->action.mouth_sound_list[0] = 0;
	this->action.mouth_sound_repeat_range[0] = 0;
	this->action.mouth_sound_repeat_range[1] = 0; 
	if(modulation >= (this->lastModulation + 10)){
		this->lastModulation = modulation;
		this->action.mouth_sound_count = 3; 
		for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
			this->action.mouth_sound_list[i] = i+16;
		// if repeat range is [x,0] it will talked delayed but only once
		// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
		// if repeat range is [0,0] it will talked instantaneouly only once
		// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
		this->action.mouth_sound_repeat_range[0] = 0;
		this->action.mouth_sound_repeat_range[1] = 0; 
	}
	else if(this->frustation == 100){
		if(rnd(0.0, 1.0) < 0.8){
			this->lastModulation = modulation;
			this->action.mouth_sound_count = 3; 
			for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
				this->action.mouth_sound_list[i] = i+16;
			// if repeat range is [x,0] it will talked delayed but only once
			// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
			// if repeat range is [0,0] it will talked instantaneouly only once
			// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
			this->action.mouth_sound_repeat_range[0] = 0;
			this->action.mouth_sound_repeat_range[1] = 0; 
		}
	}
	else {
		if(rnd(0.0, 1.0) < 0.5){
			this->lastModulation = modulation;
			this->action.mouth_sound_count = 3; 
			for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
				this->action.mouth_sound_list[i] = i+16;
			// if repeat range is [x,0] it will talked delayed but only once
			// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
			// if repeat range is [0,0] it will talked instantaneouly only once
			// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
			this->action.mouth_sound_repeat_range[0] = 0;
			this->action.mouth_sound_repeat_range[1] = 0; 
		}
	}
}






