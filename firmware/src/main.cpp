#include <Arduino.h>

#include "ble.h"
#include "log.h"
#include "pinconfig.h"
#include "sensor.h"
#include "util.h"

/**
 * Print Buffer + Variables
 */
#define PRINT_BUFFER_LINES 100
#define PRINT_BUFFER_LINE_LENGTH 48
uint8_t *printBuffer;
uint8_t *currentLineBuffer;

#if PRINT_BUFFER_LINES < 256
#define BUFFER_VAR_TYPE uint8_t
#else
#define BUFFER_VAR_TYPE uint16_t
#endif

BUFFER_VAR_TYPE printBufferIndex = 0;
BUFFER_VAR_TYPE printBufferLeft = 0;
uint8_t printBufferLock = 0;

uint16_t testCounter = 0;

/**
 * Strobe
 */
esp_timer_handle_t strobeTimer;

/**
 * BLE
 */
BLEDriver ble = BLEDriver();

/**
 * Motor
 */
hw_timer_t *motorTimer = NULL;
uint8_t stepCounter = 0;
void IRAM_ATTR onMotorTimer() {
    digitalWrite(PIN_MOTOR_STEP, !digitalRead(PIN_MOTOR_STEP));

    if (printBufferLeft > 0) {
        digitalWrite(PIN_MOTOR_ENABLE, 0);
        if (stepCounter % 4 == 0) {
            // locking
            printBufferLock = 1;
            memcpy(currentLineBuffer,
                   printBuffer + (PRINT_BUFFER_LINE_LENGTH * printBufferIndex),
                   PRINT_BUFFER_LINE_LENGTH);
            printBufferIndex = (printBufferIndex + 1) % PRINT_BUFFER_LINES;
            printBufferLeft--;
            printBufferLock = 0;

            // write to head
            for (uint8_t i = 0; i < 64; i++) {
                digitalWrite(PIN_HEAD_DATA1, getBit(currentLineBuffer, i + 0));
                digitalWrite(PIN_HEAD_DATA2, getBit(currentLineBuffer, i + 64));
                digitalWrite(PIN_HEAD_DATA3,
                             getBit(currentLineBuffer, i + 128));
                digitalWrite(PIN_HEAD_DATA4,
                             getBit(currentLineBuffer, i + 192));
                digitalWrite(PIN_HEAD_DATA5,
                             getBit(currentLineBuffer, i + 256));
                digitalWrite(PIN_HEAD_DATA6,
                             getBit(currentLineBuffer, i + 320));
                digitalWrite(PIN_HEAD_CLK, 1);
                delayMicroseconds(1);
                digitalWrite(PIN_HEAD_CLK, 0);
            }
            // latch head
            digitalWrite(PIN_HEAD_LAT, 0);
            delayMicroseconds(1);
            digitalWrite(PIN_HEAD_LAT, 1);
        }
        // strobing everytime
        digitalWrite(PIN_HEAD_STB, 1);
        digitalWrite(PIN_LED_STATUS, 0);
        esp_timer_start_once(strobeTimer, 1000);
    } else {
        digitalWrite(PIN_MOTOR_ENABLE, 1);
    }
    stepCounter++;
}

void IRAM_ATTR onStrobeTimer(void *param) {
    digitalWrite(PIN_LED_STATUS, 1);
    digitalWrite(PIN_HEAD_STB, 0);
}

void setup() {
    logInit();

    // Pinmodes
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);
    pinMode(PIN_MOTOR_STEP, OUTPUT);
    pinMode(PIN_MOTOR_DIRECTION, OUTPUT);

    pinMode(PIN_LED_STATUS, OUTPUT);

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
    digitalWrite(PIN_HEAD_STB, 0);

    // Motor
    digitalWrite(PIN_MOTOR_ENABLE, 1);
    digitalWrite(PIN_MOTOR_DIRECTION, 1);
    motorTimer = timerBegin(0, 80, true);
    timerAttachInterrupt(motorTimer, &onMotorTimer,
                         true);  // last param is edge
    timerAlarmWrite(motorTimer, 4000, true);
    timerAlarmEnable(motorTimer);

    // Strobe
    esp_timer_create_args_t strobeTimerConfig;
    strobeTimerConfig.callback = onStrobeTimer;
    esp_timer_create(&strobeTimerConfig, &strobeTimer);

    // Allocating memory for print buffer
    printBuffer =
        (uint8_t *)malloc(PRINT_BUFFER_LINES * PRINT_BUFFER_LINE_LENGTH);
    currentLineBuffer = (uint8_t *)malloc(PRINT_BUFFER_LINE_LENGTH);

    ble.init();
    logInfo("Hey there!");
}

uint8_t bleCommand = 0;
uint8_t bleLength = 0;

BUFFER_VAR_TYPE firstFreeLine = 0;
BUFFER_VAR_TYPE lastBlockLength = 0;
BUFFER_VAR_TYPE firstBlockLength = 0;

void loop() {
    if (ble.available()) {
        bleCommand = ble.dataBuffer[0];
        bleLength = ble.dataBuffer[1];

        while (printBufferLock ||
               ((PRINT_BUFFER_LINES - printBufferLeft) < bleLength)) {
            delay(10);
        }
        timerAlarmDisable(motorTimer);

        firstFreeLine =
            (printBufferIndex + printBufferLeft) % PRINT_BUFFER_LINES;

        lastBlockLength = PRINT_BUFFER_LINES - firstFreeLine;
        if (lastBlockLength > bleLength) lastBlockLength = bleLength;
        firstBlockLength = bleLength - lastBlockLength;

        memcpy(printBuffer + (PRINT_BUFFER_LINE_LENGTH * firstFreeLine),
               ble.dataBuffer + 32, lastBlockLength * PRINT_BUFFER_LINE_LENGTH);
        if (firstBlockLength > 0) {
            memcpy(printBuffer,
                   ble.dataBuffer + 32 +
                       (lastBlockLength * PRINT_BUFFER_LINE_LENGTH),
                   firstBlockLength * PRINT_BUFFER_LINE_LENGTH);
        }

        printBufferLeft += bleLength;

        timerAlarmEnable(motorTimer);

        ble.accept();
    }
}