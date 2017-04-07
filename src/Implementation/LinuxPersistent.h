//
// Created by bele on 07.04.17.
//

#ifndef CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H
#define CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H


#include <PersistentInterface.h>
#include <SD.h>


struct entry_mqtt_subscription{
    uint32_t client_subscription_count;
    char topic_name[255];
};

struct entry_will {
    char willtopic[255];
    char willmsg[255];
    uint8_t willmsg_length;
    uint8_t qos;
    bool retain;
};

struct entry_subscription{
    uint16_t topic_id;
    uint8_t qos;
    char topic_name[255];

};

struct entry_registration{
    uint16_t topic_id;
    char topic_name[255];
    bool known;
};

struct entry_publish{
    uint8_t msg[255];
    uint8_t msg_length;
    uint16_t topic_id;
    uint8_t qos;
    bool retain;
    bool dup;
    uint16_t msg_id;
    uint16_t publish_id;
    uint32_t retransmition_timeout; // not used atm
};

//TODO remove pragma and test
#pragma pack(push, 1)
struct entry_client {
    char client_id[24];
    char file_number[9];
    device_address client_address;
    CLIENT_STATUS client_status;
    uint32_t duration; // changed
    uint32_t timeout;
    uint16_t await_message_id;
    message_type await_message;
};
#pragma pack(pop)

#define PERSISTENT_DEBUG 1

#define MAXIMUM_CLIENT_ID_LENGTH 24

#define MAXIMUM_TOPIC_NAME_LENGTH 255

#define SUBSCRIBE_FILE_ENDING ".SUB"

#define REGISTRATION_FILE_ENDING ".REG"

#define WILL_FILE_ENDING ".WIL"

#define PUBLISH_FILE_ENDING ".PUB"

class LinuxPersistent : public PersistentInterface {
private:
    LoggerInterface *logger;
#ifndef Arduino_h
    SDLinux SD;
#endif
    File _open_file;

    Core *core;

    entry_client _entry_client;
    entry_registration _registration_entry;
    entry_registration _predefined_registration_entry;

    bool _not_in_client_registry;
    bool _transaction_started;

    bool _error;

    // getway connection status
    bool _is_mqttsn_online = false;
    bool _is_mqtt_online = false;

    const char *client_registry = "CLIENTS";
    const char *mqtt_sub = "MQTT.SUB";
    const char *predefined_topic = "TOPICS.PRE";
    const char *mqtt_configuration = "MQTT.CON";

private:
    size_t readCharUntil(char terminator, char *buffer, size_t buffer_size) {
        if (buffer_size < 1) return 0;
        size_t index = 0;
        while (index < buffer_size) {
            int c = _open_file.read();
            if (c < 0 || c == terminator) break;
            *buffer++ = (char) c;
            index++;
        }
        return index;
    }

    size_t readLine(char *buffer, size_t buffer_size) {
        return readCharUntil('\n', (char *) buffer, buffer_size);
    }

    void create_file(const char *file_path) {
        _open_file.close();
        _open_file = SD.open(file_path, FILE_WRITE);
        _open_file.close();
    }


    void delete_file(const char *file_path) {
        _open_file.close();
        if (SD.exists((char *) file_path)) {
            SD.remove((char *) file_path);
        }
    }

    virtual int parse_file_number_to_int(entry_client *_entry_client) {
        //int empty_space = atoit(_entry_client.file_number);
        //int client_position = strtol(_entry_client.file_number, &_entry_client.file_number, 10);
        // we have to use ataoi:
        // https://github.com/esp8266/Arduino/issues/404
        // http://www.esp8266.com/viewtopic.php?f=28&t=4001
        return atoi(_entry_client->file_number);
    }

#ifndef Arduino_h
    void setRootPath(char* rootPath){
        SD.setRootPath(rootPath);
    }
#endif

    uint32_t find_empty_entry_space_in_registry() {
        _open_file.flush();
        _open_file.close();
        _open_file = SD.open(client_registry, FILE_READ);


        size_t readChars = 0;
        uint16_t entry_number = 0;
        do {
            memset(&_entry_client, 0, sizeof(entry_client));
            size_t buffer_size = sizeof(entry_client);
            readChars = _open_file.read((char *) &_entry_client, buffer_size);
            if (readChars == 0) {
                // end of file reached
                break;
            }

            if (strlen(_entry_client.client_id) == 0) {
                // empty space found
                return entry_number;
            }
            entry_number++;
        } while (readChars > 0);
        return entry_number;
    }

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

    void remove_publish_by_publish_id(uint16_t publish_id) override;

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

private:
    bool parse_ipaddress_after_space(uint8_t *destination, const char *source, uint16_t max) {
        const char *cpy_source = source;
        char tmp_only_id[17];
        memset(tmp_only_id, 0, sizeof(tmp_only_id));

        // see: https://github.com/esp8266/Arduino/issues/572
        // char space[] = " ";
        // uint16_t start = (uint16_t) strcspn(source, space);
        uint16_t start = 0;
        while (*source++ != ' ') {
            start++;
            if (*source == '\n' || start == max) {
                return false;
            }
        }
        // see: https://github.com/esp8266/Arduino/issues/572
        // char newline[] = "";
        // uint16_t end = (uint16_t) strcspn(source, newline);
        uint16_t end = start;
        while (*source != '\n' && *source != 0) {
            *source++;
            end++;
            if (end > max) {
                return false;
            }
        }

        if (start + 1 < max && start != end && (end - start) < 17 && (end - start) > 7) {
            memcpy(tmp_only_id, &cpy_source[start + 1], end - start);
            char *found = strtok(tmp_only_id, ".");
            if (found == NULL) {
                return false;
            }
            uint8_t numbers = 0;
            while (found) {
                uint16_t tmp_number = (uint16_t) atoi(found);
                if (tmp_number > 255) {
                    return false;
                }
                *(destination + numbers) = (uint8_t) tmp_number;
                numbers++;
                found = strtok(NULL, ".");
                if (found == NULL && numbers == 4) {
                    return true;
                }
                if (found != NULL && numbers == 4) {
                    return false;
                }
            }
            return false;
        }
        return false;
    }

    bool parse_uint16_t_after_space(uint16_t *destination, const char *source, uint16_t max) {
        // lesson learned:
        // readBytesUntil does not include the untilByte
        // this means we also check for 0 (additionally to \n)
        // save a copy of the source pointer for the memcpy

        const char *cpy_source = source;
        char tmp_number[7];
        memset(tmp_number, 0, sizeof(tmp_number));
        // see: https://github.com/esp8266/Arduino/issues/572
        // char space[] = " ";
        // uint16_t start = (uint16_t) strcspn(source, space);
        uint16_t start = 0;
        while (*source++ != ' ') {
            start++;
            if (*source == '\n' || start == max) {
                return false;
            }
        }
        // see: https://github.com/esp8266/Arduino/issues/572
        // char newline[] = "";
        // uint16_t end = (uint16_t) strcspn(source, newline);
        uint16_t end = start;
        while (*source != '\n' && *source != 0) {
            *source++;
            end++;
            if (end > max) {
                return false;
            }
        }

        if (start + 1 < max && start != end && (end - start) <= 6 && (end - start) >= 1) {
            memcpy(tmp_number, &cpy_source[start + 1], end - start);
            *destination = (uint16_t) atoi(tmp_number);
            if (*destination == 0) {
                return false;
            }
            return true;
        }
        return false;
    }

    bool parse_string_after_space(char *destination, const char *source, uint16_t max) const {

        const char *cpy_source = source;
        // see: https://github.com/esp8266/Arduino/issues/572
        // char space[] = " ";
        // uint16_t start = (uint16_t) strcspn(source, space);
        uint16_t start = 0;
        while (*source++ != ' ') {
            start++;
            if (*source == '\n' || start == max) {
                return false;
            }
        }
        // see: https://github.com/esp8266/Arduino/issues/572
        // char newline[] = "";
        // uint16_t end = (uint16_t) strcspn(source, newline);
        uint16_t end = start;
        while (*source != '\n' && *source != 0) {
            *source++;
            end++;
            if (end > max) {
                return false;
            }
        }
        if (start + 1 < max && start != end) {
            memcpy(destination, &cpy_source[start + 1], end - start);
            if (strlen(destination) == 0) {
                return false;
            }
            return true;
        }
        return false;
    }

    bool parse_uint8_t_after_space(uint8_t *destination, const char *source, uint16_t max) {
        const char *cpy_source = source;
        char tmp_number[4];
        memset(tmp_number, 0, sizeof(tmp_number));
        // see: https://github.com/esp8266/Arduino/issues/572
        // char space[] = " ";
        // uint16_t start = (uint16_t) strcspn(source, space);
        uint16_t start = 0;
        while (*source++ != ' ') {
            start++;
            if (*source == '\n' || start == max) {
                return false;
            }
        }
        // see: https://github.com/esp8266/Arduino/issues/572
        // char newline[] = "";
        // uint16_t end = (uint16_t) strcspn(source, newline);
        uint16_t end = start;
        while (*source != '\n' && *source != 0) {
            *source++;
            end++;
            if (end > max) {
                return false;
            }
        }

        if (start + 1 < max && start != end && (end - start) <= 3 && (end - start) >= 1) {
            memcpy(tmp_number, &cpy_source[start + 1], end - start);
            *destination = (uint8_t) atoi(tmp_number);
            return true;
        }
        return false;
    }

};


#endif //CORE_MQTT_SN_GATEWAY_PERSISTENTIMPLEMENTATION_H
