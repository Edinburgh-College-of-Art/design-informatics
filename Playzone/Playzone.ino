/*
 * Playzone is a glass musical instrument and lighting installation. It uses an Arduino 
 * Mega 2560 that is wired with 5 buttons on digital pins, 8 pressure sensors on analog
 * pins, MIDI output and drives a NeoPixel.
 * 
 * The 5 buttons of 'keys' are handled by the KeyClass. The 8 pressure sensors are 
 * arranged in pairs and are handled by the RockerClass.
 * 
 * Created by Mark Kobine for Marzena Ostromecka 2018
 * 
 */

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// The notes played by each of the 5 keys
#define NOTE_60 60
#define NOTE_63 63
#define NOTE_66 66
#define NOTE_70 70
#define NOTE_72 72

// Rocker notes
#define NOTE_58 58
#define NOTE_59 59
#define NOTE_55 55
#define NOTE_56 56
#define NOTE_62 62
#define NOTE_65 65
#define NOTE_67 67
#define NOTE_68 68

// Digital pins used by the keys
#define PIN_60 2
#define PIN_63 3
#define PIN_66 4
#define PIN_70 5
#define PIN_72 6

// Analog pins used by the rockers
#define PIN_58 A0
#define PIN_59 A1
#define PIN_55 A2
#define PIN_56 A3
#define PIN_62 A4
#define PIN_65 A5
#define PIN_67 A6
#define PIN_68 A7

#ifdef MIDI_LIB
#include <MIDI.h>
#endif

// Which pin on the Arduino is connected to the NeoPixels?
#define PIN            7

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      358

#ifdef MIDI_LIB
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

static const unsigned ledPin = 13;      // LED pin on Arduino Uno

const int piecesPerColour = 5 * 2;

int notePlayed = 0;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

typedef struct RgbColorX
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} RgbColor;

RgbColor rgbOff = {0, 0 , 0};

typedef struct HsvColorX
{
    unsigned char h;
    unsigned char s;
    unsigned char v;
} HsvColor;

//
void setup() 
{
  pinMode(ledPin, OUTPUT);

#ifdef MIDI_LIB
  // MIDI
  MIDI.begin(4);                      // Launch MIDI and listen to channel 4
#else
  // Set MIDI baud rate:
  Serial.begin(31250);
#endif
  
  // Rocker pins
  pinMode(PIN_58, INPUT);
  pinMode(PIN_59, INPUT);
  pinMode(PIN_55, INPUT);
  pinMode(PIN_56, INPUT);
  pinMode(PIN_62, INPUT);
  pinMode(PIN_65, INPUT);
  pinMode(PIN_67, INPUT);
  pinMode(PIN_68, INPUT);

  // Key pins
  pinMode(PIN_60, INPUT_PULLUP);
  pinMode(PIN_63, INPUT_PULLUP);
  pinMode(PIN_66, INPUT_PULLUP);
  pinMode(PIN_70, INPUT_PULLUP);
  pinMode(PIN_72, INPUT_PULLUP);

  pixels.begin();

  mapGlassPieces();
  mapGlassRockers();
  
//  Serial.begin(9600);
}

//
int readRocker()
{
  int reading = 0;

  reading = analogRead(A0);
  if (reading <= 10)
    reading = -analogRead(A1);
  
  return reading;
}

#ifdef OLD
//
int readJoyStick()
{
    
  if (digitalRead(2) == LOW)
    if (digitalRead(5) == LOW)
      return 8;
      
  if (digitalRead(2) == LOW)
    if (digitalRead(3) == HIGH)
      return 1;
    else
      return 2;
      
  if (digitalRead(3) == LOW)
    if (digitalRead(4) == HIGH)
      return 3;
    else
      return 4;
    
  if (digitalRead(4) == LOW)
    if (digitalRead(5) == HIGH)
      return 5;
    else
      return 6;
    
  if (digitalRead(5) == LOW)
    if (digitalRead(2) == HIGH)
      return 7;
    else
      return 8;

  return 0;  
}
#endif

#ifndef MIDI_LIB
// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int pitch, int velocity) 
{
  Serial.write(0x90);
  Serial.write(pitch);
  Serial.write(velocity);
}

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOff(int pitch) 
{
  Serial.write(0x80);
  Serial.write(pitch);
  Serial.write(0);
}
#endif

//
void setLEDcolour(int startLEDs[], int numLEDs[], struct RgbColorX &rgb)
{
  for (int j = 0; j < piecesPerColour; j++)
  {
    if (numLEDs[j] > 0)
    {
      for(int i = startLEDs[j]; i < startLEDs[j]+numLEDs[j]; i++)
        pixels.setPixelColor(i % NUMPIXELS, pixels.Color(rgb.r, rgb.g, rgb.b));
    }
  }
  
  pixels.show();  
}

//
void loop() 
{
  // read the rocker
  int rock = readRocker();

  // convert the rocker value into a colour
  int mapped = map(rock+450, 0, 900, 0, 255);
  mapped = constrain(mapped, 0, 255);

//  Serial.print("rock ");
//  Serial.print(rock);
//  Serial.print(" mapped ");
//  Serial.println(mapped);

  struct HsvColorX hsv;
  hsv.h = mapped;
  hsv.s = 255;
  hsv.v = 255;

  struct RgbColorX rgb = HsvToRgb(hsv);
  
#ifdef OLD
  // read the joystick position
  int jPos = readJoyStick();
//  Serial.print("jPos ");
//  Serial.println(jPos);

  if (jPos == 0)
  {
    if (notePlayed)
    {
      // A note was playing but the joystick isn't being used so 
      // switch the pixels off and end the note
#ifdef MIDI_LIB
      MIDI.sendNoteOff(1, 0, 1);     // Stop the note
#else
      noteOff(notePlayed);
#endif
      digitalWrite(ledPin, LOW);
      notePlayed = 0;
  
      // Switch the pixels off
      setLEDcolour(0, NUMPIXELS, rgbOff);
    }
  }
  else
  {
    if (notePlayed && (notePlayed != jPos*8))
    {
      // The note being played has changed so turn off the old note and the LEDs
      // and start a new one
#ifdef MIDI_LIB
      MIDI.sendNoteOff(1, 0, 1);     // Stop the note
#else
      noteOff(notePlayed);
#endif
      digitalWrite(ledPin, LOW);
      notePlayed = 0;
      delay(100);                // Wait for a second

      // Switch the pixels off
      setLEDcolour(0, NUMPIXELS, rgbOff);

      // Now switch the pixels back on and start a new note
      // set the pixels in the quadrant chosen via the joystick
      int startPos = (jPos - 1) * 2;
      setLEDcolour(startPos, startPos + (NUMPIXELS / 4), rgb);
  
      // Starting a new note
      notePlayed = jPos * 8;
#ifdef MIDI_LIB
      MIDI.sendNoteOn(notePlayed, 127, 1);    // Send a Note (pitch X, velo 127 on channel 1)
#else
      noteOn(notePlayed, 127);
#endif
      delay(100);                // Wait for a second
    }
    else
    {
      // The joystick is being used
      // Set all pixels to off
      setLEDcolour(0, NUMPIXELS, rgbOff);
  
      // set the pixels in the quadrant chosen via the joystick
      int startPos = (jPos - 1) * 2;
      setLEDcolour(startPos, startPos + (NUMPIXELS / 4), rgb);
  
      digitalWrite(ledPin, HIGH);
  
      // Now handle the MIDI
      if (!notePlayed)
      {
        // Starting a new note
        notePlayed = jPos * 8;
#ifdef MIDI_LIB
        MIDI.sendNoteOn(notePlayed, 127, 1);    // Send a Note (pitch X, velo 127 on channel 1)
#else
        noteOn(notePlayed, 127);
#endif
        delay(100);                // Wait for a second
      }
    }
  }
#else // OLD
  // NEW CODE
  updateKeys();
  updateRcokers();
#endif // OLD

  delay(20);
}
