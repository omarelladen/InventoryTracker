#define TIME_TO_SLEEP 5  // s
#define US_TO_S_FACTOR 1000000  // us2s


RTC_DATA_ATTR int boot_count = 0;


void setup()
{
    Serial.begin(115200);
    delay(2000);


    boot_count++;
    Serial.println("Boot num: " + String(boot_count));

    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * US_TO_S_FACTOR);


    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
