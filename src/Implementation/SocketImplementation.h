//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H


#include <SocketInterface.h>

class SocketImplementation : public SocketInterface {

public:
    bool begin() override;

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;
};


#endif //CORE_MQTT_SN_GATEWAY_SOCKETIMPLEMENTATION_H
