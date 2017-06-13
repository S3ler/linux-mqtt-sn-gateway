//
// Created by bele on 13.06.17.
//

#include "TcpConnection.h"

device_address *TcpConnection::getDeviceAddress() {
    return &deviceAddress;
}

bool TcpConnection::send_message(uint8_t *payload, uint16_t payload_length) {
    ssize_t send_bytes = send(socketfd, payload, payload_length, 0);
    if (send_bytes == -1) {
        // TODO error!
        // disconnect
    }
    if (send_bytes == payload_length) {
        return true;
    }
    ssize_t left_bytes = (ssize_t) payload_length - send_bytes;
    // TODO unlikely to happen, i will implement it later
    // TODO implement
    return false;
}

void TcpConnection::setSocketFd(int socketfd) {
    this->socketfd = socketfd;
}

void TcpConnection::setSocketAddress(sockaddr_in *socketAddress) {
    this->socketAddress = socketAddress;
    device_address address;
    memcpy(&address.bytes[0], &socketAddress->sin_addr, 4);
    memcmp(&address.bytes[4], &socketAddress->sin_port, 2);
    this->deviceAddress = address;
    // TODO check
}

void TcpConnection::start_loop() {
    this->thread = std::thread(&TcpConnection::loop, this);
    this->tcp_socket->addTcpConnection(this);
}

void TcpConnection::loop() {
    while (!stopped) {
        // TODO implement me

        ssize_t read_length = recv(socketfd, receive_buffer, receive_buffer_size, 0);
        if (read_length == -1) {
            // TODO error!
            // disconnect
        }
        if (read_length == receive_buffer[0]) {
            // single packet
            TcpMessage* msg = new TcpMessage();
            msg->setDeviceAddress(&deviceAddress);
            msg->setPayload(receive_buffer, read_length);
            tcp_socket->receive_message(msg);
        }
        // else is very unlikely
        // TODO unlikely to happen, i will implement it later
        // TODO implement
    }
    this->tcp_socket->removeTcpConnection(this);
    close(socketfd);
}

void TcpConnection::stop_loop() {
    this->stopped = true;
    this->thread.join();
}

void TcpConnection::setTcpSocket(TcpSocket *pSocket) {
    this->tcp_socket = pSocket;
}
