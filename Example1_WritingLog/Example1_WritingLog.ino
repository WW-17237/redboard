#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog myLog; // Create instance
String File = "HEVHELLO.txt"; // Name of text file

void setup()
{
  Wire.begin(); //Initialize I2C
  myLog.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  
  //Record something to the default log
  //myLog.println("This goes to the log file");
  //Serial.println("This goes to the terminal");

  // Fetch size of file
  long sizeOfFile = myLog.size(File);

  if (sizeOfFile == -1) // Check if file exists. if size > -1 it exists but may be empty.
  {  
    myLog.create("HEVHELLO.txt");
    myLog.println("Welcome to the HEV mark 4 hazardous environment suit");
  } else {
    myLog.append("HEVHELLO.txt");
    myLog.println("Injecting Morphine");
  }

  myLog.syncFile(); // Write to card

  Serial.println();
}

void loop()
{}
