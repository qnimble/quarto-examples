/*
  This example will let you use an external 10 MHz reference, input on Trigger 1, 
  to drive all the clockson the Quarto. To enable, type 'ext on' into the serial
  port. If an external clock is not input, or is not close to 10 MHz, the Quarto
  will use an internal reference instead. The 'ext' command will show the state
  of the clock. Additionally, the green LED will flash when using an external 
  reference clock but will flash red when using the internal reference.
*/


#include "qCommand.h"
qCommand qC;
IntervalTimer OutputClock;

void setup() {
  OutputClock.begin(doToggle,1); //run doToggle every 1us (1 MHz)
  qC.addCommand("Ext",&extCommand);
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

void extCommand(qCommand& qC, Stream& S) {
  if ( qC.next() != NULL) {
    bool enable = qC.str2Bool(qC.current());
    useExtClock(enable);
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
