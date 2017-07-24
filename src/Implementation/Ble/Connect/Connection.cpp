//
// Created by bele on 09.07.17.
//

#include "Connection.h"
#include "ConnectionCallbacks.h"

Connection::Connection(const device_address *address, const char *hci_mac) {
    if (address == nullptr) {
        // TODO error
    }
    if (hci_mac == nullptr) {
        // TODO error
    }
    if (strlen(hci_mac) != 17) {
        // TODO error
    }

    strcpy(this->opt_src, hci_mac);
    memcpy(&this->address, address, sizeof(device_address));
}

void Connection::setReceiverInterface(ReceiverInterface *receiverInterface) {
    if (receiverInterface == nullptr) {
        // TODO error
    }
    this->receiverInterface = receiverInterface;
}

void Connection::setErrorState(const volatile ConnectionErrorStatus state) {
    this->errorStatus = state;
}

volatile ConnectionErrorStatus Connection::getErrorStatus() {
    return errorStatus;
}

bool Connection::isDeviceAddress(const device_address *address) {
    return memcmp(this->address.bytes, address->bytes, sizeof(device_address)) == 0;
}

void Connection::onReceive(const uint8_t *data, const uint16_t length) {
    if (receiverInterface != nullptr) {
        receiverInterface->onReceive(&this->address, data, length);
    } else {
        for (int i = 0; i < sizeof(device_address); i++) {
            std::cout << std::hex << std::uppercase << (int) address.bytes[i]
                      << std::nouppercase << std::dec
                      << std::flush;
            if (i != sizeof(device_address) - 1) {
                std::cout << ":";
            }
        }
        std::cout << " - ";
        for (int i = 0; i < length; i++) {
            std::cout << (char) data[i] << std::flush;
        }
        std::cout << std::endl;
    }
}

bool Connection::connect() {

    char peripheral_mac[50] = {0};
    bdaddr_t tmp_bdaddr;
    memcpy(tmp_bdaddr.b, this->address.bytes, sizeof(bdaddr_t));
    ba2str(&tmp_bdaddr, peripheral_mac);

    opt_sec_level = g_strdup("low");
    // opt_src = NULL;
    opt_dst = g_strdup(peripheral_mac);
    opt_dst_type = g_strdup("public");
    opt_psm = 0;

    event_loop = g_main_loop_new(NULL, FALSE);

    this->g_lib_main_thread = std::thread(&Connection::call_g_main_loop_run, this);
    //this->g_lib_main_thread.detach();

    this->state_observer = std::thread(&Connection::call_observe_state_loop, this);
    this->state_observer.join();
    return conn_state == STATE_NUS_READY;
}

void Connection::close() {
    // TODO maybe create a asynchronous close function
    // TODO asyncClose - awaitClose
    this->setState(STATE_CLOSED);

    if (g_main_loop_is_running(event_loop)) {
        g_main_loop_quit(event_loop);
        if (g_lib_main_thread.joinable()) {
            g_lib_main_thread.join();
        }
    }


    disconnect_io();
    g_source_remove(prompt_input);
    g_source_remove(prompt_signal);
    g_main_loop_unref(event_loop);

    // opt_src is fixed coded, nothing to free
    //g_free(opt_src);
    g_free(opt_dst);
    g_free(opt_sec_level);

    if (state_observer.joinable()) {
        state_observer.join();
    }
}

bool Connection::send(uint8_t *data, uint16_t length) {
    if (conn_state != STATE_NUS_READY) {
        // TODO set error stuff
        return false;
    }
    return cmd_char_write_raw(length, data);
}

bool Connection::cmd_char_write_raw(uint16_t length, uint8_t *data) {

    if (length > 20) {
        // TODO this is too long, MQTT-SN standard says: discard but i dont like it.
        printf("Command Failed: Too much data %i\n", length);
        return false;
    }

    if (conn_state != STATE_NUS_READY) {
        printf("Command Failed: Not STATE_HANDLE_READY\n");
        return false;
    }

    gint r = gatt_write_char(attrib, nus_tx_handle, data, (size_t) length,
                             char_write_req_raw_cb, this);
    return r > 0;
}

void Connection::cmd_connect() {
    GError *gerr = NULL;

    if (conn_state != STATE_DISCONNECTED) {
        // error!
        return;
    }

    opt_dst_type = g_strdup("public");

    // opt_src is the MAC of the local bluetooth adapter

    if (opt_dst == NULL) {
        printf("Command Failed: Remote Bluetooth address required\n");
        return;
    }
    printf("Attempting to connect to %s\n", opt_dst);
    iochannel = gatt_connect(opt_src, opt_dst, opt_dst_type, opt_sec_level,
                             opt_psm, opt_mtu, connect_cb, &gerr);
    if (iochannel == NULL) {
        setState(STATE_ERROR);
        printf("Command Failed: %s\n", gerr->message);
        g_error_free(gerr);
    } else {
        g_io_add_watch(iochannel, G_IO_HUP, channel_watcher, this);
        setState(STATE_PENDING_CHANGE);
    }
}

GIOChannel *
Connection::gatt_connect(const char *src, const char *dst, const char *dst_type, const char *sec_level, int psm,
                         int mtu, BtIOConnect connect_cb, GError **gerr) {
    GIOChannel *chan;
    bdaddr_t sba, dba;
    uint8_t dest_type;
    GError *tmp_err = NULL;
    BtIOSecLevel sec;

    str2ba(dst, &dba);

    /* Local adapter */
    if (src != NULL) {
        if (!strncmp(src, "hci", 3))
            hci_devba(atoi(src + 3), &sba);
        else
            str2ba(src, &sba);
    } else {
        // BDADRR_ANY is exchanged
        //#define BDADDR_ANY   (&(bdaddr_t) {{0, 0, 0, 0, 0, 0}})
        bdaddr_t bdaddr_any;
        memset(bdaddr_any.b, 0, sizeof(bdaddr_any));
        bacpy(&sba, &bdaddr_any);
    }


    /* Not used for BR/EDR */
    if (strcmp(dst_type, "random") == 0)
        dest_type = BDADDR_LE_RANDOM;
    else
        dest_type = BDADDR_LE_PUBLIC;

    if (strcmp(sec_level, "medium") == 0)
        sec = BT_IO_SEC_MEDIUM;
    else if (strcmp(sec_level, "high") == 0)
        sec = BT_IO_SEC_HIGH;
    else
        sec = BT_IO_SEC_LOW;

    if (psm == 0)
        chan = bt_io_connect(connect_cb, this, NULL,
                             &tmp_err, // ++ gpointer usercontext classe hier ersten NULL erstezen
                             BT_IO_OPT_SOURCE_BDADDR, &sba,
                             BT_IO_OPT_SOURCE_TYPE, BDADDR_LE_PUBLIC,
                             BT_IO_OPT_DEST_BDADDR, &dba,
                             BT_IO_OPT_DEST_TYPE, dest_type,
                             BT_IO_OPT_CID, ATT_CID,
                             BT_IO_OPT_SEC_LEVEL, sec,
                             BT_IO_OPT_INVALID);
    else
        chan = bt_io_connect(connect_cb, this, NULL, &tmp_err,
                             BT_IO_OPT_SOURCE_BDADDR, &sba,
                             BT_IO_OPT_DEST_BDADDR, &dba,
                             BT_IO_OPT_PSM, psm,
                             BT_IO_OPT_IMTU, mtu,
                             BT_IO_OPT_SEC_LEVEL, sec,
                             BT_IO_OPT_INVALID);

    if (tmp_err) {
        g_propagate_error(gerr, tmp_err);
        return NULL;
    }

    return chan;
}


void Connection::disconnect_io() {
    if (conn_state == STATE_CLOSED)
        return;

    g_attrib_unref(attrib);
    attrib = NULL;
    opt_mtu = 0;
    g_io_channel_shutdown(iochannel, FALSE, NULL);
    g_io_channel_unref(iochannel);
    iochannel = NULL;

    setState(STATE_CLOSED);
    //TODO check who calls whom
    //close();
}


void Connection::cmd_check_characteristic_descriptors() {
    if (conn_state != STATE_CONNECTED) {
        printf("Command Failed: Disconnected\n");
        return;
    }

    start = 0x0001;
    end = 0xffff;
    setState(STATE_PENDING_CHANGE);
    gatt_discover_desc(attrib, start, end, NULL, check_characteristic_descriptors, this);
}


void Connection::cmd_read_tx_buffer() {
    if (conn_state != STATE_HANDLE_READY) {
        printf("Command Failed: Not STATE_HANDLE_READY\n");
        return;
    }

    setState(STATE_PENDING_CHANGE);
    gatt_read_char(attrib, nus_rx_handle, cmd_read_tx_buffer_cb, this);
}

void Connection::cmd_write_tx_notify_hnd() {

    if (conn_state != STATE_TX_VALUE_SAVED) {
        printf("Command Failed: Not STATE_TX_VALUE_SAVED\n");
        return;
    }

    uint16_t handle = nus_rx_notify_handle;
    if (handle <= 0) {
        printf("Command Failed: A valid handle is required\n");
        return;
    }

    uint8_t value[] = {1, 0};
    int plen = 2;
    gatt_write_cmd(this->attrib, handle, value, plen, NULL, NULL);
    setState(STATE_RX_NOTIFY_ENABLED);
}

void Connection::cmd_set_nus_ready() {
    setState(STATE_NUS_READY);
    printf("\n\n Finally -- NUS Ready \n");
}

void Connection::call_g_main_loop_run() {
    g_main_loop_run(event_loop);
}

void Connection::call_observe_state_loop() {
    while (true) {
        if (conn_state == STATE_CLOSED || conn_state == STATE_ERROR) {
            return;
        }
        if (conn_state == STATE_PENDING_CHANGE) {
            // nothing to do
            continue;
        }
        if (conn_state == STATE_DISCONNECTED) {
            cmd_connect();
        } else if (conn_state == STATE_CONNECTED) {
            cmd_check_characteristic_descriptors();
        } else if (conn_state == STATE_HANDLE_READY) {
            cmd_read_tx_buffer();
        } else if (conn_state == STATE_TX_VALUE_SAVED) {
            cmd_write_tx_notify_hnd();
        } else if (conn_state == STATE_RX_NOTIFY_ENABLED) {
            cmd_set_nus_ready();
            return;
        }
        // TODO add timeout to calls
    }
}

void Connection::setState(const volatile ConnectionState state) {
    //std::lock_guard<std::mutex> state_lock_guard(state_mutex);
    this->conn_state = state;
}

volatile ConnectionState Connection::getState() const {
    return this->conn_state;
}

const device_address *Connection::getAddress() const {
    return &address;
}
