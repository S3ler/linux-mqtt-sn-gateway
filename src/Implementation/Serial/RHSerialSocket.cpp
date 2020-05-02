#include "RHSerialSocket.h"

#include <RHReliableDatagram.h>

RHSerialSocket::RHSerialSocket() : ownAddress{0x11},
                                   portname{"/dev/tty"},
                                   baud{38400} {
}



bool RHSerialSocket::begin() {

    if (logger != nullptr) {
        logger->log("Starting RHSerialSocket for MQTT-SN connection", 2);
    }

    serialPort = new HardwareSerial(portname.c_str());
    serialPort->begin(baud);

    driver = new RH_Serial(*serialPort);

    if (this->reliable) {
        manager = new RHReliableDatagram(*driver, this->ownAddress);
    }
    else {
        manager = new RHDatagram(*driver, this->ownAddress);
    }

    this->setManager(manager);

    return RHDatagramSocket::begin();
}
