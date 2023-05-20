/**
 * ESP32 Driver for EPL1801S2E
 * Printer code
 *
 * (C) 2023 Ole Lange
 */

#include "ble.h"

#include "log.h"

// BLE Service and Characteristic UUIDs
#define SERVICE_UUID "ADE5F08B-C00E-418F-942B-297339F7B564"  // printer service
#define WRITE_CHARACTERISTIC_UUID \
    "8b26fafb-406b-425b-b576-8ba4c37d77f2"  // write chrst

// Callback classes
class ServerCallbacks : public BLEServerCallbacks {
   private:
    BLEDriver *ble;

   public:
    void setBLEDriver(BLEDriver *ble) { this->ble = ble; }

    void onConnect(BLEServer *server) {
        logInfo("BLE Connected!");
        ble->setConnectionState(true);
    }

    /**
     * Is called when the ble server was disconnected
     */
    void onDisconnect(BLEServer *server) {
        logError("BLE Disconnected!");
        ble->setConnectionState(false);
        ble->startAdvertising();
    }
};

class CharCallbacks : public BLECharacteristicCallbacks {
   private:
    BLEDriver *ble;

   public:
    void setBLEDriver(BLEDriver *ble) { this->ble = ble; }

    void onWrite(BLECharacteristic *characteristic) {
        memcpy(ble->dataBuffer, characteristic->getData(), 512);
        ble->newData();

        while (ble->isWaiting) {
            delay(10);
        }
        ble->isWaiting = true;
    }
};

BLEDriver::BLEDriver() {}

void BLEDriver::init() {
    logInfo("Init: BLE Driver");
    BLEDevice::init("thermalprinter");
    server = BLEDevice::createServer();
    BLEService *service = server->createService(SERVICE_UUID);

    writeCharacteristic = service->createCharacteristic(
        WRITE_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ);

    BLEAdvertising *advertising = server->getAdvertising();
    advertising->start();

    ServerCallbacks *scb = new ServerCallbacks();
    CharCallbacks *ccb = new CharCallbacks();

    scb->setBLEDriver(this);
    ccb->setBLEDriver(this);

    server->setCallbacks(scb);
    writeCharacteristic->setCallbacks(ccb);

    service->start();

    dataBuffer = (uint8_t *)malloc(512);
}

void BLEDriver::startAdvertising() { server->startAdvertising(); }
void BLEDriver::setConnectionState(bool state) { isConnected = state; }

void BLEDriver::newData() { hasNewData = true; }
bool BLEDriver::available() {
    bool result = hasNewData;
    hasNewData = false;
    return result;
}
void BLEDriver::accept() { isWaiting = false; }