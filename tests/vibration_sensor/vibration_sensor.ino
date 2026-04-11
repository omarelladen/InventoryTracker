
#define PIN_D7 7

void setup()
{
    pinMode(PIN_D7, INPUT);
    Serial.begin(9600);
}

void loop()
{
    int8_t sensor_read = digitalRead(PIN_D7);
    if (!sensor_read)
    {
        Serial.println("1");
        delay(100);
    }
}
