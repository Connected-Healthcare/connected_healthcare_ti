#ifndef USER_PEER_TO_PEER_ALERT_H_
#define USER_PEER_TO_PEER_ALERT_H_

/* Standard Library Header files */
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Utility Functions */
#include "third_party/tinyprintf/tinyprintf.h"
#include "tinyprintf/bluetooth_wrapper.h"

/* Process the incoming UDP message from peer nodes */
void peer_to_peer__process_udp_msg(char *udp_msg);

enum UDP_MESSAGE_INDEX_DEFINITION {
  RLOC16_ID = 0,
  HB_HEARTRATE = 15,
  HB_OXYGEN = 16
};

#endif
