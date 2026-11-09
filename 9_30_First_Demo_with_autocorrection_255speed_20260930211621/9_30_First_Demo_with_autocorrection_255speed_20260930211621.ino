/*
  24-101: Introduction to Mechanical Engineering
  Course Instructor for Labs: Assoc. Prof. of the Practice Haidar 

  Starter Code for Counting Lines with Mobot
*/

#include <NewPing.h>
#include <Servo.h>

const int numLine = 8;
const int distVal = 8;
const int LEFT_MOTORS = 5; 
const int RIGHT_MOTORS = 6; 
const int LEFT_MOTOR_DIR = 8; 
const int RIGHT_MOTOR_DIR = 7; 
const int MOTOR_OUTPUT = 3; 

const int ServoPin = 10;
const int TriggerPin = 13;
const int EchoPin = 12;

NewPing sonar (TriggerPin, EchoPin, 200);
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

unsigned long time_left = 0;
unsigned long time_middle = 0;
unsigned long time_right = 0;

float dt_dr = 0;
float dt_dl = 0;

const int THRESHOLD_ABOVE_IS_BLACK = 450; 
const int THRESHOLD_ABOVE_IS_NOISE = 850; 

const int LINES_TO_COUNT = 2;
int blacklineCount = 0;

float damping = 0.7;
int speedCount = 100;
int baseDelay = 75;
float lineIntegralBuffer = 0.25; 
int deadzone = 40; 

//chud variables
float line_integral = 0.0;    
float max_line_i = 300.0;     
unsigned long loop_last_time = 0; 

// parallel parking global tracking coordinates
float mobot_X = 0;
float mobot_y = 0;
float mobot_theta = 90; // The car's internal heading (in degrees)

float cm = 0;
float readings[4]; 

void forward(int left, int right){  
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, HIGH);
  digitalWrite(RIGHT_MOTOR_DIR, HIGH);
  analogWrite(LEFT_MOTORS, left);
  analogWrite(RIGHT_MOTORS, right);
}

void backward(int left, int right){  
  digitalWrite(MOTOR_OUTPUT, HIGH);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
  analogWrite(LEFT_MOTORS, left);
  analogWrite(RIGHT_MOTORS, right);
}

void off (){
  digitalWrite(MOTOR_OUTPUT, LOW);
  digitalWrite(LEFT_MOTOR_DIR, LOW);
  digitalWrite(RIGHT_MOTOR_DIR, LOW);
}

void setup() {
  pinMode(LEFT_MOTORS, OUTPUT);
  pinMode(RIGHT_MOTORS, OUTPUT);
  pinMode(LEFT_MOTOR_DIR, OUTPUT);
  pinMode(RIGHT_MOTOR_DIR, OUTPUT);
  pinMode(MOTOR_OUTPUT, OUTPUT);
  pinMode(IR_RIGHT, INPUT);
  pinMode(IR_MIDDLE, INPUT);
  pinMode(IR_LEFT, INPUT);

  servo.attach(ServoPin);
  servo.write(90); // Straight head-on forward center calibration point

  Serial.begin(9600);
  lastt = millis();
}

float filter(){
  for (int i = 0; i < 4; i++) {
    readings[i] = sonar.ping_cm(); 
    if (readings[i] == 0) { readings[i] = 200.0; }
    delay(20); 
  }

  // Sort array (Selection Sort)
  for (int i = 0; i < 3; i++) {
    for (int j = i + 1; j < 4; j++) {
      if (readings[i] > readings[j]) {
        float temp = readings[i];
        readings[i] = readings[j];
        readings[j] = temp;
      }
    }
  }

  float true_cm = (readings[2] + readings[3]) / 2.0;
  return true_cm;
}

void loop() {
  // Prime the loop with a high start value out of target range
  cm = 0; 
  int confidence = 0;
  float waitTil = 43; // Distance target threshold in CM
  
  // KEEP BACKING UP UNTIL: We have achieved 3 stable, consecutive pings under 150cm
  while (confidence < 1) {
    backward(255, 255);
    
    // Using your custom filter function ensures accurate centimeter tracking data
    cm = sonar.ping_cm(); 
    
    Serial.print("Filtered Dist: "); Serial.print(cm);
    Serial.print(" cm | Confidence Streak: "); Serial.println(confidence);
    
    if (cm > waitTil) {
      confidence += 1; // Increment streak if inside target range
    } 
  }
  
  // Safe stop execution window before launching the parallel sequence turn pivot
  off();
  delay(10);
  
  cm = 0;
  backward(255, 80); // Initiate backing pivot arc turn execution
  delay(775);
  off();
  forward(100,0);
  delay(50);
  off();
  delay(99999);

  // --- WALL TRACKING WITH ORIENTATION AWARENESS ---
  int dynamic_center = 22; 

  while(true){
     // 1. Set your two exact target check points (+-10 degrees relative to active center)
     int angle_left = dynamic_center - 10;
     int angle_right = dynamic_center + 10;

     angle_left = constrain(angle_left, 5, 175);
     angle_right = constrain(angle_right, 5, 175);

     // 2. Snap to the left position and ping
     servo.write(angle_left);
     delay(180); 
     float d_left = filter();

     // 3. Snap to the right position and ping
     servo.write(angle_right);
     delay(180); 
     float d_right = filter();

     float wall_distance = 0;

     // 4. Calculate true perpendicular distance to the wall
     if (abs(d_left - d_right) > 0.05){
        float y = d_left;
        float x = d_right;
        
        float law_of_cosines_side = sqrt(pow(x, 2) + pow(y, 2) - 2.0 * x * y * cos((20.0 * 3.14159) / 180.0));
        wall_distance = (x * y * sin((20.0 * 3.14159) / 180.0)) / law_of_cosines_side;
     }
     else {
        wall_distance = d_left * cos((10.0 * 3.14159) / 180.0); 
     }

     // 5. Calculate true tracking angle error safely
     float acos_ratio = wall_distance / d_right;
     acos_ratio = constrain(acos_ratio, -1.0, 1.0); 

     float theta_from_x_rad = acos(acos_ratio);
     float theta_from_x_deg = theta_from_x_rad * 180.0 / 3.14159; 

     // 6. Direct Tracking Direction Correction
     float final_heading_correction = 0;
     if (d_left > d_right) {
        final_heading_correction = (90.0 - (theta_from_x_deg + 10.0)); 
     } else {
        final_heading_correction = -(90.0 - (theta_from_x_deg + 10.0));
     }

     // 7. Update dynamic center coordinate frame
     dynamic_center = 22 + (int)final_heading_correction;
     dynamic_center = constrain(dynamic_center, 10, 40); 

     mobot_theta = final_heading_correction;

     Serial.print("Wall Dist: "); Serial.print(wall_distance);
     Serial.print("cm | Center P: "); Serial.print(dynamic_center);
     Serial.print(" | Car Heading (Theta): "); Serial.print(mobot_theta);
     Serial.println(" deg");
  }
}
