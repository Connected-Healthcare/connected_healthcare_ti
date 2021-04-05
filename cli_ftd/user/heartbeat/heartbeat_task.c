#include "heartbeat/heartbeat_task.h"

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

/* OpenThread public API Header files */
#include <openthread/cli.h>
#include <openthread/instance.h>

/* TI Drivers / RTOS Header files */
#include <task_config.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/I2C.h>
#include <ti_drivers_config.h>

/* Utility files */
#include "third_party/tinyprintf/tinyprintf.h"

#include "heartbeat/heartbeat.h"

#define DEBUG_PRINTFS 0

#if DEBUG_PRINTFS
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

/**
 * Priority of the Application task.
 */
#ifndef TASK_CONFIG_HB_TASK_PRIORITY
#define TASK_CONFIG_HB_TASK_PRIORITY 2
#endif

/**
 * Size of the hb task call stack.
 */
#ifndef TASK_CONFIG_HB_TASK_STACK_SIZE
#define TASK_CONFIG_HB_TASK_STACK_SIZE 4096
#endif

#define GPIO_DEBUG 0

static struct bioData body;

static const uint32_t DELAY_AFTER_HEARTBEAT_INITIALIZE_SECONDS = 4;
static const uint32_t heartbeat__task_DELAY_MICROSECONDS = 250000;

/* Application thread */
void *heartbeat__task(void *arg0);

/* Getter function for Heartbeat Sensor data */
void heartbeat__get_data(char *temp_buff);

/* Application thread call stack */
static char heartbeat_stack[TASK_CONFIG_HB_TASK_STACK_SIZE];

/* Extract Heartbeat data */
void heartbeat__get_data(char *temp_buff) {
  sprintf(temp_buff, "HeartRate:%d,Oxygen:%d", body.heartRate, body.oxygen);
}

/* Task creation function for Heartbeat Sensor */
void heartbeat__taskCreate(void) {
  pthread_t thread;
  pthread_attr_t pAttrs;
  struct sched_param priParam;
  int retc;

  retc = pthread_attr_init(&pAttrs);
  assert(retc == 0);

  retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
  assert(retc == 0);

  priParam.sched_priority = TASK_CONFIG_HB_TASK_PRIORITY;
  retc = pthread_attr_setschedparam(&pAttrs, &priParam);
  assert(retc == 0);

  retc = pthread_attr_setstack(&pAttrs, (void *)heartbeat_stack,
                               TASK_CONFIG_HB_TASK_STACK_SIZE);
  assert(retc == 0);

  retc = pthread_create(&thread, &pAttrs, heartbeat__task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;

  GPIO_setConfig(CONFIG_GPIO_GLED,
                 GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW);
}

void *heartbeat__task(void *arg0) {
  GPIO_write(CONFIG_GPIO_GLED, 1);

#if 1 == GPIO_DEBUG
  uint32_t count = 0;
#endif

  int result = begin();
  if (!result) {
    debugPrintf("Sensor started!\r\n");
  } else {
    debugPrintf("Could not communicate with the sensor!!!\r\n");
  }
  debugPrintf("Configuring Sensor....\r\n");
  int error = configBpm(MODE_ONE); // Configuring just the BPM settings.
  if (!error) {
    debugPrintf("Sensor configured.\r\n");
  } else {
    debugPrintf("Error configuring sensor.\r\n");
    debugPrintf("Error: %d\r\n", error);
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up.
  sleep(DELAY_AFTER_HEARTBEAT_INITIALIZE_SECONDS);

  while (1) {
#if 1 == GPIO_DEBUG
    sleep(1);
    /* ignoring unslept return value */
    debugPrintf("LED Toggle %ld \r\n", count++);
#endif
    GPIO_toggle(CONFIG_GPIO_GLED);
    body = readBpm();

    debugPrintf("Heartrate: %d\r\n", body.heartRate);
    debugPrintf("Confidence: %d\r\n", body.confidence);
    debugPrintf("Oxygen: %d\r\n", body.oxygen);
    debugPrintf("Status: %d\r\n", body.status);
    debugPrintf("\r\n");

    usleep(heartbeat__task_DELAY_MICROSECONDS); // 250 millseconds
  }
}
