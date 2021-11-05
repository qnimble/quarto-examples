#define dataSize 1000000
EXTMEM int16_t CH1[dataSize]; //store data in external memory because it is 32MB, and regular memory is 512KB
EXTMEM int16_t CH2[dataSize]; //store data in external memory because it is 32MB, and regular memory is 512KB
IntervalTimer updateTimer;

void setup() {
  triggerMode(1,OUTPUT); //set trigger 1 as output

  //preCalculate analog output values when calculation too slow to do in real-time.
  for(uint i = 0;i<dataSize;i++) {
    CH1[i] = 18000 * sin(2*PI*i/100) + 11000 * cos(2*PI*i/4600+ .034) * sin(2*PI*i/200 + .134);
    CH2[i] = 12000 * cos(2*PI*i/124) + 17000* cos(2*PI*i/1000+ .45) * sin(2*PI*i/430 + .64);
  }

  updateTimer.begin(ddsUpdate, 5); // update every 5 microseconds
}

void ddsUpdate() {    
  static uint count = 0;
  writeDACRAW(1,CH1[count]);
  writeDACRAW(2,CH2[count]);
  count++;

  if (count >= dataSize) { //prevent overflow, reset to start at end of buffer
    count = 0; 
  }
}

void loop() {
  static unsigned long lastrun;    
  if (millis() > lastrun ) { //Run once every 500ms        
      lastrun = millis() + 500;
      toggleLEDGreen(); //toggle green LED;
  }      
}



