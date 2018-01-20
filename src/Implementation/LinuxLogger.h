//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_LOGGERIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_LOGGERIMPLEMENTATION_H


#include <LoggerInterface.h>

#ifndef Arduino_h
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
#include <RasPi.h>
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
#include <wiringPi.h>
#include <Arduino.h>
#else
#include <Arduino.h>
#endif
#endif

class LinuxLogger : public LoggerInterface {
private:
    uint8_t current_log_lvl = 2;
    uint8_t last_started_log_lvl = UINT8_MAX;

#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    // already as extern Serial in simulator.h
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
    SerialLinux Serial;
#else
#ifndef Arduino_h
    SerialLinux Serial;
#endif
#endif


public:
    bool begin() override;

    void set_log_lvl(uint8_t log_lvl) override;

    void log(char *msg, uint8_t log_lvl) override;

    void log(const char *msg, uint8_t log_lvl) override;

    void start_log(char *msg, uint8_t log_lvl) override;

    void start_log(const char *msg, uint8_t log_lvl) override;

    void set_current_log_lvl(uint8_t log_lvl) override;

    void append_log(char *msg) override;

    void append_log(const char *msg) override;
};


#endif //CORE_MQTT_SN_GATEWAY_LOGGERIMPLEMENTATION_H
