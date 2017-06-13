//
// Created by bele on 11.06.17.
//

#include <thread>
#include <BluetoothLowEnergy/gattlib/bluez/gattlib_internal.h>
#include "PerpheralConnection.h"

#define MIN(a, b)    ((a)<(b)?(a):(b))

#define NUS_CHARACTERISTIC_TX_UUID    "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define NUS_CHARACTERISTIC_RX_UUID    "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define NUS_CHARACTERISTIC_RX_CCCD    "2902"

void notification_cb(const uuid_t *uuid, const uint8_t *data, size_t data_length, void *user_data) {
    PerpheralConnection *connection = static_cast<PerpheralConnection *>(user_data);
    uuid_t nus_characteristic_rx_uuid;
    int ret = gattlib_string_to_uuid(NUS_CHARACTERISTIC_RX_UUID, strlen(NUS_CHARACTERISTIC_RX_UUID) + 1,
                                 &nus_characteristic_rx_uuid);
    if (ret) {
        fprintf(stderr, "Fail to convert characteristic RX_HANDLE to UUID.\n");
        // TODO disconnect myself from gateway
    }
    if (gattlib_uuid_cmp(uuid, &nus_characteristic_rx_uuid) == 0) {
        connection->receiveData(data, data_length);
    }
}

device_address *PerpheralConnection::getAddress() {
    return &address;
}

bool PerpheralConnection::send(uint8_t *payload, uint16_t payload_length) {
    if (payload_length > 20) {
        return false;
    }

    int ret = gattlib_write_char_by_handle(m_connection, tx_handle, payload, payload_length);
    if (ret) {
        fprintf(stderr, "Fail to send data to NUS TX characteristic.\n");
        // TODO disconnect myself from gateway
        return false;
    }
    return true;
}

bool PerpheralConnection::connect(char *mac) {
    int i, ret, total_length, length = 0;
    uuid_t nus_characteristic_tx_uuid;
    uuid_t nus_characteristic_rx_cccd;
    uuid_t nus_characteristic_rx_uuid;


    m_connection = gattlib_connect(NULL, mac, BDADDR_LE_RANDOM, BT_SEC_LOW, 0, 0);
    if (m_connection == NULL) {
        fprintf(stderr, "Fail to connect to the bluetooth device.\n");
        return false;
    }

    // Convert characteristics to their respective UUIDs
    ret = gattlib_string_to_uuid(NUS_CHARACTERISTIC_TX_UUID, strlen(NUS_CHARACTERISTIC_TX_UUID) + 1,
                                 &nus_characteristic_tx_uuid);
    if (ret) {
        fprintf(stderr, "Fail to convert characteristic TX to UUID.\n");
        return false;
    }
    ret = gattlib_string_to_uuid(NUS_CHARACTERISTIC_RX_UUID, strlen(NUS_CHARACTERISTIC_RX_UUID) + 1,
                                 &nus_characteristic_rx_uuid);
    if (ret) {
        fprintf(stderr, "Fail to convert characteristic RX_HANDLE to UUID.\n");
        return false;
    }
    ret = gattlib_string_to_uuid(NUS_CHARACTERISTIC_RX_CCCD, strlen(NUS_CHARACTERISTIC_RX_CCCD) + 1, &nus_characteristic_rx_cccd);
    if (ret) {
        fprintf(stderr, "Fail to convert characteristic RX to UUID.\n");
        return false;
    }

    // Look for handle for NUS_CHARACTERISTIC_TX_UUID
    gattlib_characteristic_t *characteristics;
    int characteristic_count;
    ret = gattlib_discover_char(m_connection, &characteristics, &characteristic_count);
    if (ret) {
        fprintf(stderr, "Fail to discover characteristic.\n");
        return false;
    }

    for (i = 0; i < characteristic_count; i++) {
        if (gattlib_uuid_cmp(&characteristics[i].uuid, &nus_characteristic_tx_uuid) == 0) {
            tx_handle = characteristics[i].value_handle;
        } else if (gattlib_uuid_cmp(&characteristics[i].uuid, &nus_characteristic_rx_uuid) == 0) {
            rx_handle = characteristics[i].value_handle;
        }
    }
    if (tx_handle == 0) {
        fprintf(stderr, "Fail to find NUS TX characteristic.\n");
        return false;
    } else if (rx_handle == 0) {
        fprintf(stderr, "Fail to find NUS RX characteristic.\n");
        return false;
    }
    free(characteristics);

    // Look for handle for NUS_CHARACTERISTIC_RX_CCCD
    gattlib_descriptor_t *descriptors;
    int descriptor_count;
    ret = gattlib_discover_desc(m_connection, &descriptors, &descriptor_count);
    if (ret) {
        fprintf(stderr, "Fail to discover descriptors.\n");
        return false;
    }

    for (i = rx_handle; i < descriptor_count; i++) {
        if (gattlib_uuid_cmp(&descriptors[i].uuid, &nus_characteristic_rx_cccd) == 0) {
            rx_cccd_handle = descriptors[i].handle;
            break;
        }

    }

    if (rx_cccd_handle == -1) {
        fprintf(stderr, "Fail to find NUS RX CCCD.\n");
        return false;
    }
    free(descriptors);

    // Enable Status Notification
    uint16_t enable_notification = 0x0001;
    gattlib_write_char_by_handle(m_connection, (uint16_t) rx_cccd_handle, &enable_notification,
                                 sizeof(enable_notification));

    // Register notification handler
    gattlib_register_notification(m_connection, notification_cb, this);


    fprintf(stderr, "NUS connection established\n");

    return true;
}

void PerpheralConnection::start_loop() {
    this->thread = std::thread(&PerpheralConnection::loop, this);
    // this->thread.detach();
}

void PerpheralConnection::loop() {
    if(!this->connect(advertise->getMAC())){
        return;
    }
    bleSocket->addPeripheralConnection(this);
    creater->removeFromBlacklist(advertise);
    while(!stopped){
        std::this_thread::sleep_for(5s);
    }
}

void PerpheralConnection::stop_loop() {
    this->stopped = true;
}

void PerpheralConnection::setPeripheralCreator(PeripherapConnectionCreator *pCreator) {
    this->creater = pCreator;
}

void PerpheralConnection::setAdvertise(BluetoothLowEnergyAdvertise *pAdvertise) {
    this->advertise = pAdvertise;
    device_address tmp_address = bleSocket->convertToDeviceAddress(this->advertise->getMAC());
    memcpy(&this->address, &tmp_address, sizeof(tmp_address));
}
void PerpheralConnection::setBLESocket(LinuxBluetoothLowEnergySocket *bleSocket) {
    this->bleSocket = bleSocket;
}

char *PerpheralConnection::getMAC() {
    return advertise->getMAC();
}

void PerpheralConnection::receiveData(const uint8_t *data, size_t data_length) {
    this->bleSocket->receive(this->getMAC(), data, data_length);
}

PerpheralConnection::~PerpheralConnection() {
    if (m_connection != NULL) {
        gattlib_disconnect(m_connection);
        free(m_connection);
    }
    if (advertise != nullptr) {
        delete advertise;
    }
}
