IntervalTimer DAC_Timer; 
uint DACcounts = 0; //store how many DAC updates have been complete

void setup() {
  triggerMode(1, INPUT); // Set trigger1 as input  
  enableInterruptTrigger(1,RISING_EDGE,&start_pattern); //On rising edge, run start_pattern
}

void start_pattern() {  
  DACcounts = 0; //reset DACcounts
  DAC_Timer.begin(updateDAC, 1); //start timer to run every 1us and call updateDAC
}

void updateDAC() {
  float value = 2.5 * sin(DACcounts * 2 * PI/100 ) ; //generate sin wave at 10kHz
  writeDAC(1,value); //write value to DAC
  DACcounts++; // increment DACcounts by one.

  if (DACcounts == 1000) { // after 1000 data points (10 periods) stop
    DAC_Timer.end();
  }
}

void loop() {
  static uint lastrun1;
  if (millis() > lastrun1 + 500) {            
      lastrun1 = millis();
      toggleLEDGreen();
  }
}
