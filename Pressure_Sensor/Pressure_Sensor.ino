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

<<<<<<< HEAD
  if (basicSensor.isConnected() == false) // Check the sensor is present
  {
    Serial.println("LPS25HB disconnected. Reset the board to try again.");
    while (1);
=======
  //HAN NOTES what is this if statement for?
  if (pressureSensor.isConnected() == false) // Check the sensor is present
  {
    Serial.println("What did you do this time the LPS25HB pressure sensor disconnected.");
    while (1)
      ;
>>>>>>> f6e04abc0a16fccdab8b11c1db0270b31a5f3af5
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

//HAN NOTES can you explain what this method is doing?
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
    //HAN NOTES some note for when the pressuresensor is not connected or disconnects could go here
  //NB that we are already in the loop method that will repeat itself and will need to move a servo at a specific time/pressure at some point
}
//HAN NOTES Now try and add in the servo code, and then the IMU example code...
