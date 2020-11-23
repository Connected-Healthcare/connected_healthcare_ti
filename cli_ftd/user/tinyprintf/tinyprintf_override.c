#include "tinyprintf/tinyprintf_override.h"

#include "line_buffer.h"
#include "tinyprintf.h"

#include <openthread/platform/uart.h>
#include <ti/drivers/UART.h>

/* Standard Library Header files */
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* POSIX Header files */
#include <pthread.h>
#include <sched.h>
#include <unistd.h>

// Tasks
// This 1ms task delay is kept to avoid hogging the CPU
// This allows other tasks to run
static const size_t TASK_DELAY = 1U * 1000U;

// Defined in platform/uart.c
extern bool PlatformUart_writeEnabled;
static const uint8_t UART_TASK_PRIORITY = 1;
static char uart_stack[2048];
static void *tinyprintf_uart_task(void *arg);
static void tinyprintf_taskCreate(void);

// Processing
static line_buffer_s line_buffer;
static uint8_t BUFFER[1024];
static void tinyprintf_putc(void *p, char c);

void tinyprintf_init(void) {
  line_buffer__init(&line_buffer, BUFFER, sizeof(BUFFER));
  init_printf(NULL, tinyprintf_putc);

  tinyprintf_taskCreate();
}

// TASK for UART sending
static void tinyprintf_taskCreate(void) {
  pthread_t thread;
  pthread_attr_t pAttrs;
  struct sched_param priParam;
  int retc;

  retc = pthread_attr_init(&pAttrs);
  assert(retc == 0);

  retc = pthread_attr_setdetachstate(&pAttrs, PTHREAD_CREATE_DETACHED);
  assert(retc == 0);

  priParam.sched_priority = UART_TASK_PRIORITY;
  retc = pthread_attr_setschedparam(&pAttrs, &priParam);
  assert(retc == 0);

  retc = pthread_attr_setstack(&pAttrs, (void *)uart_stack, sizeof(uart_stack));
  assert(retc == 0);

  retc = pthread_create(&thread, &pAttrs, tinyprintf_uart_task, NULL);
  assert(retc == 0);

  retc = pthread_attr_destroy(&pAttrs);
  assert(retc == 0);

  (void)retc;
}

// TODO, Improve the line_buffer__remove_line API to also give us the newline at
// the end
static void *tinyprintf_uart_task(void *arg) {
  char data[255] = {0};
  while (1) {
    if (PlatformUart_writeEnabled == false) {
      usleep(TASK_DELAY);
      continue;
    }

    bool recv = line_buffer__remove_line(&line_buffer, data, sizeof(data));
    if (!recv) {
      usleep(TASK_DELAY);
      continue;
    }

    // ! FIXME, We arent able to get a newline through the
    // `line_buffer__remove_line` api above
    sprintf(data, "%s\n", data);
    otPlatUartSend((const uint8_t *)data, sizeof(data));
    PlatformUart_writeEnabled = false;
  }

  return NULL;
}

// PROCESSING
static void tinyprintf_putc(void *p, char c) {
  line_buffer__add_byte(&line_buffer, c);
}
