# linux-mqtt-sn-gateway
MQTT-SN gateway for linux based operating systems.
Unlike most other transparent MQTT-SN Gateway implementations it is a aggregating Gateway.
From the beginning it is designed for bare-metal environments without a heap.

It glues the [core-mqtt-sn-gateway](https://github.com/S3ler/core-mqtt-sn-gateway) together with [transmission technology implementations](https://github.com/S3ler/linux-mqtt-sn-gateway/tree/master/src/Implementation), [MQTT-Client](https://github.com/S3ler/linux-mqtt-sn-gateway/tree/master/src/Implementation/paho) and the [core component cmplementations](https://github.com/S3ler/linux-mqtt-sn-gateway/tree/master/src/Implementation) (LinuxGateway, LinuxLogger, LinuxPersistent, LinuxSystem).

## Supported Architectures
At the moment we support:
 * x86
 * ARM (especially Raspberry Pi)
 
## Supported Transmission Technologies
For x86 we only support Ethernet (UDP & TCP) and WiFi (UDP & TCP).
On ARM (Raspberry Pi) there is additionally ZigBee, LoRa and BLE.
See the  Transmission Technology to Architecture Matrix.
### Transmission Technology to Architecture Matrix
|   	| UDP  	| TCP  	| Ethernet  	| WiFi  	| ZigBee  	| LoRa  	| BLE  	|
|---	|---	|---	|---	|---	|---	|---	|---	|
| Linux  	| &#x2705;  	| &#x274E;  	| &#x2705;  	| &#x2705;  	| &#x274C;  	| &#x274C;  	| &#x274C;  	|
| Raspberry Pi  	| &#x2705;  	| &#x274E;  	| &#x2705;  	| &#x2705;  	| &#x274E;\*  	| &#x2705;\*  	| &#x274E;  	|

\* needs additional transmission hardware

##### Legend: 
* &#x2705; implemented and tested
* &#x274E; not implemented yet
* &#x274C; will not be implemented



## Getting started UDP (x86 & ARM) - running
This is the section for all of you who only want to use the gateway.
Get a pre-built binary from [here](TODO).

The gateway needs the following configurations files: MQTT.CON, TOPICS.PRE.
Put these files into the execution directory (next to the binary).
The linux-mqtt-sn-gateway will pick and create needed files there.

The MQTT.CON file is the configuration file of the MQTT client.
It is Key Value based. Every line contains one key with the corresponding value after the first space.

The following values are NOT optional:

  * brokeraddress - IP address of the MQTT Broker, DNS or mDNS are not implemented but on the TODO list.
  * brokerport - Port of the MQTT Broker
  * clientid - MQTT client's id
  * gatewayid - Id of the gateway in the WSN

You can provide a will for the gateway (optional):

  * willtopic - topic of the will message
  * willmessage - payload of the willmessage (only ASCII supported)
  * willqos - quality of service of the will
  * willretain - retain of the will

Note that you need to provide all values of the will (there are no default values) or the gateway will connect without a will.

Example [MQTT.CON](https://github.com/S3ler/linux-mqtt-sn-gateway/blob/master/cmake-build-debug/MQTT.CON) file:

	brokeraddress 192.168.178.33
	brokerport 1884
	clientid mqtt-sn linux gateway
	willtopic /mqtt/sn/gateway
	willmessage /mqtt-sn linux gateway offline
	willqos 1
	willretain 0
	gatewayid 2

The TOPCIS.PRE file is the list of predefined MQTT topics of the gateway. Entries are space separated.
Each entry starts with the topic id followed by the topic name.
If a topic id is not unqiue in the file, the first topic if found by the gateway (starting at the beginning of the file) will be used.

Example [TOPICS.PRE](https://github.com/S3ler/linux-mqtt-sn-gateway/blob/master/cmake-build-debug/TOPICS.PRE) file:

	50 /some/predefined/topic
	20 /another/predefined/topic

## Getting started x86 - development

Clone the repository and initialize CMAKE with the Transmission Protocol (e.g. UDP)

    git clone --recursive https://github.com/S3ler/linux-mqtt-sn-gateway.git
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTRANSMISSION_PROTOCOL=UDP
    make linux-mqtt-sn-gateway

done.

For testing see the [test-mqtt-sn-gateway](https://github.com/S3ler/test-mqtt-sn-gateway) project.

## Getting started ARM (RPI & UDP) - development
At the moment we do not configure cmake for cross-compilation.
The easiest way to get started is to work on a Raspberry Pi directly.
You need a least CMake Version 3.0 for the project. Maybe you need to compile CMake on the Raspberry Pi before starting development.

The other steps are the same as for x86:
Clone the repository and initialize CMAKE with the Transmission Protocol (e.g. UDP)

    git clone --recursive https://github.com/S3ler/linux-mqtt-sn-gateway.git
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTRANSMISSION_PROTOCOL=UDP
    make linux-mqtt-sn-gateway
    
done.

## Getting started ARM (RPI & LoRa)
When running or developing the gateway with LoRa you need additional Hardware.
For testing you need Hardware for a client device (see [test-mqtt-sn-gateway - Serial Lora Client](TODO)).

### Hardware
We implement the LoRa linux-mqtt-sn-gateway on a Raspberry Pi.
We develop, test and run out gateway with the following hardware:
 * [Raspberry Pi 3](https://www.raspberrypi.org/products/raspberry-pi-3-model-b/)
 * [Dragino Lora/GPS HAT 868 Mhz](http://wiki.dragino.com/index.php?title=Lora/GPS_HAT)
 
TODO: Describe Soldering of Pins of the Dragino Lora/GPS HAT 868 Mhz
TODO: Add a Photo

### Software & Configuration
Unfortunately there is a problem ...
TODO describe the problem with SPI in the never versions
TODO enabling SPI?

### Running
Get a pre-built binary from [here](TODO).
Or obtain them by wget:

    wget TODO
    untar TODO
    
    
### Development
After you got your [hardware](TODO) and [configured your RPI](TODO)
The easiest way to get started is to work on a Raspberry Pi directly.
At the moment we do not configure cmake for cross-compilation.

You need a least CMake Version 3.0 for the project. Maybe you need to compile CMake on the Raspberry Pi before starting development.

The other steps are the same as for x86:
Clone the repository and initialize CMAKE with the Transmission Protocol (e.g. LoRa)

    git clone --recursive https://github.com/S3ler/linux-mqtt-sn-gateway.git
    cmake -G"Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DTRANSMISSION_PROTOCOL=LORA
    make linux-mqtt-sn-gateway

done. Happy coding.

## State of Project
Unfortunately we do not support all features defined in the [MQTT-SN Standard](http://mqtt.org/new/wp-content/uploads/2009/06/MQTT-SN_spec_v1.2.pdf).

Not Implemented (yet):
 * wildcard subscription
 * will update procedure
 * retransmission procedure
 * QoS 2 publishing
 * MQTT-SN Forward Encapsulation
 
Everything else is implemented and tested (see [test-mqtt-sn-gateway](https://github.com/S3ler/test-mqtt-sn-gateway) project).

## Implementation notes
We use four project as git submodules:
 * [core-mqtt-sn-gateway](https://github.com/S3ler/core-mqtt-sn-gateway)
 * [arduino-linux-abstraction](https://github.com/S3ler/arduino-linux-abstraction)
 * [mqtt-sn-sockets](https://github.com/S3ler/mqtt-sn-sockets)
 * [SimpleBluetoothLowEnergySocket](https://github.com/S3ler/SimpleBluetoothLowEnergySocket)

It runs only on Linux system but it is written against the Arduino Framework by using Linux based fake implementations for Arduino functions and libraris. It runs (successfully) on a NodeMCU with Arduino Core and a SPI SD-Card reader.
This implementation is a aggregating gateway for MQTT-SN. The aim of MQTT-SN is to support Wireless Sensor Networks (WSNs) with very large numbers of battery-operated sensors and actuators (SAs). This gateway is designed with platform independence in mind. Other MQTT-SN gateway implementation at least need a operating system (or only bridge messages). To fulfill the requierement the implementation shall use little RAM, no dynamic memory allocation, platform and hardware portability by interfaces. Further it does not rely on any physical layer, media access control (e.g IEEE 802.15.4), network layer (e.g. 6LoWPAN) or transport layer (e.g. UDP/TCP) standard or implementation.

## BLE (very unstable - not working)
    
For Bluetooth development install:

    apt install pkg-config
    apt install libglib2.0-dev
    apt install libbluetooth-dev
    
For enabling blutooth see here: https://stackoverflow.com/questions/41351514/leadvertisingmanager1-missing-from-dbus-objectmanager-getmanagedobjects
