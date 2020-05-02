//
// Created by bele on 07.04.17.
//

#include "LinuxSystem.h"
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
LinuxSystem::LinuxSystem() { }
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
LinuxSystem::LinuxSystem() { }
#else
#ifndef Arduino_h
LinuxSystem::LinuxSystem() {
//    Code in this module does not depend on the ability to reset the timer, and
//    resetTimerValue() is not available on Raspberry Pi, so don't call it.
//    resetTimerValue();
}
#endif
#endif

void LinuxSystem::set_heartbeat(uint32_t period) {
    this->heartbeat_period = period;
}

uint32_t LinuxSystem::get_heartbeat() {
    return this->heartbeat_period;
}

bool LinuxSystem::has_beaten() {
    uint32_t current = millis();
    if (current - heartbeat_current > heartbeat_period) {
        this->heartbeat_current = current;
        return true;
    }
    return false;
}

uint32_t LinuxSystem::get_elapsed_time() {
    uint32_t current = millis();
    uint32_t elapsed_time = current - elapsed_current;
    elapsed_current = current;
    return elapsed_time;
}

void LinuxSystem::sleep(uint32_t duration) {
    delay(duration);
}

void LinuxSystem::exit() {
    throw std::exception();
}
