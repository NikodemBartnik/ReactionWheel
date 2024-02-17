/*
Author: Nikodem Bartnik
website: https://indystry.cc, https://nikodembartnik.pl
email: nikodem@indystry.cc

inspired by: https://github.com/remrc/One-Axis-Reaction-Wheel-Stick
*/

#include <Wire.h>

#define MPU6050 0x68         // Device address
#define accSens 0             // 0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
#define gyroSens 1            // 0 = 250rad/s, 1 = 500rad/s, 2 1000rad/s, 3 = 2000rad/s
#define Gyro_amount 0.996     // percent of gyro in complementary filter
#define MOTOR 9
#define DIR 4

//BEST VALUES
// float Kp = 50;
// float Ki = 0.2;
// float Kd = 0.1;

// float Kp = 68;
// float Ki = 2;
// float Kd = 3;

float Kp = 75;
float Ki = 2;
float Kd = 3;
float loop_time = 10;


//IMU offset values
int16_t  AcX_offset = -750;
int16_t  AcY_offset = 360;
int16_t  AcZ_offset = 0;
int16_t  GyZ_offset = 0;
int32_t  GyZ_offset_sum = 0;
int16_t AcX, AcY, AcZ, GyZ, gyroZ;

float target_angle = 0;
float current_angle;
float acc_angle;
bool vertical = false;
float errorSum = 0;
float lastError = 0;
long current_time, previous_time = 0;
uint8_t i2cData[14];

void setup() {
  delay(1000);
  angle_setup();
  Serial.begin(115200);

  pinMode(MOTOR, OUTPUT);
  pinMode(DIR, OUTPUT);
  analogWrite(MOTOR, 255);
}


void loop() {
  current_time = millis();
  if (current_time - previous_time >= loop_time) {
    angle_calc();
    if (vertical) {
      float error = current_angle - target_angle;
      errorSum += error;
      errorSum = constrain(errorSum, -80, 80);
      float derivative = (error - lastError) / (loop_time/1000);
      int motor_pwm = constrain(Kp * error + Ki * errorSum + Kd * derivative, -255, 255);

      if (motor_pwm > 0) {
        digitalWrite(DIR, LOW);
      } else {
        digitalWrite(DIR, HIGH);
      }
      analogWrite(MOTOR, 1-abs(motor_pwm));

      if (current_angle > 0) {
        target_angle -= 0.003;
      } else {
        target_angle += 0.00;
      }
      target_angle = constrain(target_angle, -1, 1);

      lastError = error;
      previous_time = current_time;
    } else {
      target_angle = 0;
      analogWrite(MOTOR, 255);
      errorSum = 0;
    }
  }
}

void writeTo(byte device, byte address, byte value) {
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(value);
  Wire.endTransmission(true);
}

void angle_setup() {
  Wire.begin();
  delay(100);
  writeTo(MPU6050, 0x6B, 0);
  writeTo(MPU6050, 0x1C, accSens << 3); // Specifying output scaling of accelerometer
  writeTo(MPU6050, 0x1B, gyroSens << 3); // Specifying output scaling of gyroscope
  delay(100);

  for (int i = 0; i < 1024; i++) {
    angle_calc();
    GyZ_offset_sum += GyZ;
    delay(5);
  }
  GyZ_offset = GyZ_offset_sum >> 10;
}

void angle_calc() {

  Wire.beginTransmission(MPU6050);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 4, true);
  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();

  Wire.beginTransmission(MPU6050);
  Wire.write(0x47);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU6050, 2, true);
  GyZ = Wire.read() << 8 | Wire.read();

  AcX += AcX_offset;
  AcY += AcY_offset;
  AcZ += AcZ_offset;
  GyZ -= GyZ_offset;

  current_angle += GyZ * loop_time / 1000 / 65.536;
  acc_angle = atan2(AcY, -AcX) * 57.2958;
  current_angle = current_angle * Gyro_amount + acc_angle * (1.0 - Gyro_amount);

  if (abs(current_angle) > 12) vertical = false;
  if (abs(current_angle) < 0.3) vertical = true;
}
