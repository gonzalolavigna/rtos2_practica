#ifndef UART_DRIVER
#define UART_DRIVER

typedef struct Driver_proactivo_st {
   uint8_t *         pBuffer ;
   uint32_t          largo   ;
   callBackFuncPtr_t callback;
} Driver_proactivo;

circularBuffer_t cola_tx_proactivas;

void    Uart_Driver_Init    ( void );
void    uartUsbSendCallback (      );

#endif
