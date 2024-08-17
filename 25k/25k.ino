#include <Wire.h>
#include <SoftwareSerial.h>

// MPU6050 setup
const int MPU = 0x68; // MPU6050 I2C address
float AccX, AccY, AccZ;
float GyroX, GyroY, GyroZ;
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ;
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;
float gForce;
const float gForceThreshold = 1.3; // G-Force threshold for detecting an accident
const float rollPitchThreshold = 45.0; // Angle threshold for roll and pitch to detect rollover

// SIM900A setup
SoftwareSerial sim900a(2, 3); // RX, TX

void setup() {
  Serial.begin(19200); // Initialize serial communication with the computer
  Wire.begin(); // Initialize I2C communication
  sim900a.begin(9600); // Initialize serial communication with the SIM900A
  delay(1000);

  // MPU6050 initialization
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); // Wake up MPU6050
  Wire.write(0x00);
  Wire.endTransmission(true);

  // Calculate IMU error
  calculate_IMU_error();
  delay(20);

  // Initialize SIM900A
  Serial.println("Initializing SIM900A...");
  sendCommand("AT");
  sendCommand("AT+CMGF=1"); // Set SMS mode to text mode
}

void loop() {
  // === Read accelerometer data === //
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  // For a range of +-2g, we need to divide the raw values by 16384, according to the datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value

  // Calculate G-Force
  gForce = sqrt(pow(AccX, 2) + pow(AccY, 2) + pow(AccZ, 2));

  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - 0.58; // AccErrorX ~(0.58) See the calculate_IMU_error() custom function for more details
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) + 1.58; // AccErrorY ~(-1.58)
  
  // === Read gyroscope data === //
  previousTime = currentTime; // Previous time is stored before the actual time read
  currentTime = millis(); // Current time actual time read
  elapsedTime = (currentTime - previousTime) / 1000.0; // Divide by 1000 to get seconds
  Wire.beginTransmission(MPU);
  Wire.write(0x43); // Gyro data first register address 0x43
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0; // For a 250deg/s range we have to divide first the raw value by 131.0, according to the datasheet
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
  // Correct the outputs with the calculated error values
  GyroX = GyroX + 0.56; // GyroErrorX ~(-0.56)
  GyroY = GyroY - 2; // GyroErrorY ~(2)
  GyroZ = GyroZ + 0.79; // GyroErrorZ ~ (-0.😎
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by seconds (s) to get the angle in degrees
  gyroAngleX = gyroAngleX + GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY = gyroAngleY + GyroY * elapsedTime;
  yaw = yaw + GyroZ * elapsedTime;
  // Complementary filter - combine accelerometer and gyro angle values
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;

  // Check if G-Force exceeds the threshold
  if (gForce > gForceThreshold) {
    Serial.println("Accident happened");
    sendSMS("+639625693492", "This individual is in need of medical assistance. Your immediate attention is highly appreciated. ");
  }


  // Print the values on the serial monitor
  Serial.print("Roll: ");
  Serial.print(roll);
  Serial.print(" / Pitch: ");
  Serial.print(pitch);
  Serial.print(" / Yaw: ");
  Serial.print(yaw);
  Serial.print(" / G-Force: ");
  Serial.println(gForce);
  delay(1000);
}

void calculate_IMU_error() {
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    AccX = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccY = (Wire.read() << 8 | Wire.read()) / 16384.0;
    AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0;
    // Sum all readings
    AccErrorX = AccErrorX + ((atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI));
    AccErrorY = AccErrorY + ((atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI));
    c++;
  }
  // Divide the sum by 200 to get the error value
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;
  // Read gyro values 200 times
  while (c < 200) {
    Wire.beginTransmission(MPU);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU, 6, true);
    GyroX = Wire.read() << 8 | Wire.read();
    GyroY = Wire.read() << 8 | Wire.read();
    GyroZ = Wire.read() << 8 | Wire.read();
    // Sum all readings
    GyroErrorX = GyroErrorX + (GyroX / 131.0);
    GyroErrorY = GyroErrorY + (GyroY / 131.0);
    GyroErrorZ = GyroErrorZ + (GyroZ / 131.0);
    c++;
  }
  // Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  // Print the error values on the Serial Monitor
  Serial.print("AccErrorX: ");
  Serial.println(AccErrorX);
  Serial.print("AccErrorY: ");
  Serial.println(AccErrorY);
  Serial.print("GyroErrorX: ");
  Serial.println(GyroErrorX);
  Serial.print("GyroErrorY: ");
  Serial.println(GyroErrorY);
  Serial.print("GyroErrorZ: ");
  Serial.println(GyroErrorZ);
}

void sendSMS(String number, String message) {
  Serial.println("Sending SMS...");
  sim900a.print("AT+CMGS=\"");
  sim900a.print(number);
  sim900a.println("\"");
  delay(1000);

  // Wait for the '>' prompt
  unsigned long startTime = millis();
  bool promptReceived = false;
  while (millis() - startTime < 5000) { // Wait for up to 5 seconds
    if (sim900a.available()) {
      char c = sim900a.read();
      Serial.write(c);
      if (c == '>') {
        Serial.println("\nPrompt received, sending message...");
        promptReceived = true;
        break;
      }
    }
  }

  if (promptReceived) {
    sim900a.print(message);
    delay(1000);
    sim900a.write(26); // ASCII code of CTRL+Z to send the SMS
    delay(5000); // Increase delay to ensure message is sent

    Serial.println("Message sent!");

    // Read the response
    while (sim900a.available()) {
      char c = sim900a.read();
      Serial.write(c);
    }

    // Clear the buffer
    while (sim900a.available()) {
      sim900a.read();
    }
  } else {
    Serial.println("Failed to receive prompt, SMS not sent.");
  }
}

void sendCommand(String command) {
  Serial.print("Sending command: ");
  Serial.println(command);
  sim900a.println(command);
  delay(1000);
  while (sim900a.available()) {
    char c = sim900a.read();
    Serial.write(c); // Display any responses from the SIM900A
  }
}