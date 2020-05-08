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

#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
    LinuxGateway::LinuxGateway() :
        rh_driver(),
        manager(rh_driver, OWN_ADDRESS)
         { }
#endif


bool LinuxGateway::begin() {

    logger.begin();
    logger.start_log("Linux MQTT-SN Gateway version 0.0.1a starting", 1);
    systemImpl.setLogger(&logger);

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

#if defined(GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_RF95)
    wiringPiSetupGpio();
    
    // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
    if (!rh_driver.init()) {
        Serial.println("Failure init DRIVER_RH_RF95");
    }

    if (!rh_driver.setFrequency(FREQUENCY)) {
        Serial.println("Failure set FREQUENCY");
    }

    if(!rh_driver.setModemConfig(RH_RF95::MODEM_CONFIG_CHOICE)){
        Serial.println("Failure set MODEM_CONFIG_CHOICE");
    }
    mqttsnSocket.setManager(&manager);
#endif

    Gateway::setLoggerInterface(&logger);
    Gateway::setSocketInterface(&mqttsnSocket);
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
    this->stopped = false;
    this->thread = std::thread(&LinuxGateway::dispatch_loop, this);
}

void LinuxGateway::stop_loop() {
    this->stopped = true;
    this->thread.join();
}

void LinuxGateway::dispatch_loop() {

    try {
        while (!stopped) {
            this->loop();
        }
    }
    catch (LinuxSystem::ThreadTerminated& ex) {
       logger.log("System requested gateway thread termination. Stopping thread...", 0);
       this->stopped = true;
    }
}

