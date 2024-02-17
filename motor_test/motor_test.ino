#define MOTOR_PWM 3
#define MOTOR_DIR 4

void setup() {
  pinMode(MOTOR_PWM, OUTPUT);
  pinMode(MOTOR_DIR, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // Rotate motor in one direction
  Serial.println("Rotating motor clockwise");
  digitalWrite(MOTOR_DIR, LOW); // Set motor direction clockwise
  analogWrite(MOTOR_PWM, 255); // Apply full PWM signal
  delay(3000); // Rotate for 3 seconds
  
  // Stop motor
  Serial.println("Stopping motor");
  analogWrite(MOTOR_PWM, 0); // Stop motor
  delay(1000); // Wait for 1 second
  
  // Rotate motor in the other direction
  Serial.println("Rotating motor counterclockwise");
  digitalWrite(MOTOR_DIR, HIGH); // Set motor direction counterclockwise
  analogWrite(MOTOR_PWM, 255); // Apply full PWM signal
  delay(3000); // Rotate for 3 seconds
  
  // Stop motor
  Serial.println("Stopping motor");
  analogWrite(MOTOR_PWM, 0); // Stop motor
  delay(1000); // Wait for 1 second
}
