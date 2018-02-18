#include <Wire.h>
#include <Ultrasonic.h>
#include <stdlib.h>

int motor_l1, motor_l2, input_l, motor_r1, motor_r2, input_r;
Ultrasonic ultrasonic(10, 11); // Trig, Echo

double distance; // last measured distance

double const AUTO_STOP_DISTANCE = 20;

void setup() {
  Wire.begin(8);                /* join i2c bus with address 8 */
  Wire.onReceive(receiveEvent); /* register receive event */
  Wire.onRequest(requestEvent); /* register request event */
  Serial.begin(9600);           /* start serial for debug */

  // setup running gear
  setup_motor_system(3, 4, 13, 7, 8, 2); /* (L1, L2, iL, R1, R2, iR) */
  set_speed(255, 255);

  _stop();
}

void loop() {
  delay(100);
  distance = get_average_distance_ultrasonic();
  if ( distance <= AUTO_STOP_DISTANCE && distance != 0 ) {
    _stop();
    Serial.print("Stop at: ");
  }
  Serial.println(distance);
}

// function that executes whenever data is requested from master
void requestEvent() {
  // sconvert double to char array
  char charBuf[6];
  // (float, min string width, precision, char)
  dtostrf(distance, 3, 2, charBuf);
  Wire.write(charBuf); /*send string on request */
}

// function that executes whenever data is received from master
void receiveEvent() {
  while (Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);           /* print the character */
    switch (c){
      case 'f':
        forward();
        break;
      case 'b':
        backward();
        break;
      case 'r':
        right();
        break;
      case 'l':
        left();
        break;
      case 's':
        _stop();
        break;  
    }
    Serial.println();             /* to newline */
  }
}

void setup_motor_system(int L1, int L2, int iL, int R1, int R2, int iR){
  motor_l1 = L1;
  motor_l2 = L2;
  input_l = iL;
  motor_r1 = R1;
  motor_r2 = R2;
  input_r = iR;
  pinMode(motor_l1, OUTPUT);
  pinMode(motor_l2, OUTPUT);
  pinMode(input_l, OUTPUT);
  pinMode(motor_r1, OUTPUT);
  pinMode(motor_r2, OUTPUT);
  pinMode(input_r, OUTPUT);
 }

void set_speed(int leftSpeed, int rightSpeed){
  analogWrite(input_l, leftSpeed);
  analogWrite(input_r, rightSpeed);
}

void _stop(){
  digitalWrite(motor_l1, LOW);
  digitalWrite(motor_l2, LOW);
  digitalWrite(motor_r1, LOW);
  digitalWrite(motor_r2, LOW);
}

void forward(){
  digitalWrite(motor_l1, HIGH);
  digitalWrite(motor_l2, LOW);
  digitalWrite(motor_r1, LOW);
  digitalWrite(motor_r2, HIGH);
}

void backward(){
  digitalWrite(motor_l1, LOW);
  digitalWrite(motor_l2, HIGH);
  digitalWrite(motor_r1, HIGH);
  digitalWrite(motor_r2, LOW);
}

void right(){
  digitalWrite(motor_l1, HIGH);
  digitalWrite(motor_l2, LOW);
  digitalWrite(motor_r1, HIGH);
  digitalWrite(motor_r2, LOW);
}

void left(){
  digitalWrite(motor_l1, LOW);
  digitalWrite(motor_l2, HIGH);
  digitalWrite(motor_r1, LOW);
  digitalWrite(motor_r2, HIGH);
}

double get_average_distance_ultrasonic(){
  double AMOUNT_OF_MEASURES = 1;
  double distance_comulative = 0;
  for (int i=0; i <= AMOUNT_OF_MEASURES - 1; i++){
      distance_comulative = distance_comulative + ultrasonic.distanceRead();
   }
  double distance = distance_comulative / AMOUNT_OF_MEASURES;
  return distance;
}


