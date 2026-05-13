#define PIN_WAKEUP 4
#define PIN_LED 7


RTC_DATA_ATTR int boot_count = 0;


void setup()
{
    Serial.begin(115200);
    delay(2000);

    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);


    boot_count++;
    Serial.println("Boot num: " + String(boot_count));

    digitalWrite(PIN_LED, HIGH);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, LOW);
    delay(1000);  // delay to avoid multiple presses

    digitalWrite(PIN_LED, LOW);


    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
