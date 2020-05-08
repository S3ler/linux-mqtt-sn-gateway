#include "RHSerialSocket.h"
#include <RHReliableDatagram.h>


RHSerialSocket::RHSerialSocket() : ownAddress{0x11},
                                   portname{"/dev/ttyUSB0"},
                                   baud{38400} {
}


bool RHSerialSocket::begin() {

    if (logger != nullptr) {
        logger->log("Starting RHSerialSocket for MQTT-SN connection", 2);
    }

    serialPort = new HardwareSerial(portname.c_str());
    serialPort->begin(baud);

    driver = new RH_Serial(*serialPort);

    this->setManager(new RHReliableDatagram(*driver, this->ownAddress));
    this->manager->setTimeout(2000);
    this->manager->setRetries(2);

    return RHReliableDatagramSocket::begin();
}
