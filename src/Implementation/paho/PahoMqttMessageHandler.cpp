#include "PahoMqttMessageHandler.h"

#define MQTT_LOG 1

void messageArrived(MQTT::MessageData& md);

MqttMessageHandlerInterface *__mqttMessageHandler = nullptr;

void messageArrived(MQTT::MessageData& md){
    MQTT::Message &message = md.message;
    char topic_name_buffer[1024];
    memset(topic_name_buffer,0,1024);
    if (md.topicName.lenstring.len < 1024) {
        memcpy(topic_name_buffer, md.topicName.lenstring.data, md.topicName.lenstring.len);
        __mqttMessageHandler->receive_publish(topic_name_buffer, (uint8_t *) message.payload, (uint32_t) message.payloadlen);
    }
}


bool PahoMqttMessageHandler::begin() {
    if (core == nullptr) {
        return false;
    }
    __mqttMessageHandler = this;
    ipstack = IPStack();
    client = new MQTT::Client<IPStack, Countdown>(ipstack);
    return true;
}

void PahoMqttMessageHandler::setCore(Core *core) {
    this->core = core;
}


void PahoMqttMessageHandler::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}


void PahoMqttMessageHandler::setServer(uint8_t *ip, uint16_t port) {
    this->hostname = nullptr;
    this->ip_address = 0;
    ip_address |= (*ip++) << 24;
    ip_address |= (*ip++) << 16;
    ip_address |= (*ip++) << 8;
    ip_address |= (*ip) << 0;
    this->port = port;
}


void PahoMqttMessageHandler::setServer(const char *hostname, uint16_t port) {
    this->ip_address = -1;
    this->hostname = hostname;
    this->port = port;
}


bool PahoMqttMessageHandler::connect(const char *id) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    }else if(ip_address != -1){
        rc = ipstack.connect((uint32_t) ip_address, port);
    }else{
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *) id;
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttMessageHandler::connect(const char *id, const char *user, const char *pass) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    }else if(ip_address != -1){
        rc = ipstack.connect((uint32_t) ip_address, port);
    }else{
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *) id;

    data.username.cstring = (char *) user;
    data.password.cstring = (char *) pass;
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttMessageHandler::connect(const char *id, const char *willTopic, uint8_t willQos, bool willRetain,
                                     const uint8_t *willMessage, const uint16_t willMessageLength) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    }else if(ip_address != -1){
        rc = ipstack.connect((uint32_t) ip_address, port);
    }else{
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;
    data.clientID.cstring = (char *) id;

    data.willFlag = 1;
    data.will.topicName.cstring = (char *) willTopic;
    data.will.message.cstring = (char *) willMessage;
    if (willQos == 0) {
        data.will.qos = MQTT::QOS0;
    } else if (willQos == 1) {
        data.will.qos = MQTT::QOS1;
    } else if (willQos == 2) {
        data.will.qos = MQTT::QOS2;
    }else{
        return false;
    }
    //data.will.retained = (unsigned char) willRetain; // TODO check
    rc = client->connect(data);

    return rc == 0;
}

bool PahoMqttMessageHandler::connect(const char *id, const char *user, const char *pass, const char *willTopic,
                                     uint8_t willQos, bool willRetain, const uint8_t *willMessage,
                                     const uint16_t willMessageLength) {
    int rc;
    if (hostname != nullptr) {
        rc = ipstack.connect(hostname, port);
    }else if(ip_address != -1){
        rc = ipstack.connect((uint32_t) ip_address, port);
    }else{
        return false;
    }
    if (rc != 0) {
        return false;
    }
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 4;

    data.clientID.cstring = (char *) id;

    data.username.cstring = (char *) user;
    data.password.cstring = (char *) pass;

    data.willFlag = 1;
    data.will.topicName.cstring = (char *) willTopic;
    data.will.message.cstring = (char *) willMessage;
    if (willQos == 0) {
        data.will.qos = MQTT::QOS0;
    } else if (willQos == 1) {
        data.will.qos = MQTT::QOS1;
    } else if (willQos == 2) {
        data.will.qos = MQTT::QOS2;
    }else{
        return false;
    }
    data.will.retained = (unsigned char) willRetain; // TODO check
    rc = client->connect(data);

    return rc == 0;
}

void PahoMqttMessageHandler::disconnect() {
    client->disconnect();
    ipstack.disconnect();
}

bool PahoMqttMessageHandler::publish(const char *topic, const uint8_t *payload, uint16_t plength, uint8_t qos,
                                     bool retained) {
    MQTT::Message message;
    if (qos == 0) {
        message.qos = MQTT::QOS0;
    } else if (qos == 1) {
        message.qos = MQTT::QOS1;
    } else if (qos == 2) {
        message.qos = MQTT::QOS2;
    }else{
        return false;
    }
    message.payload = (void*)payload;
    message.payloadlen = plength;

    message.retained = retained;
    message.dup = false;

    int rc = client->publish(topic, message);
    return rc == 0;
}

bool PahoMqttMessageHandler::subscribe(const char *topic, uint8_t qos) {
    MQTT::QoS _qos;
    if (qos == 0) {
        _qos = MQTT::QOS0;
    } else if (qos == 1) {
        _qos = MQTT::QOS1;
    } else if (qos == 2) {
        _qos = MQTT::QOS2;
    }else{
        return false;
    }

    int rc = client->subscribe(topic, _qos, messageArrived);
    return rc == 0;
}



bool PahoMqttMessageHandler::unsubscribe(const char *topic) {
    if (topic == nullptr) {
        return true;
    }
    if (strlen(topic) == 0) {
        return true;
    }
    int rc = client->unsubscribe(topic);
    return rc == 0;
}

bool PahoMqttMessageHandler::receive_publish(char *topic, uint8_t *payload, uint32_t length) {
    CORE_RESULT publish_result = core->publish(topic, payload, length, false);
    return publish_result == SUCCESS;}

bool PahoMqttMessageHandler::loop() {
    if (client->isConnected()) {
        client->yield(300);
    }else{
        // core->notify_mqtt_disconnected();
        if(getConfigAndConnect()){
            core->notify_mqtt_connected();
#if MQTT_LOG
            logger->log("MQTT connected", 1);
#endif
            return true;
        }
        return false;
    }
    return true;
}

bool PahoMqttMessageHandler::getConfigAndConnect() {
    uint8_t server_ip[4];
    memset(&server_ip, 0, sizeof(server_ip));
    uint16_t server_port = 0;
    char client_id[24];
    memset(&client_id, 0, sizeof(client_id));
    if (core->get_mqtt_config((uint8_t *) &server_ip, &server_port, (char *) &client_id)) {
        // login
        char username[24];
        memset(&username, 0, sizeof(username));
        char password[24];
        memset(&password, 0, sizeof(password));
        // will
        char will_topic[255];
        memset(&will_topic, 0, sizeof(will_topic));
        char will_msg[255];
        memset(&will_msg, 0, sizeof(will_msg));
        uint8_t will_qos = 0;
        bool will_retain = false;

        bool has_mqtt_login_config, has_mqtt_will;
        has_mqtt_login_config = core->get_mqtt_login_config((char *) &username, (char *) password);
        has_mqtt_will = core->get_mqtt_will(will_topic, will_msg, &will_qos, &will_retain);

        setServer(server_ip, server_port);
        if (has_mqtt_login_config && has_mqtt_will) {
            return connect(client_id, username, password, will_topic, will_qos, will_retain, (const uint8_t *) will_msg,
                           (const uint16_t) (strlen(will_msg) + 1));
        } else if (has_mqtt_login_config) {
            return connect(client_id, username, password);
        } else if (has_mqtt_will) {
            return connect(client_id, will_topic, will_qos, will_retain, (const uint8_t *) will_msg,
                           (const uint16_t) (strlen(will_msg) + 1));
        } else {
            return connect(client_id);
        }

    }
    return false;
}
