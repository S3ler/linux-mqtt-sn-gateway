//
// Created by bele on 11.07.17.
//

#ifndef GATTLIB_EXPERIMENTS_CONNECTIONCALLBACKS_H
#define GATTLIB_EXPERIMENTS_CONNECTIONCALLBACKS_H

#include <glib.h>
#include <stdint.h>

extern "C" {

extern void events_handler(const uint8_t *pdu, uint16_t len, gpointer user_data);

extern void connect_cb(GIOChannel *io, GError *err, gpointer user_data);

extern gboolean channel_watcher(GIOChannel *chan, GIOCondition cond,
                                gpointer user_data);

extern void check_characteristic_descriptors(uint8_t status, GSList *descriptors, gpointer user_data);

extern void cmd_read_tx_buffer_cb(guint8 status, const guint8 *pdu, guint16 plen,
                                  gpointer user_data);

extern void char_write_req_raw_cb(guint8 status, const guint8 *pdu, guint16 plen, gpointer user_data);

};
#endif //GATTLIB_EXPERIMENTS_CONNECTIONCALLBACKS_H
