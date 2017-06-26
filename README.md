# linux-mqtt-sn-gateway
MQTT-SN gateway for linux based operating systems.

Note about the current status of this project: It is completely work in progress and only partly tested.

Untested or not implemented:
1. Sleeping Clients
2. Wildcard subscription (inlcuding registrations messages from gateway to client)
3. Will update
4. Retransmission

It runs only on Linux system but it is written against the Arduino Framework by using Linux based fake implementations for Arduino functions and libraris. It runs (successfully) on a NodeMCU with Arduino Core and a SPI SD-Card reader.
This implementation is a aggregating gateway for MQTT-SN. The aim of MQTT-SN is to support Wireless Sensor Networks (WSNs) with very large numbers of battery-operated sensors and actuators (SAs). This gateway is designed with platform independence in mind. Other MQTT-SN gateway implementation at least need a operating system (or only bridge messages). To fulfill the requierement the implementation shall use little RAM, no dynamic memory allocation, platform and hardware portability by interfaces. Further it does not rely on any physical layer, media access control (e.g IEEE 802.15.4), network layer (e.g. 6LoWPAN) or transport layer (e.g. UDP/TCP) standard or implementation.

## Getting started - running
This is the section for all of you who only want to use the gateway.
The gateway need the following configurations files: MQTT.CON, TOPICS.PRE.
Put these files into the execution directory.

The MQTT.CON file is the configuration file of the MQTT client, values are space separated.

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

Example MQTT.CON file:

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

Example TOPICS.PRE file:

	50 /some/predefined/topic
	20 /another/predefined/topic

## Getting started - development

First clone the repository

    git clone git@github.com:S3ler/linux-udp-mqtt-sn-test-client.git

then change into folder

    cd linux-mqtt-sn-gateway

finally init/update all submodules

    git submodule init
    git submodule update

done.

Updating (after init/update) all submodules is done by:

    git submodule update --recursive --remote
    
For Bluetooth development install:

    apt install pkg-config
    apt install libglib2.0-dev
    apt install libbluetooth-dev

## Implementation notes
We use two project as git submodules: [core-mqtt-sn-gateway](https://github.com/S3ler/core-mqtt-sn-gateway) and [arduino-linux-abstraction](https://github.com/S3ler/arduino-linux-abstraction).


For enabling blutooth see here: https://stackoverflow.com/questions/41351514/leadvertisingmanager1-missing-from-dbus-objectmanager-getmanagedobjects