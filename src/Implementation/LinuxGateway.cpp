//
// Created by bele on 10.04.17.
//

#include "LinuxGateway.h"

bool LinuxGateway::begin() {
    logger.start_log("Linux MQTT-SN Gateway version 0.0.1a starting", 1);

    Gateway::setLoggerInterface(&logger);
    Gateway::setSocketInterface(&udpSocket);
    Gateway::setMqttInterface(&mqtt);
    Gateway::setPersistentInterface(&persistent);
    Gateway::setSystemInterface(&systemImpl);

    return Gateway::begin();
}

void LinuxGateway::setRootPath( char *rootPath) {
    persistent.setRootPath(rootPath);
}
