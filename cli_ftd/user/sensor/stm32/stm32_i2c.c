#include "sensor/stm32/stm32_i2c.h"

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

#include "ti_drivers_config.h"
#include <ti/drivers/I2C.h>

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

#define I2C_DEBUG_PRINT 1
#if I2C_DEBUG_PRINT
#define i2c_debugPrintf(...) printf(__VA_ARGS__)
#else
#define i2c_debugPrintf(...)
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

#define ARR_SIZE 4
static const uint8_t STM32_7BIT_ADDRESS = 0xA0 >> 1;
static const uint8_t SPEC_CO_GAS_CONCENTRATION_ADDRESS = 0x00;
static const uint8_t SPEC_CO_TEMPERATURE_ADDRESS = 0x01;
static const uint8_t SPEC_CO_RELATIVE_HUMIDITY_ADDRESS = 0x02;
static const uint8_t SGP30_CARBON_DIOXIDE = 0x10;
static const uint8_t SGP30_VOC = 0x11;
static const uint8_t HTS221_TEMPERATURE_ADDRESS = 0x20;
static const uint8_t HTS221_HUMIDITY_ADDRESS = 0x21;
static const uint8_t LPS22HB_TEMPERATURE_ADDRESS = 0x22;
static const uint8_t LPS22HB_PRESSURE_ADDRESS = 0x23;
static const uint8_t TIME_OF_FLIGHT_ADDRESS = 0x24;
static const uint8_t ACCELEROMETER_ADDRESS = 0x25;
static const uint8_t GYROSCOPE_ADDRESS = 0x26;
static const uint8_t MAGNETOMETER_ADDRESS = 0x27;
static const uint8_t HEARTBEAT_ADDRESS = 0x30;
static const uint8_t GPS_ADDRESS = 0x40;

// State variables
static I2C_Params params;
static I2C_Transaction transaction;
static bool is_i2c_transaction_success;

/* Application thread */
void *stm_i2c_comm_task(void *arg0);

void stm32__init() {
  I2C_Params_init(&params);
  params.bitRate = I2C_100kHz;
  transaction.slaveAddress = STM32_7BIT_ADDRESS;
  is_i2c_transaction_success = true;
}

uint32_t stm32__spec_co_gas_concentration(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SPEC_CO_GAS_CONCENTRATION_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0;
  return stm32__convert_to_uint32(readBuf);
}

int16_t stm32__spec_co_temperature(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SPEC_CO_TEMPERATURE_ADDRESS, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  uint16_t rdata = stm32__convert_to_uint16(readBuf);
  return (int16_t)rdata;
}

uint16_t stm32__spec_co_humidity(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success = stm32__read_data(
      SPEC_CO_RELATIVE_HUMIDITY_ADDRESS, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return stm32__convert_to_uint16(readBuf);
}

uint16_t stm32__sgp30_co2(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SGP30_CARBON_DIOXIDE, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return stm32__convert_to_uint16(readBuf);
}

uint16_t stm32__sgp30_voc(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SGP30_VOC, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return stm32__convert_to_uint16(readBuf);
}

float stm32__hts221_temperature(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HTS221_TEMPERATURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = stm32__convert_to_uint32(readBuf);
  return stm32__convert_uint32_float_structure_to_float(raw_data);
}

float stm32__hts221_humidity(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HTS221_HUMIDITY_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = stm32__convert_to_uint32(readBuf);
  return stm32__convert_uint32_float_structure_to_float(raw_data);
}

float stm32__lps22hb_temperature(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(LPS22HB_TEMPERATURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = stm32__convert_to_uint32(readBuf);
  return stm32__convert_uint32_float_structure_to_float(raw_data);
}

float stm32__lps22hb_pressure(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(LPS22HB_PRESSURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = stm32__convert_to_uint32(readBuf);
  return stm32__convert_uint32_float_structure_to_float(raw_data);
}

uint32_t stm32__time_of_flight(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(TIME_OF_FLIGHT_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0;
  return stm32__convert_to_uint32(readBuf);
}

void stm32__accelerometer(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(ACCELEROMETER_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)stm32__convert_to_uint32(readBuf);
    arr[1] = (int32_t)stm32__convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)stm32__convert_to_uint32(readBuf + 8);
  }
}

void stm32__gyroscope(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(GYROSCOPE_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)stm32__convert_to_uint32(readBuf);
    arr[1] = (int32_t)stm32__convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)stm32__convert_to_uint32(readBuf + 8);
  }
}

void stm32__magnetometer(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(MAGNETOMETER_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)stm32__convert_to_uint32(readBuf);
    arr[1] = (int32_t)stm32__convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)stm32__convert_to_uint32(readBuf + 8);
  }
}

void stm32__heartbeat_data(uint16_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 2] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HEARTBEAT_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (uint16_t)stm32__convert_to_uint16(readBuf);
    arr[1] = (uint16_t)stm32__convert_to_uint16(readBuf + 2);
    arr[2] = (uint16_t)stm32__convert_to_uint16(readBuf + 4);
    arr[3] = (uint16_t)stm32__convert_to_uint16(readBuf + 6);
  }
}

void stm32__gps_data(float data_arr[]) {
  uint32_t arr[2] = {0};
  uint8_t readBuf[ARR_SIZE * 2] = {0};
  is_i2c_transaction_success =
      stm32__read_data(GPS_ADDRESS, readBuf, ARR_SIZE * 2);
  if (true == is_i2c_transaction_success) {
    arr[0] = (uint32_t)stm32__convert_to_uint32(readBuf);
    arr[1] = (uint32_t)stm32__convert_to_uint32(readBuf + 4);
    data_arr[0] = stm32__convert_uint32_float_structure_to_float(arr[0]);
    data_arr[1] = stm32__convert_uint32_float_structure_to_float(arr[1]);
  }
}

// Helper functions
bool stm32__write_data(uint8_t write_address, uint8_t data) {
  I2C_Handle handle = I2C_open(CONFIG_I2C_0, &params);
  if (!handle) {
    i2c_debugPrintf("I2C did not open\r\n");
    return false;
  }

  uint8_t writeBuf[2] = {write_address, data};

  I2C_Transaction transaction = {};
  transaction.slaveAddress = STM32_7BIT_ADDRESS;
  transaction.writeBuf = writeBuf;
  transaction.writeCount = sizeof(writeBuf);
  transaction.readBuf = NULL;
  transaction.readCount = 0;

  bool ret = I2C_transfer(handle, &transaction);
  if (!ret) {
    i2c_debugPrintf("I2C Unsuccessful transfer\r\n");
    I2C_close(handle);
    return false;
  }
  // i2c_debugPrintf("Status: %d\r\n", transaction.status);

  I2C_close(handle);
  return true;
}

bool stm32__read_data(uint8_t write_address, uint8_t readBuf[],
                      uint32_t readCount) {
  I2C_Handle handle = I2C_open(CONFIG_I2C_0, &params);
  if (!handle) {
    i2c_debugPrintf("I2C did not open\r\n");
    return false;
  }

  uint8_t writeBuf[1] = {write_address};

  I2C_Transaction transaction = {};
  transaction.slaveAddress = STM32_7BIT_ADDRESS;
  transaction.writeBuf = writeBuf;
  transaction.writeCount = sizeof(writeBuf);
  transaction.readBuf = readBuf;
  transaction.readCount = readCount;

  bool ret = I2C_transfer(handle, &transaction);
  if (!ret) {
    i2c_debugPrintf("I2C Unsuccessful transfer\r\n");
    I2C_close(handle);
    return false;
  }
  // i2c_debugPrintf("Status: %d\r\n", transaction.status);

  I2C_close(handle);
  return true;
}

uint32_t stm32__convert_to_uint32(uint8_t *data) {
  uint32_t rdata =
      ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
  ;
  // debugPrintf("%lx\r\n", rdata);
  return rdata;
}

uint16_t stm32__convert_to_uint16(uint8_t *data) {
  uint16_t rdata = ((data[1] << 8) | data[0]);
  // debugPrintf("%x\r\n", rdata);
  return rdata;
}

float stm32__convert_uint32_float_structure_to_float(uint32_t data) {
  float *rdata = (float *)&data;

#if DEBUG_PRINT
  char buffer[32] = {0};
  float__convert_float_to_string((*rdata), buffer);
  // debugPrintf("%s\r\n", buffer);
#endif

  return (*rdata);
}

void convert_float_and_print(float data, const char *identifier) {
  char buffer[12] = {0};
  float__convert_float_to_string(data, buffer);
  debugPrintf("%s: %s\r\n", identifier, buffer);
}

void uint32_print(uint32_t data, const char *identifier) {
  debugPrintf("%s: %lu\r\n", identifier, data);
}

void i32_3_print(int32_t data[], const char *identifier) {
  debugPrintf("%s: %ld %ld %ld\r\n", identifier, data[0], data[1], data[2]);
  memset(data, 0, 3 * sizeof(int32_t));
}

// STM Task-related code

/* Application thread call stack */
static char stm_i2c_comm_stack[TASK_CONFIG_STM_TASK_STACK_SIZE];

// stm_i2c_comm Task

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
