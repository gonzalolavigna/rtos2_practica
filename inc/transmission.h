#ifndef TRANSMISSION
#define TRANSMISSION

bool_t uartInitTx		( void );
void uart_TX_ISR		();
static void txCallback	( void* nil );
void Transmit_Task	   	( void* nil );

#endif
