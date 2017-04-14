//
// Created by bele on 07.04.17.
//

#include "LinuxUdpSocket.h"

bool LinuxUdpSocket::begin() {
    if (mqttsn == nullptr) {
        return false;
    }

    //create a UDP socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return false;
    }

    int enable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        return false;
    }

    // zero out the structure
    memset((char *) &si_me, 0, sizeof(si_me));

    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(PORT);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if (bind(s, (struct sockaddr *) &si_me, sizeof(si_me)) == -1) {
        return false;
    }

    // set timout
    struct timeval tv;
    tv.tv_sec = 0;  // 0 Secs Timeout
    tv.tv_usec = 300000;  // 300 ms Timeout


    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(struct timeval)) == -1) {
        return false;
    }

    // enable broadcast
    int broadcastEnable = 1;
    if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable)) == -1) {
        return false;
    }
    if (s >= 0) {
        mqttsn->notify_socket_connected();
    }
    return s >= 0;
}

void LinuxUdpSocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void LinuxUdpSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttsn = mqttSnMessageHandler;
}

device_address *LinuxUdpSocket::getBroadcastAddress() {
    memset(&broadcast_address, 0, sizeof(device_address));
    if (s < 0) {
        return &broadcast_address;
    }
    uint32_t broadcast_ip_address = INADDR_BROADCAST;
    uint16_t broadcast_port = PORT;
    memcpy(&broadcast_address.bytes, &broadcast_ip_address, sizeof(broadcast_ip_address));
    memcpy(&broadcast_address.bytes[sizeof(broadcast_ip_address)], &broadcast_port, sizeof(broadcast_port));
    return &broadcast_address;
}

device_address *LinuxUdpSocket::getAddress() {
    // http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
    memset(&own_address, 0, sizeof(device_address));
    if (s < 0) {
        return &own_address;
    }
    struct ifreq ifr;
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    ioctl(s, SIOCGIFADDR, &ifr);

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

uint8_t LinuxUdpSocket::getMaximumMessageLength() {
    if (BUFLEN > UINT8_MAX) {
        return UINT8_MAX;
    }
    return (uint8_t) BUFLEN;
}

bool LinuxUdpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    si_other.sin_addr.s_addr = getIp_address(destination);
    si_other.sin_port = getPort(destination);
    if (sendto(s, bytes, bytes_len, 0, (struct sockaddr *) &si_other, slen) == -1) {
        // we ignore it
    }
    return s >= 0;
}

bool LinuxUdpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return send(destination, bytes, bytes_len);
}

bool LinuxUdpSocket::loop() {
    if (s < 0) {
        // socket disconnected
        mqttsn->notify_socket_disconnected();
        if (this->begin()) {
            mqttsn->notify_socket_connected();
            return true;
        }
        return false;
    }
    //listening for data for 300 ms
    //printf("Waiting for data...");
    memset(&buf, 0, BUFLEN);
    if ((recv_len = recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        // print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        // printf("Data: %s\n" , buf);
        if (recv_len <= UINT8_MAX) {


            device_address client_address = getDevice_address(&si_other);
            mqttsn->receiveData(&client_address, (uint8_t *) &buf);
        }
    }
    return s >= 0;
}

device_address LinuxUdpSocket::getDevice_address(sockaddr_in *addr) const {

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

uint32_t LinuxUdpSocket::getIp_address(device_address *address) const {
    uint32_t ip_address = 0;
    memcpy(&ip_address, &address->bytes, sizeof(ip_address));
    return ip_address;
}

uint16_t LinuxUdpSocket::getPort(device_address *address) const {
    uint16_t port = 0;
    memcpy(&port, &address->bytes[sizeof(uint32_t)], sizeof(port));
    return port;
}