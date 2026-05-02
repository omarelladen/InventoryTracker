#include <sys/time.h>
#include <WiFi.h>
#include <HTTPClient.h>


#define PIN_WAKEUP 4
#define PIN_LED 23
#define PIN_BUZZER 22
#define PIN_BATTERY 34

#define TIME_LIM_REP_S 10
#define ALERT_REP_COUNT 3

#define NUM_POST_TRIES 3
#define TIMEOUT_WIFI_S 10

#define NUM_BEEPS 3
#define BEEP_DELAY 200

#define NW_SSID ""
#define NW_PASSWORD ""

#define URL "http://192.168.100.40:8000/alert"  // "http://jsonplaceholder.typicode.com/posts"

#define ITEM_ID 0

// TODO: use vars instead of defines to change based on server response


RTC_DATA_ATTR int boot_count = 0;
RTC_DATA_ATTR int fast_wakeup_count = 0;
RTC_DATA_ATTR unsigned long last_time_awake = 0;


void beep_buzzer()
{
    for (int i=0; i < NUM_BEEPS; i++)
    {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(BEEP_DELAY);
        digitalWrite(PIN_BUZZER, LOW);
        delay(BEEP_DELAY);
    }
}

bool connect_wifi(time_t *time_now)
{
    WiFi.begin(NW_SSID, NW_PASSWORD);

    // TODO: set timeout
    time(time_now);
    unsigned long time_start = *time_now;

    Serial.println("Connecting");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));

        time(time_now);
        if (*time_now - time_start > TIMEOUT_WIFI_S)
        {
            Serial.println("Timeout");
            return false;
        }
    }

    Serial.println("Connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    return true;
}

bool post_data(int battery_level)
{
    for (int i=0; i < NUM_POST_TRIES; i++)
    {
        Serial.println("Will try to POST");

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;
            http.begin(URL);

            String body = "{\"item_id\":" + String(ITEM_ID) + "," \
                          + "\"status\":\"alert\"," \
                          + "\"battery\":" + String(battery_level) + "," \
                          + "\"boot_count\":" + String(fast_wakeup_count) \
                          + "}";

            http.addHeader("Content-Type", "application/json");

            Serial.println("POSTing");
            int http_response_code = http.POST(body);

            if (http_response_code > 0)
            {
                String response = http.getString();

                Serial.println("Success sending POST");
                Serial.print("Response code: ");
                Serial.println(http_response_code);
                Serial.print("Response: ");
                Serial.println(response);

                http.end();
                return true;
            }

            Serial.println("Error sending POST");
            Serial.print("Response code: ");
            Serial.println(http_response_code);

            http.end();
        }
        else
        {
            Serial.println("Error in connection");
        }
    }

    return false;
}

void setup()
{
    Serial.begin(115200);
    delay(1500);

    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_LED,    OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, LOW);


    // Read battery level
    int battery_level = analogRead(PIN_BATTERY);
    Serial.print("Battery: ");
    Serial.println(battery_level);


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


    if (fast_wakeup_count > ALERT_REP_COUNT)  // repeated event
    {
        Serial.println("WARNING: repeated wakeup!");

        // Alert
        beep_buzzer();

        // Try to connect to Wi-Fi and send risk alert
        if (connect_wifi(&time_now))
            post_data(battery_level);
    }

    // TODO: prepare wakeup at a determined time for daily ping or retry


    boot_count++;
    Serial.print("Boot num: ");
    Serial.println(boot_count);

    digitalWrite(PIN_LED, HIGH);
    delay(200);
    digitalWrite(PIN_LED, LOW);
    delay(200);


    // Sleep

    Serial.println("Going to sleep now");
    Serial.flush();

    time(&time_now);
    last_time_awake = time_now;

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
