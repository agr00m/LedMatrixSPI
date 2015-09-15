/*
LedMatrixSPI.cpp

Copyright (c) 2015 Aaron Groom

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "LedMatrixSPI.h"

#define SETBIT(ADDRESS,BIT)   (ADDRESS |=  (1<<BIT))
#define CLEARBIT(ADDRESS,BIT) (ADDRESS &= ~(1<<BIT))
#define CHECKBIT(ADDRESS,BIT) (ADDRESS &   (1<<BIT))

LedMatrixSPI::LedMatrixSPI(uint8_t dispRows, uint8_t dispCols) {
   
   DISP_ROWS = dispRows;                  // Set the number of 8x8 display rows
   DISP_COLS = dispCols;                  // Set the number of 8x8 display columns
   NUM_DEVICES = DISP_ROWS * DISP_COLS;   // Set the total number of 8x8 displays
   NUM_COLS = DISP_COLS * 8;              // Set the number of led columns
   NUM_ROWS = DISP_ROWS * 8;              // Set the number of led rows
   SPI_BYTES = NUM_DEVICES * 2;           // Set the number of bytes in a data packet
   sDelay = 15;                           // Set the default scroll delay
   font = FONT8x8;                        // Set the default font to 8x8
   
   DDRB = (1<<SPI_MOSI)|(1<<SPI_CLK)|(1<<SPI_CS);  // Set MOSI, SCK, CS pins as output
   SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);            // SPI Enable, SPI Master mode, clock fosc/16
   
   setOPCode(OP_DISPLAYTEST, 0x00);    // Normal operation mode
   setOPCode(OP_SCANLIMIT, 0x07);      // Display all 8 digits
   setOPCode(OP_DECODEMODE, 0x00);     // No BCD decoding
   setOPCode(OP_INTENSITY, 0x01);      // Set intensity to low value
   setOPCode(OP_SHUTDOWN, 0x01);       // Wake from shut-down mode
   clearScreen();                      // Clear the array
}

void LedMatrixSPI::setIntensity(uint8_t intensity) {
	if (intensity > 15) {
      return;
   }
   setOPCode(OP_INTENSITY, intensity);
}

void LedMatrixSPI::shutdown(bool val) {
	if(val) {
		setOPCode(OP_SHUTDOWN, 0x00);
	} else {
		setOPCode(OP_SHUTDOWN, 0x01);
	}
}

void LedMatrixSPI::setFont(uint8_t newFont) {
   if (newFont != FONT5x7 && newFont != FONT8x8) {
      return;
   }
   font = newFont;
}

void LedMatrixSPI::setLed(uint8_t x, uint8_t y, bool val) {
   if (x >= NUM_COLS || y >= NUM_ROWS) {
      return;
   }
   uint8_t dispRow = y / 8;   // floor(y/8)
   uint8_t dispCol = x / 8;   // floor(x/8)
   uint8_t index = 8*(dispCol+dispRow*(DISP_COLS-1))+y;   // Calculate index
   if(val) {
      SETBIT(pixels[index], x%8);
   } else {
      CLEARBIT(pixels[index], x%8);
   }
}

void LedMatrixSPI::clear(void) {
   for (uint8_t i = 0; i < NUM_DEVICES*8; i++) {
      pixels[i] = 0x00;
   }
}

void LedMatrixSPI::clearScreen(void) {
	clear();
	update();
}

void LedMatrixSPI::scrollDelay(uint8_t val) {
	sDelay = val;
}

void LedMatrixSPI::drawChar(char c, uint8_t x, uint8_t y) {
   int8_t fHeight, rows, offset; 
   bufferChar(c);
   for (uint8_t col = 0; col < font; col++) { 
      if (x+col >= 0 && x+col < NUM_COLS) {
         if (font == FONT5x7) {
            rows = 7;
            offset = 1;
         } else if (font == FONT8x8) {
            rows = 8;
            offset = 0;
         }
         for (uint8_t row = 0; row < rows; row++) {
            if (y+row >= 0 && y+row < NUM_ROWS) {
               setLed(x+col, y+row, CHECKBIT(charBuffer[col], 7-row-offset));
            }
         }
      }
   }
}

void LedMatrixSPI::scrollChar(uint8_t c, uint8_t dispRow) {
   if (dispRow >= DISP_ROWS) {
      return;
   }
   uint8_t offset = 8*(DISP_COLS*(dispRow+1)-1), row = 0; 
   bufferChar(c);
   for (uint8_t col = 0; col < font; col++) {
      shiftLeft(dispRow);
      if (font == FONT5x7) {
         row = 1;
      } else if (font == FONT8x8) {
         row = 0;
      }
      for (row; row < 8; row++) {
         if (CHECKBIT(charBuffer[col], 7-row)) {
            SETBIT(pixels[offset+row], 7);
                          
         } else {
            CLEARBIT(pixels[offset+row], 7);
         }
      }
      update();
   }
   // If 5x7 font, shift one more to add a space between characters.
   if (font == FONT5x7) {
      shiftLeft(dispRow);
      update();
   }
}

void LedMatrixSPI::shiftLeft(uint8_t dispRow) {
	uint8_t offset = dispRow * DISP_COLS * 8, i;
   for (byte dispCol = 0; dispCol < DISP_COLS; dispCol++) {
      for (byte row = 0; row < 8; row++) {
         i = (dispCol*8)+offset+row;
         // If the MSB is 1, set the LSB of the prev index
         // before shifting.
         if (dispCol > 0 && CHECKBIT(pixels[i],0)) {
            SETBIT(pixels[i-8], 7);
         }
         pixels[i] = pixels[i] >> 1;
      }
   }
   delay(sDelay);
}

void LedMatrixSPI::line(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
   if (x1 >= NUM_COLS || x2 >= NUM_COLS || y1 >= NUM_ROWS || y2 >= NUM_ROWS) {
      return;
   }
   int8_t dx = abs(x2-x1); 
   int8_t sx = x1<x2 ? 1 : -1;
   int8_t dy = abs(y2-y1);
   int8_t sy = y1<y2 ? 1 : -1;
   int8_t err = (dx>dy ? dx : -dy)/2;
   int8_t e2;
   if (y1 == y2) {
      if (x1 > x2) {
         uint8_t temp = x2;
         x2 = x1;
         x1 = temp;
      }
      for (uint8_t x = x1; x <= x2; x++) {
         setLed(x, y1, 1);
      }
   } else {
      for(;;) {
         setLed(x1,y1, 1);
         if(x1==x1 && y1 == y2) {
            break;
         }
         e2 = err;
         if (e2 > -dx) {
            err -= dy;
            x1 += sx;
         }
         if (e2 < dy) {
            err += dx;
            y1 += sy;
         }
      }
   }
}

void LedMatrixSPI::triangle(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3) {
   if (x3 >= NUM_COLS || y3 >= NUM_ROWS) {
      return;
   }
   line(x1, y1, x2, y2);
   line(x2, y2, x3, y3);
   line(x3, y3, x1, y1);
}

void LedMatrixSPI::quad(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t x3, uint8_t y3, uint8_t x4, uint8_t y4) {
   if (x4 >= NUM_COLS || y4 >= NUM_ROWS) {
      return;
   }
   line(x1, y1, x2, y2);
   line(x2, y2, x3, y3);
   line(x3, y3, x4, y4);
   line(x4, y4, x1, y1);
}

void LedMatrixSPI::rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h) {
   if (x >= NUM_COLS || y >= NUM_ROWS) {
      return;
   }
   for (uint8_t i = 0; i < w; i++) {
      if (x+i >= 0 && x+i < NUM_COLS) {
         if(y >=0 && y < NUM_ROWS) {
            setLed(x+i, y, 1);
         }
         if (y+h-1 >= 0 && y+h-1 < NUM_ROWS) {
            setLed(x+i, y+h-1, 1);
         }
      }
   }
   for (uint8_t i = 0; i < h; i++) {
      if (y+i >= 0 && y+i < NUM_ROWS) {
         if (x >= 0 && x < NUM_COLS) {
            setLed(x, y+i, 1);
         }
         if (x+w-1 >= 0 && x+w-1 < NUM_COLS){
            setLed(x+w-1, y+i, 1);
         }
      }
   }
}

void LedMatrixSPI::circle(uint8_t h, uint8_t k, uint8_t radius) {
   uint8_t diameter = radius*2;
   ellipse(h, k, diameter, diameter);
}

void LedMatrixSPI::ellipse(uint8_t h, uint8_t k, uint8_t a, uint8_t b) {
   if (h >= NUM_COLS || k >= NUM_ROWS) {
      return;
   }
   uint8_t xPos, yPos;
   for (uint8_t x = h - a/2; x <= h + a/2; x++) {
      if (x >= 0 && x < NUM_COLS) {
         yPos = (uint8_t)round(k + sqrt(pow(b/2,2)*(1-pow(x-h,2)/pow(a/2,2))));
         if (yPos >= 0 && yPos < NUM_ROWS) {
            setLed(x, yPos, 1);
         }
         yPos = (uint8_t)round(k - sqrt(pow(b/2,2)*(1-pow(x-h,2)/pow(a/2,2))));
         if (yPos >= 0 && yPos < NUM_ROWS) {
            setLed(x, yPos, 1);
         }
      }
   }
   for (uint8_t y = k - b/2; y <= k + b/2; y++) { 
      if (y >= 0 && y < NUM_ROWS) {
         xPos = (uint8_t)round(h + sqrt(pow(a/2,2)*(1-pow(y-k,2)/pow(b/2,2))));
         if (xPos >= 0 && xPos < NUM_COLS) {
            setLed(xPos, y, 1);
         }
         xPos = (uint8_t)round(h - sqrt(pow(a/2,2)*(1-pow(y-k,2)/pow(b/2,2))));
         if (xPos >= 0 && xPos < NUM_COLS) {
            setLed(xPos, y, 1);
         }
      }  
   }  
}

void LedMatrixSPI::bufferChar(uint8_t c) { 
   for (uint8_t col = 0; col < font; col++) {  
      if (font == FONT5x7) {
         charBuffer[col] = pgm_read_byte(&font_5x7_col[c][col]);
      } else if (font == FONT8x8) {
          charBuffer[col] = pgm_read_byte(&font_8x8_col[c][col]);
      }
   }
}

void LedMatrixSPI::setOPCode(volatile uint8_t opcode, volatile uint8_t val) {
   for (uint8_t i = 0; i < SPI_BYTES; i++) {
      spiData[i++] = val;
      spiData[i] = opcode;
   }
   spiWrite();
}

void LedMatrixSPI::update(void) {
   uint8_t i, opcode;
   for (i = 0; i < SPI_BYTES; i++) {
      spiData[i] = 0x00;
   }
   for (uint8_t row = 0; row < 8; row++) {  
      i = 0;
      opcode = 8 - row;    
      for (uint8_t device = 0; device < NUM_DEVICES; device++) {
         spiData[i++] = pixels[device*8+row];
         spiData[i++] = opcode; 
      }
      spiWrite();
   } 
}

void LedMatrixSPI::spiWrite(void) {
   CLEARBIT(PORTB, SPI_CS);            // Load serial data
   for (int8_t i = SPI_BYTES - 1; i >= 0; i--) {
      SPDR = spiData[i];               // Write SPI byte
      while(!(SPSR & (1<<SPIF) ));     // Wait for transfer
   }
   SETBIT(PORTB, SPI_CS);              // Latch serial data
}

