#include <Wire.h>
#include <PWM.h> 

#define MPU6050       0x68         // Device address
#define ACCEL_CONFIG  0x1C         // Accelerometer configuration address
#define GYRO_CONFIG   0x1B         // Gyro configuration address
#define PWR_MGMT_1    0x6B
#define PWR_MGMT_2    0x6C

#define BRAKE         8 
#define PWM           9
#define DIRECTION     4

#define BUZZER        12
#define VBAT          A7

const uint16_t PWM_FREQUENCY = 20000;                 // The motor driver can handle a PWM frequency up to 20kHz
const uint16_t PWMVALUE = F_CPU / PWM_FREQUENCY / 2;  // The frequency is given by F_CPU/(2*N*ICR) - where N is the prescaler, prescaling is used so the frequency is given by F_CPU/(2*ICR) - ICR = F_CPU/PWM_FREQUENCY/2


//BEST VALUES!!!
//float X1 = 90; 
//float X2 = 4;   
//float X3 = 0.001;  

//NEW BEST
// float X1 = 95; 
// float X2 = 3;   
// float X3 = 0.002;  

float X1 = 100; 
float X2 = 3;   
float X3 = 0.002;  
float loop_time = 10;  

int pwm_s = 0;
byte dir;
int32_t motor_speed; 
uint32_t timer;
long currentT, previousT_1, previousT_2 = 0; 
int16_t AcX, AcY, AcZ, GyZ, gyroZ;

//Sensor output scaling
#define accSens 0             // 0 = 2g, 1 = 4g, 2 = 8g, 3 = 16g
#define gyroSens 1            // 0 = 250rad/s, 1 = 500rad/s, 2 1000rad/s, 3 = 2000rad/s
#define Gyro_amount 0.996     // percent of gyro in complementary filter

//IMU offset values
int16_t  AcX_offset = -750;
int16_t  AcY_offset = 360;
int16_t  AcZ_offset = 0;
int16_t  GyZ_offset = 0;
int32_t  GyZ_offset_sum = 0;

float alpha = 0.40; 
float gyroZfilt;
float target_angle = 0;
float robot_angle;
float Acc_angle;

bool vertical = false;  

uint8_t i2cData[14]; // Buffer for I2C data

void setup() {
  delay(1000);
  angle_setup();
  Serial.begin(115200);

  // Set PWM frequency to 20kHz - see the datasheet http://www.atmel.com/Images/doc8272.pdf page 128-135 
  // Set up PWM, Phase and Frequency Correct on pin 9 (OC1A) & pin 10 (OC1B) with ICR1 as TOP using Timer1
  TCCR1B = (1 << WGM13) | (1 << CS10);  // Set PWM Phase and Frequency Correct with ICR1 as TOP and no prescaling
  ICR1 = PWMVALUE;                      // ICR1 is the TOP value - this is set so the frequency is equal to 20kHz

  // Clear OC1A/OC1B on compare match when up-counting - Set OC1A/OC1B on compare match when downcounting
  TCCR1A = (1 << COM1A1) | (1 << COM1B1);
  setPWM(400); 
  
  Serial.print("PWM: "); Serial.println(PWMVALUE); Serial.print("CPU_FREQ: "); Serial.println(F_CPU); 
  
  pinMode(PWM, OUTPUT);
  pinMode(BRAKE, OUTPUT);
  pinMode(DIRECTION, OUTPUT);
  Motor_control(400);
}

void loop() {
  currentT = millis();
  if (currentT - previousT_1 >= loop_time) {
    Tuning(); 
    angle_calc();
    if (vertical) {
      digitalWrite(BRAKE, HIGH);
      gyroZ = GyZ / 131.0; // Convert to deg/s
      
      gyroZfilt = alpha * gyroZ + (1 - alpha) * gyroZfilt;
      float error = robot_angle - target_angle;
      pwm_s = -constrain(X1 * error + X2 * gyroZfilt + X3 * -motor_speed, -255, 255); 
      //Serial.println(pwm_s);

      Motor_control(pwm_s);
      motor_speed += pwm_s;

      if(robot_angle > 0){
        target_angle -= 0.01;
      }else{
        target_angle += 0.01;
      }
      
      previousT_1 = currentT;
    } else {
      motor_speed = 0;
      target_angle = 0;
      Motor_control(1);
      motor_speed = 0;
      Serial.println("Not vertical");
      digitalWrite(BRAKE, LOW);
    }
  }
}
