/*************************************************************************************
		This file is the implementation file of the HurbyMouth class
 *************************************************************************************/


#include "MotorCortex.h"

	
// static unsigned int m_z=12434,m_w=33254;
//  
// /** Random generator *****************************************************************/
// static double rnd(float min, float max) {
// 	unsigned int result;
// 	double res;
//     m_z = 36969 * (m_z & 65535) + (m_z >>16);
//     m_w = 18000 * (m_w & 65535) + (m_w >>16);
// 	result = ((m_z <<16) + m_w)%1000;
// 	res = (double)((((double)(max-min)*result)/1000) + ((double)min));
//     return (res);
// }
 
/** Random generator *****************************************************************/
double MotorCortex::GetRnd(float min, float max) {
	return (rnd(min,max));
}


/** Constructor **********************************************************************/
MotorCortex::MotorCortex(SPI *spil, DigitalOut *csl, DigitalOut *cdl, DigitalOut *rstl,DigitalOut *bll, 
						 SPI * spir, DigitalOut *csr, DigitalOut *cdr, DigitalOut *rstr,DigitalOut *blr, 
						 Servo *lear, Servo *rear, 
						 Servo *lfoot, Servo *rfoot,
						 Servo *m, HurbySpeech * speech){
	// setup default data
	this->serial = 0;
	// starts low level driver
	rstl->write(0);
    csl->write(1);
    cdl->write(1);
    rstr->write(0);
    csr->write(1);
    cdr->write(1);
    //Print("Eyes io_lines config... done\r\n");
    spil->format(8,3);
    spil->frequency(1000000);
    //Print("Eyes spi_lines config... done\r\n");		
	this->speech = speech;
	this->mouth = new HurbyMouth(m, MotorCortex::HandleMouthEvents, this);
    this->eyes = new HurbyEyes(spil, csl, cdl, rstl, bll, spir, csr, cdr, rstr, blr);
 	this->ears = new HurbyEars(lear, rear, MotorCortex::HandleEarsEvents, this);
	this->feet = new HurbyFeet(lfoot, rfoot, MotorCortex::HandleFeetEvents, this);
	// clears action list
	for(uint8_t i=0; i<ACTION_LIST_SIZE; i++){
		this->actionList[i].weight = 0;
	}
	this->actionCount = 0;
	this->selectedAction.weight = 0;
	// set status
	this->isWaiting = 1;
	this->isVoiceReady = 1;
}

/** Destructor ***********************************************************************/
MotorCortex::~MotorCortex(void){
}

/** En/Dis logger ********************************************************************/
void MotorCortex::EnDisDebugging(Serial * serial, int8_t level){
	this->mouth->EnDisDebugging(serial);
	if(level <= 0){
		this->serial = 0;
		return;
	}
	this->serial = serial;
}

/** Adds an action *******************************************************************/
int8_t MotorCortex::AddActionList(ACTION_T** actions){  
	if(this->serial){
		this->serial->printf("MC78\r\n");
	}
	// insert actions until buffer completion
	while(*actions != 0){
		if(this->actionCount >= ACTION_LIST_SIZE){
			break;
		}
		if((*actions)->weight > 0){
			// inserts without any kind of management
			memcpy(&this->actionList[this->actionCount], *actions, sizeof(ACTION_T));
			// increments and keep in range 0..7
			this->actionCount++;
			this->actionCount &= (ACTION_LIST_SIZE-1);
		}
		actions++;
	}
	// if waiting, then schedules next execution
	if(this->isWaiting && this->actionCount){
		this->isWaiting = 0;
		this->repeatTmr.attach(this, &MotorCortex::AsyncTask, 0.1);
	}
	return 0;
}

/** Movement callback ****************************************************************/
void MotorCortex::AsyncTask(void){
	if(this->serial){
		this->serial->printf("MC106\r\n");
	}
	int8_t selected = -1;
	float winner = 0;
	// selects the most weighted action
	for(uint8_t i = 0; i < this->actionCount; i++){
		if(this->actionList[i].weight >= winner){
			winner = this->actionList[i].weight;
			selected = i;
		}
	}
	if(selected >= 0){
		// copies action
		memcpy(&this->selectedAction, &this->actionList[selected], sizeof(ACTION_T));
		// clears list
		for(uint8_t i=0; i<ACTION_LIST_SIZE; i++){
			this->actionList[i].weight = 0;
		}
		this->actionCount = 0;	
	}
	if(this->selectedAction.weight == 0){
		this->isWaiting = 1;
		return;
	}
	// executes selected action
	// debug log
//	if(this->serial){
//		this->serial->printf("\r\nMOTORCORTEX_RUN\r\n");
//		this->serial->printf("    Action=%d, %s\r\n",selected,this->selectedAction.layerName);
//	}
	uint8_t eyes_pos = MoveEyes();
	MoveEars(eyes_pos);
	MoveFeet(eyes_pos);
	MoveMouth(selected);
	// repeat cycle
	double delay = rnd(this->selectedAction.repeat_range[0], this->selectedAction.repeat_range[1]);
	this->repeatTmr.attach(this, &MotorCortex::AsyncTask, delay);		
}

/** Schedules eyes movement **********************************************************/
uint8_t MotorCortex::MoveEyes(void){
	if(this->serial){
		this->serial->printf("MC147\r\n");
	}
	uint8_t position, v;
	uint8_t prob[] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	// gets eyes position
	position = this->eyes->GetPosition();
	// inhibits actual position from target
	prob[position] = 0;
	// generates random number [0,1] to select target position: north, center, south
	double r = rnd(0,1);
	// check south...
	if(r < this->selectedAction.eyes_prob_ncs[2]){
		v = Eye::MOVE_SW;
	}
	// else check center
	else if(r < (this->selectedAction.eyes_prob_ncs[1] + this->selectedAction.eyes_prob_ncs[2])){
		v = Eye::MOVE_CW;
	}
	// else is north
	else{
		v = Eye::MOVE_NW;
	}
	// generates random number [0,1] to select target position: west, center, east
	r = rnd(0,1);
	// check west...
	if(r < 0.333){
		v += 0;
	}
	// else check center
	else if(r < 0.666){
		v += 1;
	}
	// else is east
	else{
		v += 2;
	}
	// checks if target is actual position, then select the next one by default
	if(!prob[v]){
		v = (v < 2)? (v+1) : 0;
	}
	// generates random number to select percent displacement
	float percent = (float)rnd(this->selectedAction.eyes_percent_range[0], this->selectedAction.eyes_percent_range[1]);
	// debug log
	if(this->serial){
		this->serial->printf("    Eyes pos=%d, speed=%d, percent=%f\r\n",v, this->selectedAction.eyes_speed, percent);
	}
	this->eyes->LoadImage((BITMAP_FLASH**)this->selectedAction.eyes_imgs, MotorCortex::HandleEyesEvents, this);
	this->eyes->Move(v, this->selectedAction.eyes_speed, this->selectedAction.eyes_speed, percent);	
	return v;
}

/** Schedules ears movement **********************************************************/
void MotorCortex::MoveEars(uint8_t eye_pos){
	if(this->serial){
		this->serial->printf("MC201\r\n");
	}
	uint8_t w, mcode;
	// generates random number [0,1] to select target position: bcflr 0..4
	double r = rnd(0,1);	
	if(r < this->selectedAction.eyes_prob_ncs[2]){
		w = 2;
	}
	// else check center
	else if(r < (this->selectedAction.eyes_prob_ncs[1] + this->selectedAction.eyes_prob_ncs[2])){
		w = 1;
	}
	// else is north
	else{
		w = 0;
	}	
	if(eye_pos <= Eye::MOVE_NE){
		const uint8_t bcf[] = {Ear::MOVE_BACK, Ear::MOVE_CENTER, Ear::MOVE_FRONT};
		mcode = bcf[w];
	}
	else if(eye_pos <= Eye::MOVE_CE){
		const uint8_t cbf[] = {Ear::MOVE_CENTER, Ear::MOVE_BACK, Ear::MOVE_FRONT};
		mcode = cbf[w];
	}
	else{
		const uint8_t fcb[] = {Ear::MOVE_FRONT, Ear::MOVE_CENTER, Ear::MOVE_BACK};
		mcode = fcb[w];
	}
	// generates random number to select percent displacement
	float percent = (float)rnd(this->selectedAction.ears_percent_range[0], this->selectedAction.ears_percent_range[1]);
	// debug log
//	if(this->serial){
//		this->serial->printf("    Ears pos=%d, speed=%d, percent=%f\r\n", mcode, this->selectedAction.ears_speed, percent);
//	}
	this->ears->Move(Ear::BOTH, mcode, this->selectedAction.ears_speed, percent);	
}

/** Schedules feet movement **********************************************************/
void MotorCortex::MoveFeet(uint8_t eye_pos){
	if(this->serial){
		this->serial->printf("MC241\r\n");
	}
	uint8_t w, mcode;
	// generates random number [0,1] to select target position: bcflr 0..4
	double r = rnd(0,1);	
	if(r < this->selectedAction.eyes_prob_ncs[2]){
		w = 2;
	}
	// else check center
	else if(r < (this->selectedAction.eyes_prob_ncs[1] + this->selectedAction.eyes_prob_ncs[2])){
		w = 1;
	}
	// else is north
	else{
		w = 0;
	}	
	// generates random number [0,1] to select target position: NCS or LRç
	r = rnd(0,1);
	if(r < 0.5){
		if(eye_pos <= Eye::MOVE_NE){
			const uint8_t bcf[] = {HurbyFeet::BALANCE_BACK, HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_FRONT};
			mcode = bcf[w];
		}
		else if(eye_pos <= Eye::MOVE_CE){
			const uint8_t cbf[] = {HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_BACK, HurbyFeet::BALANCE_FRONT};
			mcode = cbf[w];
		}
		else{
			const uint8_t fcb[] = {HurbyFeet::BALANCE_FRONT, HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_BACK};
			mcode = fcb[w];
		}
	}
	else{
		if(eye_pos == Eye::MOVE_NW || eye_pos == Eye::MOVE_CW || eye_pos == Eye::MOVE_SW){
			const uint8_t rcl[] = {HurbyFeet::BALANCE_RIGHT, HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_LEFT};
			mcode = rcl[w];
		}
		else if(eye_pos == Eye::MOVE_NC || eye_pos == Eye::MOVE_CC || eye_pos == Eye::MOVE_SC){
			const uint8_t clr[] = {HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_LEFT, HurbyFeet::BALANCE_RIGHT};
			mcode = clr[w];
		}
		else{
			const uint8_t lcr[] = {HurbyFeet::BALANCE_LEFT, HurbyFeet::BALANCE_CENTER, HurbyFeet::BALANCE_RIGHT};
			mcode = lcr[w];
		}
	}
	// generates random number to select percent displacement
	float percent = (float)rnd(this->selectedAction.feet_percent_range[0], this->selectedAction.feet_percent_range[1]);
	// debug log
//	if(this->serial){
//		this->serial->printf("    Feet pos=%d, speed=%d, percent=%f\r\n", mcode, this->selectedAction.feet_speed, percent);
//	}
	this->feet->Move(Foot::BOTH, mcode, this->selectedAction.feet_speed, percent);	
}

/** Schedules mouth movement **********************************************************/
void MotorCortex::MoveMouth(int8_t isNewAction){
	if(this->serial){
		this->serial->printf("MC299\r\n");
	}
	double delay;
	// if selectedAction is the repetition of a previous selected, check delay configuration
	// to abort talk if required.
	if(this->isVoiceReady && this->selectedAction.mouth_sound_count && isNewAction == -1){
		// if repeat range is [x,0] it will talked delayed but only once
		// if repeat range is [0,0] it will talked instantaneouly only once
		if(this->selectedAction.mouth_sound_repeat_range[1] == 0){
			this->selectedAction.mouth_sound_count = 0;
		}
	}
	if(this->isVoiceReady && this->selectedAction.mouth_sound_count){
		this->isVoiceReady = 0;
		// checks voice range...
		// if only once
		if(this->selectedAction.mouth_sound_repeat_range[1]==0){
			// if must be delayed
			if(this->selectedAction.mouth_sound_repeat_range[0]>0){
				delay = this->selectedAction.mouth_sound_repeat_range[0];
			}
			// else if instantaneously
			else{
				delay = 0;
			}
		}
		//else if repeated
		else{
			// if must be repeated
			if(this->selectedAction.mouth_sound_repeat_range[0]>0){
				delay = rnd(this->selectedAction.mouth_sound_repeat_range[0], this->selectedAction.mouth_sound_repeat_range[1]);
			}
			// else if instantaneously
			else{
				delay = 0;
			}
		}				
		if(delay){
//			if(this->serial){
//				this->serial->printf("Talk delay=%f\r\n", delay);
//			}
			this->mouthTmr.attach(this, &MotorCortex::Talk, delay);		
		}
		else{
			this->Talk();
		}
	}
	else if(!this->isVoiceReady){
		if(this->serial){
			this->serial->printf("Talk BUSY!\r\n");
		}
		this->isVoiceReady = 1;
	}
}

/** Emit sound/voice  ****************************************************************/
void MotorCortex::Talk(void){
	if(this->serial){
		this->serial->printf("MC357\r\n");
	}
	int8_t err=1;
	this->mouthTmr.detach();
	// generates random number between the number of available sounds on list
	double rd = rnd(0, this->selectedAction.mouth_sound_count);
	uint8_t r = (uint8_t)rd;
	// debug log
	if(this->serial){
		this->serial->printf("Talking code=%d sound_id=%d\r\n", r, this->selectedAction.mouth_sound_list[r]);
	}
	this->speech->StartSpeaking(this->selectedAction.mouth_sound_list[r], tts_list[this->selectedAction.mouth_sound_list[r]]->seconds);
	err = this->mouth->Talk(this->selectedAction.mouth_sound_list[r]);
	if(err != 0){
		this->isVoiceReady = 1;
	}
}



