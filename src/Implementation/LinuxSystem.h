//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H


#include <System.h>
#include <exception>
#include <string>

#include "LoggerInterface.h"

#ifndef Arduino_h
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24) || defined(GATEWAY_TRANSMISSION_PROTOCOL_RH_SERIAL)
#include <RasPi.h>
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
//#include <RasPi.h>
#include <wiringPi.h>
#else
//#include <wiringPi.h>
#include <Arduino.h>
#endif
#endif

class LinuxSystem : public System {
private:
    uint32_t heartbeat_period = 10000;
    uint32_t heartbeat_current = 0;
    uint32_t elapsed_current = 0;
    LoggerInterface* logger = nullptr;
public:

    class ThreadTerminated : public std::exception {
        public:
          ThreadTerminated(const char* msg);
          const char* what() const throw() override;
        private:
          std::string term_msg;
    };

#ifndef Arduino_h
    LinuxSystem();
#endif

    void setLogger(LoggerInterface* logger) { this->logger = logger; }

    void set_heartbeat(uint32_t period) override;

    uint32_t get_heartbeat() override;

    bool has_beaten() override;

    uint32_t get_elapsed_time() override;

    void sleep(uint32_t duration) override;

    void exit() override;
};


#endif //CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H
