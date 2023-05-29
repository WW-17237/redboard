#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"  // Import the sd card reader library
#include <SparkFun_LPS25HB_Arduino_Library.h>        // Import the pressure sensor library
#include "ICM_20948.h"                               // Import the gyroscope library
#include <Servo.h>                                   // import the library required for servos

LPS25HB basicSensor;  // Create the object for the LPS25HB
ICM_20948_I2C gyro;   // create an gyro object
Servo chuteMotor;     // create servo object for control
//HAN NOTES and what does creating an object let us do?
OpenLog logFile;  // Create instance
ICM_20948_I2C Gyroscope;
icm_20948_DMP_data_t data;
//HAN NOTES merit ticks for descriptive names remember
String File = "data.csv";  // Name of text file
String Dir = "WW-17237-Dir";
const byte SRVPIN = 9;
int pos = 0;  // store the servo's position

//HAN NOTES can you explain what this method is doing?
// this method is triggered on startup
void setup() {
  Serial.begin(9600);
  Wire.begin();
  logFile.begin();            //Open connection to OpenLog
  basicSensor.begin();        // links to I2C port for the sensor
  chuteMotor.attach(SRVPIN);  // attaches the servo on pin byte (SRVPIN)

  //HAN NOTES what is this if statement for?
  // Check the sensor is present
  if (basicSensor.isConnected() == false) {
    Serial.println("What did you do this time the pressure sensor is disconnected.");
    while (1)
      ;
  }


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
    Serial.println("File exists appending");
    logFile.append(File);
    logFile.println("Time(ms), Pressure, Pressure Max, Pressure Min, Temperature, Temperature Max, Temperature Min, AccelerometerX, AccelerometerX Max, AccelerometerX Min, AccelerometerY, AccelerometerY Max, AccelerometerY Min, AccelerometerZ, AccelerometerZ Max, AccelerometerZ Min, GyroX, GyroX Max, GyroX Min, GyroY, GyroY Max, GyroY Min, GyroZ, GyroZ Max, GyroZ Min");
  }

  logFile.syncFile();  // Write to card
}

//HAN NOTES can you explain what this method is doing?
// this method loops all the code within running it all in sequence
void loop() {
  Gyroscope.readDMPdataFromFIFO(&data);  // Read a frame from the data
  while (basicSensor.isConnected() == true) {

    logFile.println(basicSensor.getPressure_hPa());  // Get the pressure reading in hPa
    logFile.println(", ");
    // Don't know how i'll do pressure max or min
    logFile.println(basicSensor.getTemperature_degC());  // Get the temperature in degrees C
    // Don't know how i'll do temp max or min
    logFile.println(", ");

    if ((data.header & DMP_header_bitmap_Accel) > 0)  // Check for change in Accel
    {
      float acc_x = (float)data.Raw_Accel.Data.X;  // Extract the raw accelerometer data
      float acc_y = (float)data.Raw_Accel.Data.Y;
      float acc_z = (float)data.Raw_Accel.Data.Z;

      logFile.print(F("Accel: X:"));
      logFile.print(acc_x);
      logFile.print(F(" Y:"));
      logFile.print(acc_y);
      logFile.print(F(" Z:"));
      logFile.println(acc_z);
    }



    delay(40);           // Wait - 40 ms corresponds to the maximum update rate of the sensor (25 Hz)
    logFile.syncFile();  // Write to card
  }
  //HAN NOTES some note for when the pressuresensor is not connected or disconnects could go here
  //NB that we are already in the loop method that will repeat itself and will need to move a servo at a specific time/pressure at some point
}
//HAN NOTES Now try and add in the servo code, and then the IMU example code...
