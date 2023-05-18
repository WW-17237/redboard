#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
#include <SparkFun_LPS25HB_Arduino_Library.h> // Import the pressure sensor library
LPS25HB basicSensor; // Create the object for the LPS25HB
OpenLog logFile;         // Create instance
String File = "LPS25HB.txt";  // Name of text file
String Dir = "WW-17237-Dir";

void setup() {
  Serial.begin(9600);
  Wire.begin();
  logFile.begin();  //Open connection to OpenLog
  basicSensor.begin(); // links to I2C port for the sensor

  if (basicSensor.isConnected() == false) // Check the sensor is present
  {
    Serial.println("LPS25HB disconnected. Reset the board to try again.");
    while (1);
  }


  logFile.changeDirectory(".."); //Make sure we're in the root directory
  logFile.makeDirectory(Dir); // Create directory
  logFile.changeDirectory(Dir); //Go into this new directory
  long sizeOfFile = logFile.size(File);

  if (sizeOfFile == -1)  // Check if file exists. if size > -1 it exists but may be empty.
  { // File didn't exist.
    Serial.println("File Didn't exist");
    logFile.append(File);
    logFile.println("// Begin File //");
  } else {  // File existed prior to script running
    Serial.println("File exists appending");
    logFile.append(File);
    logFile.println("// New Session //");
  }

  logFile.syncFile();  // Write to card
}

void loop()
{
  while(basicSensor.isConnected() == true) {
    logFile.println("Pressure in hPa: ");
    logFile.println(basicSensor.getPressure_hPa()); // Get the pressure reading in hPa
    logFile.println(", ");
    logFile.println(basicSensor.getTemperature_degC()); // Get the temperature in degrees C
    delay(40); // Wait - 40 ms corresponds to the maximum update rate of the sensor (25 Hz)
    logFile.syncFile();  // Write to card
  }
}
