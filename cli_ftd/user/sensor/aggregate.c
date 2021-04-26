#include "sensor/aggregate.h"

#include <stdint.h>

#include "sensor/stm32/stm32_i2c.h"

#include "utility/float.h"

/* Utility Functions */
#include "third_party/tinyprintf/tinyprintf.h"
#include "tinyprintf/bluetooth_wrapper.h"

void get_sensors_data(char *sensor_data) {
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
  char hb_debug[32] = {0};

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
  sprintf(hb_debug, "hb_status: %d\r\n", hb_data_arr[3]);
  BT_DEBUG_WRITE(hb_debug);
  spec_co_gas_concentration = stm32__spec_co_gas_concentration();
  spec_co_temperature = stm32__spec_co_temperature();
  spec_co_relative_humidity = stm32__spec_co_humidity();

  stm32__gps_data(gps_data);
  float__convert_float_to_string(gps_data[0], gps_lat);
  float__convert_float_to_string(gps_data[1], gps_long);

  sprintf(sensor_data,
          "%s,%s,%s,%s,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%ld,%lu,%d,%d,"
          "%lu,%d,%u,%s,%s",
          hts221_temperature_str, hts221_humidity_str, lps22hb_temperature_str,
          lps22hb_pressure_str, mag_data[0], mag_data[1], mag_data[2],
          accel_data[0], accel_data[1], accel_data[2], gyro_data[0],
          gyro_data[1], gyro_data[2], vl53L0x_time_of_flight, hb_data_arr[0],
          hb_data_arr[2], spec_co_gas_concentration, spec_co_temperature,
          spec_co_relative_humidity, gps_lat, gps_long);
}
