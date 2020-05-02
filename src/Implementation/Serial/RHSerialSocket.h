//
// RHSerialSocket is a socket that communicates using RadioHead's RHDatagram protocol vs.
// the protocol found in LinuxSerialSocket.h
// 
// Created by joelkoz on 05.01.20.
//
#ifndef TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H
#define TEST_MQTT_SN_GATEWAY_LINUXSERIALSOCKET_H

#include <HardwareSerial.h>
#include <RH_Serial.h>
#include <RHDatagramSocket.h>

#include <string>

class RHSerialSocket : public RHDatagramSocket {

    public:
       RHSerialSocket();

       virtual bool begin() override;


       void setSerialDeviceName(const char* deviceName) {
            this->portname = deviceName;
       }


       void setBaud(int baud) {
           this->baud = baud;
       }


       void setReliable(bool reliable) {
           this->reliable = reliable;
       }

       void setOwnAddress(uint8_t ownAddress) {
           this->ownAddress = ownAddress;
       }

    private:
       uint8_t ownAddress;
       std::string portname;
       int baud;
       bool reliable;
       HardwareSerial* serialPort;
       RH_Serial* driver;
};


#endif