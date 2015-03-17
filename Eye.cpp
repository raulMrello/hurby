/*************************************************************************************
		This file is the implementation file of the Eye class
 *************************************************************************************/


#include "Eye.h"

// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}

/** Eye Constructor ******************************************************************/
Eye::Eye(uint8_t id, SPI * spi_port, DigitalOut *cs_pin, DigitalOut *cd_pin, DigitalOut *rst_pin,DigitalOut *bl_pin){
    // delete image references
    for(uint8_t i =0 ; i < 5; i++){
        this->img[i] = 0;
    }
	// setup default data
    this->activeImg = 0;
    this->blinkStep = 0.01; //10ms
    this->blinkDelay = 2;   // 2seconds
    this->moveDelay = 0.05;  //50ms
    this->blinkStat = 0;
	this->type = id;
	this->targetPosition = MOVE_CC;
	this->OnBlinkEnd = NULL_CALLBACK_2;
	this->blinkCaller = 0;
	this->OnMoveEnd = NULL_CALLBACK_2;
	this->moveCaller = 0;
	// set position at center
    this->left = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2);
    this->top = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
    // initializes driver
    this->lcd = new LCDDriver(spi_port, cs_pin, cd_pin, rst_pin, bl_pin);   
}

/** Eye Destructor ******************************************************************/
Eye::~Eye(void){
	// destroy allocated objects
    if(this->lcd){
        delete(this->lcd);
    }   
}

/** Load eye with an image and open it ***********************************************/
void Eye::Load(BITMAP_FLASH *imgs[], void (*OnBlinkCallback)(void*, void*), void* blinkScope, void (*OnMoveCallback)(void*, void*), void *moveScope){   
	// installa event handlers
	this->OnBlinkEnd = (OnBlinkCallback)? OnBlinkCallback : NULL_CALLBACK_2;
	this->blinkCaller = blinkScope;
	this->OnMoveEnd = (OnMoveCallback)? OnMoveCallback : NULL_CALLBACK_2;
	this->moveCaller = moveScope;

    // loads image screenshots
    for(uint8_t i=0; i<5; i++){
        this->img[i] = imgs[i];
    }
    // starts displaying from first one
    this->tmr.detach();
	this->moveStat = 0;
    this->blinkStat = 0;
    this->Blink();
}


/** Open the eye in the center *******************************************************/
void Eye::Close(void){  
    this->tmr.detach();
	this->moveStat = 0;
    this->blinkStat = 0;
    this->lcd->Sleep();
}


/** Get actual target position *******************************************************/
uint8_t Eye::GetPosition(void){  
    return this->targetPosition;
}

/** Blink service ********************************************************************/
void Eye::Blink(void){
    // updates eye image
    uint8_t id = (this->blinkStat > 4)? (8-this->blinkStat) : this->blinkStat;
    this->activeImg = this->img[id];
    // refresh LCD content
    this->lcd->PutImage(this->activeImg, this->left, this->top);
    switch(id){
        // on eyes wide open, set timeout to start blinking
        case 0:{
            this->tmr.attach(this, &Eye::OnBlinkCallback, this->blinkDelay);    
            break;          
        }
        // on blinking action, reload step time for next image
        case 1:
        case 2:
        case 3:
        case 4:{
            this->tmr.attach(this, &Eye::OnBlinkCallback, this->blinkStep); 
            break;          
        }
    }
}

/** Blink callback *******************************************************************/
void Eye::OnBlinkCallback(void){
	// loads next image
    if(this->blinkStat < 8){
        this->blinkStat++;
    }
	// or restore eyes wide open image
    else{
        this->blinkStat = 0;
    }   
	// invokes blinking action
    this->Blink();
	// on eyes wide open after a blinking action, notify blink completed to external caller
	if(this->blinkStat == 0){
		this->OnBlinkEnd(this->blinkCaller, this);
	}
}

/** Movement service *****************************************************************/
void Eye::Move(uint8_t mcode, uint8_t moveSpeed, uint8_t blinkSpeed, float percent){  
	const float BLINK_SPEED[]={0.05, 0.03, 0.01}; 	// 50ms  30ms 10ms
    const float MOVE_SPEED[]={0.2, 0.1, 0.05}; 		//200ms 100ms 50ms
    uint8_t x_new, y_new;
    double xdiff;
    double ydiff;
	// intializes movement variables
    this->tmr.detach();
    this->moveStat = 0;
    this->blinkStat = 0;
	this->blinkStep = (blinkSpeed <= BLINK_FAST)? BLINK_SPEED[blinkSpeed] : BLINK_SPEED[BLINK_FAST];
	this->moveDelay = (moveSpeed <= MOVE_FAST)? MOVE_SPEED[moveSpeed] : MOVE_SPEED[MOVE_FAST];
	// loads first one (eyes wide open)
    this->activeImg = this->img[0];
	// setup final location depending on predefined code
    switch(mcode){
        case MOVE_NW:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2)+10;
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2)-10;
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2);
			}
            break;
        }
        case MOVE_NC:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2)+10;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2)-10;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}
            break;
        }
        case MOVE_NE:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2)+15;
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2)+5;
			}
			else{
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2)-15;
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2)-5;
			}
            break;
        }
        case MOVE_CW:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)+4;
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)-4;
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2);
			}
            break;
        }
        case MOVE_CC:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)+5;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)-5;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}            
            break;
        }
        case MOVE_CE:{
			if(this->type == IS_RIGHT){
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)+4;
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/2) - (IMAGE_X_SIZE/2)-4;
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2);
			}
            break;
        }
        case MOVE_SW:{
			if(this->type == IS_RIGHT){
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2);
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2);
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2);
			}
            break;
        } 
        case MOVE_SC:{
			if(this->type == IS_RIGHT){
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2)+6;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2)-6;
				y_new = (LCDDriver::Y_SIZE/2) - (IMAGE_Y_SIZE/2);
			}
            break;
        }
		case MOVE_SE:{
			if(this->type == IS_RIGHT){
				x_new = ((3*LCDDriver::X_SIZE)/4) - (IMAGE_X_SIZE/2);
				y_new = (LCDDriver::Y_SIZE/4) - (IMAGE_Y_SIZE/2);
			}
			else{
				x_new = (LCDDriver::X_SIZE/4) - (IMAGE_X_SIZE/2);
				y_new = ((3*LCDDriver::Y_SIZE)/4) - (IMAGE_Y_SIZE/2);
			}
            break;
        }


    }
    // generates a trajectory of 4 movements
    xdiff = (double)((double)x_new - this->left)/3;
    ydiff = (double)((double)y_new - this->top)/3;
	// apply percentage
	xdiff = xdiff * (double)percent;
	ydiff = ydiff * (double)percent;
	// calculate trajectory
    this->x[0] = this->left;
    this->x[1] = (uint8_t)((double)this->left+xdiff);
    this->x[2] = (uint8_t)((double)this->left+(2*xdiff));
    this->x[3] = x_new;
    this->y[0] = this->top;
    this->y[1] = (uint8_t)((double)this->top+ydiff);
    this->y[2] = (uint8_t)((double)this->top+(2*ydiff));
    this->y[3] = y_new;
	// set target
	this->targetPosition = mcode;
    // starts movement
    this->OnMoveCallback();
}

/** Movement callback ****************************************************************/
void Eye::OnMoveCallback(void){
	// load next location
    this->left = this->x[this->moveStat];
    this->top = this->y[this->moveStat];
	// update screen content
    this->lcd->PutImage(this->activeImg, this->left, this->top);
	// if movement finished...
    if(this->moveStat >= 3){
		// destroy timer, restart blinking action
        this->tmr.detach();
        this->blinkStat = 0;
        this->Blink();
		// notify movement completion event to external caller
		this->OnMoveEnd(this->moveCaller, this);
        return;     
    }
	this->moveStat++;
	// ... else, setup next timed movement update
    this->tmr.attach(this, &Eye::OnMoveCallback, this->moveDelay);
}
