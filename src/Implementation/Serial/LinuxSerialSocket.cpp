//
// Created by bele on 03.01.18.
//

#include <climits>
#include "LinuxSerialSocket.h"



bool LinuxSerialSocket::begin() {
    if (this->logger == nullptr) {
        return false;
    }
    if (this->mqttSnMessageHandler == nullptr) {
        return false;
    }

    memset(&this->broadcastAddress, 0x0, sizeof(device_address));
    this->broadcastAddress.bytes[0] = BROADCAST_ADDRESS;

    memset(&this->ownAddress, 0x0, sizeof(device_address));
    this->ownAddress.bytes[0] = OWN_ADDRESS;

    if (!initSerialPort()) {
        return false;
    }

    if (!resetController()) {
        return false;
    }
    this->mqttSnMessageHandler->notify_socket_connected();
    return true;
}

void LinuxSerialSocket::setLogger(LoggerInterface *logger) {
    this->logger = logger;
}

void LinuxSerialSocket::setMqttSnMessageHandler(MqttSnMessageHandler *mqttSnMessageHandler) {
    this->mqttSnMessageHandler = mqttSnMessageHandler;
}

device_address *LinuxSerialSocket::getBroadcastAddress() {
    return &this->broadcastAddress;
}

device_address *LinuxSerialSocket::getAddress() {
    return &this->ownAddress;
}

uint8_t LinuxSerialSocket::getMaximumMessageLength() {
    return MAX_MSG_LENGTH;
}

bool LinuxSerialSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len) {
    return send(destination, bytes, bytes_len, UINT8_MAX);
}

bool LinuxSerialSocket::send(device_address *destination, uint8_t *bytes, uint16_t bytes_len, uint8_t signal_strength) {
    write(fd, "SEND\n", 5);
    waitForOkAwaitAddress();
    sendAddress(destination);
    waitForOkAwaitData();
    sendData(bytes, bytes_len);
    waitForOkSending();
    waitForOKIdle();
    return true;
}

bool LinuxSerialSocket::loop() {
    if (fd < 0) {
        return false;
    }
    if(fd > 0){

        write(fd, "RECEIVE\n", 8);
        waitForOkSendAddress();
        readSendAddress();
        waitForOkSendData();
        readData();
        waitForOKIdle();
        if (from_address.bytes[0] != 0) {
            mqttSnMessageHandler->receiveData(&from_address, data);
        }
        return true;
    }
}

bool LinuxSerialSocket::initSerialPort() {

    fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        // error_message ("error %d opening %s: %s", errno, portname, strerror (errno));
        return false;
    }
    if (set_interface_attribs(fd, B9600, 0) < 0) {  // set speed to 115,200 bps, 8n1 (no parity)
        return false;
    }
    if (set_blocking(fd, true) < 0) { // set no blocking
        return false;
    }
    return true;
}

// FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
int LinuxSerialSocket::set_interface_attribs(int fd, int speed, int parity) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        //error_message ("error %d from tcgetattr", errno);
        return -1;
    }

    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars
    tty.c_iflag &= ~IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN] = 0;            // read doesn't block
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
    // enable reading
    tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
    tty.c_cflag |= parity;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        //error_message ("error %d from tcsetattr", errno);
        return -1;
    }
    return 0;
}

// FROM: https://stackoverflow.com/questions/6947413/how-to-open-read-and-write-from-serial-port-in-c Date: 30.12.207
int LinuxSerialSocket::set_blocking(int fd, int should_block) {
    struct termios tty;
    memset(&tty, 0, sizeof tty);
    if (tcgetattr(fd, &tty) != 0) {
        //error_message ("error %d from tggetattr", errno);
        return -1;
    }

    tty.c_cc[VMIN] = should_block ? 1 : 0;
    tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        //error_message ("error %d setting term attributes", errno);
        return -1;
    }
    return 0;
}

bool LinuxSerialSocket::resetController() {
    write(fd, "RESET\n", 7);
    return waitForOKIdle();
}

bool LinuxSerialSocket::waitForOKIdle() {
    char toCompare[] = "OK IDLE\n";
    return waitFor(toCompare);
}

bool LinuxSerialSocket::waitFor(char *toCompare) {
    char buffer[1024];
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop and use with timeout
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            // line done
            if ((strlen(toCompare) == buffer_read) && (strcmp(buffer, toCompare) == 0)) {
                return true;
            }
            buffer_read = 0;
            memset(buffer, 0x0, sizeof(buffer));
        }
    }
    return false;
}

void LinuxSerialSocket::waitForOkAwaitAddress() {
    char toCompare[] = "OK AWAIT_ADDRESS\n";
    waitFor(toCompare);
}

void LinuxSerialSocket::sendAddress(device_address* address) {
    write(fd, "ADDRESS", 7);
    for (uint16_t i = 0; i < sizeof(device_address); i++) {
        write(fd, " ", 1);
        std::string s = std::to_string(address->bytes[i]);
        write(fd, s.c_str(), s.length());
    }
    write(fd, "\n", 1);
}


void LinuxSerialSocket::waitForOkAwaitData() {
    char toCompare[] = "OK AWAIT_DATA\n";
    waitFor(toCompare);
}

void LinuxSerialSocket::sendData( uint8_t *buf, uint8_t len) {
    write(fd, "DATA", 4);
    for (uint16_t i = 0; i < len; i++) {
        write(fd, " ", 1);
        std::string s = std::to_string(buf[i]);
        write(fd, s.c_str(), s.length());
    }
    write(fd, "\n", 1);
}

void LinuxSerialSocket::waitForOkSending() {
    char toCompare[] = "OK SENDING\n";
    waitFor(toCompare);
}

void LinuxSerialSocket::waitForOkSendAddress() {
    char toCompare[] = "OK SEND_ADDRESS\n";
    waitFor(toCompare);
}

void LinuxSerialSocket::readSendAddress() {
    char buffer[1024];
    char* buffer_p = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            break;
        }
        // TODO overflow
    }


    char *token = strsep(&buffer_p, " ");
    if (token == NULL) {
        memset(&from_address, 0x0, sizeof(device_address));
        return;
    }
    if (memcmp(token, "ADDRESS", strlen("ADDRESS")) != 0) {
        memset(&from_address, 0x0, sizeof(device_address));
        return;
    }

    memset(&from_address, 0x0, sizeof(device_address));
    uint16_t destination_address_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }
        if (destination_address_length + 1 > sizeof(device_address)) {
            memset(&from_address, 0x0, sizeof(device_address));
            return;
        }
        from_address.bytes[destination_address_length++] = (uint8_t) number;
    }
    if (destination_address_length != sizeof(device_address)) {
        memset(&from_address, 0x0, sizeof(device_address));
    }

}

void LinuxSerialSocket::waitForOkSendData() {
    char toCompare[] = "OK SEND_DATA\n";
    waitFor(toCompare);
}

void LinuxSerialSocket::readData() {

    char buffer[1024];
    char* buffer_p  = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // TODO remove while(true) loop
    while (true) {
        char c;
        ssize_t n = read(fd, &c, 1);
        buffer[buffer_read++] = c;
        if (c == '\n') {
            break;
        }
        // TODO overflow
    }

    char *token = strsep(&buffer_p, " ");
    if (token == NULL) {
        memset(&data, 0x0, sizeof(data));
        data_length = 0;
        return;
    }
    if (memcmp(token, "DATA", strlen("DATA")) != 0) {
        memset(&data, 0x0, sizeof(data));
        data_length = 0;
        return;
    }


    memset(&data, 0x0, sizeof(data));
    data_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (!parseLong(token, &number)) {
            memset(&data, 0x0, sizeof(data));
            data_length = 0;
            return;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&data, 0x0, sizeof(data));
            data_length = 0;
            return;
        }
        data[data_length++] = (uint8_t) number;
    }

}

bool LinuxSerialSocket::parseLong(const char *str, long *val) {
    char *temp;
    bool rc = true;
    errno = 0;
    *val = strtol(str, &temp, 0);

    if (temp == str || (*temp != '\0' && *temp != '\n') ||
        ((*val == LONG_MIN || *val == LONG_MAX) && errno == ERANGE))
        rc = false;

    return rc;
}