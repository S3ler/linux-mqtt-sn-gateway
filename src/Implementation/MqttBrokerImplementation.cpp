//
// Created by bele on 07.04.17.
//

#include "MqttBrokerImplementation.h"

bool MqttBrokerImplementation::begin() {
    return false;
}

void MqttBrokerImplementation::setCore(Core *core) {

}

void MqttBrokerImplementation::setLogger(LoggerInterface *logger) {

}

void MqttBrokerImplementation::setServer(uint8_t *ip, uint16_t port) {

}

void MqttBrokerImplementation::setServer(const char *hostname, uint16_t port) {

}

bool MqttBrokerImplementation::connect(const char *id) {
    return false;
}

bool MqttBrokerImplementation::connect(const char *id, const char *user, const char *pass) {
    return false;
}

bool MqttBrokerImplementation::connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain,
                                       const uint8_t *willMessage, const uint16_t willMessageLength) {
    return false;
}

bool MqttBrokerImplementation::connect(const char *id, const char *user, const char *pass, const char *willTopic,
                                       uint8_t willQos, bool willRetain, const uint8_t *willMessage,
                                       const uint16_t willMessageLength) {
    return false;
}

void MqttBrokerImplementation::disconnect() {

}

bool MqttBrokerImplementation::publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos,
                                       bool retained) {
    return false;
}

bool MqttBrokerImplementation::subscribe(const char *topic, uint8_t qos) {
    return false;
}

bool MqttBrokerImplementation::unsubscribe(const char *topic) {
    return false;
}

bool MqttBrokerImplementation::receive_publish(char *topic, uint8_t *payload, uint32_t length) {
    return false;
}

bool MqttBrokerImplementation::loop() {
    return false;
}
