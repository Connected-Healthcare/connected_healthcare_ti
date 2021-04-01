#ifndef USER_TYPE_CONVERT_UTILITY_H_
#define USER_TYPE_CONVERT_UTILITY_H_

#include <inttypes.h>

uint32_t convert_to_uint32(uint8_t *data);
uint16_t convert_to_uint16(uint8_t *data);
float convert_uint32_float_structure_to_float(uint32_t data);

#endif
