/*************************************************************************************
		This file is the implementation file of the Hurby class
 *************************************************************************************/


#include "Hurby.h"
#include "Servo.h"
#include "GyroDriver.h"


// Local debug terminal (hw-independent)
static Serial *logger=0;

/** Prints a debug message ***********************************************************/
static void Print(const char * msg){
    if(logger){
        logger->printf(msg);
    }
}

/** VoiceRecognition & SpeechSynthesis */
Serial vr(p28,p27);
DigitalOut vr_rst(p20);

/** Servos Outputs */
Servo left_ear(p24);
Servo right_ear(p23);
Servo mouth_srv(p22);
Servo left_foot(p25);
Servo right_foot(p21);

/** Eyes outputs */
DigitalOut cs_left(p8);
DigitalOut cd_left(p11); 
DigitalOut rst_left(p12);
DigitalOut cs_right(p14);
DigitalOut cd_right(p15);
DigitalOut rst_right(p16);
DigitalOut backlight(p17);
SPI spi_both(p5, p6, p7); // mosi, miso, sclk

/** Tactile detectors */
DigitalIn head(p30);
DigitalIn tummy(p29);
DigitalIn angry(p26);

/** Distance detector */
DigitalIn presence(p19);

/** I2C Peripherals */
I2C i2c(p9,p10);


						/*****************************/
						/*****************************/
						/****     Class Setup     ****/
						/*****************************/
						/*****************************/



/** Main Constructor *****************************************************************/
Hurby::Hurby(Serial *pc){
	// stores debug reference and starts logging
	#if defined(ENABLE_SERIAL_LOG)
    logger = pc;
	#endif
	
    /***** SENSORY CORTEX *****/
	this->sensoryCortex = new SensoryCortex(&head, &tummy, &angry, &presence, &vr, &vr_rst, &i2c, Hurby::SensoryEvents, this);	
	#if defined(ENABLE_SERIAL_LOG)
	this->sensoryCortex->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
 
    /***** MOTOR CORTEX *****/
	this->motorCortex = new MotorCortex(&spi_both, &cs_left, &cd_left, &rst_left, &backlight, 
										&spi_both, &cs_right, &cd_right, &rst_right, &backlight, 
										&left_ear, &right_ear, &left_foot, &right_foot,	&mouth_srv, this->sensoryCortex->GetSpeechScanner());	
	#if defined(ENABLE_SERIAL_LOG)
	this->motorCortex->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
 
    /***** SUBSUMPTION *****/
	this->ssa = new Subsumption(6, Hurby::SubsumptionEvents, this);
 
    /***** HAPPINESS *****/
	this->happiness = new Happiness(0);
	this->ssa->AddBehaviour(this->happiness->GetBehaviour());
	#if defined(ENABLE_SERIAL_LOG)
	this->happiness->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
 
    /***** BOREDOM *****/
	this->boredom = new Boredom(3);
	this->ssa->AddBehaviour(this->boredom->GetBehaviour());
	#if defined(ENABLE_SERIAL_LOG)
	this->boredom->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
 
    /***** FRUSTATION *****/
	this->frustation = new Frustation(2);
	this->ssa->AddBehaviour(this->frustation->GetBehaviour());
	#if defined(ENABLE_SERIAL_LOG)
	this->frustation->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
 
    /***** SATIATION *****/
	this->satiation = new Satiation(1);
	this->ssa->AddBehaviour(this->satiation->GetBehaviour());
	#if defined(ENABLE_SERIAL_LOG)
	this->satiation->EnDisDebugging(pc, LOGGER_LEVEL);
	#endif
	
	#if defined(ENABLE_SERIAL_LOG)
    Print("Hurby is ready!\r\n");
	#endif
	
	// starts sensory cortex
	this->sensoryCortex->Start();
}

/** Main Destructor ******************************************************************/
Hurby::~Hurby(void){
	// destroy dynamic allocated objects
    if(this->sensoryCortex){
        delete(this->sensoryCortex);
    }
    if(this->motorCortex){
        delete(this->motorCortex);
    }
    if(this->ssa){
        delete(this->ssa);
    }
    if(this->happiness){
        delete(this->happiness);
    }
    if(this->boredom){
        delete(this->boredom);
    }
    if(this->frustation){
        delete(this->frustation);
    }
    if(this->satiation){
        delete(this->satiation);
    }
}


/** Debug text **********************************************************************/
void Hurby::Debug(char* txt){	
	Print(txt);
}



