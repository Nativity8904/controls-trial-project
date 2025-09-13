#include <Wire.h>
#include <Servo.h>

constexpr int BAUDRATE = 9600;
constexpr int MPUADDR = 0x68;

constexpr uint8_t ACCEL_CONFIG = 0x00; 

constexpr float AFS_SEL = 16384.0f;

Servo servo;
constexpr int SERVO_PIN = 3;

int servo_angle_offset;

class I2C_Wrapper {
  private:
  int mpuaddr;

  public:
  I2C_Wrapper(int addr) : mpuaddr(addr) {}

  void write(int location, int data) {
    Wire.beginTransmission(mpuaddr);
    Wire.write(location);
    Wire.write(data);
    Wire.endTransmission(true);
  }

};

void setup() {
  // Start I2C master
  Serial.begin(BAUDRATE);
  Wire.begin();
  delay(100);

  I2C_Wrapper mpu(MPUADDR);
  
  mpu.write(0x6B, 0x00) // Wake MPU-6050
  mpu.write(0x1C, ACCEL_CONFIG) // Change accelerometer config

  servo.attach(SERVO_PIN);
}

void loop() {

  // Read accel sensors
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x3B); // Address of accel_xout_h
  Wire.endTransmission(false);

  // Request next 6 bytes from MPU (full accel values)
  Wire.requestFrom(MPUADDR, 6);

  // Read accel values from memory
  uint8_t accel_xout_h = Wire.read(), accel_xout_l = Wire.read();
  uint8_t accel_yout_h = Wire.read(), accel_yout_l = Wire.read();
  uint8_t accel_zout_h = Wire.read(), accel_zout_l = Wire.read();

  // Combine high and low values
  int16_t accel_xout_raw = ((int16_t)((accel_xout_h << 8) | accel_xout_l));
  int16_t accel_yout_raw = ((int16_t)((accel_yout_h << 8) | accel_yout_l));
  int16_t accel_zout_raw = ((int16_t)((accel_zout_h << 8) | accel_zout_l));
  
  // Convert to in terms of g
  double accel_xout = accel_xout_raw / AFS_SEL;
  double accel_yout = accel_yout_raw / AFS_SEL;
  float accel_zout = accel_zout_raw / AFS_SEL;

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