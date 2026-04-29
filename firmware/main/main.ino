#include <sys/time.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define PIN_WAKEUP 4
#define PIN_LED 23
#define PIN_BUZZER 22

#define TIME_LIM_REP_S 20
#define ALERT_REP_COUNT 3

#define NUM_BEEPS 3
#define NUM_NW_TRIES 3

#define NW_SSID ""
#define NW_PASSWORD ""

#define URL "http://jsonplaceholder.typicode.com/posts"  // "http://10.190.26.104/send/cli"
#define BODY "msg=Alert"


RTC_DATA_ATTR int boot_count = 0;
RTC_DATA_ATTR int fast_wakeup_count = 0;
RTC_DATA_ATTR unsigned long last_time_awake = 0;


void beep_buzzer()
{
    for (int8_t i=0; i < NUM_BEEPS; i++)
    {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(200);
        digitalWrite(PIN_BUZZER, LOW);
        delay(200);
    }
}

void connect_wifi()
{
    WiFi.begin(NW_SSID, NW_PASSWORD);

    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }

    Serial.println("Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void post_data()
{
    bool success = false;
    for (int i=0; i < NUM_NW_TRIES && success == false; i++)
    {
        Serial.println("Will try to POST");

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;
            http.begin(URL);
            http.addHeader("Content-Type", "text/plain");

            Serial.println("POSTing");
            int http_response_code = http.POST(BODY);

            if (http_response_code > 0)
            {
                String response = http.getString();

                Serial.println("Success sending POST");
                Serial.print("Response code: ");
                Serial.println(http_response_code);
                Serial.print("Response: ");
                Serial.println(response);

                success = true;
            }
            else
            {
                Serial.println("Error sending POST");
                Serial.print("Response code: ");
                Serial.println(http_response_code);
            }

            http.end();
        }
        else
        {
            Serial.println("Error in connection");
        }
    }
}

void setup()
{
    Serial.begin(115200);
    delay(1500);

    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_LED,    OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, LOW);


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

        // TODO: try to connect to Wi-Fi and send basic alert or store info
    }

    last_time_awake = time_now;

    if (fast_wakeup_count > ALERT_REP_COUNT)  // repeated event
    {
        Serial.println("WARNING: repeated wakeup!");

        // Alert
        beep_buzzer();

        // TODO: try to connect to Wi-Fi and send risk alert
        connect_wifi();
        post_data();
    }

    // TODO: prepare wakeup at a determined time for daily ping or retry


    boot_count++;
    Serial.println("Boot num: " + String(boot_count));

    digitalWrite(PIN_LED, HIGH);
    delay(1000);
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
