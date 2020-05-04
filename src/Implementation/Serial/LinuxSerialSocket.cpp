//
// Created by bele on 03.01.18.
//

#include "LinuxSerialSocket.h"


bool LinuxSerialSocket::begin() {
    if (this->logger == nullptr) {
        return false;
    }
    if (this->mqttSnMessageHandler == nullptr) {
        return false;
    }

    memset(&this->broadcastAddress, 0x0, sizeof(device_address));

    memset(&this->ownAddress, 0x0, sizeof(device_address));

    if (!initSerialPort()) {
        return false;
    }

    if (!resetController()) {
        return false;
    }

    if (!updateConfiguration()) {
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
    return (uint8_t) maximumMessageLength;
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
    if (fd > 0) {

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
    if (set_interface_attribs(fd, B9600, 0) < 0) {  // set speed to 9,600 bps, 8n1 (no parity)
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

bool LinuxSerialSocket::updateConfiguration() {
    write(fd, "CONFIGURATION\n", 14);

    // STATUS
    {
        char toCompare[] = "OK SEND_STATUS\n";
        if (!waitFor(toCompare)) {
            return false;
        }

        if (!readStatus()) {
            return false;
        }
    }

    // OWN ADDRESS
    {
        char toCompare[] = "OK SEND_OWN_ADDRESS\n";
        if (!waitFor(toCompare)) {
            return false;
        }

        if (!readOwnAddress()) {
            return false;
        }
    }

    // BROADCAST ADDRESS
    {
        char toCompare[] = "OK SEND_BROADCAST_ADDRESS\n";
        if (!waitFor(toCompare)) {
            return false;
        }
        if (!readBroadcastAddress()) {
            return false;
        }
    }

    // MAXIMUM MESSAGE LENGTH
    {
        char toCompare[] = "OK SEND_MAXIMUM_MESSAGE_LENGTH\n";
        if (!waitFor(toCompare)) {
            return false;
        }
        if (!readMaximumMessageLength()) {
            return false;
        }
        // parse maximum message length
    }

    // SERIAL BUFFER SIZE
    {
        char toCompare[] = "OK SEND_SERIAL_BUFFER_SIZE\n";
        if (!waitFor(toCompare)) {
            return false;
        }
        if (!readSerialBufferSize()) {
            return false;
        }
        // parse serial buffer size
    }

    // wait for
    // "OK IDLE\n"
    return waitForOKIdle();
}


bool LinuxSerialSocket::readStatus() {

    char buffer[1024];
    char *buffer_p = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // parse status
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
        status = ERROR;
        return false;
    }
    if (memcmp(token, "STATUS", strlen("STATUS")) != 0) {
        status = ERROR;
        return false;
    }

    memset(&from_address, 0x0, sizeof(device_address));
    uint16_t destination_address_length = 0;

    if ((token = strsep(&buffer_p, " ")) != NULL) {
        uint8_t status;
        if (memcmp(token, "STARTING", strlen("STARTING")) == 0) {
            status = STARTING;
        } else if (memcmp(token, "IDLE", strlen("IDLE")) == 0) {
            status = IDLE;
        } else if (memcmp(token, "SEND", strlen("SEND")) == 0) {
            status = SEND;
        } else if (memcmp(token, "RECEIVE", strlen("RECEIVE")) == 0) {
            status = RECEIVE;
        } else if (memcmp(token, "CONFIGURATION", strlen("CONFIGURATION")) == 0) {
            status = CONFIGURATION;
        } else if (memcmp(token, "PARSE_FAILURE", strlen("PARSE_FAILURE")) == 0) {
            status = PARSE_FAILURE;
        } else if (memcmp(token, "ERROR", strlen("ERROR")) == 0) {
            status = ERROR;
        } else {
            return false;
        }
    }
    return true;
}


bool LinuxSerialSocket::readOwnAddress() {

    char buffer[1024];
    char *buffer_p = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // parse own address
    // accept N/A => 0.0.0.0.0
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
        memset(&ownAddress, 0x0, sizeof(device_address));
        return false;
    }

    if (memcmp(token, "OWN_ADDRESS", strlen("OWN_ADDRESS")) != 0) {
        memset(&ownAddress, 0x0, sizeof(device_address));
        return false;
    }

    // check if address == N/A
    memset(&ownAddress, 0x0, sizeof(device_address));
    uint16_t own_address_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (memcmp(token, "N/A", strlen("N/A")) == 0) {
            memset(&ownAddress, 0x0, sizeof(device_address));
            return false;
        }
        if (!parseLong(token, &number)) {
            memset(&ownAddress, 0x0, sizeof(device_address));
            return false;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&ownAddress, 0x0, sizeof(device_address));
            return false;
        }

        if (own_address_length + 1 > sizeof(device_address)) {
            memset(&ownAddress, 0x0, sizeof(device_address));
            return false;
        }
        ownAddress.bytes[own_address_length++] = (uint8_t) number;
    }
    if (own_address_length != sizeof(device_address)) {
        memset(&from_address, 0x0, sizeof(device_address));
        return false;
    }
    return true;
}


bool LinuxSerialSocket::readBroadcastAddress() {

    char buffer[1024];
    char *buffer_p = buffer;
    uint16_t buffer_read = 0;
    memset(buffer, 0x0, sizeof(buffer));

    // parse broadcast address
    // accept N/A => 0.0.0.0.0
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
        memset(&broadcastAddress, 0x0, sizeof(device_address));
        return false;
    }

    if (memcmp(token, "BROADCAST_ADDRESS", strlen("BROADCAST_ADDRESS")) != 0) {
        memset(&broadcastAddress, 0x0, sizeof(device_address));
        return false;
    }

    memset(&broadcastAddress, 0x0, sizeof(device_address));
    uint16_t broadcast_address_length = 0;

    while ((token = strsep(&buffer_p, " ")) != NULL) {
        long int number = 0;
        if (memcmp(token, "N/A", strlen("N/A")) == 0) {
            // accept N/A => nullptr
            broadcast_address_length = sizeof(device_address);
            memset(&broadcastAddress, 0x0, sizeof(device_address));
            break;
        }
        if (!parseLong(token, &number)) {
            memset(&broadcastAddress, 0x0, sizeof(device_address));
            return false;
        }

        if (number > UINT8_MAX || number < 0) {
            memset(&broadcastAddress, 0x0, sizeof(device_address));
            return false;
        }

        if (broadcast_address_length + 1 > sizeof(device_address)) {
            memset(&broadcastAddress, 0x0, sizeof(device_address));
            return false;
        }
        broadcastAddress.bytes[broadcast_address_length++] = (uint8_t) number;
    }
    if (broadcast_address_length != sizeof(device_address)) {
        memset(&broadcastAddress, 0x0, sizeof(device_address));
        return false;
    }
    return true;
}


bool LinuxSerialSocket::readMaximumMessageLength() {

    char buffer[1024];
    char *buffer_p = buffer;
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
        maximumMessageLength = 0;
        return false;
    }
    if (memcmp(token, "MAXIMUM_MESSAGE_LENGTH", strlen("MAXIMUM_MESSAGE_LENGTH")) != 0) {
        maximumMessageLength = 0;
        return false;
    }

    long int number = 0;
    while ((token = strsep(&buffer_p, " ")) != NULL) {
        if (!parseLong(token, &number)) {
            maximumMessageLength = 0;
            return false;
        }

        if (number > UINT8_MAX || number < 0) {
            maximumMessageLength = 0;
            return false;
        }
    }
    maximumMessageLength = (uint16_t) number;
    return true;
}

bool LinuxSerialSocket::readSerialBufferSize() {

    char buffer[1024];
    char *buffer_p = buffer;
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
        serialBufferSize = 0;
        return false;
    }
    if (memcmp(token, "SERIAL_BUFFER_SIZE", strlen("SERIAL_BUFFER_SIZE")) != 0) {
        serialBufferSize = 0;
        return false;
    }

    long int number = 0;
    while ((token = strsep(&buffer_p, " ")) != NULL) {
        if (!parseLong(token, &number)) {
            serialBufferSize = 0;
            return false;
        }

        if (number > UINT8_MAX || number < 0) {
            serialBufferSize = 0;
            return false;
        }
    }
    serialBufferSize = (uint16_t) number;
    return true;
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

void LinuxSerialSocket::sendAddress(device_address *address) {
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

void LinuxSerialSocket::sendData(uint8_t *buf, uint8_t len) {
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
    char *buffer_p = buffer;
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
    char *buffer_p = buffer;
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


