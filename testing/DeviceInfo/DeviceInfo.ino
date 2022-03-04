
IntervalTimer checkSerialReady;

void setup() {
  checkSerialReady.begin(checkSerial,100); //check every 100us for serial being reay
}

void checkSerial() {
  if (Serial) {
    displayInfo();
    checkSerialReady.end(); //Serial ready, so stop checking
  }
}

void displayInfo(void) {
  Serial.println("Quarto Device Information");
  Serial.printf("SN: %i\n",getSerialNumber());
  Serial.printf("Device ID: qN-%i.%i.%i\n",getHardwareID(),getHardwareMajorRev(),getHardwareMinorRev());
  Serial.printf("Firmware Revision: %i.%i.%i\n", getFirmwareMajorRev(),getFirmwareMinorRev(),getFirmwarePatchRev());  
}