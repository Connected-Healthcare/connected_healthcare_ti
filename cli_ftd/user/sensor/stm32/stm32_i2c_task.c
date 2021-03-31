#include "stm32_i2c_task.h"
#include "stm32_i2c.h"

/* Standard Library Header files */
#include <assert.h>

#include <stddef.h>
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include "ti_drivers_config.h"

/* Utility Functions */
#include "tinyprintf.h"
#include "utility/float.h"

/* RTOS Header files */
#include <ti/drivers/GPIO.h>

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

// Constants
/**
 * Priority of the Application task.
 */
#ifndef TASK_CONFIG_STM_TASK_PRIORITY
#define TASK_CONFIG_STM_TASK_PRIORITY 2
#endif

/**
 * Size of the STM task call stack.
 */
#ifndef TASK_CONFIG_STM_TASK_STACK_SIZE
#define TASK_CONFIG_STM_TASK_STACK_SIZE 4096
#endif

// Static Global Declarations
static char stm_i2c_comm_stack[TASK_CONFIG_STM_TASK_STACK_SIZE];

// Global Declarations
// I2C_Params params;

// Function Declarations
void *stm_i2c_comm_task(void *arg0);

// void get_I2C_Params(I2C_Params *params_new) { params_new = &params; }

// void stm32__init(void) {
//   I2C_Params_init(&params);
//   params.bitRate = I2C_100kHz;
//   set_I2C_Params(&params);
// }

void stm_i2c_comm_taskCreate(void) {
  pthread_t thread;
  pthread_attr_t pAttrs;
  struct sched_param priParam;
  int retc;

  retc = pthread_attr_init(&pAttrs);
  assert(retc == 0);

  retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
  assert(retc == 0);

  priParam.sched_priority = TASK_CONFIG_STM_TASK_PRIORITY;
  retc = pthread_attr_setschedparam(&pAttrs, &priParam);
  assert(retc == 0);

  retc = pthread_attr_setstack(&pAttrs, (void *)stm_i2c_comm_stack,
                               TASK_CONFIG_STM_TASK_STACK_SIZE);
  assert(retc == 0);

  retc = pthread_create(&thread, &pAttrs, stm_i2c_comm_task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;
}

void *stm_i2c_comm_task(void *arg0) {
  uint32_t spec_co_gas_concentration;
  int16_t spec_co_temperature;
  uint16_t spec_co_relative_humidity;

  uint32_t vl53L0x_time_of_flight;
  float hts221_temperature;
  float hts221_humidity;
  float lps22hb_pressure;
  float lps22hb_temperature;

  int32_t stm32_i32_data[3] = {0};
  uint16_t hb_data_arr[4] = {0};

  float gps_data[2] = {0.0};
  char gps_lat[32] = {0};
  char gps_long[32] = {0};

  while (1) {
    sleep(2);
    debugPrintf("TI-CC1352R - I2C Master\r\n");

    // Heartbeat Sensor - Maxim MAX32664/MAX30101
    stm32__heartbeat_data(hb_data_arr);
    debugPrintf("MAX32664_Heartbeat: Heart rate: %u | Confidence: %u | "
                "Oxygen: %u | Status: %u\r\n",
                hb_data_arr[0], (hb_data_arr[1] & 0xFF), hb_data_arr[2],
                (hb_data_arr[3] & 0xFF));
    memset(hb_data_arr, 0, sizeof(hb_data_arr));

    // Internal Sensors - ST B-L475E-IOT01A
    // Temperature/Humidity Sensor - ST HTS221
    hts221_temperature = stm32__hts221_temperature();
    convert_float_and_print(hts221_temperature, "HTS221_Temperature");
    hts221_humidity = stm32__hts221_humidity();
    convert_float_and_print(hts221_humidity, "HTS221_Humidity");
    // Temperature/Pressure Sensor - ST LPS22HB
    lps22hb_temperature = stm32__lps22hb_temperature();
    convert_float_and_print(lps22hb_temperature, "LPS22HB_Temperature");
    lps22hb_pressure = stm32__lps22hb_pressure();
    convert_float_and_print(lps22hb_pressure, "LPS22HB_Pressure");
    // Magnetic Sensor - ST LIS3MDL
    stm32__magnetometer(stm32_i32_data);
    i32_3_print(stm32_i32_data, "LIS3MDL_Magnetometer");
    // Accelerometer/Gyroscope Sensor - ST LSM6DSL
    stm32__accelerometer(stm32_i32_data);
    i32_3_print(stm32_i32_data, "LSM6DSL_Accelerometer");
    stm32__gyroscope(stm32_i32_data);
    i32_3_print(stm32_i32_data, "LSM6DSL_Gyroscope");
    // Time of Flight Sensor - ST Vl53L0X
    vl53L0x_time_of_flight = stm32__time_of_flight();
    uint32_print(vl53L0x_time_of_flight, "VL53L0X_Time of Flight");

    // CO Sensor - SPEC IOT_CO_1000
    spec_co_gas_concentration = stm32__spec_co_gas_concentration();
    spec_co_temperature = stm32__spec_co_temperature();
    spec_co_relative_humidity = stm32__spec_co_humidity();
    debugPrintf("IOT_CO_1000_CO: Gas Concentration(PPM): %lu | "
                "Temperature(C): %d | "
                "Relative Humidity: %u\r\n",
                spec_co_gas_concentration, spec_co_temperature,
                spec_co_relative_humidity);

    // GPS - Adafruit AP6H
    stm32__gps_data(gps_data);
    float__convert_float_to_string(gps_data[0], gps_lat);
    float__convert_float_to_string(gps_data[1], gps_long);
    debugPrintf("AP6H GPS: Lat: %s | Long: %s\r\n", gps_lat, gps_long);

    memset(gps_lat, 0, sizeof(gps_lat));
    memset(gps_long, 0, sizeof(gps_long));

    debugPrintf("------\r\n");

    GPIO_toggle(CONFIG_GPIO_GLED);
  }
}
