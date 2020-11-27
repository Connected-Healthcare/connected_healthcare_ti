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
static const char *udp_sample_message = "Sample";
static const char *udp_ipv6_add = "ff02::1";
static const uint16_t udp_port_num = 1234;

void udp_comm_init(otInstance *aInstance) {
  otError error = OT_ERROR_NONE;
  memset(&messageInfo, 0, sizeof(messageInfo));
  error = otUdpOpen(aInstance, &socket, NULL, NULL);

  // Specific node address
  // error = otIp6AddressFromString("fe80:0:0:0:cc95:ed45:96a5:fcc7",
  //  &messageInfo.mPeerAddr);

  // Send the UDP message to all link local FTDs:
  error = otIp6AddressFromString((const void *)udp_ipv6_add,
                                 &messageInfo.mPeerAddr);
  messageInfo.mPeerPort = udp_port_num;
}

void udp_comm_send(otInstance *aInstance) {
  static otMessage *udp_message;

  otError error = OT_ERROR_NONE;

  udp_message = otUdpNewMessage(aInstance, NULL);
  otMessageAppend(udp_message, (const void *)udp_sample_message,
                  (uint16_t)strlen(udp_sample_message));
  error = otUdpSend(&socket, udp_message, &messageInfo);
  if (error != OT_ERROR_NONE && udp_message != NULL) {
    otMessageFree(udp_message);
  }
}

void *udp_comm_task(void *arg0) {
  OtRtosApi_lock();
  otError error;
  otInstance *aInstance = OtInstance_get();
  error = otIp6SetEnabled(aInstance, true); // Ifconfig up
  udp_comm_init(aInstance);
  OtRtosApi_unlock();

  while (1) {
    if (!GPIO_read(CONFIG_GPIO_BTN1)) {
      printf("BTN-1 pressed: Thread started.\r\n");
      error = otThreadSetEnabled(aInstance, true); // Thread start
    }
    udp_comm_send(aInstance);
    sleep(1);
  }
}

void udp_comm_taskCreate() {
  static char udp_comm_stack[TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE];
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
  GPIO_setConfig(CONFIG_GPIO_BTN1, GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_RISING);
}
