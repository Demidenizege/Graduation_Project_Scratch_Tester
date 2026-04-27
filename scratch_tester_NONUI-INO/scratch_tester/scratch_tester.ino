#include "HX711.h"

/////////////////////////////////////////////////////////////////////////////
//////////////////////// VARIABLES & CONSTANTS //////////////////////////////
/////////////////////////////////////////////////////////////////////////////

// PINS
const int SERVO_MOTOR_STEP_PIN = 3;
const int SERVO_MOTOR_DIR_PIN = 2;
const int HX711_SCK = 8;
const int HX711_DOUT = 9;
const int RPWM = 6;
const int LPWM = 5;

// CONSTANTS
const int WAIT_STATE = 111;
const int PRINT_DELAY = 2000;
const int SERVO_MOTOR_DELAY = 500;
const int LOAD_CELL_DELAY = 500;
const int LINEAR_ACTUATOR_MAX_SPEED = 75;
const int LINEAR_ACTUATOR_GO_TO_UP_TIME = 10000; // TODO: CHANGE HERE BY CALIBRATING TIME
const int SERVO_MOTOR_GO_TO_CAM_TIME = 5000;     // TODO: CHANGE HERE BY CALIBRATING TIME
const int SERVO_MOTOR_RESET_TIME = 15000;        // TODO: CHANGE HERE BY CALIBRATING TIME
const double LOAD_CELL_SET_SCALE_FACTOR = -18.2583;           // TODO: CHANGE HERE BY CALIBRATING LOAD CELL
const double LOAD_CELL_LINEAR_ACTUATOR_START_THRESHOLD = 1;  // TODO: CHANGE HERE BY CALIBRATING LOAD CELL
const double LOAD_CELL_LINEAR_ACTUATOR_MAX_THRESHOLD = 3000; // TODO: CHANGE HERE BY CALIBRATING NEWTONS

// STATE VARIABLES
int currentState = WAIT_STATE;          // Corresponding state in Python GUI
float currentWeight = -1;      // Weight calculated by Load Cell
bool printedCaseTwo = false;   // For non-blocking the process
bool printedCaseThree = false; // For non-blocking the process
int loadCellMax = 7000;
byte linearActuatorSpeed = 0;
int linearActuatorGoToUpTimeSpent = 0;
int servoMotorGoToCamTimeSpent = 0;
int servoMotorResetTimeSpent = 0;
char userInput= 'a';



/// ysf
#define stepPin 3
#define dirPin 2
bool motor_run = false;


// Load Cell
HX711 scale;

bool checkLoadCell()
{
  double scaled = scale.get_units();
  //Serial.println(scaled);
  if(scaled > loadCellMax)
  {
    return true;
  } 
  else
  {
    return false;
  }
}

void linearActuatorMovement()
{
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  analogWrite(RPWM, linearActuatorSpeed);
  analogWrite(LPWM, 0);
}

void stepMotorMovement()
{
  digitalWrite(SERVO_MOTOR_DIR_PIN, LOW);
  digitalWrite(SERVO_MOTOR_STEP_PIN, HIGH);
  delayMicroseconds(SERVO_MOTOR_DELAY);
  digitalWrite(SERVO_MOTOR_STEP_PIN, LOW);
  delayMicroseconds(SERVO_MOTOR_DELAY);
}

void allReset()
{
  linearActuatorUp();
  //servoMotorLeft();
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////// SERVO MOTOR DRIVER /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupServoMotor()
{
  pinMode(SERVO_MOTOR_STEP_PIN, OUTPUT);
  pinMode(SERVO_MOTOR_DIR_PIN, OUTPUT);
}

void servoMotorRight()
{
  Serial.println("Motor Right Code Started.");
  digitalWrite(SERVO_MOTOR_DIR_PIN, LOW);
  digitalWrite(SERVO_MOTOR_STEP_PIN, HIGH);
  delayMicroseconds(SERVO_MOTOR_DELAY);
  digitalWrite(SERVO_MOTOR_STEP_PIN, LOW);
  delayMicroseconds(SERVO_MOTOR_DELAY);
}

void servoMotorLeft()
{
  digitalWrite(SERVO_MOTOR_DIR_PIN, HIGH);
  digitalWrite(SERVO_MOTOR_STEP_PIN, HIGH);
  delayMicroseconds(SERVO_MOTOR_DELAY);
  digitalWrite(SERVO_MOTOR_STEP_PIN, LOW);
  delayMicroseconds(SERVO_MOTOR_DELAY);
}


void motor()
{
  digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 800; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(700);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(700); 
  }
  delay(1000); // One second delay
  
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 3000; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
  delay(1000);  
}

void motor3()
{
  digitalWrite(dirPin,LOW); // Enables the motor to move in a particular direction
  // Makes 200 pulses for making one full cycle rotation
  for(int x = 0; x < 2500; x++) {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500);    // by changing this time delay between the steps we can change the rotation speed
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500); 
  }
}

void motor2()
{
  digitalWrite(dirPin,LOW); //Changes the rotations direction
  // Makes 400 pulses for making two full cycle rotation
  for(int x = 0; x < 1600; x++) {
    digitalWrite(stepPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin,LOW);
    delayMicroseconds(500);
  }
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////// LOAD CELL ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupLoadCell()
{
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale(LOAD_CELL_SET_SCALE_FACTOR); // TODO: CHANGE HERE BY THE CALIBRATION VALUE
  scale.tare();
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////// LINEAR ACTUATOR /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupLinearActuator()
{
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

void linearActuatorDown()
{
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  analogWrite(RPWM, 0);
  analogWrite(LPWM, linearActuatorSpeed);
}

void linearActuatorUp()
{
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  analogWrite(RPWM, linearActuatorSpeed);
  analogWrite(LPWM, 0);
}

void linearActuatorReset()
{
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  unsigned long startTime = millis();
  for (; millis() - startTime < 4000;) 
  {
    linearActuatorUp();
  }
  Serial.println("reset loop end");
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////// HELPERS /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void printState(int currentState)
{
  delay(PRINT_DELAY / 2);
  Serial.println(currentState);
  delay(PRINT_DELAY);
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////// SETUP /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setup()
{
  Serial.begin(9600);
  setupLoadCell();
  setupLinearActuator();

  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);




}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////// LOOP //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void loop()
{
  if (motor_run)
  {
    motor3();
  }

  if (checkLoadCell())
  {
    motor_run = false;
    linearActuatorReset();
    motor2();
  }

  if (Serial.available() > 0)
  {
    userInput = Serial.read();
    switch (userInput) {
      case 's':
        Serial.println("S tusuna basildi");
        linearActuatorDown();
        motor_run = true;
        Serial.println(motor_run);
        break;
      case 'c':
        Serial.println("C tusuna basildi");
        linearActuatorReset();
        motor2();
        motor_run = false;
        break;
      default:
        // if nothing else matches, do the default
        // default is optional
        break;
    }
  }
}


  // if (userInput == 's')
  // {
  //   Serial.println("S tusuna basildi");
  //   linearActuatorDown();
  // }
  // else if (checkLoadCell())
  // {
  //   //STOP
  //   //RESET
  // }
  // else if (userInput == 'c')
  // {
  //   Serial.println("C tusuna basildi");
  //   linearActuatorReset();
  // }
  // else
  // {

  // }


