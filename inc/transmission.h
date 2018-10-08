#ifndef TRANSMISSION
#define TRANSMISSION

volatile uint32_t       transmissionBeginT;
volatile uint32_t       transmissionEndT;
QueueHandle_t           processedQueue;

void            transmitTask             ( void* nil );
void            Transmit_performanceTask ( void* nil );
void            completionHandler        ( void* nil );

#endif
