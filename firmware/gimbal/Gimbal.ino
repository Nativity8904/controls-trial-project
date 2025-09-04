#include <Wire.h>

const int BAUDRATE = 9600;
const int MPUADDR = 0x68;

constexpr uint8_t DLPFCONFIG = 4; 
constexpr uint8_t GYROCONFIG = 0x10;
constexpr uint8_t ACCELEROMETERCONFIG = 0x00; 

int16_t gyro_xout;

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

  // Change DLPF Config
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x1A);
  Wire.write(DLPFCONFIG);
  Wire.endTransmission(true);

  // Change Gyro Config
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x1B);
  Wire.write(GYROCONFIG);
  Wire.endTransmission(true);

  // Change Accelerometer Config
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x1C);
  Wire.write(ACCELEROMETERCONFIG);
  Wire.endTransmission(true);

}

void loop() {

  // Read gyroscope sensors
  Wire.beginTransmission(MPUADDR);
  Wire.write(0x43);
  Wire.endTransmission(false);

  Wire.requestFrom(MPUADDR, 2);

  uint8_t xh = Wire.read(), xl = Wire.read();

  gyro_xout = (int16_t)((xh << 8) | xl);

  float scale = 32.8f;
  Serial.print(gyro_xout / scale, 2);
  Serial.print("\n");

  delay(100);
}