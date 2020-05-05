//
// RHSerialSocket is a socket that communicates using RadioHead's RHDatagram protocol vs.
// the protocol found in LinuxSerialSocket.h
// 
// Created by joelkoz on 05.01.20.
//
#ifndef TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H
#define TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H

#include <RH_Serial.h>
#include <RHReliableDatagramSocket.h>
#include <HardwareSerial.h>

#include <string>

class RHSerialSocket : public RHReliableDatagramSocket {

    public:
       RHSerialSocket();

       virtual bool begin() override;


       void setSerialDeviceName(const char* deviceName) {
            this->portname = deviceName;
       }


       void setBaud(int baud) {
           this->baud = baud;
       }

       void setOwnAddress(uint8_t ownAddress) {
           this->ownAddress = ownAddress;
       }

    private:
       uint8_t ownAddress;
       std::string portname;
       int baud;
       HardwareSerial* serialPort;
       RH_Serial* driver;
};


#endif