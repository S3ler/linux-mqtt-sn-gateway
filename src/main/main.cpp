#include <LinuxUdpSocket.h>
#include <LinuxLogger.h>
#include <LinuxSystem.h>
#include <LinuxPersistent.h>
#include <MqttBrokerImplementation.h>
#include <Gateway.h>


Gateway gateway;
LinuxUdpSocket udpSocket;
LinuxPersistent persistent;

MqttBrokerImplementation mqtt;
LinuxLogger logger;
LinuxSystem systemImpl;

void setup() {
    logger.start_log("Example MQTT-SN Gateway version 0.0.1a starting", 1);

    gateway.setLoggerInterface(&logger);
    gateway.setSocketInterface(&udpSocket);
    gateway.setMqttInterface(&mqtt);
    gateway.setPersistentInterface(&persistent);
    gateway.setSystemInterface(&systemImpl);

    while (!gateway.begin()) {
        logger.log("Error starting gateway components", 0);
        systemImpl.sleep(5000);
        systemImpl.exit();
    }
    logger.log("Gateway ready", 1);
}

int main(int argc, char* argv[]) {
    setup();
    while(true){
        gateway.loop();
    }
}



