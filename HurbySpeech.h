/*************************************************************************************
		This file is the header file of the Hurby's Speech recoginition class.
 *************************************************************************************/


#ifndef HURBY_SPEECH_H
#define HURBY_SPEECH_H

#include "mbed.h"
#include "EasyVR.h"

class HurbySpeech{
    public:
		// constructor
        HurbySpeech(Serial *vr, DigitalOut * vr_rst, void (*EventCallback)(void*, void*), void *scope);
		// destructor
        ~HurbySpeech(void);
		// starts speech recognition
		void StartRecognition(char wordset = EasyVR::NUMBER_SET);
		// starts speech playback
		void StartSpeaking(uint8_t tts_id, float duration);
		// get recognised commands/words (returns number of words)
		char GetRecognition(void);
		// get last recognised commands (optionally can erase list after reading)
		uint32_t GetCommands(uint8_t forceClear=0);
		// public const speech recognitions
		static const uint8_t RECOGNISED  = 0x10;
		// max number of words recognised i a phrase speeched
		static const uint8_t MAX_WORDS_COUNT = 16;
		// wordset for SD commands (4=TRIGGER, 5=GROUP1,...19=GROUP15)
		static const uint8_t SD_WORDSET	    = 5;
		
		// public const speech commands
		static const uint32_t CMD_PUCHI 	= 0x00000001;
		static const uint32_t CMD_BAILAR 	= 0x00000002;
		static const uint32_t CMD_REIR 		= 0x00000004;
		static const uint32_t CMD_ENFADAS 	= 0x00000008;
		static const uint32_t CMD_PEDO 		= 0x00000010;
		static const uint32_t CMD_CARICIAS 	= 0x00000020;
		static const uint32_t CMD_IZQ 		= 0x00000040;
		static const uint32_t CMD_DER 		= 0x00000080;
		static const uint32_t CMD_ARRIBA 	= 0x00000100;
		static const uint32_t CMD_ABAJO 	= 0x00000200;
		static const uint32_t CMD_HOLA 		= 0x00000400;
		static const uint32_t CMD_ADIOS 	= 0x00000800;
        static const uint32_t CMD_DORMIR 	= 0x00001000;
		
		// nº de audios en total 19 (el 0 es beep)
		static const uint32_t WAV_COUNT = 19;
		// public const speech recordings
		static const uint32_t WAV_BEEP_COUNT = 1;
		// Cuando nadie juega con él  (nº de audios)
		static const uint32_t WAV_WAIT_COUNT = 10;
		// Cuando están jugando con él (nº de audios)
		static const uint32_t WAV_PLAY_COUNT = 5;
		// Cuando le cogen por las orejas (nº de audios)
        static const uint32_t WAV_ANGER_COUNT = 3;
		
    private:
		// status
		static const uint8_t LISTENING = 1;
		static const uint8_t SPEAKING = 2;
		uint32_t commands;
		uint8_t stat;
		char wordset;
		// EasyVR driver
        EasyVR * vr;
		// Timeout safety timer
		Ticker tmr;	
		// Recognised buffer
		char word;
		void AsyncTask(void);
		// event callback installed by an external caller
		void (*OnEventCallback)(void *scope, void *args);
		// external caller reference
		void *eventCaller;

};

#endif
