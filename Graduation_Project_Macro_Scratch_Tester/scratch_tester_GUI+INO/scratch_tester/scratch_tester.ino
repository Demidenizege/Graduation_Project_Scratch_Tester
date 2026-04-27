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
const int SERVO_MOTOR_DELAY = 200;
const int LOAD_CELL_DELAY = 10;
const int LINEAR_ACTUATOR_MAX_SPEED = 120;
const int LINEAR_ACTUATOR_GO_TO_UP_TIME = 2;                  // TODO: CHANGE HERE BY CALIBRATING TIME
const int SERVO_MOTOR_GO_TO_CAM_TIME = 5000;                     // TODO: CHANGE HERE BY CALIBRATING TIME
const int SERVO_MOTOR_RESET_TIME = 15000;                      // TODO: CHANGE HERE BY CALIBRATING TIME
const double LOAD_CELL_SET_SCALE_FACTOR = -18.2583;           // TODO: CHANGE HERE BY CALIBRATING LOAD CELL
const double LOAD_CELL_LINEAR_ACTUATOR_START_THRESHOLD = 10;  // TODO: CHANGE HERE BY CALIBRATING LOAD CELL
const double LOAD_CELL_LINEAR_ACTUATOR_MAX_THRESHOLD = 4000;  // TODO: CHANGE HERE BY CALIBRATING NEWTONS

// STATE VARIABLES
int currentState = WAIT_STATE;  // Corresponding state in Python GUI
float currentWeight = -1;       // Weight calculated by Load Cell
bool printedCaseTwo = false;    // For non-blocking the process
bool printedCaseThree = false;  // For non-blocking the process
byte linearActuatorSpeed = 0;
int linearActuatorGoToUpTimeSpent = 0;
int servoMotorGoToCamTimeSpent = 0;
int servoMotorResetTimeSpent = 0;

// Load Cell
HX711 scale;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////// HELPERS /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void printState(char currentState) {
  delay(PRINT_DELAY / 2);
  Serial.println(currentState);
  delay(PRINT_DELAY);
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////// SETUP /////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(9600);
  setupServoMotor();
  setupLoadCell();
  setupLinearActuator();
}

/////////////////////////////////////////////////////////////////////////////
///////////////////////////// LOOP //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void loop() {
  if (Serial.available() > 0) {
    char userInput = Serial.read();
    if (userInput == 's') {
      currentState = 1;
    } else if (userInput == 'c') {
      currentState = 7;
    }
  }

  if (currentState == 0) {
    printState('0');
    currentState = 1;
  } else if (currentState == 1) {
    printState('1');
    double currentReading = readLoadCell();
    if (currentReading > LOAD_CELL_LINEAR_ACTUATOR_START_THRESHOLD) {
      currentState = 2;
    } else {
      linearActuatorDown();
    }
  } else if (currentState == 2) {
    printState('2');
    double currentReading = readLoadCell();
    if (currentReading > LOAD_CELL_LINEAR_ACTUATOR_MAX_THRESHOLD) {
      currentState = 3;
    } else {
      linearActuatorDown();
      servoMotorRight();
    }
  } else if (currentState == 3) {
    printState('3');
    currentState = 4;
  } else if (currentState == 4) {
    printState('4');
    if (linearActuatorGoToUpTimeSpent > LINEAR_ACTUATOR_GO_TO_UP_TIME) {
      currentState = 5;
    } else {
      linearActuatorUp();
      linearActuatorGoToUpTimeSpent++;
    }
  } else if (currentState == 5) {
    printState('5');
    if (servoMotorGoToCamTimeSpent > SERVO_MOTOR_GO_TO_CAM_TIME) {
      currentState = 6;
    } else {
      servoMotorRight();
      servoMotorGoToCamTimeSpent++;
    }
  } else if (currentState == 6) {
    printState('6');
    currentState = WAIT_STATE;
  } else if (currentState == 7) {
    printState('7');
    currentState = 8;
  } else if (currentState == 8) {
    printState('8');
    if (servoMotorResetTimeSpent > SERVO_MOTOR_RESET_TIME) {
      currentState = WAIT_STATE;
    } else {
      servoMotorLeft();
      servoMotorResetTimeSpent++;
    }
  } else if (currentState == WAIT_STATE) {
    // Do Nothing
  } else {
    // Do Nothing
  }
}

/////////////////////////////////////////////////////////////////////////////
//////////////////////// SERVO MOTOR DRIVER /////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupServoMotor() {
  pinMode(SERVO_MOTOR_STEP_PIN, OUTPUT);
  pinMode(SERVO_MOTOR_DIR_PIN, OUTPUT);
  digitalWrite(SERVO_MOTOR_DIR_PIN, LOW);
}

void servoMotorRight() {
  
  digitalWrite(SERVO_MOTOR_STEP_PIN, HIGH);
  delayMicroseconds(SERVO_MOTOR_DELAY);
  digitalWrite(SERVO_MOTOR_STEP_PIN, LOW);
  delayMicroseconds(SERVO_MOTOR_DELAY);
}

void servoMotorLeft() {
  digitalWrite(SERVO_MOTOR_DIR_PIN, HIGH);
  digitalWrite(SERVO_MOTOR_STEP_PIN, HIGH);
  delayMicroseconds(SERVO_MOTOR_DELAY);
  digitalWrite(SERVO_MOTOR_STEP_PIN, LOW);
  delayMicroseconds(SERVO_MOTOR_DELAY);
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////// LOAD CELL ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupLoadCell() {
  scale.begin(HX711_DOUT, HX711_SCK);
  scale.set_scale(LOAD_CELL_SET_SCALE_FACTOR);  // TODO: CHANGE HERE BY THE CALIBRATION VALUE
  scale.tare();
}

double readLoadCell() {
  double scaled = scale.get_units();
  Serial.println(scaled);
  return scaled;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////// LINEAR ACTUATOR /////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

void setupLinearActuator() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

void linearActuatorDown() {
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  analogWrite(RPWM, 0);
  analogWrite(LPWM, linearActuatorSpeed);
}

void linearActuatorUp() {
  linearActuatorSpeed = LINEAR_ACTUATOR_MAX_SPEED;
  analogWrite(RPWM, linearActuatorSpeed);
  analogWrite(LPWM, 0);
}
