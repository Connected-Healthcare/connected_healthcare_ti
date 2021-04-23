#ifndef USER_TINYPRINTF_BLUETOOTH_WRAPPER_H_
#define USER_TINYPRINTF_BLUETOOTH_WRAPPER_H_

#include "third_party/tinyprintf/tinyprintf.h"

#define BT_SENSOR_WRITE(message) printf("sensor,%s\r\n", message)
#define BT_ALERT_WRITE(message) printf("alert,%s\r\n", message)
#define BT_DEBUG_WRITE(message) printf("debug,%s\r\n", message)

#endif
