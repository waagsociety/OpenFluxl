/* 
OpenFluxl Arduino Code

PID Controller based on:
http://reibot.org/2011/08/07/intro-to-boost-converter/

Check wiring:
- Feedback connected to Analog 1.
- Pwm pin is on digital 11

*/

#include <PID_v1.h>

// Version
String FluxlVersion = "v.0.1";

// PID variables
double Setpoint, Input, Output;

// Initiate PID
PID myPID(&Input, &Output, &Setpoint, .8, .1, .01, DIRECT);

int targetVoltage = 100;

String buffer;

void setup(){
  
  Serial.begin(115200);
  
  pwmSetup(); // set pwm to 31 KHz
  
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0,100);
  
  // PID update speed in ms
  myPID.SetSampleTime(10); 
  
  Serial.print("OpenFluxl ");
  Serial.println(FluxlVersion);
}

void loop(){

  PidUpdate();

  // Serial Commands
  while (Serial.available()>0) {
    char c = (char)Serial.read();
    if (c == '\n') {
      if(buffer.startsWith("id")) {
        Serial.print("id:OpenFluxl "); // device identifier
        Serial.print(FluxlVersion); 
      } else if (buffer.startsWith("vol")) {
        targetVoltage = buffer.substring(3).toInt();
        
        // Sanity check
        if(targetVoltage < 0) targetVoltage = 0;
        if(targetVoltage > 300) targetVoltage = 300;
        
        // Report through serial
        Serial.print(F("Setting target voltage to "));
        Serial.print(targetVoltage);
        Serial.println(F(" V"));
      }
      else {
        Serial.println("Unknown cmd.");
      }
      buffer="";
    } else buffer+=c;
  }  
  
  delay(10);
}

void PidUpdate() {

  // Voltage devider to measure voltage
  
  double R2 = 200000;
  double R1 = 10000000;
  
  double voltage = (analogRead(A1) / 1023.)*4.9*(R2+R1)/R2;
  
  Input = voltage;
  Setpoint = targetVoltage;
  myPID.Compute();

  if(voltage > targetVoltage + 10) { 
    OCR2A = 0;
  } else if(voltage <= targetVoltage) {
    OCR2A = Output;
  }
  
  Serial.print("Voltage: ");
  Serial.print(voltage);
  
  Serial.print("\tADC voltage: ");
  Serial.print((analogRead(A1)/1023.)*4.9);
  
  Serial.print("\tduty: ");
  Serial.println((int)OCR2A);
  
}

void pwmSetup(){

  pinMode(3, OUTPUT); // OCR2B 
  pinMode(11, OUTPUT); // OCR2A
  
  TCCR2A = _BV(COM2A1) | _BV(COM2B1) | _BV(WGM20); // phase correct pwm to 31250hz
  
  TCCR2B = _BV(CS20); // change pwm frequency
  
  OCR2A = 0;
  OCR2B = 0;
  
}


