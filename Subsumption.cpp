/*************************************************************************************
		This file is the implementation file of the HurbyMouth class
 *************************************************************************************/


#include "Subsumption.h"
// callback for unassigned handlers (two void* args)
static void NULL_CALLBACK_2(void *scope, void *args){}
// static default empty action
static ACTION_T NO_Action;


/** Constructor **********************************************************************/
Subsumption::Subsumption(uint8_t numLevels, void (*EventCallback)(void*, void*), void *scope){
	// stores caller reference for delegating operations
	this->OnEventCallback = (EventCallback)? EventCallback : NULL_CALLBACK_2;
	this->eventCaller = scope;
	// sets default data
	NO_Action.weight = 0;
	this->numLevels = numLevels;
	this->level = (BEHAVIOUR_T**)malloc(numLevels * sizeof(BEHAVIOUR_T*));
	if(this->level){
		for(uint8_t i=0;i<numLevels;i++){
			this->level[i] = 0;
		}
	}
	this->actions = (ACTION_T**)malloc((numLevels+1) * sizeof(ACTION_T*));
	if(this->actions){
		for(uint8_t i=0;i<numLevels;i++){
			this->actions[i] = &NO_Action;
		}
		this->actions[numLevels] = 0;
	}
	
}

/** Destructor ***********************************************************************/
Subsumption::~Subsumption(void){
	if(this->level){
		delete(this->level);
	}
	if(this->actions){
		delete(this->actions);
	}
}

/** Adds a behaviour ****************************************************************/
void Subsumption::AddBehaviour(BEHAVIOUR_T *b){ 
	if(b){
		if(b->level < this->numLevels){
			this->level[b->level] = b;
		}
	}
}

/** Executes behaviour with new data ***********************************************/
void Subsumption::Execute(INPUT_T * inpData){
	BEHAVIOUR_T *b;
	float modulationLevel;

	// get modulating level
	b = this->level[0];
	if(b){
		b->GetOutput(b->obj, &this->actions[0]);		
		modulationLevel = this->actions[0]->weight;
	}		
	
	// apply inputs to each level
	for(uint8_t i=0; i<this->numLevels;i++){
		b = this->level[i];
		if(b){
			b->SetInput(b->obj, inpData);
		}
	}
	
	// run behaviours
	for(uint8_t i=0; i<this->numLevels;i++){
		b = this->level[i];
		if(b){
			b->RunBehaviour(b->obj, modulationLevel, this->level[0]->SetInputParam, this->level[0]);			
		}
	}
	
	// get outputs
	for(uint8_t i=1; i<this->numLevels;i++){
		b = this->level[i];
		if(b){
			b->GetOutput(b->obj, &this->actions[i]);		
		}
	}	
	// notify actions ready	
	this->OnEventCallback(this->eventCaller, &this->actions[1]);
}




