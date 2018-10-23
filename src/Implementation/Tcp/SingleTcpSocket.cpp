//git submodule update --remote --merge
// Created by bele on 13.06.17.
//


#include <fcntl.h>
#include <zconf.h>
#include "SingleTcpSocket.h"

bool SingleTcpSocket::begin() {
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

void SingleTcpSocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void SingleTcpSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *SingleTcpSocket::getBroadcastAddress() {
    return &broadcast_address;
}

device_address *SingleTcpSocket::getAddress() {
    // http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
    memset(&own_address, 0, sizeof(device_address));
    if (tcp_socket < 0) {
        return &own_address;
    }
    struct ifreq ifr;
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, network_interface, IFNAMSIZ - 1);

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

uint8_t SingleTcpSocket::getMaximumMessageLength() {
    return BUFLEN;
}

bool SingleTcpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    if (memcmp(destination, &broadcast_address, sizeof(device_address)) == 0) {
        // broadcasting is ignored
        return true;
    }
    if (connection_socket == 0) {
        return true;
    }
    return (write(connection_socket, bytes, bytes_len) == bytes_len);
}

bool SingleTcpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return this->send(destination, bytes, bytes_len);
}

bool SingleTcpSocket::loop() {
    if (tcp_socket < 0) {
        // socket disconnected
        mqttSnMessageHandler->notify_socket_disconnected();
        if (this->begin()) {
            mqttSnMessageHandler->notify_socket_connected();
            return true;
        }
        return false;
    }

    if (connection_socket <= 0) {

        save_fd = fcntl(tcp_socket, F_GETFL);
        save_fd |= O_NONBLOCK;
        fcntl(tcp_socket, F_SETFL, save_fd);

        connection_socket_address_len = sizeof(struct sockaddr_in);
        connection_socket = accept(tcp_socket,
                                   (struct sockaddr *) &connection_socket_address,
                                   &connection_socket_address_len);

        if (connection_socket > 0) {
            // connection established
            // printf("Client %s is connected ...\n",
            //       inet_ntoa(connection_socket_address.sin_addr));
        } else {
            //printf("Poll: no Client at the Socket ...\n");
            return true;
        }
    } else {
        // client connected
        /* Socket für die Datenübertragung auf NON-Blocking */
        save_fd = fcntl(connection_socket, F_GETFL);
        save_fd |= O_NONBLOCK;
        fcntl(connection_socket, F_SETFL, save_fd);
        /* Daten verarbeiten */
        uint8_t buf[BUFLEN];
        memset(buf, 0x0, sizeof(buf));
        ssize_t recv_len = recv(connection_socket, buf, sizeof(buf), 0);
        if (recv_len > 0) {
            device_address client_address = getDevice_address(&tcp_socket_address);
            mqttSnMessageHandler->receiveData(&client_address, (uint8_t *) &buf);
        }
    }

    return true;
}


bool SingleTcpSocket::initTcpSocket() {

    //create a TCP socket
    if (!(tcp_socket = socket(AF_INET, SOCK_STREAM, 0)) > 0) {
        return false;
    }

    // zero out the structure
    memset((char *) &tcp_socket_address, 0, sizeof(tcp_socket_address));

    tcp_socket_address.sin_family = AF_INET;
    tcp_socket_address.sin_addr.s_addr = INADDR_ANY;
    tcp_socket_address.sin_port = htons(PORT);

    const int resuse_addr = 1;
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &resuse_addr, sizeof(int)) == -1) {
        return false;
    }

    //bind socket to port
    if (bind(tcp_socket, (struct sockaddr *) &tcp_socket_address, sizeof(tcp_socket_address)) != 0) {
        return false;
    }

    listen(tcp_socket, 5);


    connection_socket = 0;

    return save_fd != -1;
}

device_address SingleTcpSocket::getDevice_address(sockaddr_in *addr) const {

    device_address address;
    memset(&address, 0, sizeof(address));
    {
        uint32_t ip_address = addr->sin_addr.s_addr;
        uint16_t port = addr->sin_port;

        memcpy(&address.bytes, &ip_address, sizeof(ip_address));
        if (port == 0) {
            port = PORT;
        }
        memcpy(&address.bytes[sizeof(ip_address)], &port, sizeof(port));

    }
    return address;
}

uint32_t SingleTcpSocket::getIp_address(device_address *address) const {
    uint32_t ip_address = 0;
    memcpy(&ip_address, &address->bytes, sizeof(ip_address));
    return ip_address;
}

uint16_t SingleTcpSocket::getPort(device_address *address) const {
    uint16_t port = 0;
    memcpy(&port, &address->bytes[sizeof(uint32_t)], sizeof(port));
    return port;
}
