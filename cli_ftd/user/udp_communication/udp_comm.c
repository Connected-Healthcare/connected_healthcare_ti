#include "udp_comm.h"

/* FUNCTION DECLARATIONS */
void *udp_comm_task(void *arg0);

/* UDP APIs */
void udp_comm_init(otInstance *aInstance);
void udp_comm_send(otInstance *aInstance);

/* GLOBAL VARIABLES */

/* Custom UDP Message */
static otMessageInfo messageInfo;
static otUdpSocket socket;
static otMessage *udp_message;
static char buf[10] = "Sample";
static char udp_comm_stack[TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE];

void udp_comm_init(otInstance *aInstance) {
  otError error = OT_ERROR_NONE;
  memset(&messageInfo, 0, sizeof(messageInfo));
  error = otUdpOpen(aInstance, &socket, NULL, NULL);

  // Specific node address
  error = otIp6AddressFromString("fe80:0:0:0:cc95:ed45:96a5:fcc7",
                                 &messageInfo.mPeerAddr);
  // All mesh local addresses:
  // error = otIp6AddressFromString("ff02::1", &messageInfo.mPeerAddr);
  messageInfo.mPeerPort = (uint16_t)1234;
}

void udp_comm_send(otInstance *aInstance) {
  otError error = OT_ERROR_NONE;
  udp_message = otUdpNewMessage(aInstance, NULL);
  otMessageAppend(udp_message, buf, (uint16_t)strlen(buf));
  error = otUdpSend(&socket, udp_message, &messageInfo);
  if (error != OT_ERROR_NONE && udp_message != NULL) {
    otMessageFree(udp_message);
  }
}

void *udp_comm_task(void *arg0) {
  OtRtosApi_lock();
  otError error;
  error = otIp6SetEnabled(OtInstance_get(), true);    // Ifconfig up
  error = otThreadSetEnabled(OtInstance_get(), true); // Thread start
  otInstance *aInstance = OtInstance_get();
  udp_comm_init(aInstance);
  OtRtosApi_unlock();

  while (1) {
    sleep(2);
    udp_comm_send(aInstance);
  }
}

void udp_comm_taskCreate() {
  pthread_t thread;
  pthread_attr_t pAttrs;
  struct sched_param priParam;
  int retc;

  retc = pthread_attr_init(&pAttrs);
  assert(retc == 0);

  retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
  assert(retc == 0);

  priParam.sched_priority = TASK_CONFIG_UDP_COMM_TASK_PRIORITY;
  retc = pthread_attr_setschedparam(&pAttrs, &priParam);
  assert(retc == 0);

  retc = pthread_attr_setstack(&pAttrs, (void *)udp_comm_stack,
                               TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE);
  assert(retc == 0);

  retc = pthread_create(&thread, &pAttrs, udp_comm_task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;
}
