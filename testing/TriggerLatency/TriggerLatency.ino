void setup() {  
  triggerMode(2,OUTPUT); // Use Trigger 2 as output
  enableInterruptTrigger(1,RISING_EDGE, gotTrigger); // Run gotTrigger on Trigger 1 rising edge
}

void gotTrigger(void) {  
  triggerWrite(2,HIGH); // Set Trigger 2 to go high so when know when function begins to run
  delayNanoseconds(100); // Function runs too fast to see Trigger 2 pulse on O-scope otherwise  
  triggerWrite(2,LOW); // Set Trigger 2 go to low when function completes
}

void loop() {
  static unsigned long lastrun;
  static signed long total;

  if (millis() > lastrun + 500) { //Run once every 500ms            
    toggleLEDGreen(); //toggle green LED;            
    lastrun = lastrun + 500;
  }

  total += 324*(total-2343); //do some math in main loop

  char dat;
  while (Serial.available() > 0) {
    dat = Serial.read(); //Read USB data if available
    Serial.print(dat); //Echo data back over USB
  }  
}