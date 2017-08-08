//
// Created by bele on 12.07.17.
//

#include <cstdio>
#include <mutex>
#include <iostream>
#include "Connector.h"
#include <algorithm>

bool Connector::onScanReceive(ScanResult *scanResult) {
    printf("onScanReceive: ");
    printDeviceAddress(scanResult->getDeviceAddress());

    std::lock_guard<std::mutex> scanResult_lock_guard(scanResult_mutex);
    scanResults.push_back(scanResult);
    return true;
}

void Connector::printDeviceAddress(device_address *address) {
    for (int i = 0; i < sizeof(device_address); i++) {
        std::cout << std::hex << std::uppercase << (int) address->bytes[i]
                  << std::nouppercase << std::dec
                  << std::flush;
        if (i != sizeof(device_address) - 1) {
            std::cout << ":";
        }
    }
    std::cout << std::endl;
}

void Connector::start() {
    this->stopped = false;
    this->connector_thread = std::thread(&Connector::connect_loop, this);
}

void closeIfStateError(Connection *connection) {
    if (connection->getState() == STATE_ERROR) {
        connection->close();
    }
}

bool deleteIfStateClosed(Connection *connection) {
    if (connection->getState() == STATE_CLOSED) {
        delete connection;
    }
    return true;
}

/*
 * 0. get Scanner's status
 * 1. stop Scanner
 * 2. try to connect
 * 2.1 ok => remove scanResult from Scanner's blacklist
 * 2.2 failure: reason == error => remove from blacklist
 * 2.3 failure: reason == missing service => do NOT remove from blacklist
 * 3. remove scanResult from local scanResults list
 * 4. Scanner's status == running => restart
 */
void Connector::connect_loop() {
    while (!stopped) {
        if (scanResults.size() > 0) {
            // 0. get Scanner's status
            bool scanner_status = scanner->isRunning();
            // 1. stop Scanner
            scanner->stop();
            // if you do not stop the scann first, we provoke a  deadlock
            std::lock_guard<std::mutex> scanResult_lock(scanResult_mutex);
            for (auto &&scanResult : scanResults) {
                Connection *connection = new Connection(scanResult->getDeviceAddress(), this->MAC);
                connection->setReceiverInterface(this);
                // 2. try to connect
                if (!connection->connect()) {
                    if (connection->getErrorStatus() == InternalBluetoothError) {
                        // something went wrong, but reconnecting helps in most cases
                        // failue == error => remove from blacklist (so it will be tried again)
                        scanResults.remove(scanResult);
                        scanner->removeScanResult(scanResult);
                        connection->close();
                        delete (connection);
                        break;
                    } else if (connection->getErrorStatus() == MissingService ||
                               connection->getErrorStatus() == ConnectionRefused) {
                        // if failure == no service => do not remove from blacklist
                        scanResults.remove(scanResult);
                        connection->close();
                        delete (connection);
                        break;
                    }
                }
                // 2.1 ok => remove scanResult from Scanner's blacklist
                scanResults.remove(scanResult);
                scanner->removeScanResult(scanResult);
                std::lock_guard<std::mutex> active_connections_lock_guard(active_connections_mutex);
                // we need to clean up connections with STATE_ERROR or STATE_CLOSED before we finish the connection
                // this shall prevent the case, that there is a broken and a valid connection simulatinously
                std::for_each(active_connections.begin(), active_connections.end(), &closeIfStateError);
                active_connections.remove_if(deleteIfStateClosed);

                active_connections.push_front(connection);
                break;
            }

            // 4. Scanner's status == running => restart
            if (scanner_status) {
                scanner->scan(this);
            }
        }
    }
}

// TODO this method cannot be made threadsave => convert to isConnected(const char* mac)
// TODO add method: send();
/*
const std::list<Connection *> &Connector::getActive_connections() const {
    return active_connections;
}
*/

void Connector::stop() {
    this->stopped = true;
    if (connector_thread.joinable()) {
        this->connector_thread.join();
    }
}

bool deleteAllScanResults(ScanResult *scanResult) {
    delete scanResult;
    return true;
}


bool deleteAllActiveConnections(Connection *connection) {
    delete connection;
    return true;
}

void Connector::free_connections() {
    /*
    {
        std::lock_guard<std::mutex> scanResult_lock(scanResult_mutex);
        scanResults.remove_if(deleteAllScanResults);
    }
    */
    {
        // disconnect from all
        std::lock_guard<std::mutex> active_connections_lock_guard(active_connections_mutex);
        for (auto &&connection : active_connections) {
            connection->close();
        }
        // free all
        active_connections.remove_if(deleteAllActiveConnections);
    }
}

Connector::Connector(const char *connector_mac, Scanner *scanner) {

    if (connector_mac == nullptr) {

    }
    if (strlen(connector_mac) != 17) {

    }
    strcpy(this->MAC, connector_mac);

    if (scanner == nullptr) {
        // TODO error
    }
    this->scanner = scanner;
}

bool Connector::send(const device_address *destination, const uint8_t *payload, const uint16_t payload_length) {
    std::lock_guard<std::mutex> active_connections_lock_guard(active_connections_mutex);
    for (auto &&connection : active_connections) {
        if (memcmp(destination, connection->getAddress(), sizeof(device_address)) == 0) {
            if (connection->getState() == STATE_NUS_READY) {
                return connection->send((uint8_t *) payload, (uint16_t) payload_length);
            } else if (connection->getState() == STATE_ERROR) {
                connection->close();
                return false;
            }
        }
    }
    return false;
}

bool Connector::isConnected(const device_address *address) {
    std::lock_guard<std::mutex> active_connections_lock_guard(active_connections_mutex);
    for (auto &&connection : active_connections) {
        if (memcmp(address, connection->getAddress(), sizeof(device_address)) == 0) {
            if (connection->getState() == STATE_NUS_READY) {
                return true;
            } else if (connection->getState() == STATE_ERROR) {
                connection->close();
                return false;
            }
        }
    }
    return false;
}


void Connector::close(device_address *address) {
    std::lock_guard<std::mutex> active_connections_lock_guard(active_connections_mutex);

    for (std::list<Connection *>::const_iterator iterator = active_connections.begin(),
                 end = active_connections.end();
         iterator != end; ++iterator) {
        Connection *connection = *iterator;
        connection->close();
        delete (connection);
        return;
    }
}

void Connector::onReceive(const device_address *address, const uint8_t *payload, const uint16_t payload_length) {
    if (receiverInterface != nullptr) {
        receiverInterface->onReceive(address, payload, payload_length);
    } else {
        for (int i = 0; i < sizeof(device_address); i++) {
            std::cout << std::hex << std::uppercase << (int) address->bytes[i]
                      << std::nouppercase << std::dec
                      << std::flush;
            if (i != sizeof(device_address) - 1) {
                std::cout << ":";
            }
        }
        std::cout << " - ";
        for (int i = 0; i < payload_length; i++) {
            std::cout << (char) payload[i] << std::flush;
        }
        std::cout << std::endl;
    }
}

void Connector::setReceiverInterface(ReceiverInterface *receiverInterface) {
    if(receiverInterface == nullptr){
        // TODO error
    }
    this->receiverInterface = receiverInterface;
}



