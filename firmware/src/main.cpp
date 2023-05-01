/**
 * ESP32 Driver for EPL1801S2E
 * Main file
 *
 * (C) 2023 Ole Lange
 */

#include <Arduino.h>
#include <motor.h>
#include <sensor.h>

void setup() {
    motorInit();
    sensorInit();
}

void loop() {}