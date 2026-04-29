#include <WiFi.h>
#include <HTTPClient.h>

#define NW_SSID ""
#define NW_PASSWORD ""

#define URL "http://10.190.26.104/send/cli"
#define BODY "msg=ESP32 to ESP8266"


void setup()
{
    Serial.begin(115200);
    delay(1500);

    WiFi.begin(NW_SSID, NW_PASSWORD);

    Serial.println("Connecting to Wi-Fi");
    while(WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(F("."));
    }

    Serial.println("Connected to the network");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void loop()
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

    delay(30000);
}
