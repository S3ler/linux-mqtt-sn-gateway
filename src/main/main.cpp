#include <libgen.h>
#include <LinuxGateway.h>

#if defined(D_GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
#include <bcm2835.h>
#include <RF95Socket.h>
#include <RH_NRF24.h>
#include <RHReliableDatagram.h>

RF95Socket mqttsnSocket;
RH_NRF24 rh_driver(RPI_V2_GPIO_P1_18, RPI_V2_GPIO_P1_24);
RHReliableDatagram manager(rh_driver);
#endif

LinuxGateway gateway;

void setup() {

#if defined(D_GATEWAY_TRANSMISSION_PROTOCOL_RASPBERRY_RH_NRF24)
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

    manager.setThisAddress(OWN_ADDRESS);

    gateway.setRadioHeadSocket(&mqttsnSocket);
#endif

    while (!gateway.begin()) {
        gateway.getLogger().log("Error starting gateway components", 0);
        exit(1);
    }

    gateway.getLogger().log("Gateway ready", 2);
}

int main(int argc, char *argv[]) {
    gateway.setRootPath(dirname(argv[0]));
    setup();
    gateway.start_loop();

    while(true) {
        // we need to keep the program alive
    }
}



