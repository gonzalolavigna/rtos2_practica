#ifndef TRANSMISSION
#define TRANSMISSION

extern QueueHandle_t Processed_Queue;   //una vez procesada la linea, viene a esta cola
void           Transmit_Task ( void* nil );

#endif
