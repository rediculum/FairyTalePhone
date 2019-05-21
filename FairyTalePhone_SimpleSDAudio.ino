/*
=============================================================================================
Fairy Tale Phone 1.1
by ReDiculum (Oct. - Dez. 2013)
-------------------------------
X-Mas 2013 Arduino project.
Using an old Zellweger Modell 70 (anno 1982) telephone with dial wheel.
The wheel can be spinned to count impulses and then playing one of ten different
audio fairies stored as 8bit full-rate (62500) mono PCM files named from 1.PCM to 10.PCM

SD Card > Arduino pinout:

CS (1)   -> Pin 10 (Chip select)
MOSI (2) -> Pin 11
MISO (7) -> Pin 12
SCK  (5) -> Pin 13

Ear phone (tone) -> Pin 3 and GND
Ear phone (sound) -> Pin 9 and GND
Dial wheel -> Pin 2 and GND


Changelog v1.1:
- Share the same pin for sound and tone by implementing deInit()
- Power sleep after fairy tale finished or error to save battery in case kids forget to hang up
- Power sleep after 10s if wheel hasn't been dialed to save battery (hook off by accident)
=============================================================================================

SimpleSDAudio library for audio playback from SD card
http://www.hackerspace-ffm.de/wiki/index.php?title=SimpleSDAudio
*/
#include <SimpleSDAudio.h>
#include <avr/sleep.h>

#define IMPULSE_PIN 2 // Digital Pin 2. Read impulses from dial wheel
//#define PHONE_PIN 3 // Digital Pin 3 (PWM) for phone tone output
#define PHONE_PIN 9 // Share phone tone and SDAudio pin. Always deInit before using tone()
#define SD_CHIPSELECT_PIN 10  // Digital Pin 10. SD Card chip select.
#define LED_PIN 13 // Digital Pin 13
byte countImpulse = 0; // The magic number to "dial" the music track number.

/* Sum of running millis and TIME_WAIT. 
Must be long integer if kids are not dialing quickly :)
*/
unsigned long timeWaitEnd, timeWait; 

void setup() {
  Serial.begin(57600); // Set console to 57600 baud
  Serial.println("=== FairyTalePhone based on SSDA ===");  

  pinMode(LED_PIN, OUTPUT); // onboard LED

  /* Turn on internal pullup resistor, because we pull the pin to ground.
  When the wheel gives impulse, the contact is opened (no current). That's
  when we have a digitalRead = 1.
  */
  digitalWrite(IMPULSE_PIN, HIGH);

  Serial.println("Ready to dial ...");

  // Play tone and wait here until the dial wheel has sent his first impulse 
  tone(PHONE_PIN,420);
  timeWait = millis()+ 10000; // Summarize already passed millis since Arduino runs and 10s
  while (digitalRead(IMPULSE_PIN) == 0) {
    if (millis() > timeWait) {
      hangOff();
      sleepNow();
    }
    delay(1); 
  }
  noTone(PHONE_PIN); // Pssht! Here we go!

  Serial.print("Counting impulses ");
  timeWaitEnd = millis() + 2000; // Summarize already passed millis since Arduino runs and 2s.

  // Count all the dialed impulses until timeWaitEnd has reached
  while (millis() < timeWaitEnd ) {      
    if (digitalRead(IMPULSE_PIN) == 1) {
      /* Increment the amount of impulses if the contacts are opened.
      The wheel pulses with an interval of approx 70ms
      */
      countImpulse++; 
      Serial.print(".");
      digitalWrite(LED_PIN, HIGH); // Blink LED ...
      tone(PHONE_PIN,1000); // ... and short tone (70ms) of 1khz to fill the gap
      delay(70); 
      digitalWrite(LED_PIN,LOW); 
      noTone(PHONE_PIN); 
    }
  }

  Serial.println(" "); // New line

  /* If the dial wheel has been turned more than once by nervous kids
  and the amount of counted impulses exceeds 10, we give up
  */
  if (countImpulse > 10) {
    Serial.print("Count exceeded 10 ("); Serial.print(countImpulse); Serial.println("), bye"); 
    hangOff();
    sleepNow();
  } else {
    Serial.print("Preparing fairy tale number "); Serial.println(countImpulse);
    

    // Set Chip Select Pin
    SdPlay.setSDCSPin(SD_CHIPSELECT_PIN);
    // Initialize SD Card with library SimpleSDAudio
    if (!SdPlay.init(SSDA_MODE_FULLRATE | SSDA_MODE_MONO | SSDA_MODE_AUTOWORKER)) {
      tone(PHONE_PIN,100); delay(250); noTone(PHONE_PIN); delay(400);
      tone(PHONE_PIN,80); delay(600); noTone(PHONE_PIN);
      Serial.println("Error initializing SD Card");
      sleepNow();
    }

    /* define a char array with an index size of 6 and convert
    integer "countImpulse" to char "track" using utoa() function
    http://www.nongnu.org/avr-libc/user-manual/group__avr__stdlib.html 
    */
    char track[6]; 
    utoa(countImpulse,track,10);
    // Append the suffix .PCM to the char array
    strcat(track, ".PCM");
    
    // Set the audio file if it exists and play it
    if (SdPlay.setFile(track)) {
      Serial.print("Playing..."); Serial.println(track);
      SdPlay.play();
      while(!SdPlay.isStopped()) {
        ;
      }
      SdPlay.deInit();
      hangOff();
      sleepNow();
      // That's it, we go sleeping
    } else {
      SdPlay.deInit();
      Serial.print("File "); Serial.print(track); Serial.println(" not found");
      tone(PHONE_PIN,80); delay(250); noTone(PHONE_PIN); delay(400);
      tone(PHONE_PIN,120); delay(600); noTone(PHONE_PIN);
      sleepNow();
    }
  }
}

void loop(void) { 
    ; // Nothing to do here
}

void hangOff(){
  // Hang off tone 5x
  delay(1000); 
  tone(PHONE_PIN,450); delay(250);
  noTone(PHONE_PIN); delay(250);
  tone(PHONE_PIN,450); delay(250);
  noTone(PHONE_PIN); delay(250);
  tone(PHONE_PIN,450); delay(250);
  noTone(PHONE_PIN); delay(250);
  tone(PHONE_PIN,450); delay(250);
  noTone(PHONE_PIN); delay(250);
  tone(PHONE_PIN,450); delay(250);
  noTone(PHONE_PIN); delay(250);
  delay(1000);
}

void sleepNow() {
  delay(1000);
  // Power sleep mode from AVR http://playground.arduino.cc/Learning/ArduinoSleepCode
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}
