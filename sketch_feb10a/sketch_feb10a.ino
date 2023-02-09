bool light = false;
void setup() {
  // put your setup code here, to run once:
  Serial.print("Script Started");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH);
  light = true;
  delay(500);
  digitalWrite(13, LOW);
  light = false;
  delay(500); 
}
