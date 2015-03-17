/*************************************************************************************
		This file is the implementation file of the HurbyEyes class
 *************************************************************************************/



#include "HurbyEyes.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Main Constructor *****************************************************************/
HurbyEyes::HurbyEyes(SPI *spil, DigitalOut *csl, DigitalOut *cdl, DigitalOut *rstl,DigitalOut *bll, SPI * spir, DigitalOut *csr, DigitalOut *cdr, DigitalOut *rstr,DigitalOut *blr){
    // Initiates both eyes
    this->leftEye = new Eye(Eye::IS_LEFT, spil, csl, cdl, rstl, bll);
    this->rightEye = new Eye(Eye::IS_RIGHT, spir, csr, cdr, rstr, blr);
}

/** Main Destructor *****************************************************************/
HurbyEyes::~HurbyEyes(void){
	// destroy allocated objects
    if(this->leftEye){
        delete(this->leftEye);
    }
    if(this->rightEye){
        delete(this->rightEye);
    }
}

/** Load eyes with an image and open them ********************************************/
void HurbyEyes::LoadImage(BITMAP_FLASH *img[], void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	
	// loads image and callback for both eyes, installs itself to receive blink/move events from each eye
    this->leftEye->Load(&img[0], HurbyEyes::BlinkCallback, this, HurbyEyes::MoveCallback, this);
    this->rightEye->Load(&img[5], HurbyEyes::BlinkCallback, this, HurbyEyes::MoveCallback, this);  
}

/** Move eyes with a move_code *******************************************************/
void HurbyEyes::Move(uint8_t mcode, uint8_t moveSpeed, uint8_t blinkSpeed, float percent){
	// move both eyes to the <mcode> position
	this->leftEye->Move(mcode, moveSpeed, blinkSpeed, percent);
	this->rightEye->Move(mcode, moveSpeed, blinkSpeed, percent);
}

/** Close eyes ***********************************************************************/
void HurbyEyes::Close(void){
	// close both eyes
	this->leftEye->Close();
	this->rightEye->Close();
}


/** Get actual target position *******************************************************/
uint8_t HurbyEyes::GetPosition(void){  
    return this->leftEye->GetPosition();
}


