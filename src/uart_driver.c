#include <string.h>
#include "sapi.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "qmpool.h"
#include "uart_driver.h"
#include "line_parser.h"
#include "pool_array.h"

#define TXPRO_ITEMS 200

typedef enum {
   INIT = 0,
   CONTI,
} TX_ISR_STATE;

circularBufferNew ( proactiveTxBuffer ,
                    sizeof(proactiveDriver_t ),
                    TXPRO_ITEMS );

proactiveDriver_t       txPro;
uint32_t                remainSize;
uint32_t                i;
TX_ISR_STATE            State       = INIT;
circularBufferStatus_t  bufferState;

void initUartDriver (void){
   uartConfig    ( UART_USB ,115200 );
   uartInterrupt ( UART_USB ,true   );
   uartWriteByte ( UART_USB, '\0'   ); // WAF?? asi lo pide sapi..ver los ejemplos de uart con irq
   circularBufferInit ( proactiveTxBuffer, sizeof(proactiveDriver_t ), TXPRO_ITEMS );
}
void data2UartFifo(uint8_t* data, uint8_t size,callBackFuncPtr_t Callback )
{
   proactiveDriver_t uart_txpro;
   uart_txpro.pBuffer  = data;
   uart_txpro.size     = size;
   uart_txpro.callback = Callback;
//TODO: y si cae uartUsbSendCallback en el medio de una circularBufferWrite y hace una circularBufferRead??? bum..
   circularBufferWrite ( &proactiveTxBuffer ,(uint8_t * )&uart_txpro);
   uartCallbackSet ( UART_USB ,UART_TRANSMITER_FREE ,uartUsbSendCallback ,NULL );
}
void uartUsbSendCallback (void * nil)
{
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   switch(State) {
      case INIT:
         i           = 0;
         remainSize  = 0;
         bufferState = circularBufferRead( &proactiveTxBuffer, (uint8_t *) &txPro);
         if( bufferState != CIRCULAR_BUFFER_EMPTY && txPro.size>0) {
            remainSize = txPro.size;
            State      = CONTI;
         }
         else
            uartCallbackClr( UART_USB,UART_TRANSMITER_FREE ); // no hay que hacer nada.. me avisa que ya salio el dato
         break;
      case CONTI:
         while(uartTxReady(UART_USB)) {
            uartTxWrite (UART_USB, txPro.pBuffer[i++]);
            if (--remainSize==0) {
               if(txPro.callback!=NULL)
                  txPro.callback(&txPro);          // Llamo al Callback apenas // despacho el ultimo dato
               State=INIT;
               break;
           }
         }
         break;
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
