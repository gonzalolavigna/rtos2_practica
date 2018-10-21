#ifndef UART_DRIVER
#define UART_DRIVER

#include "semphr.h"

typedef struct {
   uint8_t *         pBuffer ;
   uint32_t          size   ;
   callBackFuncPtr_t callback;
} proactiveDriver_t;

circularBuffer_t           proactiveTxBuffer;
extern SemaphoreHandle_t   circBufferMutex;

void initUartDriver      ( void                                                   );
void data2UartFifo       ( uint8_t* data, uint8_t size,callBackFuncPtr_t Callback );
void uartUsbSendCallback (                                                        );

#endif
