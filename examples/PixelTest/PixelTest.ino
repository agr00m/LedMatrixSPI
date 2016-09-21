/*
 * PixelTest.ino
 * 
 * Author:  Aaron Groom
 * Date:    01-27-2016
 * 
 * Descrption: 
 *    Tests each pixel by turning each on then off (optional).
 * 
 * Pins used:
 *    Data:    Digital pin 10
 *    Clock:   Digital pin 13
 *    CS/Load: Digital pin 11
 */

// Comment out if using the non-SPI version of the library
#define SPIWrite

// Include the LedMatrix library
#ifdef SPIWrite
#include <LedMatrixSPI.h>
#else
#include <LedMatrix.h>
#endif

const byte DISP_ROWS = 6;            // Number of 8x8 display rows
const byte DISP_COLS = 8;            // Number of 8x8 display columns
const byte LED_ROWS = DISP_ROWS * 8; // Number of LED rows
const byte LED_COLS = DISP_COLS * 8; // Number of LED columns

/*
 * Create an LedMarix object.
 * The first parameter is the number of 8x8 display rows, 
 * the second is the number of 8x8 display columns. 
 */
#ifdef SPIWrite
LedMatrixSPI lm = LedMatrixSPI(DISP_ROWS, DISP_COLS);
#else
LedMatrix lm = LedMatrix(DISP_ROWS, DISP_COLS);
#endif

bool keepLedsOn = true;    // Whether LEDs stay on or turn on then off.
int delay_time = 0;       // Time in milliseconds a pixel is on.

void setup() {
   // No setup required
}

void loop() {
   singleLed();
   while(1);
}

/*************************************************************
 * Turns each LED on starting in the upper left corner (0,0).
 *************************************************************/
void singleLed() {
   for (int yPos = 0; yPos < LED_ROWS; yPos++) {        // Each row
      for (int xPos = 0; xPos < LED_COLS; xPos++) {     // Each column
         lm.setLed(xPos, yPos, 1);                       // Turn LED on
         lm.update();                                    // Update the display
         delay(delay_time);                              // Pause for a few milliseconds
         if(!keepLedsOn) {
            lm.setLed(xPos, yPos, 0);                    // Turn LED off
            lm.update();                                 // Update the display again
         }
      }
   }
}
