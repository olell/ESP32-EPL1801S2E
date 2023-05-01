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

/**
 * Read paper sensor state
 *
 * @returns 0 -> head up; 1 -> head down
 */
uint8_t sensorReadPaper() { return digitalRead(PIN_SENSOR_PAPER); }

/**
 * Read head sensor state
 *
 * @returns 0 -> paper; 1 -> no paper
 */
uint8_t sensorReadHead() { return digitalRead(PIN_SENSOR_HEAD); }