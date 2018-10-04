#ifndef TRANSMISSION
#define TRANSMISSION

extern volatile uint32_t                   tiempo_de_salida;
extern volatile uint32_t                   tiempo_de_transmision;
extern          QueueHandle_t              Processed_Queue;

void            Transmit_Task             ( void* nil );
void            Transmit_Performance_Task ( void* nil );
void            completionHandler         ( void* nil );

#endif
