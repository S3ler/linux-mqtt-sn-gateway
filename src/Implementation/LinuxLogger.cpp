//
// Created by bele on 07.04.17.
//

#include "LinuxLogger.h"

bool LinuxLogger::begin() {
    return true;
}

void LinuxLogger::set_log_lvl(uint8_t log_lvl) {
    this->current_log_lvl = log_lvl;
}

void LinuxLogger::log(char *msg, uint8_t log_lvl) {
    log((const char *) msg, log_lvl);
}

void LinuxLogger::log(const char *msg, uint8_t log_lvl) {
    if (log_lvl > current_log_lvl) {
        return;
    }
    Serial.println();
    char millis_buffer[26];
    sprintf(millis_buffer, "%ld", millis());
    Serial.print(millis_buffer);
    Serial.print(": ");
    Serial.print(msg);
}

void LinuxLogger::start_log(char *msg, uint8_t log_lvl) {
    start_log((const char *) msg, log_lvl);
}

void LinuxLogger::start_log(const char *msg, uint8_t log_lvl) {
    last_started_log_lvl = log_lvl;
    if (last_started_log_lvl > current_log_lvl) {
        return;
    }
    Serial.println();
    char millis_buffer[26];
    sprintf(millis_buffer, "%ld", millis());
    Serial.print(millis_buffer);
    Serial.print(": ");
    Serial.print(msg);
}

void LinuxLogger::set_current_log_lvl(uint8_t log_lvl) {
    last_started_log_lvl = log_lvl;
}

void LinuxLogger::append_log(char *msg) {
    append_log((const char *) msg);
}

void LinuxLogger::append_log(const char *msg) {
    if (last_started_log_lvl > current_log_lvl) {
        return;
    }
    Serial.print(msg);
}
