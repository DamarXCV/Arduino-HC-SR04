#include "HC_SR04.h"

static uint32_t interruptStart = 0;
static uint32_t interruptStop = 0;
static bool pinState = false;
static bool interruptNewMessurement = false;
static bool interruptsAttached = false;
static uint32_t messuringStart = 0;

static void interruptMessure()
{
    pinState = !pinState;

    if (pinState) {
        interruptStart = micros();
    } else {
        interruptStop = micros();
        interruptNewMessurement = true;
    }
}

/**
 * @brief Construct a new HC_SR04 object
 *
 * @param triggerPin
 * @param echoPin
 * @param temperature Distance in celsius
 * @param maxDistance Distance in centimeter
 */
HC_SR04::HC_SR04(byte triggerPin, byte echoPin, float temperature, uint16_t maxDistance)
    : triggerPin(triggerPin)
    , echoPin(echoPin)
    , temperature(temperature)
    , maxDistance(maxDistance)
{
    speedOfSound = (331.3f + 0.606f * temperature) * 100 / 1000000; // in cm/µs for temperatures between 0°C and 100°C
    timeout = TIMEOUT_THRESHOLD * maxDistance * 2 / speedOfSound;

    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);

    digitalWrite(triggerPin, LOW);
    delayMicroseconds(10);
}

/**
 * @brief Destroy the HC_SR04 object
 *
 */
HC_SR04::~HC_SR04()
{
    if (interruptsAttached) {
        endInterrupt();
    }
}

/**
 * @brief Messure distance without interrupt
 *
 * @return float  Distance in centimeter
 */
float HC_SR04::measure()
{
    // Initiate messurement
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(INITIATE_MESSUREMENT_PULS_DURATION);
    digitalWrite(triggerPin, LOW);

    return pulseIn(echoPin, HIGH, timeout) / 2.0f * speedOfSound;
}

/**
 * @brief Messure distance without interrupt
 *
 * @param temperature In Celsius
 * @return float Distance in centimeter
 */
float HC_SR04::measure(float temperature)
{
    this->temperature = max(min(temperature, 0.0f), 100.0f);
    speedOfSound = (331.3f + 0.606f * this->temperature) * 100 / 1000000; // in cm/µs for temperatures between 0°C and 100°C
    timeout = TIMEOUT_THRESHOLD * maxDistance * 2 / speedOfSound;

    return measure();
}

/**
 * @brief Initilize the interrupt for InterruptMessurement
 *
 */
void HC_SR04::startInterrupt()
{
    interruptsAttached = true;
    attachInterrupt(digitalPinToInterrupt(echoPin), interruptMessure, CHANGE);

    pinState = digitalRead(echoPin);
}

/**
 * @brief Remove the interrupt for InterruptMessurement
 *
 */
void HC_SR04::endInterrupt()
{
    detachInterrupt(digitalPinToInterrupt(echoPin));
    interruptsAttached = false;
}

/**
 * @brief Start a interrupt messurement
 *
 */
void HC_SR04::startInterruptMessurement()
{
    interruptNewMessurement = false;

    if (!interruptsAttached) {
        startInterrupt();
    }

    // Initiate messurement
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(INITIATE_MESSUREMENT_PULS_DURATION);
    digitalWrite(triggerPin, LOW);
    messuringStart = micros();
}

/**
 * @brief Gets the interrupt messurement if available
 *
 * @return float Distance in centimeter
 */
float HC_SR04::getInterruptMessurement()
{
    if (!interruptNewMessurement) {
        return 0;
    }

    if (interruptStop < interruptStart) {
        interruptStart = interruptStop = 0;
        pinState ^= 1;
    }

    interruptNewMessurement = false;
    return (interruptStop - interruptStart) / 2.0f * speedOfSound;
}

/**
 * @brief Check if the interrupt messurement is available
 *
 * @return bool
 */
bool HC_SR04::hasNewInterruptMessurement()
{
    if (interruptNewMessurement) {
        return interruptNewMessurement;
    } else if (((micros() - messuringStart) / 2.0f * speedOfSound) > 1300.0f && pinState) {
        // Initiate messurement
        digitalWrite(triggerPin, HIGH);
        delayMicroseconds(INITIATE_MESSUREMENT_PULS_DURATION);
        digitalWrite(triggerPin, LOW);
    }
    return false;
}
