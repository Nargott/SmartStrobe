#include <SoftwareSerial.h>

#define STROBE_PIN 13

#define LIGHT_MS 25

enum StrobeStates { S_BLINK, S_OFF, S_ON };

SoftwareSerial btSerial(10, 11); // RX, TX

StrobeStates strobe_state;
static unsigned long start_millis = 0;
int strobe_blink_state = 0;
int strobe_delay = 0;
byte bpm = 1;

char buffer[5];

void setup()
{
  strobe_state = S_OFF;
  pinMode(STROBE_PIN, OUTPUT);
  pinMode(3,OUTPUT);

  Serial.begin(9600);
  btSerial.begin(9600);
}

void loop()
{ 

if ( (btSerial.available() > 1) && (btSerial.readBytes(buffer, 2) > 1) )
  {
    Serial.print(buffer[0]);
    Serial.print(buffer[1]);
    switch (buffer[0])
    {
      case 0x00:
        strobe_state = (buffer[1] == 0x01) ? S_ON : S_OFF; 
        btSerial.println("Led On Cool");
        break;
        
      case 0x01: 
        strobe_state = S_BLINK;
        bpm = buffer[1]; if (bpm <= 0) bpm = 1;
        strobe_delay = (60000/bpm - LIGHT_MS);
        btSerial.println("Led Blink ");
        Serial.print(bpm);
        break;
      case 0x02: 
        strobe_state = S_BLINK;
        bpm = buffer[1]; if (bpm <= 0) bpm = 1;
        strobe_delay = bpm;
        btSerial.println("Led Blink ");
        Serial.print(bpm);
        break;  
      
      default:
      {
        for (int i = 0; i < 10; ++i)
        {
          
          digitalWrite(STROBE_PIN, HIGH);
          delay(50);
          digitalWrite(STROBE_PIN, LOW);
          delay(50);
        }
        Serial.println("Fail ....");
      }
    }
  }
  
  switch (strobe_state)
  {
    case S_ON: digitalWrite(STROBE_PIN, HIGH); break;
    case S_OFF: digitalWrite(STROBE_PIN, LOW); break;
    
    case S_BLINK:
    {
      if (strobe_blink_state == 0) {
        start_millis = millis();
        strobe_blink_state = 1;
      }
      
      if ( (strobe_blink_state == 1) && (millis() - start_millis >= strobe_delay) )
      {
        start_millis = millis();
        
        digitalWrite(STROBE_PIN, HIGH);
        strobe_blink_state = 2;
      } else if ( (strobe_blink_state == 2) && (millis() - start_millis >= LIGHT_MS) ) {
        start_millis = millis();
        
        digitalWrite(STROBE_PIN, LOW);
        strobe_blink_state = 1;
      }
    }
  }
}
