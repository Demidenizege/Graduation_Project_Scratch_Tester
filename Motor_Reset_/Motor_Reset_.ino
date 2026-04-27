// Define pins for stepper motor control
#define stepPin 3
#define dirPin 2

// Function to initialize stepper motor pins
void setup() {
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

// Function to rotate the stepper motor clockwise (right)
void motorTurnRight() {
  digitalWrite(dirPin, HIGH); // Set direction to clockwise
  
  // Rotate right for two full cycles
  for(int x = 0; x < 8000; x++) 
  {
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(650);   
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(650);
  }
}

// Function to rotate the stepper motor counterclockwise (left)
void motorTurnLeft() {
  digitalWrite(dirPin, LOW); // Set direction to counterclockwise
  
  // Rotate left for one full cycle
  for (int x = 0; x < 4000; x++) {
    digitalWrite(stepPin, HIGH); 
    delayMicroseconds(650); // Adjust speed by changing delay
    digitalWrite(stepPin, LOW);
    delayMicroseconds(650);
  }
}

void loop() 
{
  motorTurnLeft();
}
