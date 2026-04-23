#define wakeupPin 4
#define ledPin 23


RTC_DATA_ATTR int bootCount = 0;


void setup()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(wakeupPin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);


    bootCount++;
    Serial.println("Boot num: " + String(bootCount));

    digitalWrite(ledPin, HIGH);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)wakeupPin, LOW);
    delay(1000);  // delay to avoid multiple presses

    digitalWrite(ledPin, LOW);


    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
