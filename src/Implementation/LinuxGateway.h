//
// Created by bele on 10.04.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
#define LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H

#include <thread>
#include <Gateway.h>
#include <paho/PahoMqttMessageHandler.h>
#include <atomic>
#include <Serial/LinuxSerialSocket.h>
#include "Udp/LinuxUdpSocket.h"
#include "LinuxPersistent.h"
#include "LinuxLogger.h"
#include "LinuxSystem.h"
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_BLE)
#include <Ble/BLESocket.h>
#endif
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
#include <bcm2835.h>
#include <RF95Socket.h>
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>
#endif

class LinuxGateway : public Gateway {
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_UDP)
    LinuxUdpSocket mqttsnSocket;
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_TCP)
#error "gateway transmission protocol TCP not implemented yet."
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_BLE)
    BLESocket mqttsnSocket;
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_SERIAL)
    LinuxSerialSocket mqttsnSocket;
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    //void setRadioHeadSocket(RF95Socket& mqttsnSocket);
    RH_NRF24 rh_driver;//(2,15);//(RPI_V2_GPIO_P1_18, RPI_V2_GPIO_P1_24);
    RHReliableDatagram manager;//(rh_driver);//(rh_driver);
    RF95Socket mqttsnSocket;
#elif defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)

#else
#error "No gateway transmission protocol defined."
#endif


    LinuxPersistent persistent;

    PahoMqttMessageHandler mqtt;
    LinuxLogger logger;
    LinuxSystem systemImpl;
    LinuxSystem durationSystemImpl;

    std::thread thread;
    std::atomic<bool> stopped{false};

public:
#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    LinuxGateway();
#endif

    bool begin();

    void setRootPath(char *rootPath);

    void start_loop();

    void dispatch_loop();

    void stop_loop();
#if defined(D_GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    void setRadioHeadSocket(RF95Socket* mqttsnSocket);
#endif

};


#endif //LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
