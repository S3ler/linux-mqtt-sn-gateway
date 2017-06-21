//
// Created by bele on 11.06.17.
//

#include <BluetoothLowEnergy/gattlib/include/gattlib.h>
#include "Scanner.h"

Queue<BluetoothLowEnergyAdvertise *> *Scanner::queue = nullptr;

bool Scanner::scan_enable() {
    if (this->adapter == nullptr || this->queue == nullptr) {
        return false;
    }
    return scan_enable(adapter);
}


bool Scanner::scan_enable(void *adapter) {
    if (this->queue == nullptr) {
        return false;
    }
    this->adapter = adapter;
    return (0 == gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT));
}


void Scanner::scan_disable() {
    gattlib_adapter_scan_disable(adapter);
}

void Scanner::setAdapter(void *adapter) {
    Scanner::adapter = adapter;
}

void Scanner::setScannerQueue(Queue<BluetoothLowEnergyAdvertise *> *queue) {
    Scanner::queue = queue;
}

void Scanner::ble_discovered_device(const char *addr, const char *name) {
    std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    BluetoothLowEnergyAdvertise *advertise = new BluetoothLowEnergyAdvertise(addr, name, timestamp);
    if (strlen(name) > 0) {
        queue->push(advertise);
    }
}

Scanner::~Scanner() {
    scan_disable();
}

