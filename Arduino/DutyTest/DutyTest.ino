/*
OpenFluxl Arduino Test Code
- Without Feedback or PID Control
*/

int DriverPin = 11;

void setup() {
  // put your setup code here, to run once:

  // Set PWM frequency for D3 & D11 
  TCCR2B = TCCR2B & B11111000 | B00000001;    // set timer 2 divisor to     1 for PWM frequency of 31372.55 Hz

  // Start with 0% duty cycle
  analogWrite(DriverPin,0);

}

void loop() {
  // put your main code here, to run repeatedly: 
  
  // 25% Duty Cycle: 64
  // 50% Duty Cycle: 127
  // 75% Duty Cycle: 191
  // 100% Duty Cycle: 255
  
  analogWrite(DriverPin,127);
  
}
