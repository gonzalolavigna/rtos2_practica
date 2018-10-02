#include "sapi.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "uart_driver.h"

#define CANTIDAD_ITEMS_COLA_TXPRO 8

typedef enum {
   INIT = 0,
   CONTI,
} TX_ISR_STATE;

circularBufferNew ( cola_tx_proactivas ,
                    sizeof(Driver_proactivo ),
                    CANTIDAD_ITEMS_COLA_TXPRO );

Driver_proactivo        txpro;
uint32_t                faltan_transmitir;
uint32_t                i;
TX_ISR_STATE            State       = INIT;
circularBufferStatus_t  estado_cola;

void Uart_Driver_Init (void){
   uartConfig    ( UART_USB ,115200 );
   uartInterrupt ( UART_USB ,true   );
   uartWriteByte ( UART_USB, '\0'   ); // WAF?? asi lo pide sapi..ver los ejemplos de uart con irq
   circularBufferInit ( cola_tx_proactivas, sizeof(Driver_proactivo ), CANTIDAD_ITEMS_COLA_TXPRO );
}

void Data2Uart_Fifo(uint8_t* Data, uint8_t Size,callBackFuncPtr_t Callback )
{
  Driver_proactivo uart_txpro;
  uart_txpro.pBuffer  = Data;
  uart_txpro.largo    = Size;
  uart_txpro.callback = Callback;
  circularBufferWrite ( &cola_tx_proactivas ,(uint8_t * )&uart_txpro);
}

void uartUsbSendCallback (void * nil)
{
   BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   switch(State) {
      case INIT:
         i                 = 0;
         faltan_transmitir = 0;
         estado_cola       = circularBufferRead( &cola_tx_proactivas, (uint8_t *) &txpro);
         if( estado_cola != CIRCULAR_BUFFER_EMPTY && txpro.largo>0) {
            faltan_transmitir = txpro.largo;
            State             = CONTI;
         }
         else
            uartCallbackClr( UART_USB,UART_TRANSMITER_FREE ); // no hay que hacer nada.. me avisa que ya salio el dato
         break;
      case CONTI:
            while(uartTxReady(UART_USB)) {
               gpioToggle ( LEDB );
               uartTxWrite (UART_USB, txpro.pBuffer[i++]);
               if (--faltan_transmitir==0) {
                  if(txpro.callback!=NULL)
                     txpro.callback(&txpro);          // Llamo al Callback apenas // despacho el ultimo dato
                  State=INIT;
                  break;
               }
            }
         break;
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
