/*
 * ScrollText
 * 
 * Author:  Aaron Groom
 * Date:    08-07-2015
 * 
 * Demonstrating how to scroll text using the LedMatrix library.
 * 
 * Pins used are:
 *    CS/Load: Digital pin 10
 *    Data:    Digital pin 11
 *    Clock:   Digital pin 13
 */

// Comment out if using the non-SPI version of the library
#define SPIWrite

// Include the LedMatrix library
#ifdef SPIWrite
#include <LedMatrixSPI.h>
#else
#include <LedMatrix.h>
#endif

// Defines the number of 8x8 display rows and columns
const byte NUM_ROWS = 6;
const byte NUM_COLS = 8;

/*
 * Create an LedMarix object
 * The first parameter is the number of 8x8 dot matrix
 * rows, the second is the number of columns. 
 */
#ifdef SPIWrite
LedMatrixSPI lm = LedMatrixSPI(NUM_ROWS, NUM_COLS);
#else
LedMatrix lm = LedMatrix(NUM_ROWS, NUM_COLS);
#endif
/* 
 * Enter the text you want to scroll. Leave a few blank spaces
 * at the end for an easy way to scroll off the screen.
*/
char text[] = "Hello World!          ";

void setup() {
   // No setup required. Some options you can set are
   lm.scrollDelay(5);      // Sets the delay for scrolling text. 0 = fastest
   lm.setIntensity(1);     // Sets the LED intensity. Default is 1.
   lm.setFont(FONT8x8);    // Sets the char font. Either FONT8x8 (default) or FONT5x7. 
}

void loop() {

   // Uses the built-in scrollText() function
   scrollText();     

   // An alternate method using the dispChar() function
   altScrollText();
}

/* 
 * The scrollChar function works by scrolling a single character onto 
 * the screen. The spaces between characters is already accounted for.
 */
void scrollText() {
   lm.setFont(FONT8x8);
   // Loop through each character in the text[] array
   for (int i = 0; i < sizeof(text)-1; i++) {

      /* 
       * For each character, call the scrollChar() function.
       * scrollChar(char, row) has 2 arguements:
       *  - character to scroll
       *  - 8x8 dot matrix display row to scroll it on (first row = 0)
       */
      lm.scrollChar(text[i], 0);
   }

   // Second example using line 2 with 5x7 font
   lm.setFont(FONT5x7);
   for (int i = 0; i < sizeof(text)-1; i++) {
      lm.scrollChar(text[i], 1);
   }
}

/*
 * An alternate method of scrolling text using the dispChar() function. 
 * Essentially this method works by writing the full string of text to 
 * the display starting with it just off the right side of the screen. 
 * It then redraws each character one column to the left until the entire 
 * string has been displayed. 
 * 
 * This method is much slower, but allows you to scroll text at any Y position.
*/
void altScrollText(){

   /* 
    Start drawing at the first x position off the display.
    Stop drawing at the size of text string multiplied by the font width,
    but in the negative.
   */
   lm.setFont(FONT8x8);
   for (int x = NUM_COLS * 8; x >= -sizeof(text)*8; x--) {
      
      lm.clear();       // Clear the led array of any previous data

      for (int i = 0; i < sizeof(text)-1; i++) {   // Now draw each character in the string
         
         /* 
          drawChar() takes 3 parameters:
           - Character to draw
           - X position (left)
           - Y position (top)
          The y position will always stay the same (unless you want to try something funky)
          The x position of each character is determined by the starting x 
          position from the outer for loop plus the font width multiplied 
          by its location in the text array. 
         */
         lm.drawChar(text[i], x + (FONT8x8 * i), 0);
      }
      lm.update();      // update() must be called to redraw the display
      delay(2);         // This sets the scroll speed
   }

   /*
    Example using the 5x7 font.
    The 8x8 font already has a space between characters built-in; 
    however, the 5x7 does not. If using the 5x7 font, you need to add 
    one to the font width.
   */
   lm.setFont(FONT5x7);
   for (int x = NUM_COLS * 8; x >= -sizeof(text)*(FONT5x7 + 1); x--) {  
      lm.clear();
      for (int i = 0; i < sizeof(text)-1; i++) {
         lm.drawChar(text[i], x + ((FONT5x7 + 1) * i), 8);
      }
      lm.update();
      delay(2);
   }
}


