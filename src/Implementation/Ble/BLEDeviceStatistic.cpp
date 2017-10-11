//
// Created by bele on 09.08.17.
//

#include "BLEDeviceStatistic.h"

BLEDeviceStatistic::BLEDeviceStatistic(std::string mac) : mac(mac), connectFailed(0), serviceFailed(0), connectionInProgress(false){
    lastChanged = std::chrono::system_clock::now();
}

uint8_t BLEDeviceStatistic::getConnectFailed() const {
    return connectFailed;
}

uint8_t BLEDeviceStatistic::getServiceFailed() const {
    return serviceFailed;
}


uint8_t BLEDeviceStatistic::incrementConnectFailed() {
    lastChanged = std::chrono::system_clock::now();
    connectFailed++;
    return connectFailed;
}

uint8_t BLEDeviceStatistic::incrementServiceFailed() {
    lastChanged = std::chrono::system_clock::now();
    serviceFailed++;
    return serviceFailed;
}

const std::atomic_bool &BLEDeviceStatistic::isConnectionInProgress() const {
    return connectionInProgress;
}

void BLEDeviceStatistic::setConnectionInProgress() {
    BLEDeviceStatistic::connectionInProgress = true;
}

void BLEDeviceStatistic::unsetConnectionInProgress() {
    BLEDeviceStatistic::connectionInProgress = false;
}

bool BLEDeviceStatistic::isFaulty() {
    // criteria:
    // connectFailed > 3
    // serviceFailed > 3
    // TimeDurationLastTime < 5 min (300 seconds)
    auto difference = std::chrono::duration_cast<std::chrono::seconds>(lastChanged - std::chrono::system_clock::now());
    if(difference.count() > 300){
        connectFailed = 0;
        serviceFailed = 0;
        lastChanged = std::chrono::system_clock::now();
        return false;
    }
    return connectFailed > 3 || serviceFailed > 3;

}

void BLEDeviceStatistic::reset() {
    connectFailed = 0;
    serviceFailed = 0;
    lastChanged = std::chrono::system_clock::now();
}

std::string BLEDeviceStatistic::getMac() const {
    return mac;
}

