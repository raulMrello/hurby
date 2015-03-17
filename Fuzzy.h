/* Fuzzy Inference Controller with Input and Outputs ranged into float values.
 * This is an example of use:
 *
 *
 *
 *
 * #include "FuzzyIO8.h"
 * 
 * static FuzzyIO8 *fz;
 * static int8_t buf_in[]={0,24,48,0,2,5}; // center points of each fuzzy set: In_0[0,24,48], In_1[0,2,5]
 * static int8_t buf_ou[]={0,50,100}; // center points of each singleton set: Out_0[0,50,100]
 * 
 * #define LOW	(uint8_t)0
 * #define MED	(uint8_t)1
 * #define HIGH	(uint8_t)2
 * 
 * // Knowledge base of 9 rules (2 inputs, 3 fuzzy set each -> 3*3 = 9 rules)
 * static uint8_t rules[]={	MED, 	MED, 	LOW,
 * 							HIGH,   MED, 	MED,
 * 							HIGH,   HIGH,   MED};
 * 
 * // dummy callback to receive events from fz object
 * static void FuzzyEvent(void* caller, void* evt){
 * }
 * 
 * // test example
 * void TestFuzzy(void){
 *  // creates an instance, asigning number of inputs (2) and outputs (1), their fuzzy set centers and knowledge base
 * 	fz = new FuzzyIO8(2, buf_in, 1, buf_ou, rules);
 *  // test array with input pairs to test all rules
 * 	int8_t in[] = {0,0,24,0,48,0,0,2,24,2,48,2,0,5,24,5,48,5};
 *  // receives result
 * 	int8_t ou;
 *  // index to go across input pairs
 *  uint8_t ix=0;
 *  // process only those test pairs
 * 	while(ix <= 16){
 *		// executes fuzzy system
 * 		fz->Execute(&in[ix], &ou, FuzzyEvent, 0);
 *		// print output result
 *		printf("In[0]=%d, In[1]=%d -> Result out=%d",in[ix], in[ix+1],ou);
 *		// set next test pair
 * 		ix += 2;		
 * 	}
 * }
 *
 *
 *
 *
 * 
 * Copyright (c) 2014 lord_bot
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#ifndef FUZZY_H
#define FUZZY_H

/** Required externals from this FuzzyIO8.h and FuzzyIO8.cpp */
#include "mbed.h"
// #include <stdint.h>		// Required types: uint8_t
// #include <math.h>  		// Required functions:  pow()
// #include <stdlib.h>		// Required functions:  malloc()
// #include <string.h>		// Required functions:  memcpy(), memset()

/**
 * Class Fuzzy is a Fuzzy Inference System specially designed to be executed in
 * resource-constrained MCUs, with the ability to be executed in real time with a 
 * few dozens of bytes.
 * Crisp inputs/outputs must be <float> types
 * 
 * Class constructor, configures the number of inputs and outputs, but each in/out is
 * fuzzified into 3 fuzzy sets (see constants SETS_PER_INPUT and SETS_PER_OUTPUT). If you
 * need increase the number of fuzzy sets per in/out, you can change these constants
 * according with your needs.
 */
class Fuzzy{
	public:
		/**
		 * Constructor. Sets number of inputs, outputs and their fuzzy sets center values.
		 * Fuzzifiers are formed by 3 fuzzy sets and Deffuzifiers are formed
		 * by 3 singleton points. 
		 * Also the content of the knowledge base must be passed
		 * @param num_in Number of inputs
		 * @param buf_in Array of inputs (fuzzy sets values: 3 * num_in size)
		 * @param num_ou Number of oututs
		 * @param buf_ou Array of outputs (singleton values: 3 * num_ou size)
		 * @param rules Content of the knowledge base
		 */		
		Fuzzy(uint8_t num_in, float *buf_in, uint8_t num_ou, float *buf_ou, uint8_t *rules);
	

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
		int8_t Execute(float *in, float *ou, void (*OnEvent)(void*, void*), void *caller);

		/**
		 * Enables/Disables log capabilites
		 * @param serial Serial Logger
		 */
		void EnDisDebugging(Serial * serial, int8_t level);

				
		/**
		 * static constants
		 */  																		
		static const uint8_t SETS_PER_INPUT   = 3;	//!< Num of fuzzy sets per input. Change according with your needs. 
		static const uint8_t SETS_PER_OUTPUT   = 3;	//!< Num of fuzzy sets per output. Change according with your needs. 
		
		
		/**
		 * Event codes raised by this fuzzy controller
		 */
		static const uint8_t EV_FUZZIFICATION_DONE   = 0x01; //!< Event raised after fuzzification
		static const uint8_t EV_INFERENCE_DONE       = 0x02; //!< Event raised after inference reasoning
		static const uint8_t EV_DEFUZZIFICATION_DONE = 0x04; //!< Event raised after defuzzification
		
		
		/** 
		 * Common error codes returned by this module
		 */
		static const int8_t ERR_OK         = 0;		//!< No errors.
		static const int8_t ERR_NULL_PARAM = -1;	//!< Some argument or parameter is null.
		static const int8_t ERR_RANGE	   = -2;	//!< Some argument or parameter is out of range.

		
	private:
		// serial debugger
		Serial * serial;
		uint8_t num_in;	 	//!< number of inputs
		float *in_setp; 	//!< fuzzy set points for inputs
		float *in_fz;  	//!< array of fuzzyfied inputs (memberships)
		uint8_t num_ou;	 	//!< number of outputs
		float *ou_setp; 	//!< fuzzy set points for outputs
		float *ou_dfz; 	//!< array of defuzzyfication outputs (memberships)
		uint8_t num_rules;	//!< number of rules in knwoledge base
		uint8_t *rules;		//!< array of rules
		void *evCaller;		//!< External caller
		void (*evCallback)(void* caller, void* ev_code);	//!< Event callback installed by a external caller
		
		/**
		 * Fuzzify crisp inputs <in> into fuzzy sets.
		 * @param in Input array (crisp values)
		 */		
		void Fuzzify(float * in);

		/**
		 * Inference reasoning procedure. 
		 */		
		void Inference(void);	

		/**
		 * Defuzzify outputs into crisp values. Result is returned into <ou> array
		 * @param ou Output array (crisp values)
		 */		
		void Defuzzify(float * ou);
};

#endif 

