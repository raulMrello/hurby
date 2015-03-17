/*************************************************************************************
		This file is the entry point of the Hurby Toy application: 
			- Starts debug terminal
			- Starts Hurby toy
			- Enable test cases if needed
 *************************************************************************************/


#include "mbed.h"
#include "main.h"


// Debug terminal serial interface
Serial pc(USBTX, USBRX);
#if defined(ENABLE_SERIAL_BRIDGE_RECORD) || defined(ENABLE_SERIAL_BRIDGE_DOWNLOAD)
Serial bridge(p28,p27);
DigitalOut vrst(p20);
#endif

static void CheckTests(void);

/* Starts application ****************************************************************/
int main() {
	// checks if some test is enabled
	CheckTests();
	
	// creates hurby instance
    new Hurby(&pc);
	
	// runs forever
    for(;;){
    }
}

/* Checks tests to be done ********************************************************/
static void CheckTests(void){

	/* Enable logger?? */	

	#if defined(ENABLE_SERIAL_LOG)
    #warning "ENABLE_SERIAL_LOG activado";
    pc.baud(115200);

	
	/* Enable bridge mode for training?? */	

	#elif defined(ENABLE_SERIAL_BRIDGE_RECORD)
	#warning "ENABLE_SERIAL_BRIDGE_RECORD activado";
    pc.baud(9600);
	bridge.baud(9600);

	
	/* Enable bridge mode for download?? */	

	#elif defined(ENABLE_SERIAL_BRIDGE_DOWNLOAD)
    #warning "ENABLE_SERIAL_BRIDGE_DOWNLOAD activado";
	pc.baud(115200);
	bridge.baud(115200);
	#endif

	
	/* Execute bridge mode?? */	
	
	#if defined(ENABLE_SERIAL_BRIDGE_RECORD) || defined(ENABLE_SERIAL_BRIDGE_DOWNLOAD)
	volatile char c;
	vrst.write(0);
	wait(0.1);
	vrst.write(1);
	wait(0.1);
	while(1) {
        if(pc.readable()) {
			c = pc.getc();
            bridge.putc(c);
        }
        if(bridge.readable()) {
			c = bridge.getc();
            pc.putc(c);
        }
    }
	#endif
	
	
	/* Else, Continue with some tests... */	

	/* Test fuzzy inference system */
	#if defined(ENABLE_FUZZY_TEST)
	TestFuzzy();		
	#endif		

}

