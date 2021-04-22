#include "udp_communication/udp_comm.h"

/* Standard Library Header files */
#include <assert.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otinstance.h"
#include "otsupport/otrtosapi.h"
#include <openthread-core-config.h>
#include <openthread/config.h>
#include <openthread/error.h>
#include <openthread/instance.h>
#include <openthread/thread.h>
#include <openthread/udp.h>

/* TI Drivers / RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti_drivers_config.h>

/* Utility Functions */
#include "third_party/tinyprintf/tinyprintf.h"

#include "sensor/aggregate.h"

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

/* GLOBAL MACRO DEFINITIONS */
/* UDP Task Configs */
#define TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE 4096
#define MAX_UDP_MESSAGE_BUFF_LEN 1024

/* FUNCTION DECLARATIONS */
void *udp__comm_task(void *arg0);

/* UDP APIs */
static void udp__comm_init(otInstance *aInstance);
static void udp__comm_send(otInstance *aInstance, char *);

/* GLOBAL VARIABLES */
static otUdpSocket socket;

/*
 * Reserved Multicast IPv6 Addresses:
 * IPv6 Add.     Scope          Delivered To
 * ff02::1  -  Link-Local  -  All FTDs and MEDs
 * ff02::2  -  Link-Local  -  All FTDs
 * ff03::1  -  Mesh-Local  -  All FTDs and MEDs
 * ff03::2  -  Mesh-Local  -  All FTDs
 *
 * FTD - Full Thread Device:
 * a) Always has its radio on
 * b) Subscribes to the all-routers multicast address
 * c) Maintains IPv6 address mappings
 *
 * MED - Minimal End Device:
 * a) Does not subscribe to the all-routers multicast address
 * b) Forwards all messages to its Parent
 * c) Transceiver always on
 * d) Does not need to poll for messages from its parent
 */

/* UDP Parameter Configs */
static const char *udp_ipv6_add = "ff03::1";
static const uint16_t udp_port_num = 1234;

/* UDP Task Configs */
static const int TASK_CONFIG_udp_comm_task_PRIORITY = 3;

/* UDP Receive Callback */
void handleUdpReceive(void *aContext, otMessage *udp_message,
                      const otMessageInfo *aMessageInfo) {
  OT_UNUSED_VARIABLE(aContext);
  OT_UNUSED_VARIABLE(aMessageInfo);
  uint16_t length = otMessageGetLength(udp_message);
  uint16_t msg_offset;
  char udp_decoded_buffer[length];
  msg_offset = otMessageGetOffset(udp_message);
  length = otMessageRead(udp_message, msg_offset, udp_decoded_buffer, length);
  debugPrintf("UDP Received Message: %s\r\n", udp_decoded_buffer);
}

/* Initialize UDP */
static void udp__comm_init(otInstance *aInstance) {
  otError error = OT_ERROR_NONE;
  otSockAddr sockaddr;

  memset(&socket, 0, sizeof(socket));
  memset(&sockaddr, 0, sizeof(sockaddr));

  sockaddr.mPort = udp_port_num;

  error = otUdpOpen(aInstance, &socket, handleUdpReceive, NULL);
  error = otUdpBind(&socket, &sockaddr);

  // TODO Ganesh, Check errors
  (void)error; // remove statement when error check
}

/* Send data over OpenThread network via UDP */
static void udp__comm_send(otInstance *aInstance, char *udp_final_message) {
  otMessage *udp_message;
  otError error = OT_ERROR_NONE;
  otMessageInfo messageInfo;

  /* Specific node address */
  // error = otIp6AddressFromString("fe80:0:0:0:cc95:ed45:96a5:fcc7",
  //  &messageInfo.mPeerAddr);

  memset(&messageInfo, 0, sizeof(messageInfo));

  error = otIp6AddressFromString((const void *)udp_ipv6_add,
                                 &messageInfo.mPeerAddr);
  messageInfo.mPeerPort = udp_port_num;

  udp_message = otUdpNewMessage(aInstance, NULL);
  otMessageAppend(udp_message, (const void *)udp_final_message,
                  (uint16_t)strlen(udp_final_message));
  error = otUdpSend(&socket, udp_message, &messageInfo);

  // TODO Ganesh, Check errors
  (void)error; // remove statement when error check
}

void *udp__comm_task(void *arg0) {
  char udp_temp_msg_buff[512] = {0};
  char udp_final_message[MAX_UDP_MESSAGE_BUFF_LEN] = {0};
  static otDeviceRole curr_state;

  OtRtosApi_lock();
  otInstance *aInstance = OtInstance_get();
  otError error = otIp6SetEnabled(aInstance, true); // ifconfig up
  // TODO Ganesh, Check error
  (void)error; // remove statement when error check

  udp__comm_init(aInstance);
  OtRtosApi_unlock();

  while (1) {

    // if (!GPIO_read(CONFIG_GPIO_BTN1)) {
    if (1) {
      OtRtosApi_lock();
      sprintf(udp_final_message, "RLOC16:%04x,", otThreadGetRloc16(aInstance));
      curr_state = otThreadGetDeviceRole(aInstance);
      OtRtosApi_unlock();
      if (OT_DEVICE_ROLE_CHILD == curr_state ||
          OT_DEVICE_ROLE_ROUTER == curr_state ||
          curr_state == OT_DEVICE_ROLE_LEADER) {
        get_sensors_data(udp_temp_msg_buff);
        strcat(udp_final_message, udp_temp_msg_buff);
        debugPrintf("Sent the Msg: %s\r\n", udp_final_message);
        udp__comm_send(aInstance, udp_final_message);

      } else {
        // debugPrintf("Error: Cannot send as the device is in "
        //             "Disabled/Detached/Invalid state.\r\n");
      }
    }
    memset(udp_temp_msg_buff, 0, sizeof(udp_temp_msg_buff));
    memset(udp_final_message, 0, sizeof(udp_final_message));
    sleep(5);
  }
}

/* Create UDP application task */
void udp__comm_taskCreate() {
  static char udp_comm_stack[TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE];
  pthread_t thread;
  pthread_attr_t pAttrs;
  struct sched_param priParam;

  int retc;

  retc = pthread_attr_init(&pAttrs);
  assert(retc == 0);

  retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
  assert(retc == 0);

  priParam.sched_priority = TASK_CONFIG_udp_comm_task_PRIORITY;
  retc = pthread_attr_setschedparam(&pAttrs, &priParam);
  assert(retc == 0);

  retc = pthread_attr_setstack(&pAttrs, (void *)udp_comm_stack,
                               TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE);
  assert(retc == 0);

  retc = pthread_create(&thread, &pAttrs, udp__comm_task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;
  GPIO_setConfig(CONFIG_GPIO_BTN1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
}
