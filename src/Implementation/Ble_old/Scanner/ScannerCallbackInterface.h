//
// Created by bele on 12.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_SCANNERCALLBACKINTERFACE_H
#define GATTLIB_EXPERIMENTS_SCANNERCALLBACKINTERFACE_H

#include "ScanResult.h"

class ScannerCallbackInterface {
public:
    virtual bool onScanReceive(ScanResult *scanResult)=0;
};

#endif //GATTLIB_EXPERIMENTS_SCANNERCALLBACKINTERFACE_H
