
/* CPSC 599
 * Assignment 2
 * Winter 2020
 *  
 *  References: 
 *  ADXL - SparkFun https://learn.sparkfun.com/tutorials/adxl345-hookup-guide/all
 *  Midi note fire - Mike Cook https://forum.arduino.cc/index.php?topic=253053.10;wap2
 *  ADXL 345 - Dejan https://howtomechatronics.com
*/

#include <SparkFun_ADXL345.h>
#include <Adafruit_NeoPixel.h>

//////////// Definitions ////////////

#define midiChannel (byte)0
#define offsetX   0 
#define offsetY   0
#define offsetZ   0
#define gainX     1 
#define gainY     1
#define gainZ     1
#define LED_PIN   6
#define LED_COUNT 60
const int base_note = 36;
const int note_1 = 36;
const int note_2 = 38;
const int note_3 = 40;
const int note_4 = 41;
const int note_5 = 43;
const int note_6 = 45;
const int note_7 = 47;

//////////// Variables ////////////

ADXL345 adxl = ADXL345();


// Axis Values //
int accX = 0;
int accY = 0;
int accZ = 0;

// Notes to play //

int note_y = note_3;
int note_z = note_5;

// Starting octave //

int octave = 0;

//////////// setup ////////////

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
   //LED Setup
   strip.begin();
   strip.show();
   strip.setBrightness(50);

   // Set Baud speed to match Hairless
   Serial.begin(9600);
   // Setup ADXL accelerometer
   adxl.powerOn();                   
   adxl.setRangeSetting(2);          
   adxl.setSpiBit(0);                

}

//////////// loop ////////////

void loop() {

  //// Read ADXL ////

  // Get the Accelerometer Readings
  int x,y,z;                        
  adxl.readAccel(&x, &y, &z);      
     
  accX = (x - offsetX)/gainX;       
  accY = (y - offsetY)/gainY;
  accZ = (z - offsetZ)/gainZ;
  
  //Convert x-axis values to octave value
  //octave = (int)map(accX,-265,265,0,12);
  //octave = (accX % 4);
   octave = ((int)accX)%4;

   switch(octave){
    case 0:
      colorWipe(strip.Color(69, 12, 54), 5);
      break;
    case 1:
      colorWipe(strip.Color(72, 24, 115), 5);
      break;
    case 2:
      colorWipe(strip.Color(25, 15, 128), 5);
      break;
    case 3:
      colorWipe(strip.Color(115, 16, 49), 5);
      break;
   }

  //Convert y- and z-axis values to notes//
  
  note_y = getNote(accY, octave);
  note_z = getNote(accZ, octave + 1);

  // Send notes to MIDI output //

    noteSend(0x90, note_y, 127);
    noteSend(0x90, note_z, 127);
    delay(1000);

  // Stop notes sent //
    noteSend(0x80, note_y, 127);
    noteSend(0x80, note_z, 127);
    delay(3000);
    

   
}

//////////// Methods ////////////

/*  Returns the appropriate musical note 
 *  based on axis value and scaling
 *  axis: axis value in float
 *  oct: an integer value on how many octaves to scale
*/
int getNote(float axis, int oct){

  // map axis data to values 1 ~ 7 //

  int value = map((int)(axis),-265,265,1,7);
  int note = note_1;
  
  // assign notes based on value //
  
  switch(value){
    case 1:
      note = note_1;
      //colorWipe(strip.Color(44, 158, 219), 15);
      break;
    case 2:
      note = note_2;
      //colorWipe(strip.Color(44, 108, 219), 15);
      break;
    case 3:
      note = note_3;
      //colorWipe(strip.Color(44, 58, 219), 15);
      break;
    case 4:
      note = note_4;
      //colorWipe(strip.Color(44, 8, 219), 15);
      break;
    case 5:
      note = note_5;
      //colorWipe(strip.Color(44, 208, 219), 15);
      break;
  }
  
  //scale up or down octaves;
  return note + 12*oct;
  
}

/*   Play MIDI note
 *   cmd: 0x90 for playing a note and 0x80 for stopping a note
 *   data1: note to be played
 *   data2: velocity of note
*/
 void noteSend(char cmd, char data1, char data2) {
  cmd = cmd | char(midiChannel);  // merge channel number
  Serial.write(cmd);
  Serial.write(data1);
  Serial.write(data2);
  
}

void testNote(){
  
  // Send notes to MIDI output //
  
    noteSend(0x90, note_z, 127);
    noteSend(0x90, note_y, 127);
    delay(1000);

  // Stop notes sent //
    noteSend(0x80, note_z, 127);
    noteSend(0x80, note_y, 127);
    delay(3000);
}

void colorWipe(uint32_t color, int wait){
  for (int i = 0; i<strip.numPixels(); i++){
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}
