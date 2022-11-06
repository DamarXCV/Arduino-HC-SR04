#include <Arduino.h>

#include "HC_SR04.h"

constexpr uint16_t LOOP_HZ = 1;
constexpr uint8_t TRIGGER_PIN = 40;
constexpr uint8_t ECHO_PIN = 41;

HC_SR04 sonar(TRIGGER_PIN, ECHO_PIN);

void setup()
{
    // Setup blink led
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);
    while (!Serial) { }

    sonar.startInterruptMessurement();
}

uint8_t ledState = false;
uint64_t iLoop = 0;

void loop()
{
    uint32_t loopCycleStart = micros();

    // Blink
    if (iLoop % LOOP_HZ == 0) {
        digitalWrite(LED_BUILTIN, ledState ^= 1);
    }

    if (sonar.hasNewInterruptMessurement()) {
        Serial.print("Distance: ");
        Serial.print(sonar.getInterruptMessurement());
        Serial.println(" cm");

        sonar.startInterruptMessurement();
    }

    ++iLoop;
    delayMicroseconds((1'000'000 / LOOP_HZ) - (micros() - loopCycleStart));
}
