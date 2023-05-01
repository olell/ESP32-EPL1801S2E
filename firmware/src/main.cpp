/**
 * ESP32 Driver for EPL1801S2E
 * Main file
 *
 * (C) 2023 Ole Lange
 */

#include <Arduino.h>
#include <driver/motor.h>
#include <driver/sensor.h>

void setup() {
    motorInit();
    sensorInit();
}

void loop() {}