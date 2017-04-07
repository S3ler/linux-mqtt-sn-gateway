//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_LOGGERIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_LOGGERIMPLEMENTATION_H


#include <LoggerInterface.h>

class LinuxLogger : public LoggerInterface {

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
