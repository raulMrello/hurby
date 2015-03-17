/*************************************************************************************
		This file is the implementation file of the HurbyFeet class
 *************************************************************************************/



#include "HurbySpeech.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}
	
/** Main Constructor *****************************************************************/
HurbySpeech::HurbySpeech(Serial *vr, DigitalOut * vr_rst, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	this->word = (char)-1;
	this->commands = 0;
	this->stat = 0;	
	this->vr = new EasyVR(vr, vr_rst, 9600);
  	// detect VR module
  	this->vr->detect();
 	// stop previous operations
 	this->vr->stop();	
 	// get ID
 	int id = this->vr->getID();
 	// change to 115200bps
 	this->vr->changeBaudrate(EasyVR::B115200);
// 	// set spanish language
// 	this->vr->setLanguage(EasyVR::SPANISH);
// 	// set infinite timeout for recognition
// 	this->vr->setTimeout(0);
// 	// set default knob for SI
// 	this->vr->setKnob(2);
// 	// set default strictness level
// 	this->vr->setLevel(2);
	
}

/** Main Destructor *****************************************************************/
HurbySpeech::~HurbySpeech(void){
	if(this->vr){
		delete(this->vr);
	}
}


/** Starts sensor scanning **********************************************************/
void HurbySpeech::StartRecognition(char wordset){
	if(this->stat != LISTENING){
		this->tmr.detach();
		//stops previous action (recog, train, playback)
		this->vr->stop();
		this->stat = LISTENING;	
	}
	this->wordset = wordset;
	// if builtin wordset, then recognise builtin word
	if(wordset <= EasyVR::NUMBER_SET){
		// set spanish language
		this->vr->setLanguage(EasyVR::SPANISH);
		// set infinite timeout for recognition
		this->vr->setTimeout(0);
		// set wordset recognition...
		this->vr->recognizeWord(wordset);
	}
	// else recognise custom command from a group (=wordset 4-31)
	else{
		// Get group from 0=trigger to 19=GROUP15
		wordset -= EasyVR::NUMBER_SET;
		// set infinite timeout for recognition
		this->vr->setTimeout(0);
		this->vr->recognizeCommand(wordset);
	}
	// starts periodic scanning
	tmr.attach(this, &HurbySpeech::AsyncTask, 0.05);
}

/** Speaks a tts ***********************************************************/
void HurbySpeech::StartSpeaking(uint8_t tts_id, float duration){
	if(this->stat != SPEAKING){
		this->tmr.detach();
		//stops previous action (recog, train, playback)
		this->vr->stop();
		this->stat = SPEAKING;	
	}
	this->vr->playSoundAsync(tts_id, EasyVR::VOL_FULL);	
	tmr.attach(this, &HurbySpeech::AsyncTask, duration);
}

/** Recognition timeout **************************************************************/
void HurbySpeech::AsyncTask(void){
	this->tmr.detach();
	// if listening, then check if something recognised...
	if(this->stat == LISTENING){
		// if word recognised...
		char c = 0;
		if(this->vr->RecognizedData(&c)){
			// obtengo el código del comando y lo paso a máscara de bits
			uint32_t mask = 0x00000001;
			while(c != 0){
				c--;
				mask = (uint32_t)(mask << 1);
			}
			this->commands |= mask;
			// notify recognition
			this->OnEventCallback(this->eventCaller, (void*)(uint32_t)RECOGNISED);
			// restart recognition
			// if builtin wordset, then recognise builtin word
			if(wordset <= EasyVR::NUMBER_SET)
				this->vr->recognizeWord(wordset);
			else
				this->vr->recognizeCommand(wordset);
		}
		else{
			if(c == STS_ERROR){
				// restart recognition
				// if builtin wordset, then recognise builtin word
				if(wordset <= EasyVR::NUMBER_SET)
					this->vr->recognizeWord(wordset);
				else
					this->vr->recognizeCommand(wordset);
			}
		}
		tmr.attach(this, &HurbySpeech::AsyncTask, 0.05);				
	}		
	// else if speaking
	else if(this->stat == SPEAKING){
		// restart recognition
		this->StartRecognition(wordset);	
	}
}

/** Get recognised words ******************************************************/
char HurbySpeech::GetRecognition(void){
	return this->word;
}

/** Get recognised commands ***************************************************/
uint32_t HurbySpeech::GetCommands(uint8_t forceClear){
	uint32_t cmd = this->commands;
	// erases previous commands.
	if(forceClear){
		this->commands = 0;	
	}
	return cmd;
}

