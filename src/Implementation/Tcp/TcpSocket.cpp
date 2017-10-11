//git submodule update --remote --merge
// Created by bele on 13.06.17.
//

#include "TcpSocket.h"
#include "ConnectionAcceptor.h"

bool TcpSocket::begin() {
    if (mqttSnMessageHandler == nullptr) {
        return false;
    }
    if (!initTcpSocket()) {
        return false;
    }

    memset(&broadcast_address, 255, sizeof(device_address));

    if (tcp_socket >= 0) {
        mqttSnMessageHandler->notify_socket_connected();
    }

    return tcp_socket >= 0;
}

void TcpSocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void TcpSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *TcpSocket::getBroadcastAddress() {
    return &broadcast_address;
}

device_address *TcpSocket::getAddress() {
    // http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
    memset(&own_address, 0, sizeof(device_address));
    if (tcp_socket < 0) {
        return &own_address;
    }
    struct ifreq ifr;
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    ioctl(tcp_socket, SIOCGIFADDR, &ifr);

    /* display result */
    // printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    /* parse result into device_address */
    struct sockaddr_in *addr = ((struct sockaddr_in *) &ifr.ifr_addr);

    uint32_t ip_address = addr->sin_addr.s_addr;
    uint16_t port = addr->sin_port;

    memcpy(&own_address.bytes, &ip_address, sizeof(ip_address));
    if (port == 0) {
        port = PORT;
    }
    memcpy(&own_address.bytes[sizeof(ip_address)], &port, sizeof(port));

    // another version, does the same
    memset(&own_address, 0, sizeof(device_address));

    struct ifaddrs *ifaddr, *ifa;
    int family, s, n;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    /* Walk through linked list, maintaining head pointer so we
       can free list later */

    for (ifa = ifaddr, n = 0; ifa != NULL; ifa = ifa->ifa_next, n++) {
        if (ifa->ifa_addr == NULL) {
            continue;
        }

        const char *interfaceName = "eth0";
        if (ifa->ifa_addr->sa_family == AF_INET && (strcmp(interfaceName, ifa->ifa_name) == 0)) {

            s = getnameinfo(ifa->ifa_addr,
                            (family == AF_INET) ? sizeof(struct sockaddr_in) :
                            sizeof(struct sockaddr_in6),
                            host, NI_MAXHOST,
                            NULL, 0, NI_NUMERICHOST);

            struct sockaddr_in *addr = ((struct sockaddr_in *) &ifa->ifa_addr);
            if (s != 0) {
                return &own_address;
            }
            uint32_t ip_address = 0;
            inet_pton(AF_INET, host, &ip_address);
            //uint32_t ip_address = addr->sin_addr.s_addr;
            uint16_t port = 0;

            memcpy(&own_address.bytes, &ip_address, sizeof(ip_address));
            if (port == 0) {
                port = PORT;
            }
            memcpy(&own_address.bytes[sizeof(ip_address)], &port, sizeof(port));
        }
    }

    freeifaddrs(ifaddr);


    return &own_address;
}

uint8_t TcpSocket::getMaximumMessageLength() {
    return BUFLEN;
}

bool TcpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    std::lock_guard<std::mutex> lock_guard(connection_mutex);
    for (auto &&connection : connections) {
        if (memcmp(destination, connection->getDeviceAddress(), sizeof(device_address)) == 0) {
            return connection->send_message(bytes, bytes_len);
        }
    }
    return false;
}

bool TcpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return this->send(destination, bytes, bytes_len);
}

void TcpSocket::receive_message(TcpMessage *msg) {
    receiver_queue.push(msg);
}


bool TcpSocket::loop() {
    if (!receiver_queue.empty()) {
        TcpMessage *msg = receiver_queue.pop();
        // FIXME
        // TODO solve:
        // mqttSnMessageHandler->receiveData(&msg->address, (uint8_t *) &msg->payload);
        delete (msg);
    }
    return true;
}


bool TcpSocket::initTcpSocket() {

    //create a TCP socket
    if ((tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        return false;
    }

    int resuse_addr = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &resuse_addr, sizeof(resuse_addr)) == -1) {
        return false;
    }

    // zero out the structure
    memset((char *) &tcp_socket_address, 0, sizeof(tcp_socket_address));

    tcp_socket_address.sin_family = AF_INET;
    tcp_socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    tcp_socket_address.sin_port = htons(PORT);

    //bind socket to port
    if (bind(tcp_socket, (struct sockaddr *) &tcp_socket_address, sizeof(tcp_socket_address)) == -1) {
        return false;
    }

    listen(tcp_socket, 5);

    this->tcp_connection_acceptor = new ConnectionAcceptor();
    this->tcp_connection_acceptor->setTcpSocketFd(tcp_socket);
    this->tcp_connection_acceptor->setTcpSocket(this);
    this->tcp_connection_acceptor->start_loop();
    /*
    // set timout
    struct timeval tcp_socket_timeval;
    tcp_socket_timeval.tv_sec = 0;  // 0 Secs Timeout
    tcp_socket_timeval.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(tcp_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &tcp_socket_timeval, sizeof(struct timeval)) == -1) {
        return false;
    }
    */

    /*
    // enable broadcast
    int udp_socket_broadcast_enable = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &udp_socket_broadcast_enable,
                   sizeof(udp_socket_broadcast_enable)) == -1) {
        return false;
    }
    */

    return true;
}

void TcpSocket::addTcpConnection(TcpConnection *pConnection) {
    std::lock_guard<std::mutex> lock_guard(connection_mutex);
    connections.push_back(pConnection);
}

void TcpSocket::removeTcpConnection(TcpConnection *pConnection) {
    std::lock_guard<std::mutex> lock_guard(connection_mutex);
    for (auto &&connection : connections) {
        if (connection == pConnection) {
            connections.remove(connection);
        }
    }

}

