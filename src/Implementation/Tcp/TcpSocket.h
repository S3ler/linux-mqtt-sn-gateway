//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H
#define LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H


#include <SocketInterface.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "TcpMessage.h"
#include "Queue.h"
#include "TcpConnection.h"
#include "ConnectionAcceptor.h"
#include <list>

class TcpConnection;
class ConnectionAcceptor;

#define BUFLEN 255    //Max length of buffer
#define PORT 9999     //The port on which to listen for incoming data

class TcpSocket : public SocketInterface {
private:
    struct sockaddr_in tcp_socket_address, si_other;
    LoggerInterface* logger;
    MqttSnMessageHandler* mqttSnMessageHandler;

    int tcp_socket, i, recv_len;
    Queue<TcpMessage*>receiver_queue;
    device_address broadcast_address;
    device_address own_address;

    std::list<TcpConnection*>connections;
    std::mutex connection_mutex;

    ConnectionAcceptor *tcp_connection_acceptor;

public:
    bool begin() override;

    void setLogger(LoggerInterface *logger) override;

    void setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) override;

    device_address *getBroadcastAddress() override;

    device_address *getAddress() override;

    uint8_t getMaximumMessageLength() override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) override;

    bool send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) override;

    bool loop() override;

    bool initTcpSocket();

    void receive_message(TcpMessage *msg);

    void addTcpConnection(TcpConnection *pConnection);

    void removeTcpConnection(TcpConnection *pConnection);
};


#endif //LINUX_MQTT_SN_GATEWAY_TCPSOCKET_H
