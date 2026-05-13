#define PIN_LED 13
#define BLINK_DELAY 1000


void setup()
{
    pinMode(PIN_LED, OUTPUT);
}

void loop()
{
    digitalWrite(PIN_LED, HIGH);
    delay(BLINK_DELAY);
    digitalWrite(PIN_LED, LOW);
    delay(BLINK_DELAY);
}
