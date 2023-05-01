/**
 * ESP32 Driver for EPL1801S2E
 * Header file for sensor code
 *
 * (C) 2023 Ole Lange
 */

#pragma once

#include <stdint.h>

// Sensor states
#define HEAD_UP 0
#define HEAD_DOWN 1
#define PAPER 0
#define NO_PAPER 1

void sensorInit();

uint8_t sensorReadPaper();
uint8_t sensorReadHead();