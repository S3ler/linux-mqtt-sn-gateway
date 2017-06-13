//
// Created by bele on 07.04.17.
//

#include "LinuxUdpSocket.h"

bool LinuxUdpSocket::begin() {
    if (mqttsn == nullptr) {
        return false;
    }
    if (!initUdpSocket()) {
        return false;
    }

    if (!initMulticastUdpSocket()) {
        return false;
    }

    if (udp_socket >= 0 && socket_descriptor >= 0) {
        mqttsn->notify_socket_connected();
    }

    return udp_socket >= 0;
}

bool LinuxUdpSocket::initMulticastUdpSocket() {
    static int port = 1234;

    static struct ip_mreq command;
    int loop = 1;
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    if ((socket_descriptor = socket(PF_INET,
                                    SOCK_DGRAM, 0)) == -1) {
        perror("socket()");
        return false;
        //exit(EXIT_FAILURE);
    }
    /* Mehr Prozessen erlauben, denselben Port zu nutzen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    if (bind(socket_descriptor,
             (struct sockaddr *) &sin,
             sizeof(sin)) < 0) {
        perror("bind");
        return false;
        //exit(EXIT_FAILURE);

    }
    /* Broadcast auf dieser Maschine zulassen */
    loop = 1;
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_MULTICAST_LOOP,
                   &loop, sizeof(loop)) < 0) {
        perror("setsockopt:IP_MULTICAST_LOOP");
        return false;
        //exit(EXIT_FAILURE);
    }


    /* Join the broadcast group: */
    command.imr_multiaddr.s_addr = inet_addr("224.0.0.0");
    command.imr_interface.s_addr = htonl(INADDR_ANY);
    if (command.imr_multiaddr.s_addr == -1) {
        perror("224.0.0.0 ist keine Multicast-Adresse\n");
        return false;
        //exit(EXIT_FAILURE);
    }
    if (setsockopt(socket_descriptor,
                   IPPROTO_IP,
                   IP_ADD_MEMBERSHIP,
                   &command, sizeof(command)) < 0) {
        perror("setsockopt:IP_ADD_MEMBERSHIP");
        return false;
    }

    // set timout
    struct timeval udp_socket_timeval;
    udp_socket_timeval.tv_sec = 0;  // 0 Secs Timeout
    udp_socket_timeval.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, (char *) &udp_socket_timeval, sizeof(struct timeval)) == -1) {
        return false;
    }

    // save bc address as value now
    struct sockaddr_in address;
    memset (&address, 0, sizeof (address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr ("224.0.0.0");
    address.sin_port = htons (port);

    device_address bc_address = this->getDevice_address(&address);
    memset(&broadcast_address, 0, sizeof(device_address));
    memcpy(this->broadcast_address.bytes, bc_address.bytes, sizeof(device_address));

    return true;

}

bool LinuxUdpSocket::initUdpSocket() {

    //create a UDP socket
    if ((udp_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
        return false;
    }

    int resuse_addr = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_REUSEADDR, &resuse_addr, sizeof(resuse_addr)) == -1) {
        return false;
    }

    // zero out the structure
    memset((char *) &udp_socket_address, 0, sizeof(udp_socket_address));

    udp_socket_address.sin_family = AF_INET;
    udp_socket_address.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_socket_address.sin_port = htons(PORT);

    //bind socket to port
    if (bind(udp_socket, (struct sockaddr *) &udp_socket_address, sizeof(udp_socket_address)) == -1) {
        return false;
    }

    // set timout
    struct timeval udp_socket_timeval;
    udp_socket_timeval.tv_sec = 0;  // 0 Secs Timeout
    udp_socket_timeval.tv_usec = 200000;  // 200 ms Timeout


    if (setsockopt(udp_socket, SOL_SOCKET, SO_RCVTIMEO, (char *) &udp_socket_timeval, sizeof(struct timeval)) == -1) {
        return false;
    }

    // enable broadcast
    int udp_socket_broadcast_enable = 1;
    if (setsockopt(udp_socket, SOL_SOCKET, SO_BROADCAST, &udp_socket_broadcast_enable,
                   sizeof(udp_socket_broadcast_enable)) == -1) {
        return false;
    }

    return true;
}

void LinuxUdpSocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void LinuxUdpSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttsn = mqttSnMessageHandler;
}

device_address *LinuxUdpSocket::getBroadcastAddress() {
    if (udp_socket < 0) {
        memset(&broadcast_address, 0, sizeof(device_address));
    }
    //uint32_t broadcast_ip_address = INADDR_BROADCAST;
    //uint16_t broadcast_port = PORT;
    //memcpy(&broadcast_address.bytes, &broadcast_ip_address, sizeof(broadcast_ip_address));
    //memcpy(&broadcast_address.bytes[sizeof(broadcast_ip_address)], &broadcast_port, sizeof(broadcast_port));
    return &broadcast_address;
}

device_address *LinuxUdpSocket::getAddress() {
    // http://www.geekpage.jp/en/programming/linux-network/get-ipaddr.php
    memset(&own_address, 0, sizeof(device_address));
    if (udp_socket < 0) {
        return &own_address;
    }
    struct ifreq ifr;
    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ - 1);

    ioctl(udp_socket, SIOCGIFADDR, &ifr);

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
    if (destination == &this->broadcast_address) {

        struct sockaddr_in address;
        memset (&address, 0, sizeof (address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr ("224.0.0.0");
        int port = 1234;
        address.sin_port = htons (port);
        if (sendto( socket_descriptor,  bytes, bytes_len,  0, (struct sockaddr *) &address, sizeof (address)) < 0) {
            // we ignore it
        }
        return socket_descriptor >= 0;
    }
    si_other.sin_addr.s_addr = getIp_address(destination);
    si_other.sin_port = getPort(destination);
    if (sendto(udp_socket, bytes, bytes_len, 0, (struct sockaddr *) &si_other, slen) == -1) {
        // we ignore it
    }
    return udp_socket >= 0;
}

bool LinuxUdpSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    return send(destination, bytes, bytes_len);
}

bool LinuxUdpSocket::loop() {
    if (udp_socket < 0) {
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
    if ((recv_len = recvfrom(udp_socket, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        // print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        // printf("Data: %s\n" , buf);
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            mqttsn->receiveData(&client_address, (uint8_t *) &buf);
        }
    }
    memset(&buf, 0, BUFLEN);
    if ((recv_len = recvfrom(socket_descriptor, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &slen)) != -1) {
        // print details of the client/peer and the data received
        // printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        // printf("Data: %s\n" , buf);
        if (recv_len <= UINT8_MAX) {
            device_address client_address = getDevice_address(&si_other);
            mqttsn->receiveData(&client_address, (uint8_t *) &buf);
        }
    }
    return udp_socket >= 0;
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