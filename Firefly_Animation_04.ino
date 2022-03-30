
/* Firefly animation
 *  9/3/18-9/1/19 Carl F Sutter
 *  WS2811 Strand connected to Arduino Pin 6
 *  Uses the Adafruit Neopixel Library
 */
 
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Neopixel constants
#define PIN 6        // Arduino pin for the LED strand data line
#define NUMLEDS 50  // number of LEDs total

// App constants
// The RGB values for the LED color 0-255
#define COLOR_R 50 
#define COLOR_G 100 
#define COLOR_B 0

// loop control - these are the main constants to control the animation
// note that the math is in integers right now, so if the step size is greater than the color values, it will not work well
#define NUM_STEPS_UP    5    // the number of steps to fade the color up
#define NUM_STEPS_DOWN  30   // the number of steps to fade the color down
#define LOOP_DELAY      10   // the ms delay for each loop
#define BLINK_ODDS      5    // the odds in 10,000 that the loop will start each black pixel

// internal constants and vars - no need to change
#define STATE_BLACK          0
#define STATE_MOVING_UP      1
#define STATE_MOVING_DOWN    2

uint8_t state[NUMLEDS]; 
uint8_t color_r[NUMLEDS]; 
uint8_t color_g[NUMLEDS]; 
uint8_t color_b[NUMLEDS]; 

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMLEDS, PIN, NEO_RGB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


void setup() {
  // set all the LEDS to the no blink (black) state
  for(int i=0; i<NUMLEDS; i++) {
    state[i] = STATE_BLACK;
    color_r[i] = 0;
    color_g[i] = 0;
    color_b[i] = 0;
  }
  
  // randomize the random number generator
  randomSeed(analogRead(0));  

  // setup the LED strand
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
} // setup


void loop() {
  // step through the LEDS, and update the colors as needed
  for (int i=0; i<NUMLEDS; i++) {
    switch (state[i]) {
      case STATE_BLACK:
        strip.setPixelColor(i,  0x000000);
        if (random(10000) < BLINK_ODDS) state[i] = STATE_MOVING_UP;
        break;

      case STATE_MOVING_UP:
         color_r[i] = min(COLOR_R, color_r[i] + (COLOR_R / NUM_STEPS_UP));
         color_g[i] = min(COLOR_G, color_g[i] + (COLOR_G / NUM_STEPS_UP));
         color_b[i] = min(COLOR_B, color_b[i] + (COLOR_B / NUM_STEPS_UP));
         strip.setPixelColor(i, color_r[i], color_g[i], color_b[i]);
         if ((color_r[i] == COLOR_R) && (color_g[i] == COLOR_G) && (color_b[i] == COLOR_B)) {
           state[i] = STATE_MOVING_DOWN;
         }
        break;

      case STATE_MOVING_DOWN:
         color_r[i] = max(0, color_r[i] - max(1, (COLOR_R / NUM_STEPS_DOWN)));
         color_g[i] = max(0, color_g[i] - max(1, (COLOR_G / NUM_STEPS_DOWN-1)));
         color_b[i] = max(0, color_b[i] - max(1, (COLOR_B / NUM_STEPS_DOWN)));
         strip.setPixelColor(i, color_r[i], color_g[i], color_b[i]);
         if ((color_r[i] == 0) && (color_g[i] == 0) && (color_b[i] == 0)) {
           state[i] = STATE_BLACK;
         }
        break;
        
      } // switch
    } // led loop
    
    strip.show();
    delay(LOOP_DELAY);
 } // loop

 
