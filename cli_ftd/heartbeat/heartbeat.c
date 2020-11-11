#include "heartbeat.h"

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>

/* POSIX Header files */
#include <sched.h>
#include <pthread.h>
#include <unistd.h>

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

/* OpenThread public API Header files */
#include <openthread/cli.h>
#include <openthread/instance.h>

/* OpenThread Internal/Example Header files */
#include "otsupport/otrtosapi.h"
#include "otsupport/otinstance.h"

/* Example/Board Header files */
#include "task_config.h"
#include "ti_drivers_config.h"

#if TIOP_OAD
/* OAD required Header files */
#include "oad_image_header.h"
/* Low level driverlib files (non-rtos) */
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/flash.h)
#include DeviceFamily_constructPath(driverlib/sys_ctrl.h)
#include DeviceFamily_constructPath(driverlib/cpu.h)
#endif /* TIOP_OAD */

// Import I2C Driver definitions
#include <ti/drivers/I2C.h>

// Define name for an index of an I2C bus
#define SENSORS 0

 // Define the slave address of device on the SENSORS bus
#define OPT_ADDR 0xAA

/* Application thread */
void *heartbeat_task(void *arg0);

/* Application thread call stack */
static char heartbeat_stack[TASK_CONFIG_HB_TASK_STACK_SIZE];

void heartbeat__initialize() {

  GPIO_setConfig(CONFIG_PIN_UART_RX, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_HIGH); // RST
  GPIO_setConfig(CONFIG_PIN_UART_TX, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW); // MFIO

  GPIO_write(CONFIG_PIN_UART_TX, 1); // MFIO                                  
  GPIO_write(CONFIG_PIN_UART_RX, 0); // RST
  // delay 10 ms
  usleep(10000);
  GPIO_write(CONFIG_PIN_UART_RX, 1); // RST
  sleep(1);
}

#define DEBUG 0
#if DEBUG
void heartbeat__read_sensor_version()
{
   // One-time init of I2C driver
  // I2C_init();

  // Bytes to write 0xAA 0xFF 0x03
uint8_t readBuffer[5]; // Should receive 0xAB 0x00 0x01 0x09 0x00 
  uint8_t writeBuffer[2] = {0xFF,0x03};
 

  // initialize optional I2C bus parameters
  I2C_Params params;
  I2C_Params_init(&params);
  params.bitRate = I2C_400kHz;

  // Open I2C bus for usage
  I2C_Handle i2cHandle = I2C_open(SENSORS, &params);

  // Initialize slave address of transaction
  I2C_Transaction i2cTransaction = {0};
  
  i2cTransaction.slaveAddress = 0xAA;
  i2cTransaction.writeBuf = writeBuffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  bool status = I2C_transfer(i2cHandle, &i2cTransaction);
  // if (status == false) {
  //   if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
  //         // slave address not acknowledged
  //   }
  // }

  // Close I2C
I2C_close(i2cHandle);
usleep(6000);
// I2C_Handle i2cHandle2 = I2C_open(SENSORS, &params);
// I2C_Transaction i2cTransaction2 = {0};
  
// i2cTransaction2.slaveAddress = 0xAA;
// i2cTransaction2.writeBuf = NULL;
// i2cTransaction2.writeCount = 0;
// i2cTransaction2.readBuf = readBuffer;
// i2cTransaction2.readCount = 5;

// bool status2 = I2C_transfer(i2cHandle2, &i2cTransaction2);
//   if (status2 == false) {
//     if (i2cTransaction2.status == I2C_STATUS_ADDR_NACK) {
//           // slave address not acknowledged
//     }
//   }
// I2C_close(i2cHandle2);
}

#endif

/**
 * Documented in task_config.h.
 */
void heartbeat_taskCreate(void)
{
    pthread_t           thread;
    pthread_attr_t      pAttrs;
    struct sched_param  priParam;
    int                 retc;

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

    (void) retc;

    GPIO_setConfig(CONFIG_GPIO_GLED, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW);
  
}

void *heartbeat_task(void *arg0)
{
    GPIO_write(CONFIG_GPIO_GLED, 1);
    heartbeat__initialize();
    // heartbeat__read_sensor_version();
    while (1)
    {
        sleep(2);
        /* ignoring unslept return value */
        GPIO_toggle(CONFIG_GPIO_GLED);
    }
}
