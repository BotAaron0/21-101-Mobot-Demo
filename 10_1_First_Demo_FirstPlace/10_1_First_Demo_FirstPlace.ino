/*
  24-101: Introduction to Mechanical Engineering
  Course Instructor for Labs: Assoc. Prof. of the Practice Haidar 

  Starter Code for Counting Lines with Mobot
*/

#include <NewPing.h>
#include <Servo.h>

const int numLine = 7;
const int distVal = 14;
// Define all of the pins used.  These are NOT up to us, but rather what Elegoo decided.  Don't change.
// If your car doesn't function properly  with these pins, please double check your pin connections.
const int LEFT_MOTORS = 5; // pin that controls the speed of the 2 left side motors - PWM pin
const int RIGHT_MOTORS = 6; // pin that controls the speed of the 2 right side motors - PWM pin
const int LEFT_MOTOR_DIR = 8; // pin that controls the polarity, or direction, of the 2 left side motors (HIGH = forward, LOW = backward)
const int RIGHT_MOTOR_DIR = 7; // pin that controls the polarity, or direction, of the 2 right side motors (HIGH = forward, LOW = backward)
const int MOTOR_OUTPUT = 3; // controls whether the motors are allowed to run or not

const int ServoPin = 10;
const int TriggerPin = 13;
const int EchoPin = 12;

NewPing sonar (TriggerPin, EchoPin, 30);
Servo servo;

// IR Detector Pins
const int IR_RIGHT = A0;
const int IR_MIDDLE = A1;
const int IR_LEFT = A2;

unsigned long lastt = 0;
float tnot = 0;
float i_last = 0;
float i_max = 50;
float dx_last = 0;

// Threshold values for IR detection
// Feel free to change these values if you find a better range to work with, this can be achieved by writing a separate code that only prints: Left_IR_value, Middle_IR_value, Right_IR_value
// FYI room lighting may necessitate these number to change, since the sensors will collect a different baseline in bright sunlight compared to indoor lighting at night 
const int THRESHOLD_ABOVE_IS_BLACK = 450; //below this number will register as white
const int THRESHOLD_ABOVE_IS_NOISE = 850; //above this number is discarded as noise

// The number of lines your car should detect. This value should be set at the beginning (constant) and not be changed later in the code.
const int LINES_TO_COUNT = 2;

// TO-DO: Make a global counter to represent the number of counted lines (this integer is not constant)

int blacklineCount = 0;

// TO-DO: forward function -> sets MOTOR_OUTPUT to HIGH, sets both MOTOR_DIR to HIGH, and sets both MOTORS to an int speed 
// If the mobot drives super slow, then the speed value is being regsitered as HIGH istead of a number between LOW and HIGH
// If the mobot drives super fast, then the speed value is being regsitered as LOW istead of a number between LOW and HIGH
void forward(int speed){  
  // FYI speed was just defined here as an integer, so don't define it again
  // Inputs: An integer representing the speed of the motors, this is a variable now but in the voide loop you'll input a reasonable value 
  // Outputs: None
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTORS, speed);
  analogWrite(RIGHT_MOTORS, speed);
}

void backward(int speed){  
  // FYI speed was just defined here as an integer, so don't define it again
  // Inputs: An integer representing the speed of the motors, this is a variable now but in the voide loop you'll input a reasonable value 
  // Outputs: None
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, speed);
  analogWrite(RIGHT_MOTORS, speed);
}




// TO-DO: off function -> sets MOTOR_OUTPUT to LOW
void off (){
  digitalWrite(MOTOR_OUTPUT, LOW);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
}

void setup() {
  // put your setup code here, to run once:
  
  // TO-DO: At least 6 calls to pinMode to set up the appropriate INPUTS and OUTPUTS
  pinMode(LEFT_MOTORS, OUTPUT);
  pinMode(RIGHT_MOTORS, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_OUTPUT, OUTPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_MIDDLE, INPUT);
  pinMode(IR_LEFT, INPUT);

  servo.attach(ServoPin);
  servo.write(180);
  // The line below is important for debugging
  Serial.begin(9600);

  lastt = millis();
}



void loop() {
  // put your main code here, to run repeatedly from top to bottomn:
  // TO-DO: call function forward with an appropriate input value for speed, start with value 50 (must be 0-255)
  forward(255);
  // TO-DO: Establish a boolean or integer variable to keep track of whether a black line has just been driven past

  float cm = sonar.ping_cm();
  Serial.println(cm);
  int leftReadIR = analogRead(IR_LEFT);
  int middleReadIR = analogRead(IR_MIDDLE);
  int rightReadIR = analogRead(IR_RIGHT);
  // TO-DO: collect an initial IR reading, and print it so that you can compare that value to the thresholds set at the beginning, it's OK to use all 3 or just 1 IR sensor
            // Example that allows a single value to print in the Serial Monitor: middle IR value ### 
            // Example that allows all three values to print in the Serial Monitor: ###, ###, ###
  int trueIR = (leftReadIR + rightReadIR + middleReadIR)/3;

  // TO-DO: use a While loop - the IR reading is within the appropriate range for black line, pull in a new IR value, check statement again, stay in while loop as long as mobot is still above black line and eject from loop when mobot has driven past black line 
    while (trueIR > THRESHOLD_ABOVE_IS_BLACK && trueIR < THRESHOLD_ABOVE_IS_NOISE){
      blacklineCount = blacklineCount +1;
      servo.write(0);
      delay(100);
      Serial.println("zoom");
      servo.write(240);
      if (blacklineCount == numLine){
        servo.write(125);
        off();
        delay(1);
        Serial.println("delay 3k");
        forward(55);
        float dx = sonar.ping_cm();
        while(true){
          while(dx == 0){
            forward(255);
            dx = sonar.ping_cm();
          }
          unsigned long t = millis();
          
          dx = sonar.ping_cm();
          float dt = (t - lastt)/1000.0;

          lastt = t;

          float i_raw = i_last + ((dx-(float(distVal)-0.5)) * dt);
          float i_current = constrain(i_raw, -i_max, i_max);

          float d_val = 0;

          if (dt>0){
          d_val = (dx - dx_last)/dt;
          }

          float kP = 13.5;
          float kD = 0.0;
          float kI = 0.0;

          float output = kP*(dx - (float(distVal)-0.5)) + kI * i_current + kD * d_val;

          if (output >= 0){
            forward(output);
          }
          else{
            backward(-1*output);
          }
          Serial.println("d: " + String(d_val) + " | error: " + String(dx-(float(distVal)-0.5)) + " | i: " + String(i_current));

          i_last = i_current;
          dx_last = dx;
        }
        off();

     }
      delay(50);
      Serial.print(blacklineCount + " chud");
      leftReadIR = analogRead(IR_LEFT);
      middleReadIR = analogRead(IR_MIDDLE);
      rightReadIR = analogRead(IR_RIGHT);
      trueIR = (leftReadIR + rightReadIR + middleReadIR)/3;
    }
}

  
  // TO-DO: use an if statement - the black line was passed so update the line counter appropriately, and make sure the tracker for above black line is false (here or at top of the void loop)

  // TO-DO: If your counter is at least the number of LINES_TO_COUNT, use function off to stop the motors, and add a reason to stay here instead of restarting the motors with forward function at the top of the void loop

