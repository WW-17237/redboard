#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog fileReadWrite; // Create instance
String File = "HEVHELLO.txt"; // Name of text file

//William is a great student who always does his work

void setup()
{
  Wire.begin(); //Initialize I2C
  fileReadWrite.begin(); //Open connection to OpenLog (no pun intended)

  Serial.begin(9600); //9600bps is used for debug statements
  
  //Record something to the default log
  //fileReadWrite.println("This goes to the log file");
  //Serial.println("This goes to the terminal");

  // Fetch size of file
  long sizeOfFile = fileReadWrite.size(File);

  if (sizeOfFile == -1) // Check if file exists. if size > -1 it exists but may be empty.
  {  // File didn't exist.
    fileReadWrite.create("HEVHELLO.txt");
    fileReadWrite.println("Welcome to the HEV mark 4 hazardous environment suit");
  } else { // File existed prior to script running
    fileReadWrite.append("HEVHELLO.txt");
    fileReadWrite.println("Injecting Morphine");
  }

  fileReadWrite.syncFile(); // Write to card

  Serial.println();
}

void loop()
{}
