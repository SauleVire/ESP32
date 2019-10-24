#include <PID_v1.h>
#define MotEnable 12 //Motor Enamble pin Runs on PWM signal
#define MotFwd  13  // Motor Forward pin
#define MotRev  14 // Motor Reverse pin
String readString; //This while store the user input data
int User_Input = 0; // This while convert input string into integer

int REV = 0;          // Set point REQUIRED ENCODER VALUE

double kp = 5 , ki = 1 , kd = 0.01;             // modify for optimal performance
double input = 0, output = 0, setpoint = 0;
PID myPID(&input, &output, &setpoint, kp, ki, kd, DIRECT);  

void setup() {
  pinMode(MotEnable, OUTPUT);
  pinMode(MotFwd, OUTPUT); 
  pinMode(MotRev, OUTPUT); 
  Serial.begin(115200); //initialize serial comunication


  myPID.SetMode(AUTOMATIC);   //set PID in Auto mode
  myPID.SetSampleTime(1);  // refresh rate of PID controller
  myPID.SetOutputLimits(-125, 125); // this is the MAX PWM value to move motor, here change in value reflect change in speed of motor.
}

void loop() {
  while (Serial.available()) { //Check if the serial data is available.
    delay(3);                  // a small delay
    char c = Serial.read();  // storing input data
    readString += c;         // accumulate each of the characters in readString
  }
 
  if (readString.length() >0) { //Verify that the variable contains information
  
   Serial.println(readString.toInt());  //printing the input data in integer form
    User_Input = readString.toInt();   // here input data is store in integer form
}
REV = map (User_Input, 0, 360, 0, 1600); // mapping degree into pulse
Serial.print("this is REV - "); 
Serial.println(REV);               // printing REV value  

setpoint = REV;                    //PID while work to achive this value consider as SET value
  myPID.Compute();                 // calculate new output
  pwmOut(output);  
}
void pwmOut(int out) {                               
  if (out > 0) {                         // if REV > encoderValue motor move in forward direction.    
    forward();                           // calling motor to move forward
  }
  else {
    reverse();                            // calling motor to move reverse
  }
 readString=""; // Cleaning User input, ready for new Input
}

void forward () {
  digitalWrite(MotFwd, HIGH); 
 digitalWrite(MotRev, LOW); 
  
}

void reverse () {
  digitalWrite(MotFwd, LOW); 
 digitalWrite(MotRev, HIGH); 
  
}
void finish () {
  digitalWrite(MotFwd, LOW); 
 digitalWrite(MotRev, LOW); 
  
}
