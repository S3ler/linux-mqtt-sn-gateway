#ifndef GATEWAY_PAHOMQTTMESSAGEHANDLER_H
#define GATEWAY_PAHOMQTTMESSAGEHANDLER_H

#include <MQTTClient.h>
#include <linux.cpp>
#include <netinet/in.h>
#include <MqttMessageHandlerInterface.h>

class PahoMqttMessageHandler : public MqttMessageHandlerInterface{
private:

public:
    virtual bool begin();

    virtual void setCore(Core *core);

    virtual void setLogger(LoggerInterface *logger);

    virtual void setServer(uint8_t *ip, uint16_t port);

    virtual void setServer(const char* hostname, uint16_t port);

    virtual bool connect(const char *id);

    virtual bool connect(const char *id, const char *user, const char *pass);

    virtual bool
    connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain, const uint8_t *willMessage,
            const uint16_t willMessageLength);

    virtual bool
    connect(const char *id, const char *user, const char *pass, const char *willTopic, uint8_t willQos, bool willRetain,
            const uint8_t *willMessage, const uint16_t willMessageLength);

    virtual void disconnect();

    virtual bool publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos, bool retained);

    virtual bool subscribe(const char *topic, uint8_t qos);

    virtual bool unsubscribe(const char *topic);

    virtual bool receive_publish(char *topic, uint8_t *payload, uint32_t length);

    virtual bool loop();

    IPStack ipstack;
    MQTT::Client<IPStack, Countdown, 512, 5> *client;
    const char *hostname = nullptr;
    uint16_t port = 0;
private:
    Core *core = nullptr;
    bool getConfigAndConnect() ;
    int64_t ip_address = -1;
    LoggerInterface *logger;
};


#endif //GATEWAY_PAHOMQTTMESSAGEHANDLER_H
