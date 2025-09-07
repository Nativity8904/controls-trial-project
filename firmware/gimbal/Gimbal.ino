#include <Wire.h>

constexpr int BAUDRATE = 9600;
constexpr int MPUADDR = 0x68;

constexpr uint8_t ACCEL_CONFIG = 0x00; 

constexpr float AFS_SEL = 16384.0f;

void setup() {
  // Start I2C Master
  Serial.begin(BAUDRATE);
  Wire.begin();
  delay(100);

  // Wake MPU-6050
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  // Change Accelerometer Config
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x1C);
  Wire.write(ACCEL_CONFIG);
  Wire.endTransmission(true);

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
  float accel_xout = accel_xout_raw / AFS_SEL;
  float accel_yout = accel_yout_raw / AFS_SEL;
  float accel_zout = accel_zout_raw / AFS_SEL;

  Serial.print("Accel_xout: ");
  Serial.println(accel_xout, 5);

  Serial.print("Accel_yout: ");
  Serial.println(accel_yout, 5);

  Serial.print("Accel_zout: ");
  Serial.println(accel_zout, 5);

  delay(100);
}