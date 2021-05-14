#include "peer_to_peer_alert/peer_to_peer_alert.h"

/*
 * Gets one token at a time from the char buffer 'temp_msg' passed
 * as function argument and stores it in char buffer 'token_value'
 */
static int parse_token_from_buffer(int *index_pos, char *temp_msg,
                                   char *token_value) {
  int i = 0;
  char value_from_buffer[16] = {0};
  while (temp_msg[*index_pos] != ',') {
    value_from_buffer[i] = temp_msg[*index_pos];
    *index_pos += 1;
    i++;
    if (temp_msg[*index_pos] == '\0') {
      break;
    }
  }
  value_from_buffer[strlen(value_from_buffer)] = '\0';
  strcpy(token_value, value_from_buffer);
  return 1;
}

static void get_token_value(char *udp_msg, int requested_token_index,
                            char *req_token_val) {
  int curr_index_pos = 0;
  int token_index = -1;
  char value[16] = {0};

  while (curr_index_pos < strlen(udp_msg)) {
    if (udp_msg[curr_index_pos] == '\0') {
      break;
    }
    token_index += parse_token_from_buffer(&curr_index_pos, udp_msg, &value[0]);
    if (token_index == (requested_token_index)) {
      strcpy(req_token_val, value);
      req_token_val[strlen(req_token_val)] = '\0';
      break;
    }
    memset(value, 0, sizeof(value));
    curr_index_pos++;
  }
}

/* Process the incoming UDP message from adjacent peer nodes */
void peer_to_peer__process_udp_msg(char *udp_decoded_buffer) {
  int requested_token_index = 0;
  char rloc16_str[16] = {0}, hb_hr_str[16] = {0}, hb_oxy_str[16] = {0};
  char alert_msg[256] = {0};
  char encoded_value[32] = {0};

  requested_token_index = RLOC16_ID;
  get_token_value(udp_decoded_buffer, requested_token_index, &rloc16_str[0]);
  strcat(alert_msg, rloc16_str);

  requested_token_index = HB_HEARTRATE;
  get_token_value(udp_decoded_buffer, requested_token_index, &hb_hr_str[0]);
  sprintf(encoded_value, " hb_hr: %d", (int)((atoi(hb_hr_str)) / 10));
  strcat(alert_msg, encoded_value);
  memset(encoded_value, 0, sizeof(encoded_value));

  requested_token_index = HB_OXYGEN;
  get_token_value(udp_decoded_buffer, requested_token_index, &hb_oxy_str[0]);
  sprintf(encoded_value, " hb_oxy: %d", (int)((atoi(hb_oxy_str)) / 10));
  strcat(alert_msg, encoded_value);

  /* Writing Final Alert Message to Bluetooth Module*/
  BT_ALERT_WRITE(alert_msg);
}
