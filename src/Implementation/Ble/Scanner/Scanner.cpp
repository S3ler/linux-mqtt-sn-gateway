//
// Created by bele on 11.06.17.
//

#include "Scanner.h"
#include <bluetooth/hci_lib.h>

//void Scanner::setScannerQueue(Queue<BluetoothLowEnergyAdvertise *> *queue) {}
/*
void Scanner::ble_discovered_device(const char *addr, const char *name) {
   std::chrono::milliseconds timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch());
    BluetoothLowEnergyAdvertise *advertise = new BluetoothLowEnergyAdvertise(addr, name, timestamp);
    if (strlen(name) > 0) {
        queue->push(advertise);
    }
}


*/

// global scanner
Scanner *global_scanner = nullptr;

static void sigint_handler(int sig) {
    if (global_scanner == nullptr) {
        return;
    }
    global_scanner->setSignal(sig);
}

std::list<ScanResult *> Scanner::getScanResults() {
    return scanResults;
}

void Scanner::scan(uint16_t duration) {
    global_scanner = this;
    stopped = false;
    lescan(duration);
}


void Scanner::stop() {
    stopped = true;
    if (scan_thread.joinable()) {
        scan_thread.join();
    }
}

void Scanner::lescan(uint16_t duration) {

    uint8_t own_type = LE_PUBLIC_ADDRESS;
    uint8_t scan_type = 0x01;
    uint8_t filter_type = 0;
    uint8_t filter_policy = 0x00;
    uint16_t interval = htobs(0x0010);
    uint16_t window = htobs(0x0010);
    uint8_t filter_dup = 0x01;


    bdaddr_t bdaddr;
    if (str2ba(MAC, &bdaddr) != 0) {
        perror("Converting MAC to bdaddr_t failed");
        exit(1);
    }
    int dev_id = hci_get_route(&bdaddr);
    int device_descriptor = hci_open_dev(dev_id);

    if (device_descriptor < 0) {
        perror("Could not open device");
        exit(1);
    }

    int err = hci_le_set_scan_parameters(device_descriptor, scan_type, interval, window,
                                         own_type, filter_policy, 10000);
    if (err < 0) {
        perror("Set scan parameters failed");
        perror("Are you sudo or able to access bluetoothd?");
        exit(1);
    }

    err = hci_le_set_scan_enable(device_descriptor, 0x01, filter_dup, 10000);
    if (err < 0) {
        perror("Enable scan failed");
        exit(1);
    }

    printf("LE Scan ...\n");

    err = print_advertising_devices(device_descriptor, filter_type, duration, callbackInterface);
    if (err < 0) {
        perror("Could not receive advertising events");
        exit(1);
    }

    err = hci_le_set_scan_enable(device_descriptor, 0x00, filter_dup, 10000);
    if (err < 0) {
        perror("Disable scan failed");
        // TODO we ignore it because when we connect without disabling the scanning, the disabling does not work
        //exit(1);
        stopped = true;
    }

    hci_close_dev(device_descriptor);
    stopped = true;

}


int Scanner::print_advertising_devices(int device_descriptor, uint8_t filter_type, int duration,
                                       ScannerCallbackInterface *callbackInterface) {
    unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
    struct hci_filter nf, of;
    struct sigaction sa;
    socklen_t olen;
    int len;

    olen = sizeof(of);
    if (getsockopt(device_descriptor, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
        printf("Could not get socket options\n");
        return -1;
    }

    hci_filter_clear(&nf);
    hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
    hci_filter_set_event(EVT_LE_META_EVENT, &nf);

    if (setsockopt(device_descriptor, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
        printf("Could not set socket options\n");
        return -1;
    }

    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_NOCLDSTOP;
    sa.sa_handler = sigint_handler;
    sigaction(SIGINT, &sa, NULL);

    time_t start = time(NULL);
    while (1) {
        evt_le_meta_event *meta;
        le_advertising_info *info;
        char addr[18];

        while ((len = read(device_descriptor, buf, sizeof(buf))) < 0) {
            if (errno == EINTR && signal_received == SIGINT) {
                len = 0;
                goto done;
            }

            if (errno == EAGAIN || errno == EINTR)
                continue;
            goto done;
        }

        ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
        len -= (1 + HCI_EVENT_HDR_SIZE);

        // Prior:
        //meta = (void *) ptr;
        meta = (evt_le_meta_event *) (void *) ptr;

        if (meta->subevent != 0x02)
            goto done;

        /* Ignoring multiple reports */
        info = (le_advertising_info *) (meta->data + 1);
        if (check_report_filter(filter_type, info)) {
            char name[30];

            memset(name, 0, sizeof(name));

            ba2str(&info->bdaddr, addr);
            eir_parse_name(info->data, info->length,
                           name, sizeof(name) - 1);

            // HERE
            // printf("Discovered: %s %s\n", addr, name);

            // check if connection already in list
            {
                std::lock_guard<std::mutex> list_lock_guard(list_mutex);
                device_address ble_device_address;
                memset(ble_device_address.bytes, 0x0, sizeof(device_address));
                // memcpy(ble_device_address.bytes, &info->bdaddr, sizeof(bdaddr_t));
                memcpy(ble_device_address.bytes, &info->bdaddr, sizeof(bdaddr_t));


                bool not_in_list = true;
                for (auto &&scanResult : scanResults) {
                    if (memcmp(scanResult->getDeviceAddress()->bytes, ble_device_address.bytes,
                               sizeof(device_address)) == 0) {
                        not_in_list = false;
                        break;
                    }
                }
                if (not_in_list) {
                    ScanResult *result = new ScanResult(&ble_device_address);
                    scanResults.push_front(result);
                    if (callbackInterface != nullptr) {
                        bool cb_return = callbackInterface->onScanReceive(result);
                        if (!cb_return) {
                            stopped = true;
                        }
                    }
                }
            }

        }
        if (stopped) {
            break;
        }
        if (duration != 0) {
            if (time(NULL) > start + duration) {
                // finished scan
                break;
            }
        }
    }

    done:
    setsockopt(device_descriptor, SOL_HCI, HCI_FILTER, &of, sizeof(of));

    if (len < 0) {
        return -1;
    }
    return 0;
}


int Scanner::check_report_filter(uint8_t procedure, le_advertising_info *info) {
    uint8_t flags;

    /* If no discovery procedure is set, all reports are treat as valid */
    if (procedure == 0)
        return 1;

    /* Read flags AD type value from the advertising report if it exists */
    if (read_flags(&flags, info->data, info->length))
        return 0;

    switch (procedure) {
        case 'l': /* Limited Discovery Procedure */
            if (flags & FLAGS_LIMITED_MODE_BIT)
                return 1;
            break;
        case 'g': /* General Discovery Procedure */
            if (flags & (FLAGS_LIMITED_MODE_BIT | FLAGS_GENERAL_MODE_BIT))
                return 1;
            break;
        default:
            fprintf(stderr, "Unknown discovery procedure\n");
    }

    return 0;
}

int Scanner::read_flags(uint8_t *flags, const uint8_t *data, size_t size) {
    size_t offset;

    if (!flags || !data)
        return -EINVAL;

    offset = 0;
    while (offset < size) {
        uint8_t len = data[offset];
        uint8_t type;

        /* Check if it is the end of the significant part */
        if (len == 0)
            break;

        if (len + offset > size)
            break;

        type = data[offset + 1];

        if (type == FLAGS_AD_TYPE) {
            *flags = data[offset + 2];
            return 0;
        }

        offset += 1 + len;
    }

    return -ENOENT;
}

void Scanner::eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len) {
    size_t offset;

    offset = 0;
    while (offset < eir_len) {
        uint8_t field_len = eir[0];
        size_t name_len;

        /* Check for the end of EIR */
        if (field_len == 0)
            break;

        if (offset + field_len > eir_len)
            goto failed;

        switch (eir[1]) {
            case EIR_NAME_SHORT:
            case EIR_NAME_COMPLETE:
                name_len = field_len - 1;
                if (name_len > buf_len)
                    goto failed;

                memcpy(buf, &eir[2], name_len);
                return;
        }

        offset += field_len + 1;
        eir += field_len + 1;
    }


    failed:
    snprintf(buf, buf_len, "(unknown)");
}

void Scanner::setSignal(int sig) {
    std::lock_guard<std::mutex> signal_receive_lock_guard(signal_received_mutex);
    signal_received = sig;
}

void Scanner::removeScanResult(ScanResult *scanResult) {
    std::lock_guard<std::mutex> list_lock_guard(list_mutex);
    scanResults.remove(scanResult);
    delete (scanResult);
}

void Scanner::scanWithCallback() {
    stopped = false;
    lescan(0);
    this->callbackInterface = nullptr;
}


void Scanner::scan(ScannerCallbackInterface *callbackInterface) {
    global_scanner = this;
    this->callbackInterface = callbackInterface;
    stopped = false;
    this->scan_thread = std::thread(&Scanner::scanWithCallback, this);
}

bool Scanner::isRunning() {
    return !stopped;
}


bool deleteAll(ScanResult *connection) {
    delete connection;
    return true;
}

void Scanner::free_scanResults() {
    std::lock_guard<std::mutex> scanResult_lock(list_mutex);
    scanResults.remove_if(deleteAll);
}

Scanner::Scanner(const char *scanner_mac) {
    if (scanner_mac == nullptr) {
        // TODO error
    }
    if (strlen(scanner_mac) != 17) {
        // TODO error
    }
    strcpy(this->MAC, scanner_mac);
}
