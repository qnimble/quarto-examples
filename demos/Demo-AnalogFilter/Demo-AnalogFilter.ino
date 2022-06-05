#include "qCommand.h"
qCommand qC;

const uint SampleRate = 2;

double filterA = 1e3; //default 1kHz
double alphaA = 0.012488; // alpha for default 1 kHz
double filterB = 10e3; //default 10kHz
double alphaB = 0.118381; // alpha for default 10 kHz

void setup(void) {
  configureADC(1,SampleRate,0,BIPOLAR_10V,getADC1); // Have ADC take measurement every 2us, ±10V range
  qC.setDefaultHandler(UnknownCommand);
  qC.addCommand("FilterA",&adjFilterA);
  qC.addCommand("FilterB",&adjFilterB);
  qC.addCommand("Help",&help);
}

void loop(void) {
  qC.readSerial(Serial);
  qC.readSerial(Serial2);

  static unsigned long lastrun = 0;    
  if (millis() > lastrun) { //Run once every 5000ms
    lastrun = millis() + 500;
    toggleLEDGreen();    
  }
}

void getADC1(void) {
  static double outputA = 0;
  static double outputB = 0;
  double newadc = readADC1_from_ISR(); //read ADC voltage
  
  outputA = alphaA * newadc + (1-alphaA) * outputA; //apply IIR filter on multiplied value
  outputB = alphaB * newadc + (1-alphaB) * outputB; //apply IIR filter on multiplied value
  writeDAC(1,outputA);
  writeDAC(2,outputB);
}

void adjFilterA(qCommand& qC, Stream& S) {
  adjFilter(&filterA,&alphaA,qC);
  S.printf("The filterA frequency is %f (and alpha=%f)\n",filterA,alphaA);
}

void adjFilterB(qCommand& qC, Stream& S) {
  adjFilter(&filterB,&alphaB,qC);
  S.printf("The filterB frequency is %f (and alpha=%f)\n",filterB,alphaB);
}


void adjFilter(double* filter, double* alpha, qCommand& qC) {
  if ( qC.next() != NULL) {
    double filterInput = atof(qC.current());
    if (filterInput < 0 ) {
      *filter = 0; //keep frequency filter positive
      *alpha = 0; //at filter = 0Hz, output cannot update, alpha is zero
    } else if (filterInput > 250e3/SampleRate) { // IIR no valid beyond sampleRate/4
      *filter = INFINITY;
      *alpha = 1; //no filtering
    } else {
      *filter = filterInput;
      double y = tan(PI * *filter * SampleRate * 1e-6);
      *alpha = 2 * y / (y+1);
    }
  }  
}


void help(qCommand& qC, Stream& S) {
  S.println("Available commands are:");
  qC.printAvailableCommands(S);
}

void UnknownCommand(const char* command, qCommand& qC, Stream& S) {
  S.printf("Unknown command ('%s'). Please type 'help' for a list of commands\n",command);  
}