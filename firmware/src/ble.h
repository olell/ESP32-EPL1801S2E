/**
 * ESP32 Driver for EPL1801S2E
 * Header file for printer code
 *
 * (C) 2023 Ole Lange
 */

#pragma once

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <stdint.h>

class BLEDriver {
   private:
    BLECharacteristic *writeCharacteristic;
    BLEServer *server;

    bool isConnected = false;

    bool hasNewData = false;

   public:
    BLEDriver();
    uint8_t *dataBuffer;
    bool isWaiting = true;
    void init();
    void startAdvertising();
    void setConnectionState(bool state);
    void newData();

    bool available();
    void accept();
};