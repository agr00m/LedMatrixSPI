/*
 * ScrollText
 * 
 * Author:  Aaron Groom
 * Date:    08-07-2015
 * 
 * Demonstrates the drawing functions of the LedMatrix library.
 * 
 * The basic drawing functions are:
 *    setLed(x, y, val) - 0/false = off, 1/true = on.
 *    line(x1, y1, x2, y2);
 *    triangle(x1, y1, x2, y2, x3, y3);
 *    quad(x1, y1, x2, y2, x3, y3, x4, y4);
 *    rect(x, y, width, height);
 *    circle(x, y, radius);
 *    ellipse(x, y, width, height);
 * 
 * Pins used:
 *    Data:    Digital pin 12
 *    Clock:   Digital pin 11
 *    CS/Load: Digital pin 10
 */

// Comment out if using the non-SPI version of the library
#define SPIWrite

// Include the LedMatrix library
#ifdef SPIWrite
#include <LedMatrixSPI.h>
#else
#include <LedMatrix.h>
#endif

// Here you setup the size of your LED Matrix. 
const byte NUM_ROWS = 6;               // Number of 8x8 display rows
const byte NUM_COLS = 8;               // Number of 8x8 display columns

const byte maxWidth = NUM_COLS * 8;    // Maximum display width
const byte maxHeight = NUM_ROWS * 8;   // Maximum display height
const double rads = 0.0174532925;      // Used for clock demo

/*
 Create an LedMarix object
 The first parameter is the number of 8x8 dot matrix
 rows, the second is the number of columns. 
*/
#ifdef SPIWrite
LedMatrixSPI lm = LedMatrixSPI(NUM_ROWS, NUM_COLS);
#else
LedMatrix lm = LedMatrix(NUM_ROWS, NUM_COLS);
#endif

void setup() {
   // No setup required for LedMatrix library
   randomSeed(analogRead(0));    // Intialize random number generator
}

void loop() {
   drawQuad();
   drawLines();
   drawEllipse();
   bouncingBall();
}

/*************************************************************
 * Draws several random lines.
 *************************************************************/
void drawLines() {
   
   int numLines = 6;             // Number of lines to draw
   
   for (int i = 0; i < numLines; i++) {
      lm.line(random(0,maxWidth), random(0, maxHeight), random(0,maxWidth), random(0, maxHeight));
      lm.update();
      delay(1000);
   }
   lm.clearScreen();
}

/*************************************************************
 * Draws a diamond using the quad() function.
 *************************************************************/
void drawQuad() {
   int center = (maxHeight / 2) - 1;
   for (int i = 0; i < 4; i++) {
      lm.quad(0, center, center, 0, center*2, center, center, center*2);
      lm.update();
      delay(1000);
   }
}

/*************************************************************
 * Draws varying shaped ellipses, centered on the display, 
 * from 1 width to max height to max width and 1 height.
 * NOTE: Ellipse can be bigger than display will allow.
 *************************************************************/
void drawEllipse() {
   
   int height = maxHeight-1, width;       // Ellipse height and width
   
   for (width = 1; width < maxWidth-1; width++) {
      lm.clear();
      lm.ellipse(maxWidth/2, maxHeight/2, width, height);
      lm.update();
      delay(20);
   }
   
   for (height = maxHeight-1; height > 0; height--) {
      lm.clear();
      lm.ellipse(maxWidth/2, maxHeight/2, width, height);
      lm.update();
      delay(20);
   }
   lm.clearScreen();
}

/*************************************************************
 * Draws a circle that bounces around on the screen.
 *************************************************************/
void bouncingBall() {

   int counter = 0;           // Counts the # of times the ball bounces
   int radius = 4;            // Set radius of circle
   
   // Start in center of screen
   int xPos = maxWidth / 2;     
   int yPos = maxHeight / 2;
   
   // Randomly generate a direction
   int xDir = random(-5, 5);
   int yDir = random(-5, 5);

   // Bounce 80 times
   while(counter < 80) {
      lm.clear();
      if (yPos + radius >= maxHeight-1 || yPos - radius <= 0) {
         yDir *= -1;             
      }
      if (xPos + radius >= maxWidth-1 || xPos - radius <= 0) {
         xDir *= -1;             
      }
      xPos += xDir;
      yPos += yDir;
      lm.circle(xPos, yPos, radius);
      lm.update();
      delay(90);
      counter++;
   }
   lm.clearScreen();
}
