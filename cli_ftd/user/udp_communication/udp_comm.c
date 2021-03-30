/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
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

/* TI-specific files */
#include "ti_drivers_config.h"
#include <ti/drivers/GPIO.h>

/* Third party tool files */
#include "tinyprintf.h"
#include "utility/float.h"

#include "sensor/stm32/stm32_i2c.h"
#include "udp_comm.h"

/* GLOBAL MACRO DEFINITIONS */
/* UDP Task Configs */
#define TASK_CONFIG_UDP_COMM_TASK_STACK_SIZE 4096
#define MAX_UDP_MESSAGE_BUFF_LEN 1024

/* FUNCTION DECLARATIONS */
void *udp_comm_task(void *arg0);
static void get_sensors_data(char *);

/* UDP APIs */
static void udp_comm_init(otInstance *aInstance);
static void udp_comm_send(otInstance *aInstance);

/* GLOBAL VARIABLES */
/* Custom UDP Message */
static otMessageInfo messageInfo;
static otUdpSocket socket;
static const char *udp_ipv6_add = "ff02::1";
static const uint16_t udp_port_num = 1234;
char udp_final_message[MAX_UDP_MESSAGE_BUFF_LEN];

/* UDP Task Configs */
static const int TASK_CONFIG_UDP_COMM_TASK_PRIORITY = 3;

static void udp_comm_init(otInstance *aInstance) {
  otError error = OT_ERROR_NONE;
  memset(&messageInfo, 0, sizeof(messageInfo));
  error = otUdpOpen(aInstance, &socket, NULL, NULL);

  /* Specific node address */
  // error = otIp6AddressFromString("fe80:0:0:0:cc95:ed45:96a5:fcc7",
  //  &messageInfo.mPeerAddr);

  /* Send the UDP message to all link local FTDs: */
  error = otIp6AddressFromString((const void *)udp_ipv6_add,
                                 &messageInfo.mPeerAddr);
  messageInfo.mPeerPort = udp_port_num;
}

static void udp_comm_send(otInstance *aInstance) {
  static otMessage *udp_message;

  otError error = OT_ERROR_NONE;

  udp_message = otUdpNewMessage(aInstance, NULL);
  otMessageAppend(udp_message, (const void *)udp_final_message,
                  (uint16_t)strlen(udp_final_message));
  error = otUdpSend(&socket, udp_message, &messageInfo);
  if (error != OT_ERROR_NONE && udp_message != NULL) {
    otMessageFree(udp_message);
  }
}

static void get_sensors_data(char *udp_temp_msg_buff) {
  float hts221_temperature;
  float hts221_humidity;
  char hts221_temperature_str[32] = {0};
  char hts221_humidity_str[32] = {0};

  float lps22hb_pressure;
  float lps22hb_temperature;
  char lps22hb_pressure_str[32] = {0};
  char lps22hb_temperature_str[32] = {0};

  int32_t mag_data[3] = {0};
  int32_t accel_data[3] = {0};
  int32_t gyro_data[3] = {0};
  uint32_t vl53L0x_time_of_flight;

  uint16_t hb_data_arr[4] = {0};

  uint32_t spec_co_gas_concentration;
  int16_t spec_co_temperature;
  uint16_t spec_co_relative_humidity;

  float gps_data[2] = {0.0};
  char gps_lat[32] = {0};
  char gps_long[32] = {0};

  hts221_temperature = stm32__hts221_temperature();
  hts221_humidity = stm32__hts221_humidity();
  float__convert_float_to_string(hts221_temperature, hts221_temperature_str);
  float__convert_float_to_string(hts221_humidity, hts221_humidity_str);

  lps22hb_temperature = stm32__lps22hb_temperature();
  lps22hb_pressure = stm32__lps22hb_pressure();
  float__convert_float_to_string(lps22hb_temperature, lps22hb_temperature_str);
  float__convert_float_to_string(lps22hb_pressure, lps22hb_pressure_str);

  stm32__magnetometer(mag_data);
  stm32__accelerometer(accel_data);
  stm32__gyroscope(gyro_data);
  vl53L0x_time_of_flight = stm32__time_of_flight();

  stm32__heartbeat_data(hb_data_arr);

  spec_co_gas_concentration = stm32__spec_co_gas_concentration();
  spec_co_temperature = stm32__spec_co_temperature();
  spec_co_relative_humidity = stm32__spec_co_humidity();

  stm32__gps_data(gps_data);
  float__convert_float_to_string(gps_data[0], gps_lat);
  float__convert_float_to_string(gps_data[1], gps_long);

  sprintf(udp_temp_msg_buff,
          "%s,%s,%s,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%lu,%d,%d,"
          "%lu,%d,%u,%s,%s",
          hts221_temperature_str, hts221_humidity_str, lps22hb_temperature_str,
          lps22hb_pressure_str, mag_data[0], mag_data[1], mag_data[2],
          accel_data[0], accel_data[1], accel_data[2], gyro_data[0],
          gyro_data[1], gyro_data[2], vl53L0x_time_of_flight, hb_data_arr[0],
          hb_data_arr[2], spec_co_gas_concentration, spec_co_temperature,
          spec_co_relative_humidity, gps_lat, gps_long);
}

void *udp_comm_task(void *arg0) {
  char udp_temp_msg_buff[512] = {0};
  OtRtosApi_lock();
  otError error;
  otInstance *aInstance = OtInstance_get();
  error = otIp6SetEnabled(aInstance, true); // Ifconfig up
  udp_comm_init(aInstance);
  OtRtosApi_unlock();

  while (1) {
    /* For now, sending of all sensor data is done only on button press.
     * It will be removed during final stage and the data will get sent
     * automatically */
    if (!GPIO_read(CONFIG_GPIO_BTN1)) {
      OtRtosApi_lock();
      snprintf(udp_final_message, sizeof(udp_final_message), "RLOC16:%04x,",
               otThreadGetRloc16(aInstance));
      OtRtosApi_unlock();
      get_sensors_data(udp_temp_msg_buff);
      strcat(udp_final_message, udp_temp_msg_buff);
      printf("BTN-1 pressed: Sent the Msg: %s\r\n", udp_final_message);
      udp_comm_send(aInstance);
    }
    memset(udp_temp_msg_buff, 0, sizeof(udp_temp_msg_buff));
    memset(udp_final_message, 0, sizeof(udp_final_message));
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
