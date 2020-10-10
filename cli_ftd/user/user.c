#include "user.h"

#include <openthread/platform/uart.h>

void user__send_hello() {
  const uint8_t data[] = "Hello World\r\n";
  otPlatUartSend(data, (sizeof(data) / sizeof(uint8_t)) - 1);
}
