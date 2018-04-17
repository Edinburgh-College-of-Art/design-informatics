//
// Uses 3 x 100kOhm potentiometers to vary the red, green and blue values
//
//  The LCD circuit:
// * LCD RS pin to digital pin 12
// * LCD (E)nable pin to digital pin 11
// * LCD D4 pin to digital pin 5
// * LCD D5 pin to digital pin 4
// * LCD D6 pin to digital pin 3
// * LCD D7 pin to digital pin 2
// * LCD R/W pin to ground
// * LCD VSS pin to ground
// * LCD VCC pin to 5V
// * 10K resistor:
// * ends to +5V and ground
// * wiper to LCD VO pin (pin 3)
//
// MK - Add rotary encoder to wind through the rainbow
// MK - Replace potentiometers with rotary encoders
// MK - Add separate power supply for LEDs
//

#include <Adafruit_NeoPixel.h>
#include <LiquidCrystal.h>

#ifdef __AVR__
  #include <avr/power.h>
#endif

// LEDs
#define PIN 6
#define NUM_LEDS 8      // Can be more with an external power supply for the LEDs

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

// LCD
// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

int red = 0;
int green = 0;
int blue = 0;

//
void setup() 
{
  strip.begin();

  // Potentiometers - replace with 3 rotary encoders or R, G, B buttons + 1 rotary encoder
  pinMode(A0, INPUT); // RED
  pinMode(A1, INPUT); // GREEN
  pinMode(A2, INPUT); // BLUE
   
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  Serial.begin(9600);
  Serial.println("Ready");
}

//
void loop() 
{
  bool changed = false;

  // Read the current potentiometer values
  int newRed = analogRead(A0);
  int newGreen = analogRead(A1);
  int newBlue = analogRead(A2);

  newRed = map(newRed, 100, 800, 255, 0);
  newRed = constrain(newRed, 0, 255);
  newGreen = map(newGreen, 100, 800, 255, 0);
  newGreen = constrain(newGreen, 0, 255);
  newBlue = map(newBlue, 100, 800, 255, 0);
  newBlue = constrain(newBlue, 0, 255);

  // Only update the LEDs and LCD if the values have changed
  if (red != newRed || green != newGreen || blue != newBlue)
  {
    red = newRed;
    green = newGreen;
    blue = newBlue;

    // Print the RGB values to the serial monitor
    Serial.print("Red ");
    Serial.print(red);
    Serial.print(" (0x");
    Serial.print(red, HEX);
    Serial.print(") green ");
    Serial.print(green);
    Serial.print(" (0x");
    Serial.print(red, HEX);
    Serial.print(") blue ");
    Serial.print(blue);
    Serial.print(" (0x");
    Serial.print(blue, HEX);
    Serial.println(")");
  
    // Display the RGB values as decimal
    lcd.setCursor(0, 0);
    lcd.print("r");
    if (red < 100)
      lcd.print(" ");
    if (red < 10)
      lcd.print(" ");
    lcd.print(red);
    lcd.setCursor(5, 0);
    lcd.print("g");
    if (green < 100)
      lcd.print(" ");
    if (green < 10)
      lcd.print(" ");
    lcd.print(green);
    lcd.setCursor(10, 0);
    lcd.print("b");
    if (blue < 100)
      lcd.print(" ");
    if (blue < 10)
      lcd.print(" ");
    lcd.print(blue);
  
    // Now display the values as hex
    lcd.setCursor(0, 1);
    lcd.print("rx");
    if (red < 0x10)
      lcd.print("0");
    lcd.print(red, HEX);
    lcd.setCursor(5, 1);
    lcd.print("gx");
    if (green < 0x10)
      lcd.print("0");
    lcd.print(green, HEX);
    lcd.setCursor(10, 1);
    lcd.print("bx");
    if (blue < 0x10)
      lcd.print("0");
    lcd.print(blue, HEX);

    // Set the LEDs
    for(int i = 0; i < NUM_LEDS; i++)
      strip.setPixelColor(i, red, green, blue);
  
    strip.show();
  }

  // Pause a bit
  delay(250);
}
