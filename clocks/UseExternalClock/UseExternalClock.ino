/*
  This example will let you use an external 10 MHz reference, input on Trigger 1, 
  to drive all the clocks on the Quarto. To enable, type 'ext on' into the serial
  port. If an external clock is not input, or is not close to 10 MHz, the Quarto
  will use an internal reference instead. The 'ext' command will show the state
  of the clock. Additionally, the green LED will flash when using an external 
  reference clock but will flash red when using the internal reference.

  Please note that only Quarto's running firmware 1.30.X or higher support this functionality.
  If you are running earlier firmware and want to update, please contact qNimble at
  https://qnimble.com/Contact

*/


#include "qCommand.h"
qCommand qC;
IntervalTimer OutputClock;

void setup() {
  OutputClock.begin(doToggle,1); //run doToggle every 1us (1 MHz)

  if (getFirmwareMinorRev() >= 30) {
    qC.addCommand("Ext",&extCommand);
  } else {
    qC.addCommand("Ext",&notSupported);
  }
  triggerMode(2,OUTPUT); //set Trigger 2 as output for 1 MHz clock
}

void loop() {
  qC.readSerial(Serial);
  qC.readSerial(Serial2);
  static unsigned long lastrun = 0;
  static bool toggle = false;
  if (millis() > lastrun) { //Run once every 200ms
    lastrun = millis() + 200;
    toggle = !toggle;
    if (toggle) {
      setLED(false,false,false);
    } else {
      if (readExtClockActive()) {
        setLED(false,true,false);
      } else {
        setLED(true,false,false);
      }
    }
  }
}

void doToggle(void) {
  triggerToggle(2);
}

void notSupported(qCommand& qC, Stream& S) {
  S.printf("Firmware Revision: %i.%i.%i does not support using external clock\n", getFirmwareMajorRev(),getFirmwareMinorRev(),getFirmwarePatchRev());
  S.println("If you want to upgrade the Quarto's firmware to support this feature, please contact qNimble");
}

void extCommand(qCommand& qC, Stream& S) {
  if ( qC.next() != NULL) {
    bool enable = qC.str2Bool(qC.current());
    useExtClock(enable,1); //use trigger 1 as external clock
  }
  bool status = readExtClockEnabled();
  if (status) {
    bool status = readExtClockActive();
    if (status) {
      S.print("External Clock Enabled and Active\n");
    } else {
      S.print("External Clock Enabled but not Active\n");
    }
  } else {
    S.print("External Clock Disabled\n");
  }
}
