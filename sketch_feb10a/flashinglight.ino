bool light = false;
void setup() {
  // put your setup code here, to run once:
  Serial.print("Script Started");
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(13, HIGH); // Turn the light on
  light = true; // Little bool to tell when the light is on
  delay(500); // half second delay
  digitalWrite(13, LOW); // turn the light off
  light = false; // Little bool to tell when the light is on
  delay(500); // half second delay
}
