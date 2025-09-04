#include <Wire.h>

const int BAUDRATE = 19200;
const int MPUADDR = 0x68;

void setup() {
  // Start I2C Master
  Serial.begin(BAUDRATE);
  Wire.begin();
  delay(100);

  // Wake MPU-6050
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission();

  // Create I2C frame
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x75);
  Wire.endTransmission(false);

  // Request 1 byte from the MPU-6050 WHO_AM_I register
  Wire.requestFrom(MPUADDR, 1);

  // Read I2C data from request and print whoami
  uint8_t whoami = Wire.read();
  Serial.print(whoami, HEX);
}

void loop() {

}