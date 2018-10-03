#ifndef TRANSMISSION
#define TRANSMISSION

extern QueueHandle_t Processed_Queue;   //una vez procesada la linea, viene a esta cola
void           Transmit_Task ( void* nil );
void Transmit_Performance_Task ( void* nil );
void completionHandler   	   ( void* nil );


extern volatile uint32_t tiempo_de_salida;
extern volatile uint32_t tiempo_de_transmision;


#endif
