/*************************************************************************************
		This file is the implementation file of the HurbyMouth class
 *************************************************************************************/


#include "SensoryCortex.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}
// Create the local filesystem under the name "local"
static LocalFileSystem local("local");               


/** Constructor **********************************************************************/
SensoryCortex::SensoryCortex(DigitalIn *head, DigitalIn *tummy, DigitalIn *angry, DigitalIn *presence, Serial *vr, DigitalOut * vr_rst, I2C *i2c, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	// setup RTC (if no time then reset)
	#warning "Siempre reinicio el reloj en reset";
	set_time(1);
	if(time(NULL) == 0 || time(NULL) == (uint32_t)-1){
		set_time(1);
	}
	// sets default data
	this->serial = 0;
	this->firstTouch = time(NULL);	
	this->lastTouch = time(NULL);	
	this->lastDetect = time(NULL);
	this->lastLift = time(NULL);
	this->evtFlags = 0;
	this->inpData.tslp = TSLP_RANGE;
	this->inpData.tsld = TSLD_RANGE;
	this->inpData.tsll = TSLL_RANGE;
	this->inpData.lcmd = 0;
	this->inpData.tlu = 0;		
	this->inpData.ptt = 0;
	
	// creates touch scanner
	this->touchScanner = new HurbyTouch(head, tummy, angry, SensoryCortex::HandleTouchEvents, this);
	// creates presence detect scanner
	this->detectScanner = new HurbyDetect(presence, SensoryCortex::HandleTouchEvents, this);
	// creates speech recognition scanner and player
	this->speechScanner = new HurbySpeech(vr, vr_rst, SensoryCortex::HandleTouchEvents, this);
	// creates permanent store manager
	this->ps = new PStore(i2c);
	// restore data from ps
	#warning "De momento no leo los datos de la eprom";
// 	if(!this->ps->IsOK()){
// 		this->ps->Lock();
// 		this->ps->Write(PStore::BASE_ADDR+(1*sizeof(uint32_t)), &this->ptt, sizeof(uint32_t));
// 		this->ps->Write(PStore::BASE_ADDR+(2*sizeof(uint32_t)), &this->firstTouch, sizeof(time_t));
// 		this->ps->Unlock();		
// 	}
// 	this->ps->Read(PStore::BASE_ADDR+(1*sizeof(uint32_t)), &this->ptt, sizeof(uint32_t));
// 	this->ps->Read(PStore::BASE_ADDR+(2*sizeof(uint32_t)), &this->firstTouch, sizeof(time_t));
// 	
	// set status
	this->isWaiting = 1;
	
}

/** Destructor ***********************************************************************/
SensoryCortex::~SensoryCortex(void){
	if(this->touchScanner){
		delete(this->touchScanner);
	}
	if(this->detectScanner){
		delete(this->detectScanner);
	}
}

/** En/Dis logger ********************************************************************/
void SensoryCortex::EnDisDebugging(Serial * serial, int8_t level){
	if(level <= 0){
		this->serial = 0;
		return;
	}
	this->serial = serial;
}

/** En/Dis logger ********************************************************************/
HurbySpeech * SensoryCortex::GetSpeechScanner(void){
	return(this->speechScanner);
}

/** En/Dis presence detection ********************************************************/
void SensoryCortex::EnDisPresenceDetection(uint8_t endis){
	if(endis){
		this->detectScanner->Start();
	}
	else{
		this->detectScanner->Stop();
	}
}

/** Starts execution *****************************************************************/
void SensoryCortex::Start(void){  
	// starts active execution
	this->touchScanner->Start();
	this->detectScanner->Start();
	this->speechScanner->StartRecognition(HurbySpeech::SD_WORDSET);
	this->tmr.attach(this, &SensoryCortex::AsyncTask, 0.1);
}

/** State Machine execution *********************************************************/
void SensoryCortex::AsyncTask(void){
	if(this->serial){
		this->serial->printf("SC107\r\n");
	}
	const float DEFAULT_SCAN_TIME = 20;
	// process event
	this->scanTime = DEFAULT_SCAN_TIME;
	if(this->isWaiting){
		if((this->evtFlags & (EV_HEAD | EV_TUMMY | EV_SPEECH)) != 0){
			this->isWaiting = 0;
			this->inpData.tslp = 0;
			if(this->inpData.ptt == 0){
				this->firstTouch = time(NULL);
				this->lastTouch = time(NULL);
				this->inpData.ptt = PTT_RANGE/10;
			}
			else{
				this->inpData.ptt = (uint32_t)(time(NULL) - this->firstTouch);
			}		
			this->inpData.touchEvt = this->evtFlags;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_TOUCH\r\n");
			}
		}
		if((this->evtFlags & EV_EARS) != 0){
			this->isWaiting = 0;
			this->lastLift = time(NULL);
			this->inpData.tsll = 0;
			this->inpData.tlu = 5;
			this->inpData.touchEvt = this->evtFlags;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_HURT\r\n");
			}
			this->scanTime = 5;
		}
		else if((this->evtFlags & (EV_DETECT | EV_SPEECH)) != 0){
			this->isWaiting = 0;
			this->lastDetect = time(NULL);
			this->inpData.tsld = 0;
			if((this->evtFlags & EV_SPEECH) != 0){			
				this->inpData.lcmd = this->speechScanner->GetCommands();
			}			
			this->inpData.touchEvt = this->evtFlags;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_DETECT\r\n");
			}
		}
		else{ //[timeout]
			this->inpData.tslp = (uint32_t)(time(NULL) - this->lastTouch);
			this->inpData.tsld = (uint32_t)(time(NULL) - this->lastDetect);
			this->inpData.tsll = (uint32_t)(time(NULL) - this->lastLift);
			this->inpData.touchEvt = 0;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_TIMEOUT\r\n");
			}
		}
		if((uint32_t)(time(NULL) - this->lastTouch) >= (uint32_t)TSLP_RANGE){
			this->inpData.ptt = 0;
		}
		this->inpData.voiceCmd = TTS_NO_VOICE; // by default
	}
	else{
		if((this->evtFlags & (EV_HEAD | EV_TUMMY | EV_SPEECH)) != 0){
			this->inpData.touchEvt = this->evtFlags;
			this->inpData.tslp = 0;
			this->lastTouch = time(NULL);				
			this->inpData.ptt = (uint32_t)(time(NULL) - this->firstTouch);
			if((this->evtFlags & EV_SPEECH) != 0){			
				this->inpData.lcmd = this->speechScanner->GetCommands();
			}
			if(this->serial){
				this->serial->printf("\r\nSENSORY_TOUCH\r\n");
			}
			this->OnEventCallback(this->eventCaller, &this->inpData);
		}
		if((this->evtFlags & EV_EARS) != 0){
			this->inpData.tsll = 0;
			this->inpData.tlu = (uint32_t)(time(NULL) - this->lastLift);
			this->inpData.touchEvt = this->evtFlags;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_HURT\r\n");
			}
			this->OnEventCallback(this->eventCaller, &this->inpData);
			this->scanTime = 5;
		}
		else if((this->evtFlags & EV_DETECT) != 0){
			this->isWaiting = 0;
			this->lastDetect = time(NULL);
			this->inpData.tsld = 0;
			this->inpData.touchEvt = this->evtFlags;
			if(this->serial){
				this->serial->printf("\r\nSENSORY_DETECT\r\n");
			}
		}
		else{ //[timeout]
			uint8_t stat, evt;
			this->inpData.touchEvt = 0;
			this->inpData.tslp = (uint32_t)(time(NULL) - this->lastTouch);
			this->inpData.tsld = (uint32_t)(time(NULL) - this->lastDetect);
			if(this->serial){
				this->serial->printf("\r\nSENSORY_TIMEOUT_WAIT\r\n");
			}
			this->touchScanner->GetStat(&stat,&evt);
			if((stat & EV_EARS) != 0){
				this->inpData.touchEvt = EV_EARS;
				this->inpData.tsll = 0;
				this->inpData.tlu = (uint32_t)(time(NULL) - this->lastLift);
				this->inpData.ptt = 0;
				this->scanTime = 5;				
			}
			else{								
				this->inpData.tsll = (uint32_t)(time(NULL) - this->lastLift);
				this->inpData.tlu = 0;
				// save data to ps
				#warning "De momento no grabo los datos en la eprom";
// 				this->ps->Lock();
// 				this->ps->Write(PStore::BASE_ADDR, &this->numTimes, sizeof(uint32_t));
// 				this->ps->Write(PStore::BASE_ADDR+(1*sizeof(uint32_t)), &this->ptt, sizeof(uint32_t));
// 				this->ps->Write(PStore::BASE_ADDR+(2*sizeof(uint32_t)), &this->firstTouch, sizeof(time_t));
// 				this->ps->Unlock();		
				// switch to waiting state
				this->isWaiting = 1;
			}
		}
		this->inpData.voiceCmd = TTS_NO_VOICE; // by default
	}
	if(this->serial){
		this->serial->printf("    EVENT =%d\r\n", this->evtFlags);
		this->serial->printf("    tslp =%dsec\r\n", this->inpData.tslp);
		this->serial->printf("    tsld =%dsec\r\n", this->inpData.tsld);
		this->serial->printf("    ptt  =%dsec\r\n", this->inpData.ptt);
	}
	
	this->OnEventCallback(this->eventCaller, &this->inpData);
	this->evtFlags = 0;
	this->tmr.attach(this, &SensoryCortex::AsyncTask, this->scanTime);
}




