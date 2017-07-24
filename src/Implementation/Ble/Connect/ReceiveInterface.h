//
// Created by bele on 22.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_RECEIVEINTERFACE_H
#define GATTLIB_EXPERIMENTS_RECEIVEINTERFACE_H

#include <cstdint>

class ReceiverInterface {

public:
    virtual ~ReceiverInterface() {}

    virtual void onReceive(const device_address *address, const uint8_t *payload, const uint16_t payload_length) = 0;

};

#endif //GATTLIB_EXPERIMENTS_RECEIVEINTERFACE_H
