/**
 * ESP32 Driver for EPL1801S2E
 * Header file for stepper motor code
 *
 * (C) 2023 Ole Lange
 */
#pragma once

#include <stdint.h>

void motorInit();

void motorStep();
void motorStep(uint8_t steps);
void motorStep(uint8_t steps, uint8_t direction);

void motorEnable();
void motorDisable();