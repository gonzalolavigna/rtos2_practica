#ifndef SYS_PERFORMANCE
#define SYS_PERFORMANCE

extern uint32_t id_de_paquete;
extern volatile uint32_t                   tiempo_de_salida;
extern volatile uint32_t                   tiempo_de_transmision;

uint32_t now();

void Performance_Task( void* nil );

#endif
