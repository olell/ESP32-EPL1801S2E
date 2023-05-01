/**
 * ESP32 Driver for EPL1801S2E
 * Sensor code
 *
 * (C) 2023 Ole Lange
 */

#include <Arduino.h>
#include <pinconfig.h>
#include <sensor.h>

/**
 * Init sensor code
 */
void sensorInit() {
    pinMode(PIN_SENSOR_HEAD, INPUT);
    pinMode(PIN_SENSOR_PAPER, INPUT);
}
