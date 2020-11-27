#include "heartbeat.h"

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread public API Header files */
#include <openthread/cli.h>
#include <openthread/instance.h>

/* OpenThread Internal/Example Header files */

/* Example/Board Header files */
#include "task_config.h"
#include "ti_drivers_config.h"

#include <string.h>

#include "user/heartbeat_logic/heartbeat_logic.h"

// Import I2C Driver definitions
#include <ti/drivers/I2C.h>

#include "tinyprintf.h"

#define GPIO_DEBUG 0
static struct bioData body;

static const uint32_t DELAY_AFTER_HEARTBEAT_INITIALIZE_SECONDS = 4;
static const uint32_t HEARTBEAT_TASK_DELAY_MICROSECONDS = 250000;

/* Application thread */
void *heartbeat_task(void *arg0);

/* Application thread call stack */
static char heartbeat_stack[TASK_CONFIG_HB_TASK_STACK_SIZE];

// Heartbeat Task

/**
 * Documented in task_config.h.
 */
void heartbeat_taskCreate(void) {
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

  retc = pthread_create(&thread, &pAttrs, heartbeat_task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;

  GPIO_setConfig(CONFIG_GPIO_GLED,
                 GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH | GPIO_CFG_OUT_LOW);
}

void *heartbeat_task(void *arg0) {
  GPIO_write(CONFIG_GPIO_GLED, 1);

#if GPIO_DEBUG
  uint32_t count = 0;
#endif

  int result = begin();
  if (!result) {
    printf("Sensor started!\r\n");
  } else {
    printf("Could not communicate with the sensor!!!\r\n");
  }
  printf("Configuring Sensor....\r\n");
  int error = configBpm(MODE_ONE); // Configuring just the BPM settings.
  if (!error) {
    printf("Sensor configured.\r\n");
  } else {
    printf("Error configuring sensor.\r\n");
    printf("Error: %d\r\n", error);
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up.
  sleep(DELAY_AFTER_HEARTBEAT_INITIALIZE_SECONDS);

  while (1) {
#if GPIO_DEBUG
    sleep(1);
    /* ignoring unslept return value */
    printf("LED Toggle %ld \r\n", count++);
#endif
    GPIO_toggle(CONFIG_GPIO_GLED);

    body = readBpm();
    printf("Heartrate: %d\r\n", body.heartRate);
    printf("Confidence: %d\r\n", body.confidence);
    printf("Oxygen: %d\r\n", body.oxygen);
    printf("Status: %d\r\n", body.status);
    printf("\r\n");
    usleep(HEARTBEAT_TASK_DELAY_MICROSECONDS); // 250 millseconds
  }
}
