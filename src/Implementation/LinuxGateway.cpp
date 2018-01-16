//
// Created by bele on 10.04.17.
//

#include <thread>
#include "LinuxGateway.h"

#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    LinuxGateway::LinuxGateway() :
	rh_driver(RPI_V2_GPIO_P1_18, RPI_V2_GPIO_P1_24),
        manager(rh_driver, OWN_ADDRESS)
	 { }
#endif


bool LinuxGateway::begin() {
    logger.start_log("Linux MQTT-SN Gateway version 0.0.1a starting", 1);

#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    if(!bcm2835_init()){
        Serial.println("Failure init bcm2835");
    }
    if (!rh_driver.init()) {
        Serial.println("Failure init DRIVER_RH_NRF24");
    }
    if(!rh_driver.setChannel(1)) {
        Serial.println("Failure init setChannel(1)");
    }
    if (!rh_driver.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPowerm18dBm)) {
        Serial.println("Failure set DataRate250kbps, TransmitPowerm18dBm");
    }
    mqttsnSocket.setManager(&manager);
#endif

    Gateway::setLoggerInterface(&logger);
#if defined(D_GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
    Gateway::setSocketInterface(mqttsnSocket);
#else
    Gateway::setSocketInterface(&mqttsnSocket);
#endif
    Gateway::setMqttInterface(&mqtt);
    Gateway::setPersistentInterface(&persistent);
    Gateway::setSystemInterface(&systemImpl);
    Gateway::setDurationSystemInterface(&durationSystemImpl);

    return Gateway::begin();
}

void LinuxGateway::setRootPath( char *rootPath) {
    persistent.setRootPath(rootPath);
}

void LinuxGateway::start_loop() {
    this->thread = std::thread(&LinuxGateway::dispatch_loop, this);
}

void LinuxGateway::stop_loop() {
    this->stopped = true;
    this->thread.join();
}

void LinuxGateway::dispatch_loop() {
    while (!stopped) {
        this->loop();
    }
}

#if defined(D_GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
void LinuxGateway::setRadioHeadSocket(RF95Socket* mqttsnSocket){
    this->mqttsnSocket = mqttsnSocket;
}
#endif
