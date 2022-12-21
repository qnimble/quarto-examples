/*
  This example will sample the ADC 'on-demand' from the Quarto. We will configure
  the ADC clock to be external on Trigger 1, but drive the trigger 1 output to 
  cause the ADC to sample the data. Trigger 1 can controlled with the triggerWrite
  command, but in this example we will instead use the trigger clock functions to
  have the trigger 1 output a clock signal at a fixed frequency and then turn this
  functionality on and off to toggle ADC sampling. By configuring the Quarto, via 
  the configureADC command, to fire every 1us, it will sample the ADC on every 
  trigger signal.

  Please note that only Quarto's running firmware 1.32.X or higher support this functionality.
  If you are running earlier firmware and want to update, please contact qNimble at
  https://qnimble.com/Contact

*/

bool supported = false;
void setup() {  
  triggerMode(2,OUTPUT); //set Trigger 2 as output for when ADC measurement occurs  
  
  if (getFirmwareMinorRev() < 32) {
    supported = false;
    Serial.printf("Firmware Revision: %i.%i.%i does not support using external clock\n", getFirmwareMajorRev(),getFirmwareMinorRev(),getFirmwarePatchRev());
    Serial.println("If you want to upgrade the Quarto's firmware to support this feature, please contact qNimble");
  } else {
    supported = true;
    useExtADCClock(true,1); //use trigger 1 as external clock
  }

  configureADC(1,1,0,BIPOLAR_2500mV,getADC1); /* Have ADC take measurement every 1us
                                                (When using a 1 MHz ADC clock)  */

  setTriggerClockFreq(1e6); //Set trigger output clock to be at 1 MHz
}

void loop(void) {  
  static unsigned long lastrun = 0;
  static bool toggle = false;
  if (millis() > lastrun) { //Run once every 500ms
    lastrun = millis() + 500;
    toggle = !toggle;
    if (toggle) {
      setLED(false,false,false);      
      useTriggerClockOutput(1,false); // disable Trigger Output Clock on Trig 1
    } else {
      if (supported) {
        setLED(false,true,false);
        useTriggerClockOutput(1,true); // enable Trigger Output Clock on Trig 1        
      } else {
        setLED(true,false,false);
        Serial.printf("Firmware Revision: %i.%i.%i does not support using external clock\n", getFirmwareMajorRev(),getFirmwareMinorRev(),getFirmwarePatchRev());
      }
    }
  }
}


void getADC1(void) {
  triggerWrite(2,HIGH); // Set Trigger 2 high as soon as new ADC data is ready
  double newadc = readADC1_from_ISR(); //read ADC voltage

  writeDAC(1,2.5*newadc); //Write read ADC value to DAC, but 2.5 times larger    
  triggerWrite(2,LOW); //Set Trigger 2 low when interrupt complete
}



