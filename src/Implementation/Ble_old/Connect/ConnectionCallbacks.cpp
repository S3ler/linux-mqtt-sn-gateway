//
// Created by bele on 11.07.17.
//

#include "ConnectionCallbacks.h"
#include "Connection.h"

extern "C" {

void events_handler(const uint8_t *pdu, uint16_t len, gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);
    if (connection == NULL) {
        g_printerr("Connection is NULL in  events_handler");
        // TODO handle error
    }

    uint8_t *opdu;
    uint16_t handle, i, olen;
    size_t plen;
    GString *s;

    handle = get_le16(&pdu[1]);

    switch (pdu[0]) {
        case ATT_OP_HANDLE_NOTIFY:
            s = g_string_new(NULL);
            g_string_printf(s, "Notification handle = 0x%04x value: ",
                            handle);
            if (handle == connection->nus_rx_notify_handle) {
                printf("nus_rx_notify_handle");
            } else if (handle == connection->nus_rx_handle) {
                printf("nus_rx_handle");
                connection->onReceive(pdu, len);
            }
            // HERE: notify handle
            break;
        case ATT_OP_HANDLE_IND:
            s = g_string_new(NULL);
            g_string_printf(s, "Indication   handle = 0x%04x value: ",
                            handle);
            break;
        default:
            printf("Command Failed: Invalid opcode\n");
            return;
    }

    g_string_free(s, TRUE);

    if (pdu[0] == ATT_OP_HANDLE_NOTIFY)
        return;

    opdu = g_attrib_get_buffer(connection->attrib, &plen);
    olen = enc_confirmation(opdu, plen);

    if (olen > 0)
        g_attrib_send(connection->attrib, 0, opdu, olen, NULL, NULL, NULL);
}

void connect_cb(GIOChannel *io, GError *err, gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);

    if (connection == NULL) {
        g_printerr("Connection is NULL in connect_cb");
        // TODO handle error
        // this should never happen!
        // log and die!
    }

    uint16_t mtu;
    uint16_t cid;


    if (err) {
        connection->setState(STATE_ERROR);
        if (err->code == 111) {
            connection->setErrorState(ConnectionRefused);
        }
        printf("Command Failed: %s\n", err->message);
        return;
    }

    bt_io_get(io, &err, BT_IO_OPT_IMTU, &mtu,
              BT_IO_OPT_CID, &cid, BT_IO_OPT_INVALID);

    if (err) {
        g_printerr("Can't detect MTU, using default: %s", err->message);
        g_error_free(err);
        mtu = ATT_DEFAULT_LE_MTU;
    }

    if (cid == ATT_CID)
        mtu = ATT_DEFAULT_LE_MTU;

    connection->attrib = g_attrib_new(connection->iochannel, mtu, false);
    g_attrib_register(connection->attrib, ATT_OP_HANDLE_NOTIFY, GATTRIB_ALL_HANDLES,
                      events_handler, connection, NULL);
    g_attrib_register(connection->attrib, ATT_OP_HANDLE_IND, GATTRIB_ALL_HANDLES,
                      events_handler, connection, NULL);
    connection->setState(STATE_CONNECTED);
    printf("Connection successful\n");

    // called in the connection class
    // connection->cmd_check_characteristic_descriptors();
}

gboolean channel_watcher(GIOChannel *chan, GIOCondition cond,
                         gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);
    if (connection == NULL) {
        g_printerr("Connection is NULL in channel_watcher");
        // TODO handle error
    }
    connection->disconnect_io();
    return FALSE;
}


void check_characteristic_descriptors(uint8_t status, GSList *descriptors, gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);
    if (connection == NULL) {
        g_printerr("Connection is NULL in check_characteristic_descriptors");
        // TODO handle error
    }

    GSList *l;

    if (status) {
        printf("Command Failed: Discover descriptors failed: %s\n",
               att_ecode2str(status));
        return;
    }

    /*
    if (connection->getState() != STATE_HANDLE_CHECKING) {
        printf("Command Failed: Invalid State - must be STATE_HANDLE_CHECKING\n");
        return;
    }
    */

    bool done = false;
    bool hnd_tx = FALSE;
    bool hnd_rx = FALSE;
    bool hnd_rx_cccd = FALSE;

    for (l = descriptors; l; l = l->next) {
        // Prior:
        // struct gatt_desc *desc = l->data;
        // Now
        struct gatt_desc *desc = (gatt_desc *) l->data;

        printf("handle: 0x%04x, uuid: %s\n", desc->handle,
               desc->uuid);
        if (!done) {
            if (strcmp(desc->uuid, TX_CHRC_UUID) == 0) {
                if (!hnd_tx) {
                    hnd_tx = TRUE;
                    connection->nus_tx_handle = desc->handle;
                } else {
                    printf("Command Failed: Discovered duplicate hnd_tx");
                    return;
                }
            }
            if (strcmp(desc->uuid, RX_CHRC_UUID) == 0) {
                if (!hnd_rx) {
                    hnd_rx = TRUE;
                    connection->nus_rx_handle = desc->handle;
                } else {
                    printf("Command Failed: Discovered duplicate hnd_rx");
                    return;
                }
            }
            if (!hnd_rx_cccd && hnd_rx && strcmp(desc->uuid, RX_CCCD_UUID) == 0) {
                if (!hnd_rx_cccd) {
                    hnd_rx_cccd = TRUE;
                    connection->nus_rx_notify_handle = desc->handle;
                }
            }
            if (hnd_tx & hnd_rx & hnd_rx_cccd) {
                done = true;
            }
        }
    }
    if (done) {
        printf("Found all characteristics and CCCDs");
        connection->setState(STATE_HANDLE_READY);

        // called in Connection class
        // connection->cmd_read_tx_buffer();
        // call next
    } else {
        printf("Command Failed: Did not discover necessary characteristics and CCCDs");
    }
}

void cmd_read_tx_buffer_cb(guint8 status, const guint8 *pdu, guint16 plen,
                           gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);
    if (connection == NULL) {
        g_printerr("Connection is NULL in cmd_read_tx_buffer_cb");
        // TODO handle error
    }

    uint8_t value[plen];
    ssize_t vlen;
    int i;
    GString *s;

    /*
    if (conn_state != STATE_RX_VALUE_CLEARING) {
        printf("Command Failed: Not STATE_RX_VALUE_CLEARING\n");
        return;
    }
    */

    if (status != 0) {
        printf("Command Failed: Characteristic value/descriptor read failed: %s\n",
               att_ecode2str(status));
        return;
    }

    vlen = dec_read_resp(pdu, plen, value, sizeof(value));
    if (vlen < 0) {
        printf("Command Failed: Protocol error\n");
        return;
    }

    memset(connection->tx_buffer, 0, sizeof(connection->tx_buffer));
    s = g_string_new("Characteristic value/descriptor: ");
    for (i = 0; i < vlen; i++) {
        g_string_append_printf(s, "%02x ", value[i]);
        connection->tx_buffer[i] = value[i];
    }

    printf("%s\n", s->str);

    g_string_free(s, TRUE);

    connection->setState(STATE_TX_VALUE_SAVED);
    // Strange behaviour expected from bluez
    // it increases the dd and so the pointer to the Connection class is incremented too
    // then the reinterpret_cast casts the class false of course
    // and the attrib member field for Connection is dereferences false in the
    // guint gatt_write_cmd(GAttrib *attrib, uint16_t handle, const uint8_t *value,
    // within the next function:
    // connection->cmd_write_tx_notify_hnd();

    // now the solution is a little bit hack but it works:
    // a thread within Connection observes the own State and when
    // connection->conn_state = STATE_TX_VALUE_SAVED;
    // we call then cmd_write_tx_notify_hnd()
}

void char_write_req_raw_cb(guint8 status, const guint8 *pdu, guint16 plen,
                           gpointer user_data) {
    Connection *connection = reinterpret_cast<Connection *>(user_data);
    if (connection == NULL) {
        g_printerr("Connection is NULL in char_write_req_raw_cb");
        // TODO handle error
    }


    if (status != 0) {
        printf("Command Failed: Characteristic Write Request failed: "
                       "%s\n", att_ecode2str(status));
        connection->setState(STATE_ERROR);
        return;
    }

    if (!dec_write_resp(pdu, plen) && !dec_exec_write_resp(pdu, plen)) {
        printf("Command Failed: Protocol error\n");
        connection->setState(STATE_ERROR);
        return;
    }

    printf("Characteristic value was written successfully\n");
    connection->setState(STATE_NUS_READY);
}
};