//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H


#include <System.h>

class LinuxSystem : public System {
public:
    void set_heartbeat(uint32_t period) override;

    uint32_t get_heartbeat() override;

    bool has_beaten() override;

    uint32_t get_elapsed_time() override;

    void sleep(uint32_t duration) override;

    void exit() override;
};


#endif //CORE_MQTT_SN_GATEWAY_SYSTEMIMPLEMENTATION_H
