#include "tinyprintf/tinyprintf_override.h"

#include "tinyprintf.h"

#include <openthread/platform/uart.h>
#include <stdint.h>
#include <string.h>

// TODO, Optimize this with a Linebuffer later
static uint8_t BUFFER[255];
static size_t counter = 0;

static void tinyprintf_putc(void *p, char c);
static void tinyprintf_add_to_buffer(char c);
static void tinyprintf_transmit_buffer(void);
static void tinyprintf_reset_buffer(void);

void init_tinyprintf(void) { init_printf(NULL, tinyprintf_putc); }

static void tinyprintf_putc(void *p, char c) {
  tinyprintf_add_to_buffer(c);

  if (c == '\n') {
    tinyprintf_transmit_buffer();
    tinyprintf_reset_buffer();
  }
}

static void tinyprintf_add_to_buffer(char c) {
  BUFFER[counter] = (uint8_t)c;

  // BUFFER[254] == 0
  // BUFFER[253] == (last character)
  if (counter != (sizeof(BUFFER) - 2)) {
    counter++;
  }
}

static void tinyprintf_transmit_buffer(void) {
  otPlatUartSend(BUFFER, sizeof(BUFFER) - 1);
}

static void tinyprintf_reset_buffer(void) {
  memset(BUFFER, 0, sizeof(BUFFER));
  counter = 0;
}
