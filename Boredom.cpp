/*************************************************************************************
		This file is the implementation file of the Boredom class
 *************************************************************************************/

#include "Boredom.h"

// Modulation macro
#define MODULATE(a)	(float)((modulation * (a))/100)

static float buf_in[]={ 0,TSLP_RANGE/2,TSLP_RANGE,
						0,TSLD_RANGE/2,TSLD_RANGE}; // center points of each fuzzy set: In_0], In_1
static float buf_ou[]={0,50,100}; 					// center points of each singleton set: Out_0
#define LOW		(uint8_t)0
#define MED		(uint8_t)1
#define HIGH	(uint8_t)2
// Knowledge base of 9 rules (2 inputs, 3 fuzzy set each -> 3*3 = 9 rules)
static uint8_t rules[]={ LOW, MED, HIGH,
						 LOW, MED, HIGH,
 						 LOW, LOW, HIGH};


/** Line calculator *****************************************************************/
static float lineCalculation(float x1, float x2, float y1, float y2, float x){
	if((x2>x1 && x>x2) || (x2<x1 && x<x2))
		x=x2;
	if((x2>x1 && x<x1) || (x2<x1 && x>x1))
		x=x1;
	return (((y2-y1)*(x-x1)/(x2-x1))+y1);
}

/** Constructor **********************************************************************/
Boredom::Boredom(uint8_t prio){
	this->serial = 0;
	this->priority = prio+1;
	this->lastActivity = -1;
	this->activity = this->lastActivity;
	this->lastModulation = -1;
	this->tslp = 0;
	this->tsld = 0;
	// setup behaviour interface
	this->bhv.level = prio;
	this->bhv.obj = this;
	this->bhv.SetInput = Boredom::SetInput;
	this->bhv.SetInputParam = Boredom::SetInputParam;
	this->bhv.RunBehaviour = Boredom::RunBehaviour;
	this->bhv.GetOutput = Boredom::GetOutput;	
	strcpy(this->action.layerName, "boredom");
	// setup fuzzy inference instance
	this->fuzzy = new Fuzzy(2, buf_in, 1, buf_ou, rules);
}

/** Destructor ***********************************************************************/
Boredom::~Boredom(void){
	if(this->fuzzy){
		delete(this->fuzzy);
	}
}

/** Gets  behaviour ******************************************************************/
BEHAVIOUR_T* Boredom::GetBehaviour(void){ 
	return(&this->bhv);
}

/** En/Dis logger ********************************************************************/
void Boredom::EnDisDebugging(Serial * serial, int8_t level){
	if(level <= 0){
		this->serial = 0;
		return;
	}
	this->serial = serial;
	this->fuzzy->EnDisDebugging(serial, level-1);
}

/** Updates its inputs **************************************************************/
void Boredom::Run(float modulation){
	if(this->serial){
		this->serial->printf("B75\r\n");
	}
	float formattedInput[2];
	formattedInput[0] = (float)this->tslp;
	formattedInput[1] = (float)this->tsld;
	this->lastActivity = this->activity;
	this->fuzzy->Execute(formattedInput, &this->boredom, 0, 0);
	this->activity = this->boredom;
	// fill the action to be published to the subsumption architecture. It is application
	// dependent and it can be modified according with the function to be done.
	if(this->lastActivity == -1){
		this->lastActivity = this->activity;
	}
	//FillAction(this->boredom/100, this->activity, this->lastActivity, &this->action);
	this->SetAction(modulation);
	// debug log
	if(this->serial){
		this->serial->printf("\r\nBOREDOM_RUN\r\n");
		this->serial->printf("    tslp=%dsec\r\n", this->tslp);
		this->serial->printf("    tsld=%dsec\r\n", this->tsld);
		this->serial->printf("    modul=%f\r\n", modulation);
		this->serial->printf("    weight=%f\r\n", this->action.weight);
		this->serial->printf("    level=%f\r\n", this->boredom);
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
void Boredom::SetAction(float modulation){
	if(this->serial){
		this->serial->printf("B115\r\n");
	}
	if(this->lastModulation == -1){
		this->lastModulation = modulation;
	}
	this->action.weight = this->activity;
	this->action.repeat_range[0] = (this->activity < 50)? lineCalculation(0,50,10,6,(float)this->activity) : lineCalculation(50,100,6,3,(float)this->activity);
	this->action.repeat_range[1] = (this->activity < 50)? lineCalculation(0,50,15,8,(float)this->activity) : lineCalculation(50,100,8,5,(float)this->activity);
	// Eyes types according with happiness modulation state from (unhappy to happy)
	uint8_t eyeId[] = {0,1,6,5,2,7,4}; 
	// changes eye each 14.3% of happiness modulation
	for(uint8_t i=0;i<7;i++){
		if(modulation < ((float)14.3*(1+i))){
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
	// clear voice previous processing
	this->action.mouth_sound_count = 0; 
	this->action.mouth_sound_list[0] = 0; 
	this->action.mouth_sound_repeat_range[0] = 0;
	this->action.mouth_sound_repeat_range[1] = 0;		

	// if change from last this->activity is unhappier 
	if(modulation >= (this->lastModulation + 20)){
		this->lastModulation = modulation;
		this->action.mouth_sound_count = 5; // Can reproduce up to 3 diferent voice/sound recordings (example of use)
		for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
			this->action.mouth_sound_list[i] = i+1;
		// if repeat range is [x,0] it will talked delayed but only once
		// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
		// if repeat range is [0,0] it will talked instantaneouly only once
		// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
		this->action.mouth_sound_repeat_range[0] = 0;
		this->action.mouth_sound_repeat_range[1] = 0; 
	}
	//else if happier say "Yuppie" instantaneously
	else if(modulation <= (this->lastModulation - 10)){
		this->lastModulation = modulation;
		this->action.mouth_sound_count = 6; // Can reproduce up to 3 diferent voice/sound recordings (example of use)
		for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
			this->action.mouth_sound_list[i] = i+6;
		// if repeat range is [x,0] it will talked delayed but only once
		// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
		// if repeat range is [0,0] it will talked instantaneouly only once
		// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
		this->action.mouth_sound_repeat_range[0] = 0;
		this->action.mouth_sound_repeat_range[1] = 0; 
	}
	else if(modulation == 0 && this->lastModulation == 0){
		if(rnd(0.0, 1.0) < 0.75){
			this->action.mouth_sound_count = 11; // Can reproduce up to 3 diferent voice/sound recordings (example of use)
			for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
				this->action.mouth_sound_list[i] = i+1;
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
			this->action.mouth_sound_count = 11; // Can reproduce up to 3 diferent voice/sound recordings (example of use)
			for(uint8_t i=0; i<this->action.mouth_sound_count;i++)
				this->action.mouth_sound_list[i] = i+1;
			// if repeat range is [x,0] it will talked delayed but only once
			// if repeat range is [x,y] it will talked delayed betwwen x and y multiple times
			// if repeat range is [0,0] it will talked instantaneouly only once
			// if repeat range is [0,y] it will talked instantaneouly and repeated each y delay
			this->action.mouth_sound_repeat_range[0] = 0;
			this->action.mouth_sound_repeat_range[1] = 0; 
		}
	}
}







