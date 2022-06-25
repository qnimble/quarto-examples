#include "qCommand.h"
qCommand qC; //use qCommand to change varibles over serial port

IntervalTimer updateTimer;
IntervalTimer blink; // use timer to cause LED to blink

/* Fixed Constants */
const uint8_t updateRate = 5; //sample every 5us

/* Adjustable global variables */
double frequency = 10e3; //default 10kHz
double phaseShift = 90; //defualt 90 deg out of phase
double amplitude = 3;

void setup() {
  triggerMode(1,OUTPUT); //set trigger 1 as output
  updateTimer.begin(ddsUpdate, updateRate); // update every 5 microseconds
  blink.begin(toggleLEDGreen,500e3); //run toggleLEDGreen one every 500e3 microseconds, or twice a second
  qC.setDefaultHandler(UnknownCommand);
  qC.addCommand("Help",&help);  
  qC.assignVariable("Freq",&frequency);
  qC.assignVariable("Phase",&phaseShift);
  qC.assignVariable("Amp",&amplitude);  
}

void ddsUpdate() {    
  static double phase = 0;
  phase += 2*PI * frequency * updateRate / 1e6; // Increment phase by 2*pi*freq/updateRate
  triggerWrite(1,phase > PI); //Set trigger HIGH if phase 180 - 360, low is 0 - 180deg.
  
  writeDAC(1,abs(amplitude*sin(phase))); //Output absolute value of sin wave
  writeDAC(2,amplitude * sq((sin(phase+phaseShift*PI/180.0)))); //output square of sine wave with phase shift

  if (phase >= 2*PI) { //keep phase between 0 and 2*PI
    phase -= 2*PI; 
  }
}

void loop(void) {
  qC.readSerial(Serial);
  qC.readSerial(Serial2);
}

void help(qCommand& qC, Stream& S) {
  S.println("Available commands are:");
  qC.printAvailableCommands(S);
}

void UnknownCommand(const char* command, qCommand& qC, Stream& S) {
  S.printf("Unknown command ('%s'). Please type 'help' for a list of commands\n",command);  
}
