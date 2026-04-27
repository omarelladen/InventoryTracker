#include <time.h>


#define PRINT_INTERVAL_S 4


time_t last_time = 0;


void setup()
{
    Serial.begin(115200);
    delay(1500);

    // In the 1st boot the clock starts at 0 (00:00:00 01/01/1970)
}

void loop()
{
    time_t time_now;
    time(&time_now);

    if (time_now - last_time >= PRINT_INTERVAL_S)
    {
        last_time = time_now;

        Serial.print("Timestamp: ");
        Serial.println(time_now);

        // Format to HH:MM:SS
        struct tm info;
        localtime_r(&time_now, &info);

        Serial.print("Time: ");
        Serial.print(info.tm_hour);
        Serial.print(":");
        Serial.print(info.tm_min);
        Serial.print(":");
        Serial.print(info.tm_sec);
        Serial.println();

        Serial.println("-------------------------");
    }
}
