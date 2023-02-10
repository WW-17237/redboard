int num = 0;
// number of seconds till the light flashes
int countdown = 1;

void setup()
{
    pinMode(13, LOW);
}
void loop()
{
  num = num + 1;
  if (num > countdown) {
    digitalWrite(13, HIGH);
    delay(500);
    digitalWrite(13, LOW);
    delay(500);
  } else {
    delay(1000);
  }
}