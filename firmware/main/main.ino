#define PIN_WAKEUP 4
#define PIN_LED 23
#define PIN_BUZZER 22


RTC_DATA_ATTR int boot_count = 0;


void beep_buzzer()
{
    for (int8_t i=0; i<3; i++)
    {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(200);
        digitalWrite(PIN_BUZZER, LOW);
        delay(200);
    }
}

void setup()
{
    // Setup

    Serial.begin(115200);
    delay(1500);

    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, LOW);


    // Wakeup actions

    boot_count++;
    Serial.println("Boot num: " + String(boot_count));

    digitalWrite(PIN_LED, HIGH);
    delay(1000);  // delay to avoid multiple presses
    digitalWrite(PIN_LED, LOW);
    delay(300);

    beep_buzzer();

    // Sleep

    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
