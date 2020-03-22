 /*
   This code is developed from code from the PulseSensor library.

   Copyright World Famous Electronics LLC - see LICENSE
   Contributors:
     Joel Murphy, https://pulsesensor.com
     Yury Gitman, https://pulsesensor.com
     Bradford Needham, @bneedhamia, https://bluepapertech.com

   Licensed under the MIT License, a copy of which
   should have been included with this software.

   This software is not intended for medical use.
*/


#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>

/*
   The heartbeat data can be viewed in the Serial Plotter which can be accessed under "Tools."
*/
const int OUTPUT_TYPE = SERIAL_PLOTTER;

const int PULSE_INPUT = A0; // Analog input pin connected to purple wire of pulse sensor
const int PULSE_BLINK = 13; // Pin 13 is the on-board LED that should blink when a heartbeat is detected
const int PULSE_FADE = 5; // Smooth fade with each pulse
const int THRESHOLD = 550; // Adjust this number to avoid noise when idle

/*
   All the PulseSensor Playground functions.
*/
PulseSensorPlayground pulseSensor;

void setup() {
  /*
     If you are running into errors with the Serial Monitor not working, make sure that you have the baud rate at 115200 on the Serial Monitor.
  */
  Serial.begin(115200);

  // Configure the PulseSensor manager.

  pulseSensor.analogInput(PULSE_INPUT);
  pulseSensor.blinkOnPulse(PULSE_BLINK);
  pulseSensor.fadeOnPulse(PULSE_FADE);

  pulseSensor.setSerial(Serial);
  pulseSensor.setOutputType(OUTPUT_TYPE);
  pulseSensor.setThreshold(THRESHOLD);

  // Now that everything is ready, start reading the PulseSensor signal.
  if (!pulseSensor.begin()) {
    /*
       PulseSensor initialization failed,
       likely because our particular Arduino platform interrupts
       aren't supported yet.

       If your Sketch hangs here, try PulseSensor_BPM_Alternative.ino,
       which doesn't use interrupts.
    */
    for(;;) {
      // Flash the led to show things didn't work.
      digitalWrite(PULSE_BLINK, LOW);
      delay(50);
      digitalWrite(PULSE_BLINK, HIGH);
      delay(50);
    }
  }
}

void loop() {
  /*
     Wait a bit.
     We don't output every sample, because our baud rate
     won't support that much I/O.
  */
  delay(50);

 // Serial.write(1);
 
  // write the latest sample to Serial.
  // pulseSensor.outputSample();

  /*
     If a beat has happened since we last checked,
     write the per-beat information to Serial.
   */ 
  if (pulseSensor.sawStartOfBeat()) {
   // pulseSensor.outputBeat();

   int bpm = pulseSensor.getBeatsPerMinute();
   int ibi = pulseSensor.getInterBeatIntervalMs()/6;
   

   // Since we have three data points being sent to Max, we need to label them. "a" means BPM. "b" means IBI. "c" means LS.
   Serial.write("a "); 
   Serial.write(bpm);
   Serial.print("bpm: ");
   Serial.println(bpm, DEC);
   Serial.write("b ");
   Serial.write(ibi );
   Serial.print("ibi: ");
   Serial.println(ibi, DEC);
  }


  // Even if there is no pulse happening, we still send the LS data. This allows us to recreate the heartbeat signal in Max.
   int ls = pulseSensor.getLatestSample()/5;
   Serial.write("c ");
   Serial.write(ls);
   Serial.print("ls: ");
   Serial.println(ls, DEC);
}
