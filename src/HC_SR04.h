#pragma once

#include <Arduino.h>

constexpr uint8_t INITIATE_MESSUREMENT_PULS_DURATION = 5; // Should be 10 but 5 works also, so why waste 9µs
constexpr float TIMEOUT_THRESHOLD = 1.33f;

class HC_SR04 {
private:
    uint8_t triggerPin;
    uint8_t echoPin;

    float temperature;
    float speedOfSound;

    uint16_t maxDistance;
    uint16_t timeout;

    void startInterrupt();
    void endInterrupt();

public:
    HC_SR04(uint8_t triggerPin, uint8_t echoPin, float temperature = 20.0f, uint16_t maxDistance = 400);
    ~HC_SR04();

    float measure();
    float measure(float temperature);

    void startInterruptMessurement();
    float getInterruptMessurement();
    bool hasNewInterruptMessurement();
};
