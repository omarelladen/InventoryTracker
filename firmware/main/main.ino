#include <sys/time.h>
#include <WiFi.h>
#include <HTTPClient.h>


#define DEBUG false

#if DEBUG
    #define PRINT(x)        Serial.print(x)
    #define PRINTLN(x)      Serial.println(x)
    #define SERIAL_BEGIN(r) Serial.begin(r)
    #define DELAY(t)        delay(t)
    #define SERIAL_FLUSH()  Serial.flush()
#else
    #define PRINT(x)
    #define PRINTLN(x)
    #define SERIAL_BEGIN(r)
    #define DELAY(t)
    #define SERIAL_FLUSH()
#endif


#define S3 false

#if S3  // Waveshare ESP32-S3-Zero
    #define PIN_WAKEUP 4
    #define PIN_LED 7
    #define PIN_BUZZER 6
    #define PIN_BATTERY 8
#else   // DOIT ESP32 DEVKIT V1
    #define PIN_WAKEUP 4
    #define PIN_LED 23
    #define PIN_BUZZER 22
    #define PIN_BATTERY 34
#endif


#define TIME_LIM_REP_S 10
#define ALERT_REP_COUNT 3

#define NUM_POST_TRIES 3
#define TIMEOUT_WIFI_S 10
#define NW_CHECK_DELAY 300

#define NUM_BEEPS 3
#define BEEP_DELAY 200

#define LED_DELAY 100

#define BAUD_RATE 115200

#define NW_SSID ""
#define NW_PASSWORD ""

#define URL "http://192.168.100.40:8000/alert"
         // "http://10.255.132.80:8000/alert"

#define ITEM_ID 0

// TODO: use vars instead of defines to change based on server response


RTC_DATA_ATTR int boot_count = 0;
RTC_DATA_ATTR int fast_wakeup_count = 0;
RTC_DATA_ATTR time_t last_time_awake = 0;


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

    time(time_now);
    time_t time_start = *time_now;

    PRINTLN("Connecting");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(NW_CHECK_DELAY);
        PRINT(".");

        time(time_now);
        if (*time_now - time_start > TIMEOUT_WIFI_S)
        {
            PRINTLN("Timeout");
            // TODO: save msg in a buffer and send later
            return false;
        }
    }

    PRINTLN("Connected");
    PRINT("IP: ");
    PRINTLN(WiFi.localIP());

    return true;
}

bool post_data(int battery_level, String status)
{
    for (int i=0; i < NUM_POST_TRIES; i++)
    {
        PRINTLN("Will try to POST");

        if (WiFi.status() == WL_CONNECTED)
        {
            HTTPClient http;
            http.begin(URL);

            String body = "{\"item_id\":"     + String(ITEM_ID)       + "," \
                          + "\"status\":"     + "\"" + status + "\""  + "," \
                          + "\"battery\":"    + String(battery_level) + "," \
                          + "\"boot_count\":" + String(fast_wakeup_count) \
                          + "}";

            http.addHeader("Content-Type", "application/json");

            PRINTLN("POSTing");
            int http_response_code = http.POST(body);

            if (http_response_code > 0)
            {
                String response = http.getString();

                PRINTLN("Success sending POST");
                PRINT("Response code: ");
                PRINTLN(http_response_code);
                PRINT("Response: ");
                PRINTLN(response);

                http.end();
                return true;
            }

            PRINTLN("Error sending POST");
            PRINT("Response code: ");
            PRINTLN(http_response_code);

            http.end();
        }
        else
        {
            PRINTLN("Error in connection");
        }
    }

    return false;
}

void setup()
{
    SERIAL_BEGIN(BAUD_RATE);
    DELAY(1500);

    pinMode(PIN_WAKEUP, INPUT_PULLUP);
    pinMode(PIN_LED,    OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);

    esp_sleep_enable_ext0_wakeup((gpio_num_t)PIN_WAKEUP, LOW);


    // Read battery level
    int battery_level = analogRead(PIN_BATTERY);
    PRINT("Battery: ");
    PRINTLN(battery_level);


    time_t time_now;
    time(&time_now);

    time_t time_diff = time_now - last_time_awake;

    PRINT("Time: ");
    PRINTLN(time_now);
    PRINT("Last time awake: ");
    PRINTLN(last_time_awake);
    PRINT("Diff: ");
    PRINTLN(time_diff);

    if (boot_count == 0)
    {
        if (connect_wifi(&time_now))
            post_data(battery_level, "reset");
    }

    if (time_diff < TIME_LIM_REP_S)
    {
        fast_wakeup_count++;
        PRINT("Repeated wakeup! Count: ");
        PRINTLN(fast_wakeup_count);
    }
    else
    {
        // Reset count
        fast_wakeup_count = 1;
        PRINTLN("Normal wakeup");
    }


    if (fast_wakeup_count > ALERT_REP_COUNT)
    {
        // Alert
        beep_buzzer();

        // Try to connect to Wi-Fi and send risk alert
        if (connect_wifi(&time_now))
            post_data(battery_level, "risk");
    }

    // TODO: prepare wakeup at a determined time for daily ping or retry


    boot_count++;
    PRINT("Boot num: ");
    PRINTLN(boot_count);

    digitalWrite(PIN_LED, HIGH);
    delay(LED_DELAY);


    // Sleep

    PRINTLN("Going to sleep now");
    SERIAL_FLUSH();

    time(&time_now);
    last_time_awake = time_now;

    esp_deep_sleep_start();
}

void loop()
{
    // not called
}
