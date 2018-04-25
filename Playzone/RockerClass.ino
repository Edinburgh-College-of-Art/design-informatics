class rocker
{
  private:
    const int pin1;               // Digital pin on the Arduino
    const int pin2;               // Digital pin on the Arduino
    const int note1;              // Note played by this key
    const int note2;              // Note played by this key
    const int minPressure = 100;
    const int maxPressure = 800;
    int notePlaying;              // Are we currently playing this note
    int channel;                  // Always channel 1 (= 0) just now
    int velocity;                 // Volume. Always max just now (= 127)
    unsigned long noteStart;      // Time the note started
    int startLEDs[piecesPerColour];
    int numLEDs[piecesPerColour];
    uint32_t colour;              // LED colour
    
  public:
    rocker(int p1, int p2, int n1, int n2) : pin1(p1), pin2(p2), note1(n1), note2(n2)
    {
      noteStart = 0;
      notePlaying = 0;
      channel = 1;
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
      // Read data pins
      int pressure1 = analogRead(pin1);
      int pressure2 = analogRead(pin2);
      bool keyPressed = (pressure1 > minPressure) || (pressure2 > minPressure);
      
      if (!keyPressed)
      {
        // Neither side of the rocker pressed
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
      
          // Turn on the pixels for this glass colour
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
            if (pressure1 > minPressure)
            {
              velocity = map(pressure1, 0, maxPressure, 0, 127);
              velocity = constrain(velocity, 0, 127);
              playNote(note1);
            }
            else
            {
              velocity = map(pressure2, 0, maxPressure, 0, 127);
              velocity = constrain(velocity, 0, 127);
              playNote(note2);
            }
            delay(100);                // Wait for a bit
          }
        }
        else
        {
          // The rocker is being pressed and there's a note already
          // playing so let's pitch bend!
          if (((pressure1 > minPressure) && notePlaying == note1) || 
              ((pressure2 > minPressure) && notePlaying == note2))
          {
            // Pitch bend
            uint16_t bend;
  
            if (pressure1 > minPressure)
              bend = map(pressure1, 0, maxPressure, 0, 0x3fff);
            else
              bend = map(pressure2, 0, maxPressure, 0, 0x3fff);
  
            bend = constrain(bend, 0, 0x3fff);
            
            pitchBend(bend);
          }
          else
          {
            // MK what's hapening here? A new note has started. Will this ever happen?? 
            // MK problem - it may be different note playing now! i.e. the other half of the
            // rocker. In that case we need to cancel the existing note and thn start a new one
            // Is this a problem. Not sure now??
          }
        }
        
        if (notePlaying && (millis() > noteStart + sustainDuration))
        {
          // Start vibration
          vibrate();
        }
      }
    }

    //
    void pitchBend(uint16_t bend)
    {
      // Pitch bend value is only 14 bits
      bend = bend & 0x3fff;
      uint8_t LSB = bend & 0x7f;
      uint8_t MSB = ((bend & 0x3f80) >> 7) & 0x7f;
      Serial.write(0xe0 | channel);
      Serial.write(LSB);
      Serial.write(MSB);
    }
    
    // Start playting our note
    void playNote(int n)
    {
      notePlaying = n;
      noteStart = millis();
      Serial.write(0x90 | channel);
      Serial.write(n);
      Serial.write(velocity);
    }

    // Stop playing our note
    void endNote()
    {
      noteStart = 0;
      Serial.write(0x80 | channel);
      Serial.write(notePlaying);
      Serial.write(0);
      notePlaying = 0;
    }

    void vibrate()
    {
      // MK don't believe this can be done in MIDI and needs
      // to be done by the synth
    }
};

// Instantiate the objects that handle the glass 'rockers'
rocker rocker5859(PIN_58, PIN_59, NOTE_58, NOTE_59);
rocker rocker5556(PIN_55, PIN_56, NOTE_55, NOTE_56);
rocker rocker6265(PIN_62, PIN_65, NOTE_62, NOTE_65);
rocker rocker6768(PIN_67, PIN_68, NOTE_67, NOTE_68);

// Map the LED pixels to the glass pieces for each rocker
void mapGlassRockers()
{
  rocker5859.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
  rocker5556.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
  rocker6265.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
  rocker6768.setPiecesMap(20, 7, 59, 7, 245, 8, 232, 4, 261, 4, 289, 3, 304, 3, 331, 2, 341, 2);
}

// Update each rocker instance
void updateRcokers()
{
  rocker5859.update();
  rocker5556.update();
  rocker6265.update();
  rocker6768.update();
}

