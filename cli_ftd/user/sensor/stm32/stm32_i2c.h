#ifndef USER_SENSOR_STM32_STM32_I2C_H_
#define USER_SENSOR_STM32_STM32_I2C_H_

#include <inttypes.h>
#include <stdbool.h>

#include <string.h>

void stm32__init(void);

uint32_t stm32__spec_co_gas_concentration(void);
int16_t stm32__spec_co_temperature(void);
uint16_t stm32__spec_co_humidity(void);
uint16_t stm32__sgp30_co2(void);
uint16_t stm32__sgp30_voc(void);

float stm32__hts221_temperature(void);
float stm32__hts221_humidity(void);
float stm32__lps22hb_temperature(void);
float stm32__lps22hb_pressure(void);
uint32_t stm32__time_of_flight(void);

void stm32__accelerometer(int32_t arr[]);
void stm32__gyroscope(int32_t arr[]);
void stm32__magnetometer(int32_t arr[]);
void stm32__heartbeat_data(uint16_t arr[]);
void stm32__gps_data(float data_arr[]);

// Helper functions
bool stm32__write_data(uint8_t write_address, uint8_t data);
bool stm32__read_data(uint8_t write_address, uint8_t readBuf[],
                      uint32_t readCount);

uint32_t stm32__convert_to_uint32(uint8_t *data);
uint16_t stm32__convert_to_uint16(uint8_t *data);
float stm32__convert_uint32_float_structure_to_float(uint32_t data);
void convert_float_and_print(float data, const char *identifier);
void uint32_print(uint32_t data, const char *identifier);
void i32_3_print(int32_t data[], const char *identifier);

/**
 * Creation function for the stm32_i2c application task.
 */
extern void stm_i2c_comm_taskCreate(void);

#endif
