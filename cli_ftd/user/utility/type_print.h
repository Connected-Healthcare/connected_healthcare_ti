#ifndef USER_TYPE_PRINT_UTILITY_H_
#define USER_TYPE_PRINT_UTILITY_H_

#include "third_party/tinyprintf/tinyprintf.h"
#include <inttypes.h>

void convert_float_and_print(float data, const char *identifier);
void uint32_print(uint32_t data, const char *identifier);
void i32_3_print(int32_t data[], const char *identifier);

#endif
