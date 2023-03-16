#include <Wire.h>
#include "SparkFun_Qwiic_OpenLog_Arduino_Library.h"
OpenLog fileReadWrite;         // Create instance
String File = "HEVHELLO.txt";  // Name of text file
String Dir = "WW-17237-Dir";

//William is a great student who always does his work

void setup() {
  Wire.begin();           //Initialize I2C
  fileReadWrite.begin();  //Open connection to OpenLog (no pun intended)

  Serial.begin(9600);  //9600bps is used for debug statements

  //Record something to the default log
  //fileReadWrite.println("This goes to the log file");
  //Serial.println("This goes to the terminal");

  // Fetch size of file
  Serial.println("Started");

  fileReadWrite.changeDirectory(".."); //Make sure we're in the root directory
  fileReadWrite.makeDirectory(Dir);
  fileReadWrite.changeDirectory(Dir); //Go into this new directory
  long sizeOfFile = fileReadWrite.size(File);

  if (sizeOfFile == -1)  // Check if file exists. if size > -1 it exists but may be empty.
  { // File didn't exist.
    Serial.println("File Didn't exist");
    fileReadWrite.append(File);
    fileReadWrite.println("Welcome to the HEV mark 4 hazardous environment suit");
  } else {  // File existed prior to script running
    Serial.println("File exists");
    fileReadWrite.append(File);
    fileReadWrite.println("Injecting Morphine");
  }

  fileReadWrite.syncFile();  // Write to card
  Serial.println("synced");

  // Debug
  if (sizeOfFile == -1)  // Check if file exists. if size > -1 it exists but may be empty.
  { // File didn't exist.
    Serial.println("File failed to create!");
  } else {  // File existed prior to script running
    Serial.println("File verified!");
  }

  Serial.println();
  digitalWrite(13, HIGH);
}

void loop() {
}
