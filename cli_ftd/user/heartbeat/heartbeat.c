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

#include <openthread/platform/uart.h>

#include <string.h>

#include "user/heartbeat_logic/heartbeat_logic.h"


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

#include <ti/display/Display.h>

// Define name for an index of an I2C bus
#define SENSORS 0

 // Define the slave address of device on the SENSORS bus
#define OPT_ADDR 0xAA

struct bioData body;

/* Application thread */
void *heartbeat_task(void *arg0);

/* Application thread call stack */
static char heartbeat_stack[TASK_CONFIG_HB_TASK_STACK_SIZE];

// Quick test functions

void heartbeat__initialize() {

  GPIO_setConfig(CONFIG_PIN_2, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_HIGH); // RST
  GPIO_setConfig(CONFIG_PIN_3, GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
                                    GPIO_CFG_OUT_LOW); // MFIO

  GPIO_write(CONFIG_PIN_3, 1); // MFIO - Pin 26                                 
  GPIO_write(CONFIG_PIN_2, 0); // RST - Pin 25
  // delay 10 ms
  usleep(10000);
  GPIO_write(CONFIG_PIN_2, 1); // RST
  sleep(1);
}

void heartbeat__read_sensor_version()
{
  Display_Handle hSerial = Display_open(Display_Type_UART, NULL);
  Display_printf(hSerial, 1, 0, "Hit heartbeat__read_sensor_version()");
   // One-time init of I2C driver
  // I2C_init();

  // Bytes to write 0xAA 0xFF 0x03
  uint8_t readBuffer[3]; // Should receive 0xAB 0x00 0x01 0x09 0x00 
  uint8_t writeBuffer[2] = {0xFF,0x03};
 
  memset(&readBuffer, 0 , sizeof(readBuffer));

  // initialize optional I2C bus parameters
  I2C_Params params;
  I2C_Params_init(&params);
  params.bitRate = I2C_400kHz;

  // Open I2C bus for usage
  I2C_Handle i2cHandle = I2C_open(CONFIG_I2C_0, &params);

  // Initialize slave address of transaction
  I2C_Transaction i2cTransaction = {0};
  
  i2cTransaction.slaveAddress = 0x55;
  i2cTransaction.writeBuf = writeBuffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readBuf = NULL;
  i2cTransaction.readCount = 0;

  bool status = I2C_transfer(i2cHandle, &i2cTransaction);
  if (status == false) {
    if (i2cTransaction.status == I2C_STATUS_ADDR_NACK) {
          // slave address not acknowledged
          Display_printf(hSerial, 1, 0, "Slave address t1 not acknowledged");
    }
  }
  else{
     Display_printf(hSerial, 1, 0, "Success");
  }

  // Close I2C
I2C_close(i2cHandle);
usleep(6000);
I2C_Handle i2cHandle2 = I2C_open(CONFIG_I2C_0, &params);
I2C_Transaction i2cTransaction2 = {0};
  
i2cTransaction2.slaveAddress = 0x55;
i2cTransaction2.writeBuf = NULL;
i2cTransaction2.writeCount = 0;
i2cTransaction2.readBuf = readBuffer;
i2cTransaction2.readCount = 3;

bool status2 = I2C_transfer(i2cHandle2, &i2cTransaction2);
  if (status2 == false) {
    if (i2cTransaction2.status == I2C_STATUS_ADDR_NACK) {
          // slave address not acknowledged
          Display_printf(hSerial, 1, 0, "Slave address t2 not acknowledged");
    }
  }
  else{
     Display_printf(hSerial, 1, 0, "Success");
  }
I2C_close(i2cHandle2);
for(int i=0;i<3;i++)
{
  Display_printf(hSerial, 1, 0, "%x ",readBuffer[i]);
  
}
Display_printf(hSerial, 1, 0, "\n");
Display_close(hSerial);
}

// Heartbeat Task

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
    // otPlatUartEnable();
    // heartbeat__initialize();
    // Display_printf(hSerial, 1, 0, "Hit");
    // char buffer[20] = "Hello World!";
    // uint32_t count = 0;
    int result = begin();
    if (!result){
    Display_Handle hSerial = Display_open(Display_Type_UART, NULL);

    Display_printf(hSerial, 1, 0,"Sensor started!");

    Display_close(hSerial);
    }
    else
    {
    Display_Handle hSerial = Display_open(Display_Type_UART, NULL);

      Display_printf(hSerial, 1, 0,"Could not communicate with the sensor!!!");

    Display_close(hSerial);

    }
    Display_Handle hSerial = Display_open(Display_Type_UART, NULL);
    Display_printf(hSerial, 1, 0,"Configuring Sensor....");
    Display_close(hSerial);
    int error = configBpm(MODE_ONE); // Configuring just the BPM settings. 
    if(!error){
      Display_Handle hSerial = Display_open(Display_Type_UART, NULL);
    Display_printf(hSerial, 1, 0,"Sensor configured.");
    Display_close(hSerial);
  }
  else {
     Display_Handle hSerial = Display_open(Display_Type_UART, NULL);
     Display_printf(hSerial, 1, 0,"Error configuring sensor.");
     Display_printf(hSerial, 1, 0,"Error: %d", error); 
     Display_close(hSerial);
  }

  // Data lags a bit behind the sensor, if you're finger is on the sensor when
  // it's being configured this delay will give some time for the data to catch
  // up. 
  sleep(4);
    while (1)
    {
        // sleep(5);
        /* ignoring unslept return value */
        GPIO_toggle(CONFIG_GPIO_GLED);
        // heartbeat__read_sensor_version();

        body = readBpm();
        Display_Handle hSerial = Display_open(Display_Type_UART, NULL);
        Display_printf(hSerial, 1, 0,"Heartrate: %d", body.heartRate);
        Display_printf(hSerial, 1, 0,"Confidence: %d",body.confidence);
        Display_printf(hSerial, 1, 0,"Oxygen: %d",body.oxygen);
        Display_printf(hSerial, 1, 0,"Status: %d",body.status);
        Display_close(hSerial); 
        usleep(250000); // 250 millseconds
        // Display_Handle hSerial2 = Display_open(Display_Type_UART, NULL);
        // Display_printf(hSerial2, 1, 0, "LED Toggle %d", count++);
        // Display_close(hSerial2);
        // otPlatUartSend(buffer,sizeof(buffer));
        // otCliOutputBytes(buffer, sizeof(buffer));
    }
}
