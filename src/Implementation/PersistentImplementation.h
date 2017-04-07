//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H


#include <PersistentInterface.h>

class PersistentImplementation : public PersistentInterface {
public:
    bool begin() override;

    void setCore(Core *core) override;

    void setLogger(LoggerInterface *logger) override;

    void start_client_transaction(const char *client_id) override;

    void start_client_transaction(device_address *address) override;

    uint8_t apply_transaction() override;

    bool client_exist() override;

    void delete_client(const char *client_id) override;

    void add_client(const char *client_id, device_address *address, uint32_t duration) override;

    void reset_client(const char *client_id, device_address *address, uint32_t duration) override;

    void set_client_await_message(message_type msg_type) override;

    message_type get_client_await_message_type() override;

    void set_timeout(uint32_t timeout) override;

    bool has_client_will() override;

    void get_client_will(char *target_willtopic, uint8_t *target_willmsg, uint8_t *target_willmsg_length,
                         uint8_t *target_qos, bool *target_retain) override;

    void set_client_willtopic(char *willtopic, uint8_t qos, bool retain) override;

    void set_client_willmessage(uint8_t *willmsg, uint8_t willmsg_length) override;

    void delete_will() override;

    void get_last_client_address(device_address *address) override;

    void get_nth_client(uint64_t n, char *client_id, device_address *target_address, CLIENT_STATUS *target_status,
                        uint32_t *target_duration, uint32_t *target_timeout) override;

    const char *get_topic_name(uint16_t topic_id) override;

    uint16_t get_topic_id(char *topic_name) override;

    bool is_topic_known(uint16_t topic_id) override;

    bool set_topic_known(uint16_t topic_id, bool known) override;

    void add_client_registration(char *topic_name, uint16_t *topic_id) override;

    char *get_predefined_topic_name(uint16_t topic_id) override;

    void set_client_state(CLIENT_STATUS status) override;

    void set_client_duration(uint32_t duration) override;

    CLIENT_STATUS get_client_status() override;

    uint16_t get_client_await_msg_id() override;

    void set_client_await_msg_id(uint16_t msg_id) override;

    bool is_subscribed(const char *topic_name) override;

    void add_subscription(const char *topic_name, uint16_t topic_id, uint8_t qos) override;

    void delete_subscription(uint16_t topic_id) override;

    int8_t get_subscription_qos(const char *topic_name) override;

    uint16_t get_subscription_topic_id(const char *topic_name) override;

    bool has_client_publishes() override;

    uint16_t get_nth_subscribed_topic_id(uint16_t n) override;

    uint16_t get_client_subscription_count() override;

    bool decrement_global_subscription_count(const char *topic_name) override;

    bool increment_global_subscription_count(const char *topic_name) override;

    uint32_t get_global_topic_subscription_count(const char *topic_name) override;

    void add_client_publish(uint8_t *data, uint8_t data_len, uint16_t topic_id, bool retain, uint8_t qos, bool dup,
                            uint16_t msg_id) override;

    void get_next_publish(uint8_t *data, uint8_t *data_len, uint16_t *topic_id, bool *retain, uint8_t *qos, bool *dup,
                          uint16_t *publish_id) override;

    void set_publish_msg_id(uint16_t publish_id, uint16_t msg_id) override;

    void remove_publish_by_msg_id(uint16_t msg_id) override;

    void remove_publish_by_publish_id(uint16_t msg_id) override;

    uint16_t get_advertise_duration() override;

    bool get_gateway_id(uint8_t *gateway_id) override;

    bool get_mqtt_config(uint8_t *server_ip, uint16_t *server_port, char *client_id) override;

    bool get_mqtt_login_config(char *username, char *password) override;

    bool get_mqtt_will(char *will_topic, char *will_msg, uint8_t *will_qos, bool *will_retain) override;

    uint8_t set_mqttsn_disconnected() override;

    uint8_t set_mqtt_disconnected() override;

    uint8_t set_mqtt_connected() override;

    uint8_t set_mqttsn_connected() override;

    bool is_mqttsn_online() override;

    bool is_mqtt_online() override;

    void get_client_id(char *client_id) override;
};


#endif //CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H
