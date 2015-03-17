/*************************************************************************************
	Happiness level depends on 3 inputs: Frustation, Satiation and Boredom. Each input
	is in range [0..100]. Happiness is calculated through the execution of 2 consecutive
	fuzzy inference systems: Frustation-Boredom which gives a FRBO level. This output is
	combined with the Satiation input to execute a second fuzzy system. The output result
	is a variation of happiness level with range [-20..+20]. Applying this variation to
	last happiness level, give us the final result of happiness which will be ranged into 
	[0..100].
 *************************************************************************************/

#include "Happiness.h"

// Range for fuzzy inference
#define MAX_HAPPY_VALUE	100

static float buf_in_frbo[]={0,50,100,0,50,100}; // center points of each fuzzy set: In_0], In_1
static float buf_ou_frbo[]={-15,0,15}; 			// center points of each singleton set: Out_0
static float buf_in_fbsa[]={-15,0,15,0,50,100}; // center points of each fuzzy set: Out_0, In_2
static float buf_ou_fbsa[]={-15,0,15}; 			// center points of each singleton set: Out_1
#define LOW		(uint8_t)0
#define MED		(uint8_t)1
#define HIGH	(uint8_t)2
// Knowledge base of  rules (2 inputs, 3 fuzzy set each -> 3*3 = 9 rules)
static uint8_t rules_frbo[]={HIGH, MED, LOW,
							  MED, MED, LOW,
							  LOW, LOW, LOW};
// Knowledge base of  rules (2 inputs, 3 fuzzy set each -> 3*3 = 9 rules)
static uint8_t rules_fbsa[]={ LOW, LOW,  MED,
							  LOW, MED,  MED,
							  MED, MED, HIGH};


/** Constructor **********************************************************************/
Happiness::Happiness(uint8_t prio){
	this->serial = 0;
	this->priority = prio+1;
	this->lastHappiness = 50;
	this->frustation=0;
	this->satiation=0;
	this->boredom=0;
	this->happiness = 50;
	this->action.weight = this->happiness;
	// setup behaviour interface
	this->bhv.level = prio;
	this->bhv.obj = this;
	this->bhv.SetInput = Happiness::SetInput;
	this->bhv.SetInputParam = Happiness::SetInputParam;
	this->bhv.RunBehaviour = Happiness::RunBehaviour;
	this->bhv.GetOutput = Happiness::GetOutput;	
	// setup fuzzy inference instance
	this->frbo = new Fuzzy(2, buf_in_frbo, 1, buf_ou_frbo, rules_frbo);
	this->fbsa = new Fuzzy(2, buf_in_fbsa, 1, buf_ou_fbsa, rules_fbsa);
}

/** Destructor ***********************************************************************/
Happiness::~Happiness(void){
	if(this->frbo){
		delete(this->frbo);
	}
	if(this->fbsa){
		delete(this->fbsa);
	}
}

/** Gets  behaviour ******************************************************************/
BEHAVIOUR_T* Happiness::GetBehaviour(void){ 
	return(&this->bhv);
}

/** Gets  happiness ******************************************************************/
float Happiness::GetHappiness(void){ 
	return(this->happiness);
}


/** En/Dis logger ********************************************************************/
void Happiness::EnDisDebugging(Serial * serial, int8_t level){
	if(level <= 0){
		this->serial = 0;
		return;
	}
	this->serial = serial;
	this->frbo->EnDisDebugging(serial, level-1);
	this->fbsa->EnDisDebugging(serial, level-1);
}

/** Updates its inputs **************************************************************/
void Happiness::Run(void){
	if(this->serial){
		this->serial->printf("H88\r\n");
	}
	float formattedInput[2];
	float frboOut, fbsaOut;
	formattedInput[0] = this->frustation;
	formattedInput[1] = this->boredom;
	this->lastHappiness = this->happiness;
	this->frbo->Execute(formattedInput, &frboOut, 0, 0);
	formattedInput[0] = frboOut;
	formattedInput[1] = this->satiation;
	this->fbsa->Execute(formattedInput, &fbsaOut, 0, 0);
	// fill the action to be published to the subsumption architecture. It is application
	// dependent and it can be modified according with the function to be done.
	this->happiness = (float)(this->lastHappiness + frboOut + fbsaOut);
	if(this->happiness < 0)
		this->happiness = 0;
	if(this->happiness > 100)
		this->happiness = 100;
	this->action.weight = this->happiness;
	// debug log
	if(this->serial){
		this->serial->printf("\r\nHAPPINESS_RUN\r\n");
		this->serial->printf("    Satiation=%f\r\n", this->satiation);
		this->serial->printf("    Frustation=%f\r\n", this->frustation);
		this->serial->printf("    Boredom=%f\r\n", this->boredom);
		this->serial->printf("    Level=%f\r\n", this->happiness);
	}
}







