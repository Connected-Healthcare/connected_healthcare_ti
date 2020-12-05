#ifndef USER_HEARTBEAT_H_
#define USER_HEARTBEAT_H_

/**
 * Creation funciton for the heartbeat application task.
 */
extern void heartbeat_taskCreate(void);
extern void heartbeat__get_data(char *temp_buff);
#endif
