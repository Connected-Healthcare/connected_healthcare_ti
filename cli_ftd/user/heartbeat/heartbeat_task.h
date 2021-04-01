#ifndef USER_HEARTBEAT_TASK_H_
#define USER_HEARTBEAT_TASK_H_

/* Task creation function for Heartbeat Sensor */
void heartbeat__taskCreate(void);

/* Getter function for Heartbeat Sensor data */
void heartbeat__get_data(char *temp_buff);
#endif
