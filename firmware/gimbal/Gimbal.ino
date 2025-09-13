#include <Wire.h>
#include <Servo.h>

constexpr int BAUDRATE = 9600;
constexpr int MPUADDR = 0x68;

constexpr float AFS_SEL = 16384.0f;

class MPU6050 {
  private:
  int mpuaddr;

  public:
  MPU6050(int address) : mpuaddr(address) {}
  
  enum MPU6050_Register : uint8_t {
    PWR_MGMT_1 = 0x6B,
    ACCEL_CONFIG = 0x1C,
    ACCEL_X_OUT_H = 0x3B
  };

  void write(uint8_t address, uint8_t data) {
    Wire.beginTransmission(mpuaddr);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission(true);
  }

  void read(uint8_t address, size_t length, uint8_t* destination) {
    Wire.beginTransmission(mpuaddr);
    Wire.write(address);
    Wire.endTransmission(false);
    Wire.requestFrom(mpuaddr, length);
    
    for (int n = 0; n < length; n++) {
      destination[n] = Wire.read();
    }
  }

};

Servo servo;
constexpr int SERVO_PIN = 3;

int servo_angle_offset;

uint8_t accel_raw[4]; 

void setup() {
  // Start I2C master
  Serial.begin(BAUDRATE);
  Wire.begin();
  delay(100);

  MPU6050 mpu(MPUADDR);
  
  mpu.write(MPU6050::PWR_MGMT_1, 0x00); // Wake MPU-6050
  mpu.write(MPU6050::ACCEL_CONFIG, 0x00); // Change accelerometer config

  servo.attach(SERVO_PIN);
}

void loop() {
  MPU6050 mpu(MPUADDR);

  mpu.read(MPU6050::ACCEL_X_OUT_H, 4, accel_raw);

  // Combine high and low values
  int16_t accel_xout_raw = ((int16_t)((accel_raw[0] << 8) | accel_raw[1]));
  int16_t accel_yout_raw = ((int16_t)((accel_raw[2] << 8) | accel_raw[3]));
  
  // Convert to in terms of g
  double accel_xout = accel_xout_raw / AFS_SEL;
  double accel_yout = accel_yout_raw / AFS_SEL;

  float angle = (atan2f(accel_yout, accel_xout)) * (float)(180/3.1415926535);

  // Convert the angle to {0, 180}
  int servo_angle = (int)(angle) + 90;

  // Offset
  if (Serial.available()) {
    servo_angle_offset = Serial.parseInt();
  }
  servo_angle += servo_angle_offset;

  servo.write(servo_angle);

  // Print relevant values
  Serial.print("accel_xout: ");
  Serial.println(accel_xout);

  Serial.print("accel_yout: ");
  Serial.println(accel_yout);

  Serial.print("servo_angle: ");
  Serial.print(servo_angle);
  Serial.print("\n");

  delay(100);
}