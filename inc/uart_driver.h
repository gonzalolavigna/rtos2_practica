#ifndef UART_DRIVER
#define UART_DRIVER

typedef struct {
   uint8_t *         pBuffer ;
   uint32_t          size   ;
   callBackFuncPtr_t callback;
} proactiveDriver_t;

circularBuffer_t proactiveTxBuffer;


void     poolPut4DriverProactivo ( proactiveDriver_t* D                                   );
void     initUartDriver          ( void                                                   );
void     data2UartFifo           ( uint8_t* data, uint8_t size,callBackFuncPtr_t Callback );
void     uartUsbSendCallback     (                                                        );
void     dynamicData2UartFifo    ( uint8_t* data, uint8_t size                            );

#endif
