#include "sensor/stm32/stm32_i2c.h"

/* Standard Library Header files */
#include <stdint.h>
#include <string.h>

/* TI Drivers / RTOS Header files */
#include "ti_drivers_config.h"
#include <ti/drivers/I2C.h>

/* Utility Functions */
#include "third_party/tinyprintf/tinyprintf.h"
#include "utility/float.h"
#include "utility/type_convert.h"
#include "utility/type_print.h"

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

/* Initialization function for I2C slave */
void stm32__init(void) {
  I2C_Params_init(&params);
  params.bitRate = I2C_100kHz;
  transaction.slaveAddress = STM32_7BIT_ADDRESS;
  is_i2c_transaction_success = true;
}

/* Getter functions for sensor data */
uint32_t stm32__spec_co_gas_concentration(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SPEC_CO_GAS_CONCENTRATION_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0;
  return convert_to_uint32(readBuf);
}

int16_t stm32__spec_co_temperature(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SPEC_CO_TEMPERATURE_ADDRESS, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  uint16_t rdata = convert_to_uint16(readBuf);
  return (int16_t)rdata;
}

uint16_t stm32__spec_co_humidity(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success = stm32__read_data(
      SPEC_CO_RELATIVE_HUMIDITY_ADDRESS, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return convert_to_uint16(readBuf);
}

uint16_t stm32__sgp30_co2(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SGP30_CARBON_DIOXIDE, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return convert_to_uint16(readBuf);
}

uint16_t stm32__sgp30_voc(void) {
  uint8_t readBuf[ARR_SIZE >> 1] = {0};
  is_i2c_transaction_success =
      stm32__read_data(SGP30_VOC, readBuf, sizeof(readBuf));
  if (true != is_i2c_transaction_success)
    return 0;
  return convert_to_uint16(readBuf);
}

float stm32__hts221_temperature(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HTS221_TEMPERATURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = convert_to_uint32(readBuf);
  return convert_uint32_float_structure_to_float(raw_data);
}

float stm32__hts221_humidity(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HTS221_HUMIDITY_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = convert_to_uint32(readBuf);
  return convert_uint32_float_structure_to_float(raw_data);
}

float stm32__lps22hb_temperature(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(LPS22HB_TEMPERATURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = convert_to_uint32(readBuf);
  return convert_uint32_float_structure_to_float(raw_data);
}

float stm32__lps22hb_pressure(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(LPS22HB_PRESSURE_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0.0;
  uint32_t raw_data = convert_to_uint32(readBuf);
  return convert_uint32_float_structure_to_float(raw_data);
}

uint32_t stm32__time_of_flight(void) {
  uint8_t readBuf[ARR_SIZE] = {0};
  is_i2c_transaction_success =
      stm32__read_data(TIME_OF_FLIGHT_ADDRESS, readBuf, ARR_SIZE);
  if (true != is_i2c_transaction_success)
    return 0;
  return convert_to_uint32(readBuf);
}

void stm32__accelerometer(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(ACCELEROMETER_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)convert_to_uint32(readBuf);
    arr[1] = (int32_t)convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)convert_to_uint32(readBuf + 8);
  }
}

void stm32__gyroscope(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(GYROSCOPE_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)convert_to_uint32(readBuf);
    arr[1] = (int32_t)convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)convert_to_uint32(readBuf + 8);
  }
}

void stm32__magnetometer(int32_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 3] = {0};
  is_i2c_transaction_success =
      stm32__read_data(MAGNETOMETER_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (int32_t)convert_to_uint32(readBuf);
    arr[1] = (int32_t)convert_to_uint32(readBuf + 4);
    arr[2] = (int32_t)convert_to_uint32(readBuf + 8);
  }
}

void stm32__heartbeat_data(uint16_t arr[]) {
  uint8_t readBuf[ARR_SIZE * 2] = {0};
  is_i2c_transaction_success =
      stm32__read_data(HEARTBEAT_ADDRESS, readBuf, sizeof(readBuf));
  if (true == is_i2c_transaction_success) {
    arr[0] = (uint16_t)convert_to_uint16(readBuf);
    arr[1] = (uint16_t)convert_to_uint16(readBuf + 2);
    arr[2] = (uint16_t)convert_to_uint16(readBuf + 4);
    arr[3] = (uint16_t)convert_to_uint16(readBuf + 6);
  }
}

void stm32__gps_data(float data_arr[]) {
  uint32_t arr[2] = {0};
  uint8_t readBuf[ARR_SIZE * 2] = {0};
  is_i2c_transaction_success =
      stm32__read_data(GPS_ADDRESS, readBuf, ARR_SIZE * 2);
  if (true == is_i2c_transaction_success) {
    arr[0] = (uint32_t)convert_to_uint32(readBuf);
    arr[1] = (uint32_t)convert_to_uint32(readBuf + 4);
    data_arr[0] = convert_uint32_float_structure_to_float(arr[0]);
    data_arr[1] = convert_uint32_float_structure_to_float(arr[1]);
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
