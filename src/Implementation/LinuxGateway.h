//
// Created by bele on 10.04.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
#define LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H


#include <Gateway.h>
#include <paho/PahoMqttMessageHandler.h>
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

public:
    bool begin();
    void setRootPath( char* rootPath);
};


#endif //LINUX_MQTT_SN_GATEWAY_LINUXGATEWAY_H
