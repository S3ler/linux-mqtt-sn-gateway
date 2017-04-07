//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_MQTTBROKERIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_MQTTBROKERIMPLEMENTATION_H


#include <MqttMessageHandlerInterface.h>

class MqttBrokerImplementation : public MqttMessageHandlerInterface {
public:
    bool begin() override;

    void setCore(Core *core) override;

    void setLogger(LoggerInterface *logger) override;

    void setServer(uint8_t *ip, uint16_t port) override;

    void setServer(const char *hostname, uint16_t port) override;

    bool connect(const char *id) override;

    bool connect(const char *id, const char *user, const char *pass) override;

    bool connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain, const uint8_t *willMessage,
                 const uint16_t willMessageLength) override;

    bool
    connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos, bool willRetain,
            const uint8_t *willMessage, const uint16_t willMessageLength) override;

    void disconnect() override;

    bool publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos, bool retained) override;

    bool subscribe(const char *topic, uint8_t qos) override;

    bool unsubscribe(const char *topic) override;

    bool receive_publish(char *topic, uint8_t *payload, uint32_t length) override;

    bool loop() override;
};


#endif //CORE_MQTT_SN_GATEWAY_MQTTBROKERIMPLEMENTATION_H
