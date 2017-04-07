//
// Created by bele on 07.04.17.
//

#include "LinuxPersistent.h"

bool LinuxPersistent::begin() {
    return false;
}

void LinuxPersistent::setCore(Core *core) {

}

void LinuxPersistent::setLogger(LoggerInterface *logger) {

}

void LinuxPersistent::start_client_transaction(const char *client_id) {

}

void LinuxPersistent::start_client_transaction(device_address *address) {

}

uint8_t LinuxPersistent::apply_transaction() {
    return 0;
}

bool LinuxPersistent::client_exist() {
    return 0;
}

void LinuxPersistent::delete_client(const char *client_id) {

}

void LinuxPersistent::add_client(const char *client_id, device_address *address, uint32_t duration) {

}

void LinuxPersistent::reset_client(const char *client_id, device_address *address, uint32_t duration) {

}

void LinuxPersistent::set_client_await_message(message_type msg_type) {

}

message_type LinuxPersistent::get_client_await_message_type() {
    return MQTTSN_PINGREQ;
}

void LinuxPersistent::set_timeout(uint32_t timeout) {

}

bool LinuxPersistent::has_client_will() {
    return false;
}

void LinuxPersistent::get_client_will(char *target_willtopic, uint8_t *target_willmsg,
                                               uint8_t *target_willmsg_length, uint8_t *target_qos,
                                               bool *target_retain) {

}

void LinuxPersistent::set_client_willtopic(char *willtopic, uint8_t qos, bool retain) {

}

void LinuxPersistent::set_client_willmessage(uint8_t *willmsg, uint8_t willmsg_length) {

}

void LinuxPersistent::delete_will() {

}

void LinuxPersistent::get_last_client_address(device_address *address) {

}

void LinuxPersistent::get_nth_client(uint64_t n, char *client_id, device_address *target_address,
                                              CLIENT_STATUS *target_status, uint32_t *target_duration,
                                              uint32_t *target_timeout) {

}

const char *LinuxPersistent::get_topic_name(uint16_t topic_id) {
    return nullptr;
}

uint16_t LinuxPersistent::get_topic_id(char *topic_name) {
    return 0;
}

bool LinuxPersistent::is_topic_known(uint16_t topic_id) {
    return false;
}

bool LinuxPersistent::set_topic_known(uint16_t topic_id, bool known) {
    return false;
}

void LinuxPersistent::add_client_registration(char *topic_name, uint16_t *topic_id) {

}

char *LinuxPersistent::get_predefined_topic_name(uint16_t topic_id) {
    return nullptr;
}

void LinuxPersistent::set_client_state(CLIENT_STATUS status) {

}

void LinuxPersistent::set_client_duration(uint32_t duration) {

}

CLIENT_STATUS LinuxPersistent::get_client_status() {
    return EMPTY;
}

uint16_t LinuxPersistent::get_client_await_msg_id() {
    return 0;
}

void LinuxPersistent::set_client_await_msg_id(uint16_t msg_id) {

}

bool LinuxPersistent::is_subscribed(const char *topic_name) {
    return false;
}

void LinuxPersistent::add_subscription(const char *topic_name, uint16_t topic_id, uint8_t qos) {

}

void LinuxPersistent::delete_subscription(uint16_t topic_id) {

}

int8_t LinuxPersistent::get_subscription_qos(const char *topic_name) {
    return 0;
}

uint16_t LinuxPersistent::get_subscription_topic_id(const char *topic_name) {
    return 0;
}

bool LinuxPersistent::has_client_publishes() {
    return false;
}

uint16_t LinuxPersistent::get_nth_subscribed_topic_id(uint16_t n) {
    return 0;
}

uint16_t LinuxPersistent::get_client_subscription_count() {
    return 0;
}

bool LinuxPersistent::decrement_global_subscription_count(const char *topic_name) {
    return false;
}

bool LinuxPersistent::increment_global_subscription_count(const char *topic_name) {
    return false;
}

uint32_t LinuxPersistent::get_global_topic_subscription_count(const char *topic_name) {
    return 0;
}

void LinuxPersistent::add_client_publish(uint8_t *data, uint8_t data_len, uint16_t topic_id, bool retain,
                                                  uint8_t qos, bool dup, uint16_t msg_id) {

}

void LinuxPersistent::get_next_publish(uint8_t *data, uint8_t *data_len, uint16_t *topic_id, bool *retain,
                                                uint8_t *qos, bool *dup, uint16_t *publish_id) {

}

void LinuxPersistent::set_publish_msg_id(uint16_t publish_id, uint16_t msg_id) {

}

void LinuxPersistent::remove_publish_by_msg_id(uint16_t msg_id) {

}

void LinuxPersistent::remove_publish_by_publish_id(uint16_t msg_id) {

}

uint16_t LinuxPersistent::get_advertise_duration() {
    return 0;
}

bool LinuxPersistent::get_gateway_id(uint8_t *gateway_id) {
    return false;
}

bool LinuxPersistent::get_mqtt_config(uint8_t *server_ip, uint16_t *server_port, char *client_id) {
    return false;
}

bool LinuxPersistent::get_mqtt_login_config(char *username, char *password) {
    return false;
}

bool LinuxPersistent::get_mqtt_will(char *will_topic, char *will_msg, uint8_t *will_qos, bool *will_retain) {
    return false;
}

uint8_t LinuxPersistent::set_mqttsn_disconnected() {
    return 0;
}

uint8_t LinuxPersistent::set_mqtt_disconnected() {
    return 0;
}

uint8_t LinuxPersistent::set_mqtt_connected() {
    return 0;
}

uint8_t LinuxPersistent::set_mqttsn_connected() {
    return 0;
}

bool LinuxPersistent::is_mqttsn_online() {
    return false;
}

bool LinuxPersistent::is_mqtt_online() {
    return false;
}

void LinuxPersistent::get_client_id(char *client_id) {

}
