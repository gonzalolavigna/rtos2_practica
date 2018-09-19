#ifndef PROCESS
#define PROCESS

extern QueueHandle_t Upper_Queue;
extern QueueHandle_t Lower_Queue;

void Init_Process ( void      );
void Upper_Task   ( void* nil );
void Lower_Task   ( void* nil );

#endif
