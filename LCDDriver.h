#ifndef _LCDDRIVER_H_
#define _LCDDRIVER_H_

#include "mbed.h"
#include "eyes_img.h"


class LCDDriver{
    public:
        LCDDriver(SPI * spi_port, DigitalOut *cs_pin, DigitalOut *cd_pin, DigitalOut *rst_pin,DigitalOut *bl_pin);
        void Clear(void);
        void Sleep(void);
        void PutImage(BITMAP_FLASH* bitmap, uint8_t left, uint8_t top);
        static const uint8_t Y_SIZE = 64;
        static const uint8_t X_SIZE = 128;
    private:
        static const uint16_t COLOR_BLACK = 0xFFFF;
        static const uint16_t COLOR_WHITE = 0x0000;     
        static const uint16_t MEM_SIZE = 1024;
        void PutPixel(uint8_t x, uint8_t y, uint16_t color);
        uint16_t GetPixel(uint8_t x, uint8_t y);
        void Refresh(void);
        uint8_t mem[8][128];  
        DigitalOut *cs;
        DigitalOut *cd;
        DigitalOut *rst;
        DigitalOut *bl;
        SPI *spi;
        uint16_t _color;
        uint8_t _clipRgn;
        uint8_t _clipLeft;
        uint8_t _clipTop;
        uint8_t _clipRight;
        uint8_t _clipBottom;    
};

#endif
