#include "type_print.h"
#include "utility/float.h"

#include <string.h>

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

/* Utility to print the given Identifier along with its data.
 * For example: GPS
 */

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
