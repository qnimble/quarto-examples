bool servoActive;

void setup(void) { 
  configureADC(1,1,0,BIPOLAR_1250mV,getADC1); // Have ADC take measurement every 1us, ±1.25V range
  triggerMode(1, INPUT); // Set trigger1 as input
  servoActive = triggerRead(1); //set servoActive variable initially based on trigger 1 level
  enableInterruptTrigger(1,BOTH_EDGES,&servo_en); //Run servo_en function on rising and falling edge change of trigger 1
}

void servo_en(void) {
  servoActive = triggerRead(1);  
}

void getADC1(void) {
  static double integral = 0;
  static double prev_adc = 0;
  double newadc = readADC1_from_ISR(); //read ADC voltage
  
  double setpoint = 0.25; // Target value for ADC to read

  if (servoActive) {
    double prop = (newadc - setpoint) * 1.975; //proportional
    integral += (newadc - setpoint) * 0.01; // integral gain
    double diff = ( newadc - prev_adc) * .00001; // turn diff down for accuracate BW measurement
    double newdac = prop + integral + diff;
    
    writeDAC(1,-newdac); //invert for negative feedback  
  }
  prev_adc = newadc; //store new adc value for differential calculation
}


void loop(void) {
   static unsigned long lastrun = 0;    
  
  if (millis() > lastrun) { //Run once every 1000ms
    lastrun = millis() + 1000;
    toggleLEDGreen();
    //Serial.println("This runs every second");
  }
}
