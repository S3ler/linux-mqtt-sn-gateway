//
// Created by bele on 12.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_SCANNERCALLBACKPRINT_H
#define GATTLIB_EXPERIMENTS_SCANNERCALLBACKPRINT_H


#include "../Scanner/ScannerCallbackInterface.h"
#include "Connection.h"
#include "../Scanner/Scanner.h"
#include "ReceiveInterface.h"
#include <list>
#include <thread>
#include <atomic>


class Connector : public ScannerCallbackInterface, public ReceiverInterface {
private:
    std::mutex scanResult_mutex;
    std::list<ScanResult *> scanResults;

    std::mutex active_connections_mutex;
    std::list<Connection *> active_connections;
    volatile std::atomic<bool> stopped;
    std::thread connector_thread;

    void printDeviceAddress(device_address *address);

    void connect_loop();

    Scanner *scanner = nullptr;

    char MAC[18] = {0};

    ReceiverInterface *receiverInterface = nullptr;
    
public:

    Connector(const char *connector_mac, Scanner *scanner);

    void setReceiverInterface(ReceiverInterface *receiverInterface);

    void start();

    void stop();

    void free_connections();

    // const std::list<Connection *> &getActive_connections() const;

    bool onScanReceive(ScanResult *scanResult) override;

    bool send(const device_address *destination, const uint8_t *payload, const uint16_t payload_length);

    void onReceive(const device_address *address, const uint8_t *payload, const uint16_t payload_length) override;

    bool isConnected(const device_address *address);

    void close(device_address *address);
};


#endif //GATTLIB_EXPERIMENTS_SCANNERCALLBACKPRINT_H
