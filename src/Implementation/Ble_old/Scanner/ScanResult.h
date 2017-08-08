//
// Created by bele on 09.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_SCANRESULT_H
#define GATTLIB_EXPERIMENTS_SCANRESULT_H


#include <stdint.h>
#include <global_defines.h>

class ScanResult {
private:
    device_address address;
public:
    device_address *getDeviceAddress() const;

    ScanResult(const device_address *address);
};


#endif //GATTLIB_EXPERIMENTS_SCANRESULT_H
