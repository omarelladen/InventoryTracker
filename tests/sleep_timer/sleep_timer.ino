#define TIME_TO_SLEEP 5  // s
#define uS_TO_S_FACTOR 1000000  // us2s


RTC_DATA_ATTR int bootCount = 0;


void setup()
{
    Serial.begin(115200);
    delay(2000);


    bootCount++;
    Serial.println("Boot num: " + String(bootCount));

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);


    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // This is not going to be called
}
