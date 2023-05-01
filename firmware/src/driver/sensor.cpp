/**
 * ESP32 Driver for EPL1801S2E
 * Sensor code
 *
 * (C) 2023 Ole Lange
 */

#include <Arduino.h>
#include <driver/sensor.h>
#include <pinconfig.h>

/**
 * Init sensor code
 */
void sensorInit() {
    pinMode(PIN_SENSOR_HEAD, INPUT);
    pinMode(PIN_SENSOR_PAPER, INPUT);

    pinMode(PIN_THERMISTOR_HEAD, INPUT);
    pinMode(PIN_THERMISTOR_MOTOR, INPUT);

    // Configure ADC
    analogReadResolution(12);
    analogSetAttenuation(ADC_2_5db);
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

/**
 * Read thermistor temperature
 *
 * @param pin pin of the thermistor
 * @returns temperature in °C
 */
float sensorReadThermistor(uint8_t pin) {
    uint16_t adcVal = analogRead(pin);
    // -> The next line assumes that the ADC is linear, which it isn't!
    // TODO: Check if the measurement is correct enough!
    float voltage = (adcVal / 4096.0) * 1.1;                    // in V
    float r = ((voltage * 150000) / (3.3 - voltage)) / 1000.0;  // in kΩ
    float t =
        (1.0 / (log(r / 30.0) / 3760.0 + (1.0 / 298.0))) - 273.0;  // in °C
    return t;
}

/**
 * Read motor thermistor
 *
 * @returns motor temperature in °C
 */
float sensorReadMotorTemperature() {
    return sensorReadThermistor(PIN_THERMISTOR_MOTOR);
}

/**
 * Read head thermistor
 *
 * @returns head temperature in °C
 */
float sensorReadHeadTemperature() {
    return sensorReadThermistor(PIN_THERMISTOR_HEAD);
}