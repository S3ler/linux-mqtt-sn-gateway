#include <LinuxUdpSocket.h>
#include <LinuxLogger.h>
#include <LinuxPersistent.h>
#include <Gateway.h>
#include <libgen.h>
#include <LinuxGateway.h>

LinuxGateway gateway;

void setup() {
    while (!gateway.begin()) {
        std::cout << "Error starting gateway components" << std::endl;
        exit(1);
    }
    std::cout << "Gateway ready" << std::endl;
}

int main(int argc, char *argv[]) {
    gateway.setRootPath(dirname(argv[0]));
    setup();
    while (true) {
        gateway.loop();
    }
}



