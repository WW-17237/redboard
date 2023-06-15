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
float acc_z;
float acc_y;
float acc_x;
float maxTemp;
float minTemp;
float maxPress;
float minPress;
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
    while (1);
  }
//HAN NOTES this will set the max and min to the same value that will then cause issues in you if statements lower down?
  maxTemp = basicSensor.getTemperature_degC();
  minTemp = basicSensor.getTemperature_degC();
  maxPress = basicSensor.getPressure_hPa();
  minPress = basicSensor.getPressure_hPa();

  logFile.changeDirectory("..");  //Make sure we're in the root directory
  logFile.makeDirectory(Dir);     // Create directory
  logFile.changeDirectory(Dir);   //Go into this new directory
  long sizeOfFile = logFile.size(File);

  if (sizeOfFile == -1)  // Check if file exists. if size > -1 it exists but may be empty.
  {                      // File didn't exist.
    Serial.println("File Didn't exist");
    logFile.append(File);
    logFile.println("Time(ms), Pressure, Pressure Max, Pressure Min, Temperature, Temperature Max, Temperature Min, AccelerometerX, AccelerometerX Max, AccelerometerX Min, AccelerometerY, AccelerometerY Max, AccelerometerY Min, AccelerometerZ, AccelerometerZ Max, AccelerometerZ Min, GyroX, GyroX Max, GyroX Min, GyroY, GyroY Max, GyroY Min, GyroZ, GyroZ Max, GyroZ Min");
  } else {  // File existed prior to script running
    Serial.println("File exists wiping");
    logFile.removeFile(File);
    logFile.append(File);
    logFile.println("Time(ms), Pressure, Pressure Max, Pressure Min, Temperature, Temperature Max, Temperature Min, AccelerometerX, AccelerometerX Max, AccelerometerX Min, AccelerometerY, AccelerometerY Max, AccelerometerY Min, AccelerometerZ, AccelerometerZ Max, AccelerometerZ Min, GyroX, GyroX Max, GyroX Min, GyroY, GyroY Max, GyroY Min, GyroZ, GyroZ Max, GyroZ Min");
  }

  logFile.syncFile();  // Write to card
}

// this method loops all the code within running it all in sequence
void loop() {
  //HAN NOTES I would suggest taking this chunk of code that deals with the sensors and having it in is own method like your servo
  //HAN NOTES just to keep the code easier to read the order of things in the loop
  Gyroscope.readDMPdataFromFIFO(&data);  // Read a frame from the data
  while (basicSensor.isConnected() == true) { //HAN NOTES what about the advanced IMU sensor? or do you have two of these if statements
    acc_x = (float)data.Raw_Accel.Data.X;  // Extract the raw accelerometer data
    acc_y = (float)data.Raw_Accel.Data.Y;
    acc_z = (float)data.Raw_Accel.Data.Z;

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

    storeData();

    if (acc_z <= 0.2) {  // if accelleration is < 0.2 trigger chute
      chuteOpen();       // Trigger servo method
    }

    delay(40); // Wait 40 ms the maximum update rate of the sensor (25 Hz)
   }
}

void storeData() {
  // Write sensor data to CSV file
    logFile.println(basicSensor.getPressure_hPa());  // Get the pressure reading in hPa
    logFile.print(",");
    logFile.print(maxPress); // trip highest pressure
    logFile.print(",");
    logFile.print(minPress); // trip lowest pressure
    logFile.print(",");
    logFile.print(basicSensor.getTemperature_degC());  // Get the temperature in degrees C
    logFile.print(",");
    logFile.print(maxTemp); // trip highest temp
    logFile.print(",");
    logFile.print(minTemp); // trip lowest temp
    logFile.print(",");
    logFile.print(acc_x);
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelXMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelXMin());
    logFile.print(",");
    logFile.print(acc_y);
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelYMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelYMin());
    logFile.print(",");
    logFile.print(acc_z);
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelZMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getAccelZMin());
    logFile.print(",");
    //logFile.print(gyroX);
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroXMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroXMin());
    logFile.print(",");
    //logFile.print(gyroY);
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroYMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroYMin());
    logFile.print(",");
    //logFile.print(gyroZ);
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroZMax());
    logFile.print(",");
    //logFile.print(Gyroscope.getGyroZMin());
    logFile.syncFile();  // Write to card
}

void chuteOpen() {  // Method for opening the chute
  if (!chuteDeployed) {
    chuteDeployed = true;
    chuteMotor.write(90);
  }
}

