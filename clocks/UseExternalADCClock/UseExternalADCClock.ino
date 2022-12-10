/*
  This example will let you use an external clock, input on Trigger 1, as the ADC
  clock. On startup, the Quarto will use an internal 1 MHz clock as the ADC clock.
  By configuring the Quarto, via the configureADC command, to fire every 1us, it 
  will sample the ADC on every clock cycle. Thus when switching to an external ADC
  clock, the ADC will sample every time the external clock goes high. Type 
  "ADCExt On" to get Quarto to start using an external ADC clock from trigger 1.

  If the Quarto were configured to fire every 4us, then the ADC would sample on 
  every fourth clock pulse. If each ADC channel were configured at 4us, then the
  Quarto would sample an ADC channel on every rising edge of the external clock,
  rotating through the 4 different channels in order.

  Please note that only Quarto's running firmware 1.32.X or higher support this functionality.
  If you are running earlier firmware and want to update, please contact qNimble at
  https://qnimble.com/Contact

*/

#include "qCommand.h"
qCommand qC;


void setup() {  
  triggerMode(2,OUTPUT); //set Trigger 2 as output for when ADC measurement occurs  
  
  if (getFirmwareMinorRev() >= 32) {
    qC.addCommand("ADCExt",&adcextCommand);
  } else {
    qC.addCommand("ADCExt",&notSupported);
  }

  configureADC(1,1,0,BIPOLAR_2500mV,getADC1); /* Have ADC take measurement every 1us
                                                (When using a 1 MHz ADC clock)  */

}

void loop(void) {
  qC.readSerial(Serial);
  qC.readSerial(Serial2);
  
  static unsigned long lastrun = 0;    
  if (millis() > lastrun) { //Run once every 5000ms
    lastrun = millis() + 500;
    toggleLEDBlue();    
  }
}


void getADC1(void) {
  triggerWrite(2,HIGH); // Set Trigger 2 high as soon as new ADC data is ready
  double newadc = readADC1_from_ISR(); //read ADC voltage

  writeDAC(1,2.5*newadc); //Write read ADC value to DAC, but 2.5 times larger    
  triggerWrite(2,LOW); //Set Trigger 2 low when interrupt complete
}


void adcextCommand(qCommand& qC, Stream& S) {
  if ( qC.next() != NULL) {
    bool enable = qC.str2Bool(qC.current());
    useExtADCClock(enable, 1); //Set ADC Clock to external from trigger 1
  }
  bool status = readExtADCClockEnabled();
  if (status) {    
    S.print("External ADC Clock Enabled\n");
  } else {
    S.print("External ADC Clock Disabled\n");
  }
}


void notSupported(qCommand& qC, Stream& S) {
  S.printf("Firmware Revision: %i.%i.%i does not support using external clock\n", getFirmwareMajorRev(),getFirmwareMinorRev(),getFirmwarePatchRev());
  S.println("If you want to upgrade the Quarto's firmware to support this feature, please contact qNimble");
}

