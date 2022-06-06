#define HEAT_PIN 12
#define FAN_PIN 10
byte lastButtonState = LOW;
byte currentButtonState = LOW;
unsigned long lastButtonDebounceTime = 0;
unsigned long buttonDebounceDelay = 20;
int serialcode = 0;

void powerOffAll()
{
    digitalWrite(HEAT_PIN, LOW);
    digitalWrite(FAN_PIN, LOW);
}
void setup()
{
  Serial.begin(9600);
  pinMode(HEAT_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  powerOffAll();
}
void loop()
{
  if(Serial.available() > 0) {
    serialcode = Serial.parseInt();
  }
  if (serialcode == 10) {
      digitalWrite(HEAT_PIN, HIGH);
    }
  if (serialcode == 20) {
      digitalWrite(HEAT_PIN, LOW);
    }
  if (serialcode == 30) {
      digitalWrite(FAN_PIN, HIGH);
    }
  if (serialcode == 40) {
      digitalWrite(FAN_PIN, LOW);
    }
}
