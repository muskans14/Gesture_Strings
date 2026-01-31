#include <Wire.h>

const int MPU_ADDR = 0x68;

int16_t ax, ay, az;
float baseMag = 0;

void readAccel() {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 6, true);

  ax = Wire.read() << 8 | Wire.read();
  ay = Wire.read() << 8 | Wire.read();
  az = Wire.read() << 8 | Wire.read();
}

long magnitude() {
  return sqrt((long)ax * ax + (long)ay * ay + (long)az * az);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Wake MPU6050
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  // Calibrate gravity magnitude
  long sum = 0;
  for (int i = 0; i < 150; i++) {
    readAccel();
    sum += magnitude();
    delay(5);
  }
  baseMag = sum / 150.0;
}

void loop() {
  readAccel();

  // Roll angle (expanded range)
  float roll = atan2(ay, az) * 180.0 / PI;
  roll *= 2.5;   // IMPORTANT: allows all 6 strings to be crossed

  // Strum force (gravity removed)
  int force = abs(magnitude() - baseMag) / 50;

  // EXACT format required by Python
  Serial.print(roll, 1);
  Serial.print(":");
  Serial.println(force);

  delay(10); // ~100 Hz
}
