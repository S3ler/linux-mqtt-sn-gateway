//
// Created by bele on 09.08.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_BLEDEVICESTATISTIC_H
#define LINUX_MQTT_SN_GATEWAY_BLEDEVICESTATISTIC_H


#include <cstdint>
#include <chrono>
#include <atomic>
#include <string>

class BLEDeviceStatistic {
public:
    explicit BLEDeviceStatistic(std::string mac);

    uint8_t incrementConnectFailed();

    uint8_t incrementServiceFailed();

    uint8_t getConnectFailed() const;

    uint8_t getServiceFailed() const;

    const std::atomic_bool &isConnectionInProgress() const;

    void setConnectionInProgress();

    void unsetConnectionInProgress();

    bool isFaulty();

    void reset();

private:
    uint8_t connectFailed;
    uint8_t serviceFailed;
    std::chrono::system_clock::time_point lastChanged;
    std::atomic_bool connectionInProgress;
};


#endif //LINUX_MQTT_SN_GATEWAY_BLEDEVICESTATISTIC_H
