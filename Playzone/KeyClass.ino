const int sustainDuration = 1000;

class key 
{
  private:
    const int pin;                // Digital pin on the Arduino
    const int note;               // Note played by this key
    bool notePlaying;             // Are we currently playing this note
    int channel;                  // Always channel 1 (= 0) just now
    int velocity;                 // Volume. Always max just now (= 127)
    unsigned long noteStart;      // Time the note started
//    const int startLED;           // Start num of LED
//    const int numLEDs;            // No. of LEDs for this key
    int startLEDs[piecesPerColour];
    int numLEDs[piecesPerColour];
    uint32_t colour;              // LED colour
    
  public:
    key(int p, int n, int c) : pin(p), note(n), channel(c)
    {
      noteStart = 0;
      notePlaying = false;
      velocity = 127;
//      colour = (0x000000ff << 16) | (0x00000032 << 8) | 0x00000032;
      colour = 0x00ffffff;

      // Zero out the map of LED numbers to coloured glass pieces
      for (int i = 0; i < piecesPerColour; i++)
      {
        startLEDs[i] = 0;
        numLEDs[i] = 0;
      }
    }

    // Map the LEDs to glass pieces
    void setPiecesMap(int a, int an, int b=0, int bn=0, int c=0, int cn=0,
                      int d=0, int dn=0, int e=0, int en=0, int f=0, int fn=0,
                      int g=0, int gn=0, int h=0, int hn=0, int i=0, int in=0,
                      int j=0, int jn=0)
    {
      startLEDs[0] = a;
      numLEDs[0] = an;
      startLEDs[1] = b;
      numLEDs[1] = bn;
      startLEDs[2] = c;
      numLEDs[2] = cn;
      startLEDs[3] = d;
      numLEDs[3] = dn;
      startLEDs[4] = e;
      numLEDs[4] = en;
      startLEDs[5] = f;
      numLEDs[5] = fn;
      startLEDs[6] = g;
      numLEDs[6] = gn;
      startLEDs[7] = h;
      numLEDs[7] = hn;
      startLEDs[8] = i;
      numLEDs[8] = in;
      startLEDs[9] = j;
      numLEDs[9] = jn;
    }
    
    // Check to see if this key has been pressed
    void update()
    {
      // Read data pin
      bool keyPressed = (digitalRead(pin) == LOW);
      
      if (!keyPressed)
      {
        // Not pressed
        if (notePlaying)
        {
          // Turn off note
          endNote();
  
          // Switch the pixels off
          setLEDcolour(startLEDs, numLEDs, rgbOff);
        }
      }
      else
      {
        // Starting a new note?
        if (!notePlaying)
        {
          // Set all pixels to off
          setLEDcolour(startLEDs, numLEDs, rgbOff);
      
          // Set the pixels in the quadrant chosen via the joystick
          // MK fix this later -
          RgbColorX rgb;
//          rgb.r = (colour & 0x00ff0000) >> 16;
//          rgb.g = (colour & 0x0000ff00) >> 8;
//          rgb.b =  colour & 0x000000ff;
          rgb.r = 255;
          rgb.g = 255;
          rgb.b = 255;
          setLEDcolour(startLEDs, numLEDs, rgb);
      
          // Now handle the MIDI
          if (!notePlaying)
          {
            // Starting a new note
            playNote();
            delay(100);                // Wait for a bit
          }
        }
        
        if (notePlaying && (millis() > noteStart + sustainDuration))
        {
          // Start vibration
          vibrate();
        }
      }
    }

    // Start playting our note
    void playNote()
    {
      notePlaying = true;
      noteStart = millis();
      Serial.write(0x90 | channel);
      Serial.write(note);
      Serial.write(velocity);
    }

    // Stop playing our note
    void endNote()
    {
      notePlaying = false;
      noteStart = 0;
      Serial.write(0x80 | channel);
      Serial.write(note);
      Serial.write(0);
    }

    void vibrate()
    {
      // MK don't believe this can be done in MIDI and needs
      // to be done by the synth
    }
};

// Instantiate the objects that handle the glass 'keys'
key key60(PIN_60, NOTE_60, 0);
key key63(PIN_63, NOTE_63, 1);
key key66(PIN_66, NOTE_66, 2);
key key70(PIN_70, NOTE_70, 3);
key key72(PIN_72, NOTE_72, 4);

// Map the LED pixels to the glass pieces for each key
void mapGlassPieces()
{
  key60.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
  key63.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
  key66.setPiecesMap(114, 8, 101, 5, 130, 5, 86, 5, 145, 5, 286, 2, 308, 2, 317, 4, 353, 4);
  key70.setPiecesMap(114, 8, 101, 5, 130, 5, 86, 5, 145, 5, 286, 2, 308, 2, 317, 4, 353, 4);
  key72.setPiecesMap(11, 5, 27, 5, 53, 6, 70, 5, 178, 7, 185, 7, 284, 2, 310, 2);
}

// Update each key instance
void updateKeys()
{
  key60.update();
  key63.update();
  key66.update();
  key70.update();
  key72.update();
}

