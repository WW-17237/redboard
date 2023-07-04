#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"  // Import the sd card reader library
#include <SparkFun_LPS25HB_Arduino_Library.h>        // Import the pressure sensor library
#include "ICM_20948.h"                               // Import the gyroscope library
#include <Servo.h>                                   // import the library required for servos

LPS25HB basicSensor;  // Create the object for the LPS25HB
Servo chuteMotor;     // create servo object for control
OpenLog logFile;      // Create instance
ICM_20948_I2C Gyroscope;
icm_20948_DMP_data_t data;  // create gyro object
String File = "data.csv";   // Name of text file
String Dir = "WW-17237-Dir";
float acc_z, acc_y, acc_x;  // Acceleration floats
double roll, pitch, yaw;    // Euler angle doubles
float maxTemp, minTemp;     // MinMax temperature floats
float maxPress, minPress;   // MinMax pressure floats
const byte SRVPIN = 9;
bool chuteDeployed = false;

// this method is triggered on startup
void setup() {
  Serial.begin(9600);
  Wire.begin();
  logFile.begin();            //Open connection to OpenLog
  basicSensor.begin();        // links to I2C port for the sensor
  chuteMotor.attach(SRVPIN);  // attaches the servo on pin byte (SRVPIN)
  chuteMotor.write(0);        // Reset the servo to 0 degrees
  // Check the sensor is present
  if (basicSensor.isConnected() == false) {
    Serial.println("What did you do this time the pressure sensor is disconnected.");
    while (1)
      ;
  }
  //HAN NOTES this will set the max and min to the same value that will then cause issues in you if statements lower down?
  maxTemp , minTemp = basicSensor.getTemperature_degC();
  maxPress , minPress = basicSensor.getPressure_hPa();

  logFile.changeDirectory("..");  //Make sure we're in the root directory
  logFile.makeDirectory(Dir);     // Create directory
  logFile.changeDirectory(Dir);   //Go into this new directory
  long sizeOfFile = logFile.size(File);

  if (sizeOfFile == -1)  // Check if file exists. if size > -1 it exists but may be empty.
  {                      // File didn't exist.
    Serial.println("File Didn't exist");
    logFile.append(File);
    logFile.println("Time(ms), Pressure, Pressure Max, Pressure Min, Temperature, Temperature Max, Temperature Min, AccelerometerX, AccelerometerX Max, AccelerometerX Min, AccelerometerY, AccelerometerY Max, AccelerometerY Min, AccelerometerZ, AccelerometerZ Max, AccelerometerZ Min, Roll, GyroX Max, GyroX Min, Pitch, GyroY Max, GyroY Min, Yaw, GyroZ Max, GyroZ Min");
  } else {  // File existed prior to script running
    Serial.println("File exists wiping");
    logFile.removeFile(File);
    logFile.append(File);
    logFile.println("Time(ms), Pressure, Pressure Max, Pressure Min, Temperature, Temperature Max, Temperature Min, AccelerometerX, AccelerometerX Max, AccelerometerX Min, AccelerometerY, AccelerometerY Max, AccelerometerY Min, AccelerometerZ, AccelerometerZ Max, AccelerometerZ Min, Roll, GyroX Max, GyroX Min, Pitch, GyroY Max, GyroY Min, Yaw, GyroZ Max, GyroZ Min");
  }

  logFile.syncFile();  // Write to card //HAN NOTES it just pauses here to check that any r/w has finished before moving on
}

// this method loops all the code within running it all in sequence
void loop() {
  while (basicSensor.isConnected() && Gyroscope.isConnected()) {
    gyroData();
    cacheMinMax();
    storeData();

    if (acc_z <= 0.2) {  // if acceleration is < 0.2 trigger chute
      chuteOpen();       // Trigger servo method
    }

    delay(40);  // Wait 40 ms the maximum update rate of the sensor (25 Hz)
  }
}

// Grab data from the Gyro and stores it in variables for later use //HAN NOTES and what does it do with it?
void gyroData() {
  Gyroscope.readDMPdataFromFIFO(&data);             // Read a frame from gyro
  if ((data.header & DMP_header_bitmap_Quat6) > 0)  // We have asked for GRV data so we should receive Quat6
  {
    // Scale to +/- 1
    double q1 = ((double)data.Quat6.Data.Q1) / 1073741824.0;  // Convert to double. Divide by 2^30
    double q2 = ((double)data.Quat6.Data.Q2) / 1073741824.0;  // Convert to double. Divide by 2^30
    double q3 = ((double)data.Quat6.Data.Q3) / 1073741824.0;  // Convert to double. Divide by 2^30
    double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));
    double q2sqr = q2 * q2;
    // roll (x-axis rotation)
    double t0 = +2.0 * (q0 * q1 + q2 * q3);
    double t1 = +1.0 - 2.0 * (q1 * q1 + q2sqr);
    roll = atan2(t0, t1) * 180.0 / PI;
    // pitch (y-axis rotation)
    double t2 = +2.0 * (q0 * q2 - q3 * q1);
    t2 = t2 > 1.0 ? 1.0 : t2;
    t2 = t2 < -1.0 ? -1.0 : t2;
    pitch = asin(t2) * 180.0 / PI;
    // yaw (z-axis rotation)
    double t3 = +2.0 * (q0 * q3 + q1 * q2);
    double t4 = +1.0 - 2.0 * (q2sqr + q3 * q3);
    yaw = atan2(t3, t4) * 180.0 / PI;

    acc_x = (float)data.Raw_Accel.Data.X;  // Extract the raw accelerometer data
    acc_y = (float)data.Raw_Accel.Data.Y;
    acc_z = (float)data.Raw_Accel.Data.Z;
  }
}

// This method stores all the maximum and minimum values for temperature and pressure
void cacheMinMax() {
  // Get max temp if the current temp is higher update max temp
  if (maxTemp < basicSensor.getTemperature_degC()) {
    maxTemp = basicSensor.getTemperature_degC();
  }
  // Get min temp if current temp is lower then update min temp
  if (minTemp > basicSensor.getTemperature_degC()) {
    minTemp = basicSensor.getTemperature_degC();
  }
  // Get max pressure by storing whatever the highest pressure
  // is between launch and current time
  if (maxPress < basicSensor.getPressure_hPa()) {
    maxPress = basicSensor.getPressure_hPa();
  }
  // get the min pressure by storing the lowest recorded pressure
  // between launch and current time
  if (minPress > basicSensor.getPressure_hPa()) {
    minPress = basicSensor.getPressure_hPa();
  }
}

// This method prints all the data to a csv file
void storeData() {
  // Write sensor data to CSV file
  logFile.println(basicSensor.getPressure_hPa());  // Get the pressure reading in hPa
  logFile.print(",");
  logFile.print(maxPress);  // trip highest pressure
  logFile.print(",");
  logFile.print(minPress);  // trip lowest pressure
  logFile.print(",");
  logFile.print(basicSensor.getTemperature_degC());  // Get the temperature in degrees C
  logFile.print(",");
  logFile.print(maxTemp);  // trip highest temp
  logFile.print(",");
  logFile.print(minTemp);  // trip lowest temp
  logFile.print(",");
  logFile.print(acc_x);  // Current acceleration on x axis
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelXMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelXMin());
  logFile.print(",");
  logFile.print(acc_y);  // Current acceleration on y axis
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelYMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelYMin());
  logFile.print(",");
  logFile.print(acc_z);  // Current acceleration on z axis
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelZMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getAccelZMin());
  logFile.print(",");
  logFile.print(roll);
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroXMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroXMin());
  logFile.print(",");
  logFile.print(pitch);
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroYMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroYMin());
  logFile.print(",");
  logFile.print(yaw);
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroZMax());
  logFile.print(",");
  //logFile.print(Gyroscope.getGyroZMin());
  logFile.syncFile();  // Write to card
}

// Method for opening the chute
void chuteOpen() {  
  if (!chuteDeployed) {
    chuteDeployed = true;
    chuteMotor.write(90);
  }
}
