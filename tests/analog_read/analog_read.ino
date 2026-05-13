#define PIN_ANALOG 8
#define READ_DELAY 500


void setup()
{
    Serial.begin(115200);
    delay(1500);
}

void loop()
{
    int analog_value = analogRead(PIN_ANALOG);
    Serial.println(analog_value);
    delay(READ_DELAY);
}
