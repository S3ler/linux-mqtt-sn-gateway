//
// Created by bele on 07.04.17.
//

#include "PersistentImplementation.h"

bool PersistentImplementation::begin() {
    return false;
}

void PersistentImplementation::setCore(Core *core) {

}

void PersistentImplementation::setLogger(LoggerInterface *logger) {

}

void PersistentImplementation::start_client_transaction(const char *client_id) {

}

void PersistentImplementation::start_client_transaction(device_address *address) {

}

uint8_t PersistentImplementation::apply_transaction() {
    return 0;
}

bool PersistentImplementation::client_exist() {
    return 0;
}

void PersistentImplementation::delete_client(const char *client_id) {

}

void PersistentImplementation::add_client(const char *client_id, device_address *address, uint32_t duration) {

}

void PersistentImplementation::reset_client(const char *client_id, device_address *address, uint32_t duration) {

}

void PersistentImplementation::set_client_await_message(message_type msg_type) {

}

message_type PersistentImplementation::get_client_await_message_type() {
    return MQTTSN_PINGREQ;
}

void PersistentImplementation::set_timeout(uint32_t timeout) {

}

bool PersistentImplementation::has_client_will() {
    return false;
}

void PersistentImplementation::get_client_will(char *target_willtopic, uint8_t *target_willmsg,
                                               uint8_t *target_willmsg_length, uint8_t *target_qos,
                                               bool *target_retain) {

}

void PersistentImplementation::set_client_willtopic(char *willtopic, uint8_t qos, bool retain) {

}

void PersistentImplementation::set_client_willmessage(uint8_t *willmsg, uint8_t willmsg_length) {

}

void PersistentImplementation::delete_will() {

}

void PersistentImplementation::get_last_client_address(device_address *address) {

}

void PersistentImplementation::get_nth_client(uint64_t n, char *client_id, device_address *target_address,
                                              CLIENT_STATUS *target_status, uint32_t *target_duration,
                                              uint32_t *target_timeout) {

}

const char *PersistentImplementation::get_topic_name(uint16_t topic_id) {
    return nullptr;
}

uint16_t PersistentImplementation::get_topic_id(char *topic_name) {
    return 0;
}

bool PersistentImplementation::is_topic_known(uint16_t topic_id) {
    return false;
}

bool PersistentImplementation::set_topic_known(uint16_t topic_id, bool known) {
    return false;
}

void PersistentImplementation::add_client_registration(char *topic_name, uint16_t *topic_id) {

}

char *PersistentImplementation::get_predefined_topic_name(uint16_t topic_id) {
    return nullptr;
}

void PersistentImplementation::set_client_state(CLIENT_STATUS status) {

}

void PersistentImplementation::set_client_duration(uint32_t duration) {

}

CLIENT_STATUS PersistentImplementation::get_client_status() {
    return EMPTY;
}

uint16_t PersistentImplementation::get_client_await_msg_id() {
    return 0;
}

void PersistentImplementation::set_client_await_msg_id(uint16_t msg_id) {

}

bool PersistentImplementation::is_subscribed(const char *topic_name) {
    return false;
}

void PersistentImplementation::add_subscription(const char *topic_name, uint16_t topic_id, uint8_t qos) {

}

void PersistentImplementation::delete_subscription(uint16_t topic_id) {

}

int8_t PersistentImplementation::get_subscription_qos(const char *topic_name) {
    return 0;
}

uint16_t PersistentImplementation::get_subscription_topic_id(const char *topic_name) {
    return 0;
}

bool PersistentImplementation::has_client_publishes() {
    return false;
}

uint16_t PersistentImplementation::get_nth_subscribed_topic_id(uint16_t n) {
    return 0;
}

uint16_t PersistentImplementation::get_client_subscription_count() {
    return 0;
}

bool PersistentImplementation::decrement_global_subscription_count(const char *topic_name) {
    return false;
}

bool PersistentImplementation::increment_global_subscription_count(const char *topic_name) {
    return false;
}

uint32_t PersistentImplementation::get_global_topic_subscription_count(const char *topic_name) {
    return 0;
}

void PersistentImplementation::add_client_publish(uint8_t *data, uint8_t data_len, uint16_t topic_id, bool retain,
                                                  uint8_t qos, bool dup, uint16_t msg_id) {

}

void PersistentImplementation::get_next_publish(uint8_t *data, uint8_t *data_len, uint16_t *topic_id, bool *retain,
                                                uint8_t *qos, bool *dup, uint16_t *publish_id) {

}

void PersistentImplementation::set_publish_msg_id(uint16_t publish_id, uint16_t msg_id) {

}

void PersistentImplementation::remove_publish_by_msg_id(uint16_t msg_id) {

}

void PersistentImplementation::remove_publish_by_publish_id(uint16_t msg_id) {

}

uint16_t PersistentImplementation::get_advertise_duration() {
    return 0;
}

bool PersistentImplementation::get_gateway_id(uint8_t *gateway_id) {
    return false;
}

bool PersistentImplementation::get_mqtt_config(uint8_t *server_ip, uint16_t *server_port, char *client_id) {
    return false;
}

bool PersistentImplementation::get_mqtt_login_config(char *username, char *password) {
    return false;
}

bool PersistentImplementation::get_mqtt_will(char *will_topic, char *will_msg, uint8_t *will_qos, bool *will_retain) {
    return false;
}

uint8_t PersistentImplementation::set_mqttsn_disconnected() {
    return 0;
}

uint8_t PersistentImplementation::set_mqtt_disconnected() {
    return 0;
}

uint8_t PersistentImplementation::set_mqtt_connected() {
    return 0;
}

uint8_t PersistentImplementation::set_mqttsn_connected() {
    return 0;
}

bool PersistentImplementation::is_mqttsn_online() {
    return false;
}

bool PersistentImplementation::is_mqtt_online() {
    return false;
}

void PersistentImplementation::get_client_id(char *client_id) {

}
