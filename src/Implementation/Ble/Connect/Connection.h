//
// Created by bele on 09.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_CONNECTION_H
#define GATTLIB_EXPERIMENTS_CONNECTION_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "../Scanner/ScanResult.h"
#include "ReceiveInterface.h"
#include <stdint.h>
#include <iostream>

#include <glib.h>
#include "../include/gattlib.h"
#include <sys/queue.h>


#include <bluetooth/bluetooth.h>
#include <thread>
#include <mutex>

extern "C" {
#include "../bluez/bluez5/src/shared/crypto.h"
#include "../bluez/bluez5/lib/hci_lib.h"
#include "../bluez/bluez5/lib/uuid.h"

#include "../bluez/bluez5/attrib/att.h"
#include "../bluez/bluez5/btio/btio.h"
#include "../bluez/bluez5/attrib/gatt.h"
#include "../bluez/bluez5/src/shared/util.h"
}


#define TX_CHRC_UUID "6e400003-b5a3-f393-e0a9-e50e24dcca9e"
#define RX_CHRC_UUID "6e400002-b5a3-f393-e0a9-e50e24dcca9e"
#define RX_CCCD_UUID "00002902-0000-1000-8000-00805f9b34fb"

enum ConnectionErrorStatus {
    MissingService,
    InternalBluetoothError,
    ConnectionRefused,
    ConnectionDisconnected
};

enum ConnectionState {
    STATE_DISCONNECTED,
    STATE_CONNECTED,
    STATE_HANDLE_READY,
    STATE_TX_VALUE_SAVED,
    STATE_RX_NOTIFY_ENABLED,
    STATE_NUS_READY,

    STATE_PENDING_CHANGE,
    STATE_CLOSED,
    STATE_ERROR
};


class Connection {
private:
    device_address address;
    volatile ConnectionErrorStatus errorStatus = ConnectionDisconnected;
    std::thread state_observer;
    std::thread g_lib_main_thread;

    std::mutex state_mutex;
    volatile ConnectionState conn_state = STATE_DISCONNECTED;

public:
    Connection(const device_address *address, const char *hci_mac);

    void setReceiverInterface(ReceiverInterface *receiverInterface);

    void setErrorState(const volatile ConnectionErrorStatus state);

    volatile ConnectionErrorStatus getErrorStatus();

    bool isDeviceAddress(const device_address *address);

    bool connect();

    void close();

    bool send(uint8_t *data, uint16_t length);

    void onReceive(const uint8_t *data, const uint16_t length);

    void call_g_main_loop_run();

    void call_observe_state_loop();

    void setState(const volatile ConnectionState state);

    volatile ConnectionState getState() const;

    ReceiverInterface *receiverInterface = nullptr;

public:
    char *opt_dst = NULL;
    // FIXME: make MAC configureable
    char opt_src[18] = {0};
    //char *opt_src = NULL;
    char *opt_dst_type = NULL;
    char *opt_sec_level = NULL;
    int opt_psm = 0;
    int opt_mtu = 0;
    int start;
    int end;

    uint16_t nus_rx_notify_handle = 0;
    uint16_t nus_rx_handle = 0;
    uint16_t nus_tx_handle = 0;
    char tx_buffer[20] = {0};


    GIOChannel *iochannel = NULL;
    GAttrib *attrib = NULL;
    GMainLoop *event_loop;

    guint prompt_input;
    guint prompt_signal;

    void cmd_connect();

    GIOChannel *gatt_connect(const char *src, const char *dst,
                             const char *dst_type, const char *sec_level,
                             int psm, int mtu, BtIOConnect connect_cb,
                             GError **gerr);

    void cmd_check_characteristic_descriptors();

    void disconnect_io();

    void cmd_read_tx_buffer();

    void cmd_write_tx_notify_hnd();

    void cmd_set_nus_ready();

    bool cmd_char_write_raw(uint16_t length, uint8_t *data);

    const device_address *getAddress() const;


};


#endif //GATTLIB_EXPERIMENTS_CONNECTION_H
