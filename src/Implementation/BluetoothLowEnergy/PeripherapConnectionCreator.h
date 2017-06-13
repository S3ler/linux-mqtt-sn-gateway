//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_PRIMARYGATTFILTER_H
#define LINUX_MQTT_SN_GATEWAY_PRIMARYGATTFILTER_H


#include <global_defines.h>
#include "Queue.h"
#include "BluetoothLowEnergyAdvertise.h"
#include <list>
#include <thread>
#include "PerpheralConnection.h"
#include "BlacklistEntry.h"
#include <cstring>


#define BLACKLIST_TIMEOUT 120000

class LinuxBluetoothLowEnergySocket;
class PerpheralConnection;

class PeripherapConnectionCreator {
public:
    Queue<BluetoothLowEnergyAdvertise*> queue;

    Queue<BluetoothLowEnergyAdvertise *> * getQueue();

    std::list<BlacklistEntry*> blacklist;
    std::mutex blacklist_mutex;

    LinuxBluetoothLowEnergySocket* bleSocket= nullptr;

    void start_loop() ;

    void loop();

    void stop_loop() ;

    bool stopped;
    std::thread thread;


    bool isOnBlackList(BluetoothLowEnergyAdvertise *advertise);

    void addToBlacklist(BluetoothLowEnergyAdvertise* advertise);

    void removeFromBlacklist(BluetoothLowEnergyAdvertise *advertise);

    bool isConnected(BluetoothLowEnergyAdvertise *pAdvertise);

    void setBleSocket(LinuxBluetoothLowEnergySocket *bleSocket);

};


#endif //LINUX_MQTT_SN_GATEWAY_PRIMARYGATTFILTER_H
