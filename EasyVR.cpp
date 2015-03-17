
#include "EasyVR.h"

/**
	Creates an EasyVR object, using a communication object implementing the 
	#Stream interface (such as #HardwareSerial, or the modified #SoftwareSerial
	and #NewSoftSerial).
	@param s the Stream object to use for communication with the EasyVR module
	@param baudrate New baudrate or 0 for using default baudrate previously set on <s>
*/
EasyVR::EasyVR(Serial * s, DigitalOut * rst, uint32_t baudrate){
	this->_s = s;
	this->_rst = rst;
	if(baudrate != 0){
		this->_s->baud(baudrate);
	}
	this->hwReset();
};

/*****************************************************************************/

void EasyVR::send(char c){
	wait(.001);
	_s->putc(c);
}

/*
inline void EasyVR::sendArg(int8_t c)
{
  send(c + ARG_ZERO);
}
*/
#define sendArg(c) send((c) + ARG_ZERO)

inline void EasyVR::sendGroup(int8_t c){
	wait(.001);
	_s->putc(c + ARG_ZERO);
	wait(.0019); // worst case time to cache a full group in memory
}

char EasyVR::recv(int16_t timeout){ // negative means forever
	while (timeout >= 0 && !_s->readable()) {
		wait(.001);
		if (timeout > 0)
			--timeout;			
	}
	if(_s->readable()){
		return _s->getc();
	}
	return STS_TIMEOUT;
}

bool EasyVR::recvArg(int8_t& c, int16_t timeout){
	send(ARG_ACK);
	int r = recv(timeout);
	c = r - ARG_ZERO;
	return r >= ARG_MIN && r <= ARG_MAX;
}

/*****************************************************************************/

bool EasyVR::detect(){
	uint8_t i;
	char c;
	for (i = 0; i < 5; ++i){
		sendCmd(CMD_BREAK);
		c = recv(WAKE_TIMEOUT);
		if (c == STS_SUCCESS || c == STS_AWAKEN)
			return true;
	}
	return false;
}

bool EasyVR::stop(){
	sendCmd(CMD_BREAK);
	uint8_t rx = recv(WAKE_TIMEOUT);
	if (rx == STS_INTERR || rx == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::sleep(int8_t mode){
	sendCmd(CMD_SLEEP);
	sendArg(mode);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

int8_t EasyVR::getID(){
	sendCmd(CMD_ID);
	if (recv(DEF_TIMEOUT) == STS_ID){
		int8_t rx;
		if (recvArg(rx, DEF_TIMEOUT))
		return rx;
	}
	return -1;
}

bool EasyVR::setLanguage(int8_t lang){        
	sendCmd(CMD_LANGUAGE);
	sendArg(lang);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::setTimeout(int8_t seconds){
	sendCmd(CMD_TIMEOUT);
	sendArg(seconds);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::setKnob(int8_t knob){
	sendCmd(CMD_KNOB);
	sendArg(knob);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::setLevel(int8_t level){
	sendCmd(CMD_LEVEL);
	sendArg(level);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::setDelay(uint16_t millis){
	sendCmd(CMD_DELAY);
	if (millis <= 10)
		sendArg(millis);
	else if (millis <= 100)
		sendArg(millis / 10 + 9);
	else if (millis <= 1000)
		sendArg(millis / 100 + 18);
	else
		return false;
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::changeBaudrate(int8_t baudrate){
	sendCmd(CMD_BAUDRATE);
	sendArg(baudrate);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS){
		this->_s->baud((115200/baudrate));
		return true;
	}
	return false;
}


bool EasyVR::addCommand(int8_t group, int8_t index){
	sendCmd(CMD_GROUP_SD);
	sendGroup(group);
	sendArg(index);
	int rx = recv(DEF_TIMEOUT);
	if (rx == STS_SUCCESS)
		return true;
	_status.v = 0;
	if (rx == STS_OUT_OF_MEM)
		_status.b._memfull = true;
	return false;
}

bool EasyVR::removeCommand(int8_t group, int8_t index){
	sendCmd(CMD_UNGROUP_SD);
	sendGroup(group);
	sendArg(index);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::setCommandLabel(int8_t group, int8_t index, const char* name){
	sendCmd(CMD_NAME_SD);
	sendGroup(group);
	sendArg(index);
	int8_t len = 31;
	for (const char* p = name; *p != 0 && len > 0; ++p, --len){
		if (*p >= '0' && *p <= '9')
			--len;
	}
	len = 31 - len;
	sendArg(len);
	for (int8_t i = 0; i < len; ++i){
		char c = name[i];
		if (c >= '0' && c <= '9'){
			send('^');
			sendArg(c - '0');
		}
		else if (c == '^'){
			send(c & ~0x20); // to uppercase
		}
		else{
			send('_');
		}
	}
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::eraseCommand(int8_t group, int8_t index){
	sendCmd(CMD_ERASE_SD);
	sendGroup(group);
	sendArg(index);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}


bool EasyVR::getGroupMask(uint32_t& mask){
	sendCmd(CMD_MASK_SD);
	if (recv(DEF_TIMEOUT) == STS_MASK){
		int8_t rx;
		for (int8_t i = 0; i < 4; ++i){
			if (!recvArg(rx, DEF_TIMEOUT))
				return false;
			((uint8_t*)&mask)[i] |= rx & 0x0F;
			if (!recvArg(rx, DEF_TIMEOUT))
				return false;
			((uint8_t*)&mask)[i] |= (rx << 4) & 0xF0;
		}
		return true;
	}
	return false;
}

int8_t EasyVR::getCommandCount(int8_t group){
	sendCmd(CMD_COUNT_SD);
	sendArg(group);
	if (recv(DEF_TIMEOUT) == STS_COUNT) {
		int8_t rx;
		if (recvArg(rx, DEF_TIMEOUT)){
			return rx == -1 ? 32 : rx;
		}
	}
	return -1;
}

bool EasyVR::dumpCommand(int8_t group, int8_t index, char* name, uint8_t& training){
	sendCmd(CMD_DUMP_SD);
	sendGroup(group);
	sendArg(index);

	if (recv(DEF_TIMEOUT) != STS_DATA)
		return false;
  
	int8_t rx;
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	training = rx & 0x07;
	if (rx == -1 || training == 7)
		training = 0;
  
	_status.v = 0;
	_status.b._conflict = (rx & 0x18) != 0;
	_status.b._command = (rx & 0x08) != 0;
	_status.b._builtin = (rx & 0x10) != 0;
  
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	_value = rx;

	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	int8_t len = rx == -1 ? 32 : rx;
	for ( ; len > 0; --len, ++name)  {
		if (!recvArg(rx, DEF_TIMEOUT))
			return false;
		if (rx == '^' - ARG_ZERO){
			if (!recvArg(rx, DEF_TIMEOUT))
				return false;
			*name = '0' + rx;
			--len;
		}
		else{
			*name = ARG_ZERO + rx;
		}
	}
	*name = 0;
	return true;
}

int8_t EasyVR::getGrammarsCount(void){
	sendCmd(CMD_DUMP_SI);
	sendArg(-1);
	if (recv(DEF_TIMEOUT) == STS_COUNT){
		int8_t rx;
		if (recvArg(rx, DEF_TIMEOUT)){
			return rx == -1 ? 32 : rx;
		}
	}
	return -1;
}

bool EasyVR::dumpGrammar(int8_t grammar, uint8_t& flags, uint8_t& count){
	sendCmd(CMD_DUMP_SI);
	sendArg(grammar);
	if (recv(DEF_TIMEOUT) != STS_GRAMMAR)
		return false;  
	int8_t rx;
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	flags = rx == -1 ? 32 : rx;  
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	count = rx;
	return true;
}

bool EasyVR::getNextWordLabel(char* name){
	int8_t count;
	if (!recvArg(count, DEF_TIMEOUT))
		return false;
	if (count == -1)
		count = 32;
  
	for ( ; count > 0; --count, ++name){
		int8_t rx;
		if (!recvArg(rx, DEF_TIMEOUT))
		return false;    
		if (rx == '^' - ARG_ZERO)    {
			if (!recvArg(rx, DEF_TIMEOUT))
				return false;      
			*name = '0' + rx;
			--count;
		}
		else {
		*name = ARG_ZERO + rx;
		}
	}
	*name = 0;
	return true;
}

void EasyVR::trainCommand(int8_t group, int8_t index){
	sendCmd(CMD_TRAIN_SD);
	sendGroup(group);
	sendArg(index);
}

void EasyVR::recognizeCommand(int8_t group){
	sendCmd(CMD_RECOG_SD);
	sendArg(group);
}

void EasyVR::recognizeWord(int8_t wordset){
	sendCmd(CMD_RECOG_SI);
	sendArg(wordset);
}

bool EasyVR::hasFinished(){
	int8_t rx = recv(NO_TIMEOUT);
	if (rx < 0){
		return false;  
	}
	
	_status.v = 0;	  
	switch (rx)  {
		case STS_SUCCESS:
			return true;
  
		case STS_SIMILAR:
			_status.b._builtin = true;
			if (recvArg(rx, DEF_TIMEOUT)){
				_value = rx;
				return true;
			}
			break;

		case STS_RESULT:
			_status.b._command = true;  
			if (recvArg(rx, DEF_TIMEOUT)){
				_value = rx;
				return true;
			}
			break;
    
		case STS_TOKEN:
			_status.b._token = true;  
			if (recvArg(rx, DEF_TIMEOUT)){
				_value = rx << 5;
				if (recvArg(rx, DEF_TIMEOUT)){
					_value |= rx;
					return true;
				}
			}
			break;
    
		case STS_TIMEOUT:
			_status.b._timeout = true;
			return true;
		
		case STS_INVALID:
			_status.b._invalid = true;
			return true;
		
		case STS_ERROR:
			_status.b._error = true;
			if (recvArg(rx, DEF_TIMEOUT)){
				_value = rx << 4;
				if (recvArg(rx, DEF_TIMEOUT)){
					_value |= rx;
					return true;
				}
			}
			break;
	}
	// unexpected condition (communication error)
	_status.v = 0;
	_status.b._error = true;
	return true;
}

bool EasyVR::setPinOutput(int8_t pin, int8_t value){
	sendCmd(CMD_QUERY_IO);
	sendArg(pin);
	sendArg(value);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

int8_t EasyVR::getPinInput(int8_t pin, int8_t config){
	sendCmd(CMD_QUERY_IO);
	sendArg(pin);
	sendArg(config);
	if (recv(DEF_TIMEOUT) == STS_PIN)  {
		int8_t rx;
		if (recvArg(rx, DEF_TIMEOUT))
		return rx;
	}
	return -1;
}

bool EasyVR::playPhoneTone(int8_t tone, uint8_t duration){
	sendCmd(CMD_PLAY_DTMF);
	sendArg(-1); // distinguish DTMF from SX
	sendArg(tone);
	sendArg(duration - 1);
	if (recv((tone < 0 ? duration * 1000 : duration * 40) + DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::playSound(int16_t index, int8_t volume){
	sendCmd(CMD_PLAY_SX);
	sendArg((index >> 5) & 0x1F);
	sendArg(index & 0x1F);
	sendArg(volume);
	if (recv(PLAY_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

void EasyVR::playSoundAsync(int16_t index, int8_t volume){
	sendCmd(CMD_PLAY_SX);
	sendArg((index >> 5) & 0x1F);
	sendArg(index & 0x1F);
	sendArg(volume);
}

void EasyVR::detectToken(int8_t bits, int8_t rejection, uint16_t timeout){
	sendCmd(CMD_RECV_SN);
	sendArg(bits);
	sendArg(rejection);
	if (timeout > 0)
		timeout = (timeout * 2 + 53)/ 55; // approx / 27.46 - err < 0.15%
	sendArg((timeout >> 5) & 0x1F);
	sendArg(timeout & 0x1F);
}

bool EasyVR::sendToken(int8_t bits, uint8_t token){
	sendCmd(CMD_SEND_SN);
	sendArg(bits);
	sendArg((token >> 5) & 0x1F);
	sendArg(token & 0x1F);
	sendArg(0);
	sendArg(0);
	if (recv(TOKEN_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

void EasyVR::sendTokenAsync(int8_t bits, uint8_t token){
	sendCmd(CMD_SEND_SN);
	sendArg(bits);
	sendArg((token >> 5) & 0x1F);
	sendArg(token & 0x1F);
	sendArg(0);
	sendArg(0);
}

bool EasyVR::embedToken(int8_t bits, uint8_t token, uint16_t delay){
	sendCmd(CMD_SEND_SN);
	sendArg(bits);
	sendArg((token >> 5) & 0x1F);
	sendArg(token & 0x1F);
	delay = (delay * 2 + 27) / 55; // approx / 27.46 - err < 0.15%
	if (delay == 0) // must be > 0 to embed in some audio
		delay = 1;
	sendArg((delay >> 5) & 0x1F);
	sendArg(delay & 0x1F);
	if (recv(DEF_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}

bool EasyVR::dumpSoundTable(char* name, int16_t& count){
	sendCmd(CMD_DUMP_SX);

	if (recv(DEF_TIMEOUT) != STS_TABLE_SX)
		return false;
  
	int8_t rx;
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	count = rx << 5;
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	count |= rx;
  
	if (!recvArg(rx, DEF_TIMEOUT))
		return false;
	int len = rx;
	for (int8_t i = 0, k = 0; i < len; ++i, ++k){
		if (!recvArg(rx, DEF_TIMEOUT))
			return false;
		if (rx == '^' - ARG_ZERO){
			if (!recvArg(rx, DEF_TIMEOUT))
				return false;
			++i;
			name[k] = '0' + rx;
		}
		else{
			name[k] = ARG_ZERO + rx;
		}
	}
	name[len] = 0;
	return true;
}


bool EasyVR::resetAll(){
	sendCmd(CMD_RESETALL);
	sendArg(17);
	if (recv((int16_t)RESET_TIMEOUT) == STS_SUCCESS)
		return true;
	return false;
}


void EasyVR::hwReset(){
	this->_rst->write(0);
	wait(0.1);
	this->_rst->write(1);
	wait(0.1);
}


void EasyVR::testWordset(uint8_t wordset){
	volatile int id;
	char c;
	// detect VR module
	detect();
	// stop previous operations
	stop();	
	// get ID
	id = getID();
	// set spanish language
	setLanguage(EasyVR::SPANISH);
	// set infinite timeout for recognition
	setTimeout(0);
	// set wordset recognition...
	recognizeWord(wordset);
	// bucle de reconocimiento
	for(;;){
		c = recv();
		switch(c){
			/* Respuestas de control */
			case STS_SIMILAR:{ //'s'
				wait(0.1);
				send(' ');
				do{
					wait(0.001);
					c = recv();
				}while(c == (char)-1 || c == STS_TIMEOUT);
				char cmd = c - ARG_ZERO;
				#warning "PROCESAR COMANDO";
				recognizeWord(wordset);				
				break;
			}
			case STS_TIMEOUT:{ //'t'
				break;
			}
			case STS_ERROR:{ //'e'
				stop();
				recognizeWord(wordset);
				break;
			}
			default:{
				break;
			}
		}
	}	
}


/**
* chequea si ha obtenido un dato v�lido
* @param cmd Comando reconocido
* @return true si ha reconocido, false si no ha reconocido nada
*/
bool EasyVR::RecognizedData(char *cmd){
	char c = recv();
	switch(c){
		/* Respuestas de control */
		case STS_SIMILAR:{ //'s'
			wait(0.1);
			send(' ');
			do{
				wait(0.001);
				c = recv();
			}while(c == (char)-1 || c == STS_TIMEOUT);
			*cmd = c - ARG_ZERO;
			return true;
		}
		case STS_TIMEOUT:{ //'t'			
			break;
		}
		case STS_ERROR:{ //'e'
			stop();
			break;
		}
		default:{
			break;
		}
	}
	*cmd = c;
	return false;
}

