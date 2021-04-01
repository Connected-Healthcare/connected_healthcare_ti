#include "utility/type_print.h"

/* Standard Library Header files */
#include <string.h>

/* Utility Functions */
#include "utility/float.h"

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

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
