#include "qCommand.h"
qCommand qC;

const uint SampleRate = 12;

double frequency = 10e3;
double phase = 0;
double amplitude = 5;
double filter = 10; //default 1kHz
double alpha = .000126; // alpha for default 1 kHz
double output = 0;

void setup(void) {
  configureADC(1,SampleRate,0,BIPOLAR_10V,getADC1); // Have ADC take measurement every 2us, ±10V range
  qC.assignVariable("Freq",&frequency);
  qC.assignVariable("Phase",&phase);
  qC.assignVariable("Amp",&amplitude);
  qC.assignVariable("Output",&output);
  qC.addCommand("Filter",&adjFilter);
  triggerMode(1,OUTPUT);
  triggerMode(2,OUTPUT);
}

void loop(void) {
  qC.readSerial(Serial);
  qC.readSerial(Serial2);
}

void getADC1(void) {
  //triggerWrite(1,1);
  static double nextSin = 0;
  static double nextSinWithPhase = 0;
  static double cycle = 0; //track phase/(2*pi) which is cycle fraction (one cycle=2pi).
  double newadc = readADC1_from_ISR(); //read ADC voltage

  writeDAC(1,amplitude*nextSin); //DAC output is sin(2*pi*cycle)
  double multiplied = newadc * nextSinWithPhase; //put phase in units if degrees
  writeDAC(2,multiplied);
  output = alpha * multiplied + (1-alpha) * output; //apply IIR filter on multiplied value

  //writeDAC(2,output);
  //writeDAC(2,newadc);
  //triggerWrite(1,0);
  //triggerWrite(2,1);


  cycle += frequency / (1e6/SampleRate); //1e6/SampleRate is sample freq (500kHz);  freq normed to that
  if (cycle > 1) cycle -= 1; //reset phase so no overflows
  nextSin = sin(2*PI*cycle);
  nextSinWithPhase = sin(2*PI*(cycle + phase/360));
  

  //triggerWrite(1,0);

}

void adjFilter(qCommand& qC, Stream& S) {
  if ( qC.next() != NULL) {
    double filterInput = atof(qC.current());
    if (filterInput < 0 ) {
      filter = 0; //keep frequency filter positive
      alpha = 0; //at filter = 0Hz, output cannot update, alpha is zero
    } else if (filterInput > 250e3/SampleRate) { // IIR no valid beyond sampleRate/4
      filter = INFINITY;
      alpha = 1; //no filtering
    } else {
      filter = filterInput;
      double y = tan(PI * filter * SampleRate * 1e-6);
      alpha = 2 * y / (y+1);
    }
  }
    S.printf("The filter frequency is %f (and alpha=%f)\n",filter,alpha);
}