#include "LCDDriver.h"



/****************************************************************************************************************/
/****************************************************************************************************************/
/*                                      LCDDriver Class                                                         */
/****************************************************************************************************************/
/****************************************************************************************************************/

/** Constructor **********************************************************************/
LCDDriver::LCDDriver(SPI * spi_port, DigitalOut *cs_pin, DigitalOut *cd_pin, DigitalOut *rst_pin,DigitalOut *bl_pin){
    // assign io lines
    this->spi = spi_port;
    this->cs = cs_pin;
    this->cd = cd_pin;
    this->rst = rst_pin;
    this->bl = bl_pin;
    
    // backlight off
    this->bl->write(0);

    // cs=1 & rst=1
    this->cs->write(1); //ISelClear();
    this->rst->write(1); //IRstClear();

    //display init sequence
    this->cs->write(1); //ISelClear();
    this->rst->write(0); //IRstSet();
    wait(0.001); //1ms
    this->rst->write(1); //IRstClear();
    wait(0.05);  //50ms     
    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0xFA);//Set Adv. Program Control 0
    this->spi->write(0x93);
    this->cs->write(1); //ISelClear();

    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0x24); //Set VLCD Resistor Ratio 100b
    this->cs->write(1); //ISelClear();
 
    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0x81); //Set Electronic Volume
    this->spi->write(0x30); //Set Electronic Volume
    this->cs->write(1); //ISelClear();

    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0xA2); //Set LCD Bias Ratio
    this->cs->write(1); //ISelClear();

    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0x2F); //Set Power Control
    this->cs->write(1); //ISelClear();
      
    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
	this->spi->write(0xA4); //Set All Pixel OFF
    this->cs->write(1); //ISelClear();
  
    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0xAF); //Set Display Enable
    this->cs->write(1); //ISelClear();
    
    // clear RAM memory and refresh LCD content
	this->Clear();
}

/** Erase RAM memory *****************************************************************/
void LCDDriver::Clear(void){
  memset(this->mem,0,this->MEM_SIZE);
  this->Refresh();
} 

/** Set sleep mode *******************************************************************/
void LCDDriver::Sleep(void){
    this->bl->write(0); //  IBlOff();
    this->cs->write(0); //ISelActive();
    this->cd->write(0); //ICmdOn();
    this->spi->write(0xAE); //Set Display Disable
    this->cs->write(1); //ISelClear();
}

/** Prints an image on RAM memory from left and top position *************************/
void LCDDriver::PutImage(BITMAP_FLASH* bitmap, uint8_t left, uint8_t top){
    FLASH_BYTE  *flashAddress;
    register FLASH_BYTE *tempFlashAddress;
    uint8_t temp = 0;
    uint16_t sizeX, sizeY;
    uint16_t x, y;
    uint16_t xc, yc;
    uint8_t stretchX, stretchY;
    uint16_t pallete[2];
    uint8_t mask;
    uint16_t color;

     // Image address
    flashAddress = bitmap->address;

    // Move pointer to size information
    flashAddress += 2;

    // Read image size
    sizeY = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    sizeX = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[1] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
    pallete[0] = *((FLASH_WORD *)flashAddress);
    flashAddress += 2;
	
	// Erase memory content
	memset(this->mem,0,this->MEM_SIZE);

    yc = top;
    for(y = 0; y < sizeY; y++)
    {
        tempFlashAddress = flashAddress;
        for(stretchY = 0; stretchY < 1; stretchY++)
        {
            flashAddress = tempFlashAddress;
            mask = 0;
            xc = left;
            for(x = 0; x < sizeX; x++)
            {
                // Read 8 pixels from flash
                if(mask == 0){
                    temp = *flashAddress;
                    flashAddress++;
                    mask = 0x80;
                }
                // Set color
                if(mask & temp){
                    color = pallete[1];
                }
                else{
                    color = pallete[0];
                }
                // Write pixel to screen
                for(stretchX = 0; stretchX < 1; stretchX++){
                    PutPixel(xc++, yc, color);
                }
                // Shift to the next pixel
                mask >>= 1;
            }
            yc++;
        }
    }
    this->Refresh();
}


/** Prints a pixel on RAM memory *****************************************************/
void LCDDriver::PutPixel(uint8_t x, uint8_t y, uint16_t color){
    uint8_t mask;
//     if(_clipRgn){
//         if(x<_clipLeft)
//             return;
//         if(x>_clipRight)
//             return;
//         if(y<_clipTop)
//             return;
//         if(y>_clipBottom)
//             return;
//     }  
    // address
    //bit
    mask = (uint8_t)(1 << (y%8));
    if(color == this->COLOR_BLACK){
        this->mem[y/8][x] |= (uint8_t)mask;  
    }
    else{
        this->mem[y/8][x] &= ~mask;
    }
 }

/** Get pixel color from RAM memory **************************************************/
uint16_t LCDDriver::GetPixel(uint8_t x, uint8_t y){
    if(this->mem[y/8][x] & (1 << (y%8)))
        return this->COLOR_BLACK; 
    return  this->COLOR_WHITE;
} 


/** Refresh LCD content **************************************************************/
void LCDDriver::Refresh(void){
    uint8_t x,y;  
	this->bl->write(1); //IBLlOn();
    this->cs->write(0); //ISelActive();
    for(y=0;y<8;y++){
        this->cd->write(0); //ICmdOn();
        this->spi->write(0x00); //Set Column Address L
        this->spi->write(0x10); //Set Column Address H
        this->spi->write(0xB0 | y); //Set Page Address
            
        for(x=0;x<128;x++){
            this->cd->write(1); //ICmdOff();
            this->spi->write(this->mem[y][x]);
        }
    }
    this->cs->write(1); //ISelClear();    
}
