#include <sys/time.h>

#define PIN_WAKEUP 4
#define PIN_LED 23
#define PIN_BUZZER 22

#define TIME_LIM_REP_S 10


RTC_DATA_ATTR int boot_count = 0;
RTC_DATA_ATTR int fast_wakeup_count = 0;
RTC_DATA_ATTR unsigned long last_time_awake = 0;


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


    // Time

    time_t time_now;
    time(&time_now);

    unsigned long time_diff = time_now - last_time_awake;

    Serial.print("Time: ");
    Serial.println(time_now);
    Serial.print("Last time awake: ");
    Serial.println(last_time_awake);
    Serial.print("Diff: ");
    Serial.println(time_diff);

    if (time_diff < TIME_LIM_REP_S)
    {
        fast_wakeup_count++;
        Serial.print("Repeated wakeup! Count: ");
        Serial.println(fast_wakeup_count);
    }
    else
    {
        // Reset count
        fast_wakeup_count = 1;
        Serial.println("Normal wakeup");
    }

    last_time_awake = time_now;

    if (fast_wakeup_count > 3)  // repeated event
    {
        Serial.println("WARNING: repeated wakeup!");

        // Alert
        beep_buzzer();
    }


    // Wakeup actions

    boot_count++;
    Serial.println("Boot num: " + String(boot_count));

    digitalWrite(PIN_LED, HIGH);
    delay(1000);  // delay to avoid multiple presses
    digitalWrite(PIN_LED, LOW);
    delay(300);

    // Sleep

    Serial.println("Going to sleep now");
    Serial.flush();

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
