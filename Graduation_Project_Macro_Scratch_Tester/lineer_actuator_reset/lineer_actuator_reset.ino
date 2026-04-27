// Define pins for linear actuator control
const int RPWM = 6;
const int LPWM = 5;

// Define constants for linear actuator speed and direction
const int LINEAR_ACTUATOR_MAX_SPEED = 120; // Maximum speed for PWM control

// Function to initialize linear actuator pins
void setup() {
  pinMode(RPWM, OUTPUT);
  pinMode(LPWM, OUTPUT);
}

// Function to move linear actuator downward
void linearActuatorDown() {
  analogWrite(RPWM, 0); // Stop PWM signal to RPWM pin
  analogWrite(LPWM, LINEAR_ACTUATOR_MAX_SPEED); // Set LPWM pin to maximum speed
}

// Function to move linear actuator upward
void linearActuatorUp() {
  analogWrite(RPWM, LINEAR_ACTUATOR_MAX_SPEED); // Set RPWM pin to maximum speed
  analogWrite(LPWM, 0); // Stop PWM signal to LPWM pin
}

void loop() {
  // Example usage
  linearActuatorUp();

}

