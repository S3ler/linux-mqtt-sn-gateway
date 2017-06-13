//
// Created by bele on 11.06.17.
//

#include <BluetoothLowEnergy/gattlib/include/gattlib.h>
#include "Scanner.h"

// TODO static class member init
Queue<BluetoothLowEnergyAdvertise *> *Scanner::queue = nullptr;

void Scanner::start_loop() {
    this->thread = std::thread(&Scanner::loop, this);
    this->thread.detach();
}

void Scanner::loop() {
    /*while(!stopped){
        // TODO implement me
    }
    gattlib_adapter_scan_disable(adapter);*/
}

void Scanner::stop_loop() {
    this->stopped = true;
}

void Scanner::setScannerQueue(Queue<BluetoothLowEnergyAdvertise *> *queue) {
    Scanner::queue = queue;
}


bool Scanner::scan_enable(void *adapter) {
    // TODO adapter not threadsafe
    this->adapter = adapter;
    int ret;
    ret = gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT);
    if (ret) {
        fprintf(stderr, "ERROR: Failed to scan.\n");
        return false;
    }
    return true;
}



void Scanner::ble_discovered_device(const char* addr, const char* name) {
    std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    BluetoothLowEnergyAdvertise* advertise = new BluetoothLowEnergyAdvertise(addr, name, timestamp);
    queue->push(advertise);
    /*
    if (name) {
        printf("Discovered %s - '%s'\n", addr, name);
    } else {
        printf("Discovered %s\n", addr);
    }
    */
}

Scanner::~Scanner() {
    gattlib_adapter_scan_disable(adapter);
}