/*************************************************************************************
		This file includes test cases for: 
			- Eyes blinking
			- Eyes movement
			- Eyes reloading
 *************************************************************************************/

#include "mbed.h"
#include "main.h"

// local reference of hurby object
static Hurby *hurby;
Serial* serial;
Serial* device;

void TestVoice(Hurby *h){
	// stores the hurby reference
	hurby = h;
	serial = hurby->GetLogger();
	device = hurby->GetVR();
	while(1) {
        if(serial->readable()) {
            device->putc(serial->getc());
        }
        if(device->readable()) {
            serial->putc(device->getc());
        }
    }
}

