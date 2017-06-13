//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_CONNECTIONACCEPTOR_H
#define LINUX_MQTT_SN_GATEWAY_CONNECTIONACCEPTOR_H


#include "TcpSocket.h"
#include <thread>
#include <unistd.h>

class TcpSocket;

class ConnectionAcceptor {
private:
    TcpSocket *tcp_socket;
    int tcp_socket_fd;
    std::thread thread;
    bool stopped;

public:
    void setTcpSocket(TcpSocket *tcp_socket);

    void setTcpSocketFd(int tcp_socket);

    void start_loop();

    void stop_loop();

    void loop();
};


#endif //LINUX_MQTT_SN_GATEWAY_CONNECTIONACCEPTOR_H
