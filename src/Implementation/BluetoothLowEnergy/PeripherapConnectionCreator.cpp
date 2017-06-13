//
// Created by bele on 11.06.17.
//


#include "PeripherapConnectionCreator.h"

void PeripherapConnectionCreator::start_loop() {
    this->thread = std::thread(&PeripherapConnectionCreator::loop, this);
    // TODO detaching needed?
    // this->thread.detach();
}

void PeripherapConnectionCreator::loop() {
    while (!stopped) {
        if (!queue.empty()) {
            BluetoothLowEnergyAdvertise *advertise = queue.pop();
            if (isOnBlackList(advertise)) {
                delete advertise;
                continue;
            }
            if (isConnected(advertise)) {
                delete advertise;
                continue;
            }
            printf("Discovered %s - '%s'\n", advertise->getMAC(), advertise->getName());
            PerpheralConnection *connection = new PerpheralConnection();
            // TODO advertise in blacklist => fliegt auf die fresse
            addToBlacklist(advertise);
            connection->setBLESocket(bleSocket);
            connection->setAdvertise(advertise);
            connection->setPeripheralCreator(this);
            connection->start_loop();
        }
    }
    while(!queue.empty()){
        BluetoothLowEnergyAdvertise *advertise = queue.pop();
        delete advertise;
    }
}

void PeripherapConnectionCreator::stop_loop() {
    this->stopped = true;
    this->thread.join();
}


bool PeripherapConnectionCreator::isOnBlackList(BluetoothLowEnergyAdvertise *advertise) {
    std::chrono::milliseconds current_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    std::lock_guard<std::mutex> lock_guard(blacklist_mutex);
    for (auto &&entry : blacklist) {
        if (strcmp(advertise->getMAC(), entry->mac) == 0) {
            std::chrono::milliseconds delta = current_time - entry->timestamp;
            if (delta.count() > BLACKLIST_TIMEOUT) {
                blacklist.remove(entry);
                return false;
            }
            return true;
        }
    }
    return false;
}

void PeripherapConnectionCreator::removeFromBlacklist(BluetoothLowEnergyAdvertise *advertise) {
    std::lock_guard<std::mutex> lock_guard(blacklist_mutex);
    for (auto &&entry : blacklist) {
        char *mac = entry->mac;
        if (strcmp(advertise->getMAC(), mac) == 0) {
            blacklist.remove(entry);
            return;
        }
    }
}


void PeripherapConnectionCreator::addToBlacklist(BluetoothLowEnergyAdvertise *advertise) {
    BlacklistEntry* entry = new BlacklistEntry();
    strcpy(entry->mac, advertise->getMAC());
    entry->timestamp = advertise->getTimestamp();
    std::lock_guard<std::mutex> lock_guard(blacklist_mutex);
    blacklist.push_back(entry);
}

bool PeripherapConnectionCreator::isConnected(BluetoothLowEnergyAdvertise *pAdvertise) {
    return bleSocket->isConnected(pAdvertise->getMAC());
}

void PeripherapConnectionCreator::setBleSocket(LinuxBluetoothLowEnergySocket *bleSocket) {
    PeripherapConnectionCreator::bleSocket = bleSocket;
}

Queue<BluetoothLowEnergyAdvertise *> * PeripherapConnectionCreator::getQueue()  {
    return &queue;
}

