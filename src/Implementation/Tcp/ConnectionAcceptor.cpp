//
// Created by bele on 13.06.17.
//

#include "ConnectionAcceptor.h"

void ConnectionAcceptor::setTcpSocket(TcpSocket *tcp_socket) {
    this->tcp_socket = tcp_socket;
}

void ConnectionAcceptor::setTcpSocketFd(int tcp_socket) {
    this->tcp_socket_fd = tcp_socket;
}

void ConnectionAcceptor::start_loop() {
    this->thread = std::thread(&ConnectionAcceptor::loop, this);
}

void ConnectionAcceptor::loop() {
    while (!stopped || tcp_socket_fd < 0) {
        struct sockaddr_in *cli_addr;
        socklen_t clilen;
        clilen = sizeof(cli_addr);
        int newsockfd = accept(tcp_socket_fd, (struct sockaddr *) cli_addr, &clilen);
        if (newsockfd < 0) {
            // TODO error
            // or break? and mention error
            continue;
        }
        // TODO check cli_addr;
        TcpConnection *tcpConnection = new TcpConnection();
        tcpConnection->setSocketFd(newsockfd);
        tcpConnection->setSocketAddress(cli_addr);
        tcpConnection->setTcpSocket(tcp_socket);
        tcpConnection->start_loop();
    }
    close(tcp_socket_fd);
}

void ConnectionAcceptor::stop_loop() {
    this->stopped = stopped;
    this->thread.join();
}

