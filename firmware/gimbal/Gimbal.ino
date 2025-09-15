#include <Wire.h>
#include <Servo.h>

constexpr int BAUDRATE = 9600;
constexpr int MPUADDR = 0x68;
constexpr int SERVO_PIN = 3;

class MPU6050 {
  private:
  int mpuaddr_;
  struct {
    int16_t x;
    int16_t y;
    int16_t z;
  } accel_;

  public:
  MPU6050(int address) 
    : mpuaddr_(address) {}
  
  enum MPU6050_Register : uint8_t {
    PWR_MGMT_1 = 0x6B,
    ACCEL_CONFIG = 0x1C,
    ACCEL_X_OUT_H = 0x3B
  };

  void Write(uint8_t address, uint8_t data) {
    Wire.beginTransmission(mpuaddr_);
    Wire.write(address);
    Wire.write(data);
    Wire.endTransmission(true);
  }

  // Function that reads from an address to a certain length and outputs to an array.
  void Read(uint8_t address, size_t length, uint8_t* destination) {
    Wire.beginTransmission(mpuaddr_);
    Wire.write(address);
    Wire.endTransmission(false);
    Wire.requestFrom(mpuaddr_, length);
    
    for (int n = 0; n < length; n++) {
      destination[n] = Wire.read();
    }
  }

  void ReadAccel() {
    uint8_t rawAccel[6];
    Read(ACCEL_X_OUT_H, 6, rawAccel);
    
    accel_.x = ((int16_t)((rawAccel[0] << 8) | rawAccel[1]));
    accel_.y = ((int16_t)((rawAccel[2] << 8) | rawAccel[3]));
    accel_.z = ((int16_t)((rawAccel[4] << 8) | rawAccel[5]));
  }

  int16_t CurrentAccelX() {
    return accel_.x;
  }

  int16_t CurrentAccelY() {
    return accel_.y;
  }

  void PrintAccel() {
    Serial.print("X_accel: ");
    Serial.println(accel_.x);
    Serial.print("Y_accel: ");
    Serial.println(accel_.y);
    Serial.print("Z_accel: ");
    Serial.println(accel_.z);
  }

};

class Gimbal {
  private:
  uint8_t servo_pin_;
  Servo servo_;
  int offset_deg_;
  int servo_angle_deg_;

  public:
  Gimbal(uint8_t servo_pin, int offset_deg = 0)
    : servo_pin_(servo_pin), offset_deg_(offset_deg), servo_angle_deg_(90) {}

  void Attach() {
    servo_.attach(servo_pin_);
  }

  void UpdateSensorAngleAccel(int16_t ax, int16_t ay) {
    float angle = (atan2f((float)ay, (float)ax)) * 57.29746936f;
    servo_angle_deg_ = (int)(angle) + 90 + offset_deg_;
  }

  void UpdateInputOffset(int new_offset_deg) {
    offset_deg_ = (new_offset_deg) * (-1);
  }
  
  void Write() {
    servo_.write(servo_angle_deg_);
  }

  void PrintAngle() {
    Serial.print("servo_angle: ");
    Serial.print(servo_angle_deg_);
    Serial.print("\n");
  }

};

MPU6050 mpu(MPUADDR);

Gimbal gimbal(SERVO_PIN, 0);

void setup() {
  // Start I2C master
  Serial.begin(BAUDRATE);
  Wire.begin();
  delay(100);

  mpu.Write(MPU6050::PWR_MGMT_1, 0x00); // Wake MPU-6050
  mpu.Write(MPU6050::ACCEL_CONFIG, 0x00); // Change accelerometer config

  gimbal.Attach();
};

void loop() { 
  mpu.ReadAccel();

  gimbal.UpdateSensorAngleAccel(mpu.CurrentAccelX(), mpu.CurrentAccelY());

  if (Serial.available()) {
    gimbal.UpdateInputOffset(Serial.parseInt());
  }

  gimbal.Write();

  mpu.PrintAccel();
  gimbal.PrintAngle();

  delay(100);

}