//
// Created by bele on 13.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_TCPCONNECTION_H
#define LINUX_MQTT_SN_GATEWAY_TCPCONNECTION_H


#include <global_defines.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "TcpSocket.h"
#include <unistd.h>

#define RECEIVE_BUFFER_SIZE 255
class TcpSocket;

class TcpConnection {
private:
    device_address deviceAddress;
    int socketfd;
    sockaddr_in *socketAddress;
    std::thread thread;
    bool stopped;
    size_t receive_buffer_size = RECEIVE_BUFFER_SIZE;
    uint8_t receive_buffer[RECEIVE_BUFFER_SIZE];
    TcpSocket *tcp_socket;

public:
    device_address* getDeviceAddress();
    bool send_message(uint8_t* payload, uint16_t payload_length);

    void setSocketFd(int socketfd);


    void setSocketAddress(sockaddr_in *socketAddress);

    void start_loop();
    void stop_loop();
    void loop();


    void setTcpSocket(TcpSocket *pSocket);

};


#endif //LINUX_MQTT_SN_GATEWAY_TCPCONNECTION_H
