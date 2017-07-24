//
// Created by bele on 11.06.17.
//

#ifndef LINUX_MQTT_SN_GATEWAY_SCANNER_H
#define LINUX_MQTT_SN_GATEWAY_SCANNER_H

#include "ScanResult.h"
#include "ScannerCallbackInterface.h"

#include <stdint.h>

#include <list>
#include <atomic>
#include <mutex>

#include <cstdlib>
#include <signal.h>
#include <csignal>
#include <zconf.h>
#include <errno.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <thread>


#define FLAGS_AD_TYPE               0x01
#define FLAGS_LIMITED_MODE_BIT      0x01
#define FLAGS_GENERAL_MODE_BIT      0x02
#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */
#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */

class Scanner {
private:
    std::mutex signal_received_mutex;
    volatile int signal_received = 0;
    volatile std::atomic<bool> stopped;
    std::list<ScanResult *> scanResults;
    std::mutex list_mutex;
    ScannerCallbackInterface *callbackInterface = nullptr;
    std::thread scan_thread;

    char MAC[18] = {0};

public:

    Scanner(const char *scanner_mac);

    void scan(uint16_t duration);

    void scan(ScannerCallbackInterface *callbackInterface);

    void stop();

    void free_scanResults();

    std::list<ScanResult *> getScanResults();

    bool isRunning();

    void setSignal(int sig);

    void removeScanResult(ScanResult *scanResult);

private:
    void scanWithCallback();

    void lescan(uint16_t duration);

    int print_advertising_devices(int device_descriptor, uint8_t filter_type, int duration,
                                  ScannerCallbackInterface *callbackInterface);

    int check_report_filter(uint8_t procedure, le_advertising_info *info);

    int read_flags(uint8_t *flags, const uint8_t *data, size_t size);

    void eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len);


};

#endif //LINUX_MQTT_SN_GATEWAY_SCANNER_H
