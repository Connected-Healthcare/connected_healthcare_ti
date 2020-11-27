#ifndef USER_HEARTBEAT_H_
#define USER_HEARTBEAT_H_

/**
 * Priority of the Application task.
 */
#ifndef TASK_CONFIG_HB_TASK_PRIORITY
#define TASK_CONFIG_HB_TASK_PRIORITY 2
#endif

/**
 * Size of the hb task call stack.
 */
#ifndef TASK_CONFIG_HB_TASK_STACK_SIZE
#define TASK_CONFIG_HB_TASK_STACK_SIZE 4096
#endif

/**
 * Creation funciton for the heartbeat application task.
 */
extern void heartbeat_taskCreate(void);

#endif
