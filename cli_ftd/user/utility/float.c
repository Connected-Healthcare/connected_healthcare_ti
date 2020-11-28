#include "utility/float.h"

#include <math.h>

#include "tinyprintf.h"

// https://www.microchip.com/forums/m293227.aspx
// Only prints upto 2 decimal places
// NOTE, Decimal values can be slightly inaccurate at times
void float__convert_float_to_string(float data, char *buffer) {
  sprintf(buffer, "%d.%02u", (int)data, (int)fabs(((data - (int)data) * 100)));
}

void float__print_float(float data) {
  char buffer[32] = {0};
  float__convert_float_to_string(data, buffer);
  printf("%s\r\n", buffer);
}
