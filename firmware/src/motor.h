/**
 * ESP32 Driver for EPL1801S2E
 * Header file for stepper motor code
 *
 * (C) 2023 Ole Lange
 */
#pragma once

#include <stdint.h>

void motor_init();

void motor_step();
void motor_step(uint8_t steps);
void motor_step(uint8_t steps, uint8_t direction);

void motor_enable();
void motor_disable();