#include "FuzzyIO8.h"

/***********************************************************************************/
/***********************************************************************************/
/* Private memebers ****************************************************************/
/***********************************************************************************/
/***********************************************************************************/

// memberships are scaled in uint8_t values with range [0..100]
#define MEMBERSHIP(u)	(uint8_t)(100*(u))
// default event callback.
static void NULL_CALLBACK(void* caller, void* ev_code){}
	

/***********************************************************************************/
/***********************************************************************************/
/* Private functions ****************************************************************/
/***********************************************************************************/
/***********************************************************************************/

/**
 * Fuzzify crisp inputs <in> into triangle fuzzy sets: /\
 * @param in Input array (crisp values)
 */		
void FuzzyIO8::Fuzzify(int8_t * in){
	for(uint8_t i=0; i<this->num_in; i++){
		int8_t value = in[i];
		for(uint8_t s=0; s<SETS_PER_INPUT; s++){
			uint8_t v = (i*SETS_PER_INPUT)+s;
			if(s==0){
				if(value <= this->in_setp[v]){
					this->in_fz[v] = MEMBERSHIP(1.0);
				}
				else if(value > this->in_setp[v+1]){
					this->in_fz[v] = MEMBERSHIP(0.0);
				}
				else{
					this->in_fz[v] = MEMBERSHIP((float)(this->in_setp[v+1]-value)/(float)(this->in_setp[v+1]-this->in_setp[v]));
				}
			}
			else if(s==SETS_PER_INPUT-1){
				if(value <= this->in_setp[v-1]){
					this->in_fz[v] = MEMBERSHIP(0.0);
				}
				else if(value >= this->in_setp[v]){
					this->in_fz[v] = MEMBERSHIP(1.0);
				}
				else{
					this->in_fz[v] = MEMBERSHIP((float)(value-this->in_setp[v-1])/(float)(this->in_setp[v]-this->in_setp[v-1]));
				}
			}
			else{
				if(value <= this->in_setp[v-1]){
					this->in_fz[v] = MEMBERSHIP(0.0);
				}
				else if(value >= this->in_setp[v+1]){
					this->in_fz[v] = MEMBERSHIP(0.0);
				}
				else if(value >= this->in_setp[v]){
					this->in_fz[v] = MEMBERSHIP((float)(this->in_setp[v+1]-value)/(float)(this->in_setp[v+1]-this->in_setp[v]));
				}
				else{
					this->in_fz[v] = MEMBERSHIP((float)(value-this->in_setp[v-1])/(float)(this->in_setp[v]-this->in_setp[v-1]));
				}
			}
		}	
	}
}

		
/**
 * Inference reasoning procedure. 
 */		
void FuzzyIO8::Inference(void){
	//array to store input sets according with the matching rule
	uint8_t *indexes = (uint8_t*)malloc(this->num_in);
	for(uint8_t r=0;r<this->num_rules;r++){
		// get active fuzzy set for each input
		indexes[0] = r % SETS_PER_INPUT;
		for(uint8_t i=1;i<this->num_in;i++){
			indexes[i] = (uint8_t)((double)r/pow((double)SETS_PER_INPUT, i));
		}
		// get the output fuzzy set of the selected rule (range 0 .. SETS_PER_OUTPUT-1)
		uint8_t ouset = this->rules[r];
		// Apply AND operation of each input for the selected rule
		uint8_t sum = MEMBERSHIP(1.0);
		for(uint8_t i=0;i<this->num_in;i++){
			uint8_t s = (i*SETS_PER_INPUT)+indexes[i];
			if(sum >= this->in_fz[s]){
				sum = this->in_fz[s];
			} 
		}
		// Sum (OR) the result to the selected output fuzzy set
		this->ou_dfz[ouset] = (sum >= this->ou_dfz[ouset])? sum : this->ou_dfz[ouset];		
	}
}


/**
 * Defuzzify outputs into crisp values. Result is returned into <ou> array
 * @param ou Output array (crisp values)
 */		
void FuzzyIO8::Defuzzify(int8_t * ou){
	for (uint8_t o=0; o<this->num_ou; o++){
		float sum=0, den=0;
		for(uint8_t s=0; s<SETS_PER_OUTPUT; s++){
			uint8_t v = (o*SETS_PER_OUTPUT)+s;
			sum += (this->ou_setp[v] * this->ou_dfz[v]);
			den += this->ou_dfz[v];
		}
		ou[o] = (int8_t)(sum/den);
	}
}
	

/***********************************************************************************/
/***********************************************************************************/
/* Public functions ****************************************************************/
/***********************************************************************************/
/***********************************************************************************/
	
/**
 * Constructor. Sets number of inputs, outputs and their fuzzy sets center values.
 * Fuzzifiers are formed by 3 fuzzy sets and Deffuzifiers are formed
 * by 3 singleton points. 
 * Inputs and outputs' crisp values must be in range [-127,127]
 * Also the content of the knowledge base must be passed
 * @param num_in Number of inputs
 * @param buf_in Array of inputs (fuzzy sets values: 3 * num_in size)
 * @param num_ou Number of oututs
 * @param buf_ou Array of outputs (singleton values: 3 * num_ou size)
 * @param rules Content of the knowledge base
 */		
FuzzyIO8::FuzzyIO8(uint8_t num_in, int8_t *buf_in, uint8_t num_ou, int8_t *buf_ou, uint8_t *rules){
	if(num_in){
		this->num_in = num_in;
		this->in_setp = (int8_t*)malloc(num_in * SETS_PER_INPUT);
		if(buf_in && this->in_setp){
			memcpy(this->in_setp, buf_in, (num_in * SETS_PER_INPUT));
		}
		this->in_fz = (uint8_t*)malloc(num_in * SETS_PER_INPUT);
		this->num_rules = pow((double)SETS_PER_INPUT, this->num_in);
		this->rules = (uint8_t*)malloc(this->num_rules);
		if(rules && this->rules){
			memcpy(this->rules, rules, this->num_rules);
		}
	}
	if(num_ou){
		this->num_ou = num_ou;
		this->ou_setp = (int8_t*)malloc(num_ou * SETS_PER_OUTPUT);
		if(buf_ou && this->ou_setp){
			memcpy(this->ou_setp, buf_ou, (num_ou * SETS_PER_OUTPUT));
		}
		this->ou_dfz = (uint8_t*)malloc(num_ou * SETS_PER_OUTPUT);		
	}
}
	

/**
 * Executes the fuzzy inference, applying a new input pattern <in>.
 * It will carry out 3 processes: fuzzification-reasoning-defuzzification and generates
 * an array of output values <ou> (1 for each output). During this internal
 * operation, several EVENTS will be raised, so if an external caller is installed, 
 * it can be notified about these events, and hence get the ability to control its 
 * execution or extract temporary internal data for logging/debugging tasks.
 *
 * @param in Input's crisp values array (1 per input)
 * @param ou Output's crisp values array (1 per output)
 * @param OnEvent Callback reference, installed by a caller
 * @param caller Object acting as a caller 
 * @return Error codes
 */		
int8_t FuzzyIO8::Execute(int8_t *in, int8_t *ou, void (*OnEvent)(void*, void*), void *caller){
	// check params
	if(!in){
		return ERR_NULL_PARAM;
	}
	// install caller
	this->evCallback = (OnEvent)? OnEvent : NULL_CALLBACK;
	this->evCaller = caller;
	
	// clear outputs
	memset(this->ou_dfz, 0, this->num_ou * SETS_PER_OUTPUT);
	
	// fuzzify inputs, then notify to caller
	Fuzzify(in);
	this->evCallback(this->evCaller, (void*)EV_FUZZIFICATION_DONE);
	
	// executes fuzzy inference, then notify to caller
	Inference();
	this->evCallback(this->evCaller, (void*)EV_INFERENCE_DONE);
	
	// defuzzify outputs, then notify to caller
	Defuzzify(ou);
	this->evCallback(this->evCaller, (void*)EV_DEFUZZIFICATION_DONE);
	return ERR_OK;	
}

