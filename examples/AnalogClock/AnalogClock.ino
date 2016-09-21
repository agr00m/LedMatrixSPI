// Comment out if using the non-SPI version of the library
#define SPIWrite

// Include the LedMatrix library
#ifdef SPIWrite
#include <LedMatrixSPI.h>
#else
#include <LedMatrix.h>
#endif

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
  // put your setup code here, to run once:

}

void loop() {

   int16_t sHand, mHand, hHand, center;
   uint8_t centerX = maxWidth / 2 - 1;
   uint8_t centerY = maxHeight / 2 - 1;
   
   // Set the start time
   int hour = 1;
   int minute = 4;
   int second = 40;
   
   // Set the length of each hand
   int shLen = 23;
   int mhLen = 16;
   int hhLen = 10;

   // Find the center
   if (maxWidth > maxHeight)
      center = (maxHeight / 2) - 1;
   else
      center = (maxWidth / 2) - 1;

   // Run the clock for some amount of time
   while(1) {

      // Calculate the degree each had is pointed
      sHand = round((360 * second / 60) - 90);
      mHand = round((360 * minute / 60) - 90);
      hHand = round((360 * hour   / 12) - 90);

      lm.clear(); // Clear the LED array, but not the screen
      lm.circle(centerX, centerY, center);  // Draw the clock

      // Draw the 3 hands
      lm.line(centerX, centerY, centerX + round(shLen * cos(sHand * rads)), centerY + round(shLen * sin(sHand * rads)));
      lm.line(centerX, centerY, centerX + round(mhLen * cos(mHand * rads)), centerY + round(mhLen * sin(mHand * rads)));
      lm.line(centerX, centerY, centerX + round(hhLen * cos(hHand * rads)), centerY + round(hhLen * sin(hHand * rads)));
      
      lm.update();   // Update the display
      delay(975);    // Delay almost a second (leave time for program overhead)
      second++;      // Increment the seconds

      // Reset seconds and increment minutes
      if (second > 59) {
         second = 0;
         minute++;
      }
      // Reset minutes and increment hours
      if (minute > 59) {
         minute = 0;
         hour++;
      }
      // Reset hours
      if (hour > 11) {
         hour = 0;
      }
   }
}
