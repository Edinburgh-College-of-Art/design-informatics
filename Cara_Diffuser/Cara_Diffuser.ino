/*
 * Arduino Uno sketch to drive a hacked diffuser. The hack breaks out the button
 * on the front of the device to trigger the diffuser from a remote, secondary button.
 */

const int buttonPin = 2;             // Secondary button
const int mosfetPin = 13;            // Uused to trigger the MOSFET
const int onDuration = 15000;        // The diffuser runs for 15 seconds
const int buttonPressDelay = 500;

int mosfetState = LOW;               // the current state of the MOSFET
int buttonState;                     // the current reading from button
int lastButtonState = HIGH;          // the previous reading button
bool diffuserOn = false;

unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time
unsigned long startTime = 0;

//
void setup() 
{ 
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(mosfetPin, OUTPUT);

  // set initial MOSFET state to off
  digitalWrite(mosfetPin, mosfetState);
}

//
void loop() 
{
  int reading = digitalRead(buttonPin);

  // check to see if you just pressed the button and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) 
  {
    // reset the debounce timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) 
  {
    // if the button state has changed:
    if (reading != buttonState) 
    {
      buttonState = reading;

      // Only trigger the MOSFET if the new button state is LOW
      if (buttonState == LOW) 
      {
        // Press the button once to turn the diffuser on
        digitalWrite(mosfetPin, HIGH);
        delay(buttonPressDelay);
        digitalWrite(mosfetPin, LOW);

        mosfetState = HIGH;
        diffuserOn = true;
        startTime = millis();
      }
    }
  }

  // Save reading for next time round the loop
  lastButtonState = reading;

  // Is it time to turn off the differ?
  if (diffuserOn && (startTime + onDuration < millis()))
  {
    // Diffuser has been on for onDuration milliseconds so it's time to turn it off
    startTime = 0;
    mosfetState = LOW;
    diffuserOn = false;

    // 'Press' button twice to turn the diffuser off
    digitalWrite(mosfetPin, HIGH);
    delay(buttonPressDelay);
    digitalWrite(mosfetPin, LOW);
    delay(buttonPressDelay);
    digitalWrite(mosfetPin, HIGH);
    delay(buttonPressDelay);
    digitalWrite(mosfetPin, LOW);
  }
}
