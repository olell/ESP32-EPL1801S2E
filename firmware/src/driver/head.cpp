/**
 * ESP32 Driver for EPL1801S2E
 * Stepper motor code
 *
 * (C) 2023 Ole Lange
 */
#include <Arduino.h>
#include <driver/head.h>
#include <driver/sensor.h>
#include <pinconfig.h>

/**
 * Init head driver
 */
void headInit() {
    // PinModes
    pinMode(PIN_HEAD_DATA1, OUTPUT);
    pinMode(PIN_HEAD_DATA2, OUTPUT);
    pinMode(PIN_HEAD_DATA3, OUTPUT);
    pinMode(PIN_HEAD_DATA4, OUTPUT);
    pinMode(PIN_HEAD_DATA5, OUTPUT);
    pinMode(PIN_HEAD_DATA6, OUTPUT);
    pinMode(PIN_HEAD_CLK, OUTPUT);
    pinMode(PIN_HEAD_LAT, OUTPUT);
    pinMode(PIN_HEAD_STB, OUTPUT);
    pinMode(PIN_HEAD_RANK1, INPUT_PULLUP);
    pinMode(PIN_HEAD_RANK2, INPUT_PULLUP);

    digitalWrite(PIN_HEAD_LAT, 1);
}

/**
 * Read Rank value from head
 *
 * @returns rank (0...3 -> A...D)
 */
uint8_t headReadRank() {
    return (digitalRead(PIN_HEAD_RANK2) << 1) | digitalRead(PIN_HEAD_RANK1);
}

/**
 * Write row data
 *
 * @param row pointer to an array of 384 pixel values (0/1)
 */
void headWriteRow(const uint8_t* row) {
    for (uint8_t i = 0; i < 64; i++) {
        digitalWrite(PIN_HEAD_DATA1, row[i]);
        digitalWrite(PIN_HEAD_DATA2, row[64 + i]);
        digitalWrite(PIN_HEAD_DATA3, row[128 + i]);
        digitalWrite(PIN_HEAD_DATA4, row[192 + i]);
        digitalWrite(PIN_HEAD_DATA5, row[256 + i]);
        digitalWrite(PIN_HEAD_DATA6, row[320 + i]);
        digitalWrite(PIN_HEAD_CLK, 1);
        delayMicroseconds(1);
        digitalWrite(PIN_HEAD_CLK, 0);
    }
    digitalWrite(PIN_HEAD_LAT, 0);
    delayMicroseconds(1);
    digitalWrite(PIN_HEAD_LAT, 1);
}

/**
 * Strobe row for a given period of time
 *
 * @param time time in microseconds to strobe
 */
void headStrobe(uint16_t time) {
    digitalWrite(PIN_HEAD_STB, 1);
    delayMicroseconds(time);
    digitalWrite(PIN_HEAD_STB, 0);
}

/**
 * Calculates head strobe time
 *
 * @param vp supply voltage
 * @param rowSum sum of all pixels in the current row
 */
uint16_t headCalculateStrobeTime(float vp, uint16_t rowSum) {
    float rav;
    uint8_t rank = headReadRank();
    if (rank == 0) rav = 190;
    if (rank == 1) rav = 177;
    if (rank == 2) rav = 164;
    if (rank == 3) rav = 150;
    float w = (pow(vp, 2) * rav) / pow(26.4 + rav, 2);
    float t = sensorReadHeadTemperature();
    float a = 1.1;
    float t1 = (0.230 / w) * (1 + (a / 100.0 * (20 - t)));
    float x = 0;
    if (vp >= 4.2 && vp <= 4.5) {
        if (t >= -5 && t <= 5) x = 240;
        if (t > 5 && t <= 20) x = 188;
        if (t > 20 && t <= 35) x = 133;
        if (t > 35 && t <= 50) x = 90;
        if (t > 50 && t <= 70) x = 69;
    }
    if (vp > 4.5 && vp <= 5.0) {
        if (t >= -5 && t <= 5) x = 150;
        if (t > 5 && t <= 20) x = 121;
        if (t > 20 && t <= 35) x = 86;
        if (t > 35 && t <= 50) x = 53;
        if (t > 50 && t <= 70) x = 29;
    }
    if (vp > 5.0 && vp <= 5.5) {
        if (t >= -5 && t <= 5) x = 90;
        if (t > 5 && t <= 20) x = 82;
        if (t > 20 && t <= 35) x = 50;
        if (t > 35 && t <= 50) x = 31;
        if (t > 50 && t <= 70) x = 25;
    }
    if (vp > 5.5 && vp <= 6.0) {
        if (t >= -5 && t <= 5) x = 63;
        if (t > 5 && t <= 20) x = 40;
        if (t > 20 && t <= 35) x = 31;
        if (t > 35 && t <= 50) x = 17;
        if (t > 50 && t <= 70) x = 17;
    }
    if (vp > 6.0 && vp <= 7.0) {
        if (t >= -5 && t <= 5) x = 36;
        if (t > 5 && t <= 20) x = 27;
        if (t > 20 && t <= 35) x = 10;
        if (t > 35 && t <= 50) x = 10;
        if (t > 50 && t <= 70) x = 10;
    }
    if (vp > 7.0 && vp <= 8.5) {
        if (t >= -5 && t <= 5) x = 9;
        if (t > 5 && t <= 20) x = 5;
        if (t > 20 && t <= 35) x = 0;
        if (t > 35 && t <= 50) x = 0;
        if (t > 50 && t <= 70) x = 0;
    }
    float t2 = (1 + x / 100.0) * t1;

    // Skipping divide correction because I don't understand what "number of
    // dividing means"...
    float y = 0;
    float t3 = (1 + y / 100.0) * t2;

    float z = rowSum / 64;
    float t4 = (1 + z / 100.0) * t3;

    return t4 * 1000;
}