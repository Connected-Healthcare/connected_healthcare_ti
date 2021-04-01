#include "type_convert.h"

#include "utility/float.h"

#define DEBUG_PRINT 0
#if DEBUG_PRINT
#define debugPrintf(...) printf(__VA_ARGS__)
#else
#define debugPrintf(...)
#endif

uint32_t convert_to_uint32(uint8_t *data) {
  uint32_t rdata =
      ((data[3] << 24) | (data[2] << 16) | (data[1] << 8) | data[0]);
  ;
  // debugPrintf("%lx\r\n", rdata);
  return rdata;
}

uint16_t convert_to_uint16(uint8_t *data) {
  uint16_t rdata = ((data[1] << 8) | data[0]);
  // debugPrintf("%x\r\n", rdata);
  return rdata;
}

float convert_uint32_float_structure_to_float(uint32_t data) {
  float *rdata = (float *)&data;

#if DEBUG_PRINT
  char buffer[32] = {0};
  float__convert_float_to_string((*rdata), buffer);
  // debugPrintf("%s\r\n", buffer);
#endif

  return (*rdata);
}
