/**
 * ESP32 Driver for EPL1801S2E
 * Stepper motor code
 *
 * (C) 2023 Ole Lange
 */
#include <Arduino.h>
#include <driver/motor.h>
#include <pinconfig.h>

// Step On - Sleep - Step Off - Sleep
#define MOTOR_SLEEP_PER_STEP 1000

/**
 * Init motor driver
 */
void motorInit() {
    pinMode(PIN_MOTOR_DIRECTION, OUTPUT);
    pinMode(PIN_MOTOR_STEP, OUTPUT);
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);

    motorDisable();
}

/**
 * Make a single step in default direction
 */
void motorStep() { motorStep(1); }

/**
 * Make the given amount in default direction
 *
 * @param steps amount of steps to do
 */
void motorStep(uint8_t steps) { motorStep(steps, 1); }

/**
 * Make a given amount of steps in the given direction
 *
 * @param steps amount of steps to do
 * @param direction direction to step in
 */
void motorStep(uint8_t steps, uint8_t direction) {
    digitalWrite(PIN_MOTOR_DIRECTION, direction);
    while (steps) {
        digitalWrite(PIN_MOTOR_STEP, 1);
        delayMicroseconds(MOTOR_SLEEP_PER_STEP);
        digitalWrite(PIN_MOTOR_STEP, 0);
        delayMicroseconds(MOTOR_SLEEP_PER_STEP);
        steps--;
    }
}

/**
 * Enable the motor driver
 */
void motorEnable() { digitalWrite(PIN_MOTOR_ENABLE, 0); }
/**
 * Disable the motor driver
 */
void motorDisable() { digitalWrite(PIN_MOTOR_ENABLE, 1); }