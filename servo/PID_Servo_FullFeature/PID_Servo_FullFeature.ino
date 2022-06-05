#include "qCommand.h"
qCommand qC; //use qCommand to change varibles over serial port

IntervalTimer blink; // use timer to cause LED to blink

/* Fixed Constants */
const uint8_t sampleRate = 1; //sample every 1us
const double toHz = 2*PI*sampleRate/(1e6);

/* Adjustable global variables */
bool servoActive; //toggle active Servo 
bool intHold; //toggle integrator hold
double setpoint = 0.0; // Target value for ADC to read
double propGain = 1.975; // proportional gain
double diffFreq = 1e9; //frequency where D feedback crosses unity gain (infinity is off)
double diffFreqMax = 1e9; //frequency where D feedback is clamped
double intFreq = 0; // frequency where integrator crosses unity gain

/* internal globals */
double diffClamp = 0; //used to set filtering on diff

void setup(void) { 
  configureADC(1,sampleRate,0,BIPOLAR_1250mV,getADC1); // Have ADC take measurement every sampleRate μs, ±1.25V range
  triggerMode(1, INPUT); // Set trigger1 as input
  intHold = triggerRead(1); //set integrator hold based initially  on trigger 1 level
  enableInterruptTrigger(1,BOTH_EDGES,&IntHold_en); //Run IntHold_en function on rising and falling edge change of trigger 1 to turn on/off integrator hold
  blink.begin(toggleLEDGreen,500e3); //run toggleLEDGreen one every 500e3 microseconds, or twice a second

  /* Configure commands */
  qC.setDefaultHandler(UnknownCommand);
  qC.addCommand("Help",&help);  
  qC.assignVariable("Servo",&servoActive);
  qC.assignVariable("Setpoint", &setpoint);
  qC.assignVariable("PGain", &propGain);    
  qC.assignVariable("IntFreq", &intFreq);
  qC.assignVariable("DiffFreq", &diffFreq);
  qC.assignVariable("PGain", &propGain);    
  qC.addCommand("DiffMaxFreq", adjDiffGainMax);
}

void IntHold_en(void) {
  intHold = triggerRead(1);  
}

void getADC1(void) {
  static double prev_adc = 0;
  static double integral = 0;
  double newadc = readADC1_from_ISR(); //read ADC voltage
  static double diffAve = 0;
  if (servoActive) {
    if (intHold == false) { //skip updates if integrator hold enabled
      double prop = (newadc - setpoint) * propGain; //proportional
      integral += (newadc - setpoint) * intFreq * toHz; // integral gain, rescaled into frequency units      
      
      diffAve = (diffClamp) * (newadc-prev_adc) + (1-diffClamp) * diffAve; //filter differential to clamp diff gain
      double diff = diffAve / ( diffFreq * toHz ); // diff gain, rescaled into frequency units
      double newdac = prop + integral + diff;
      writeDAC(1,-newdac); //invert for negative feedback  
    } 
    prev_adc = newadc; //store new adc value for differential calculation
  } else {
      writeDAC(1,0.0); //Set output to zero when servo is off
      integral = 0; //Zero output integrator when servo off
  }
}

void adjDiffGainMax(qCommand& qC, Stream& S) {  
  if ( qC.next() != NULL) {
    diffFreqMax = atof(qC.current()); //rescale diff gain max to freq where diff should cut out    
    if (diffFreqMax < diffFreq ) { //keep frequency higher than differential frequency
      diffFreqMax = diffFreq; //keep frequency filter positive
      diffClamp = 0; //if diffFreqMax = diffFreq, gain is 1 and differential clamps as it starts, so no update and alpha for filter is zero
    } else if (diffFreqMax > 250e3/sampleRate) { // IIR not valid beyond sampleRate/4
      diffFreqMax = 250e3/sampleRate;
      diffClamp = 1; //no filtering
    } else {
      double y = tan(PI * diffFreqMax * sampleRate * 1e-6);
      diffClamp = 2 * y / (y+1);
    }
  }
  if (diffFreq == 0) {
    S.printf("Differential is off, no diff max\n");
  } else {
    S.printf("The diff gain flattens at freq = %f kHz (gain max is %f)\n",diffFreqMax/1e3,diffFreqMax/diffFreq);
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