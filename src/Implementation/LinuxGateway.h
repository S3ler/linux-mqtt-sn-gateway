//
// Created by bele on 10.04.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
#define LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H

#include <thread>
#include <Gateway.h>
#include <paho/PahoMqttMessageHandler.h>
#include <atomic>
#include "LinuxUdpSocket.h"
#include "LinuxPersistent.h"
#include "LinuxLogger.h"
#include "LinuxSystem.h"

class LinuxGateway : public Gateway{
    LinuxUdpSocket udpSocket;
    LinuxPersistent persistent;

    PahoMqttMessageHandler mqtt;
    LinuxLogger logger;
    LinuxSystem systemImpl;

    std::thread thread;
    std::atomic<bool> stopped{false};

public:
    bool begin();
    void setRootPath( char* rootPath);

    void start_loop();
    void dispatch_loop();
    void stop_loop();



};


#endif //LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
