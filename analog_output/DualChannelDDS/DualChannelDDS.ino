IntervalTimer updateTimer;

void setup() {
  triggerMode(1,OUTPUT); //set trigger 1 as output
  updateTimer.begin(ddsUpdate, 5); // update every 5 microseconds
}

void ddsUpdate() {    
  static float phase = 0;
  phase += 2*PI / 1000; // Increment phase by .36deg or 1/1000 of a period
  triggerWrite(1,phase > PI); //Set trigger HIGH if phase 180 - 360, low is 0 - 180deg.
  
  writeDAC(1,abs(3.4*sin(phase))); //Output absolute value of 200Hz sin wave
  writeDAC(2,abs(3.4*cos(phase))); //Output absolute value of 200Hz cos wave

  if (phase >= 2*PI) { //keep phase between 0 and 2*PI
    phase -= 2*PI; 
  }
}

void loop() {
  static unsigned long lastrun;    
  if (millis() > lastrun + 500) { //Run once every 500ms
      toggleLEDGreen(); //toggle green LED;
  }      
}



