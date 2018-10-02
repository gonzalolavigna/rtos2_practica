#ifndef TRANSMISSION
#define TRANSMISSION

typedef struct Driver_proactivo_st {
   uint8_t *         pBuffer ;
   uint32_t          largo   ;
   callBackFuncPtr_t callback;
} Driver_proactivo;

void Transmit_Task ( void* nil );
void Transmit_Performance_Task ( void* nil );
void txCallback                ( void* nil );
void completionHandler   	   ( void* nil );

bool_t         uartInitTx    ( void      );
static void    uart_TX_ISR   (           );
void    UART_TX_Proact( uint8_t * pBuffer, uint32_t largo, callBackFuncPtr_t callback);

extern volatile uint32_t tiempo_de_salida;
extern volatile uint32_t tiempo_de_transmision;


#endif
