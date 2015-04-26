/* 
OpenFluxl Arduino Code

PID Controller based on:
http://reibot.org/2011/08/07/intro-to-boost-converter/

Check wiring:
- Feedback connected to Analog 1.
- Pwm pin is on digital 3

*/

#include <PID_v1.h>

// Version
String FluxlVersion = "v.0.1";

// PID variables
double Setpoint, Input, Output;

// Initiate PID
PID myPID(&Input, &Output, &Setpoint, .8, .1, .01, DIRECT);

// Vars
int targetVoltage = 100;
int numFluxls = 6;
String buffer;

// Pins
int DriverPin = 3;
int FluxlPins[] = {7,8,9,10,11,12};

void setup(){
  
  // Open Serial Connection
  Serial.begin(115200);

  // Set Pin Modes
  for(i=0;i<numFluxls;i++) {
    pinMode(FluxlPins[i],OUTPUT);
  }
  pinMode(DriverPin, OUTPUT); // Mosfet Driver (OCR2A)
  
  // Change PWM frequency
  pwmSetup(); // set pwm to 31 KHz
  
  // Set PID
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0,255);
  
  // PID update speed in ms
  myPID.SetSampleTime(10); 
  
  // Print message
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
        Serial.println(FluxlVersion); 
      } else if (buffer.startsWith("vol")) {
        // Change target voltage
        targetVoltage = buffer.substring(3).toInt();
        
        // Sanity check
        if(targetVoltage < 0) targetVoltage = 0;
        else if(targetVoltage > 300) targetVoltage = 300;
        else targetVoltage = 0;
        
        // Report through serial
        Serial.print(F("Setting target voltage to: "));
        Serial.print(targetVoltage);
        Serial.println(F(" V"));
      } else if (buffer.startsWith("fluxl")) {
        // move droplet to new fluxl
        int tmpInt = buffer.substring(3).toInt();
        
        // Sanity check
        if(tmpInt < 0) tmpInt = 0;
        else if(tmpInt > 5) tmpInt = 5;
        else tmpInt = 0; 
        
        moveDrop(tmpInt);
      }
      else {
        Serial.println(F("Unknown cmd."));
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
    analogWrite(DriverPin,0); //OCR2A = 0;
  } else if(voltage <= targetVoltage) {
    analogWrite(DriverPin,Output); //OCR2A = Output;
  }
  
  Serial.print("Voltage: ");
  Serial.print(voltage);
  
  Serial.print("\tADC voltage: ");
  Serial.print((analogRead(A1)/1023.)*4.9);
  
  Serial.print("\tduty: ");
  Serial.println((int)Output);
  
}

void moveDrop(int TargetFluxl) {
  // Open the circuit through the target Fluxl
  for(i=0;i<numFluxls;i++) {
    if(i==TargetFluxl) {
      digitalWrite(FluxlPins[i],HIGH);
      
      // Report through serial
      Serial.print(F("Moving droplet to Fluxl: "));
      Serial.println(i);      
    }
    else {
      digitalWrite(FluxlPins[i],LOW);
    }
  }
}

void pwmSetup(){
  
/* Derived from: http://letsmakerobots.com/content/changing-pwm-frequencies-arduino-controllers */
//---------------------------------------------- Set PWM frequency for D5 & D6 -------------------------------
//TCCR0B = TCCR0B & B11111000 | B00000001;    // set timer 0 divisor to     1 for PWM frequency of 62500.00 Hz
//TCCR0B = TCCR0B & B11111000 | B00000010;    // set timer 0 divisor to     8 for PWM frequency of  7812.50 Hz
TCCR0B = TCCR0B & B11111000 | B00000011;    // set timer 0 divisor to    64 for PWM frequency of   976.56 Hz
//TCCR0B = TCCR0B & B11111000 | B00000100;    // set timer 0 divisor to   256 for PWM frequency of   244.14 Hz
//TCCR0B = TCCR0B & B11111000 | B00000101;    // set timer 0 divisor to  1024 for PWM frequency of    61.04 Hz

//---------------------------------------------- Set PWM frequency for D9 & D10 ------------------------------
//TCCR1B = TCCR1B & B11111000 | B00000001;    // set timer 1 divisor to     1 for PWM frequency of 31372.55 Hz
//TCCR1B = TCCR1B & B11111000 | B00000010;    // set timer 1 divisor to     8 for PWM frequency of  3921.16 Hz
TCCR1B = TCCR1B & B11111000 | B00000011;    // set timer 1 divisor to    64 for PWM frequency of   490.20 Hz
//TCCR1B = TCCR1B & B11111000 | B00000100;    // set timer 1 divisor to   256 for PWM frequency of   122.55 Hz
//TCCR1B = TCCR1B & B11111000 | B00000101;    // set timer 1 divisor to  1024 for PWM frequency of    30.64 Hz

//---------------------------------------------- Set PWM frequency for D3 & D11 ------------------------------
TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000010;    // set timer 2 divisor to     8 for PWM frequency of  3921.16 Hz
//TCCR2B = TCCR2B & B11111000 | B00000011;    // set timer 2 divisor to    32 for PWM frequency of   980.39 Hz
//TCCR2B = TCCR2B & B11111000 | B00000100;    // set timer 2 divisor to    64 for PWM frequency of   490.20 Hz
//TCCR2B = TCCR2B & B11111000 | B00000101;    // set timer 2 divisor to   128 for PWM frequency of   245.10 Hz
//TCCR2B = TCCR2B & B11111000 | B00000110;    // set timer 2 divisor to   256 for PWM frequency of   122.55 Hz
//TCCR2B = TCCR2B & B11111000 | B00000111;    // set timer 2 divisor to  1024 for PWM frequency of    30.64 Hz

  analogWrite(11,0);
  analogWrite(DriverPin,0);
  
}
