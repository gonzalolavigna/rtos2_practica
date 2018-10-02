#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "transmission.h"
#include "line_parser.h"
#include "pool_array.h"
#include "text_process.h"
#include "uart_driver.h"

QueueHandle_t Processed_Queue;   //una vez procesada la linea, viene a esta cola

void Pool_Put4Uart_Tx_t(Driver_proactivo* D)
{
   QMPool_put ( Pool4Size(D->largo ),D->pBuffer );
}

void Transmit_Task ( void* nil )
{
   Line_t L;
   uint8_t Aux_Buf[2];
   Processed_Queue = xQueueCreate ( 10,sizeof(Line_t ));

   while (TRUE) {
     xQueueReceive ( Processed_Queue, &L, portMAX_DELAY );

     Aux_Buf[0] = STX_VALID; // header
     Aux_Buf[1] = L.Op;      // operacion
     Data2Uart_Fifo ( Aux_Buf ,2   ,NULL );

     Data2Uart_Fifo ( L.Data  ,L.T ,(callBackFuncPtr_t )Pool_Put4Uart_Tx_t );

     Aux_Buf[0] = ETX_VALID; // trailer
     Data2Uart_Fifo ( Aux_Buf ,1   ,NULL );

     //debug para generar un corte de linea por cada trama
     Aux_Buf[0] = '\r';
     Aux_Buf[1] = '\n';
     Data2Uart_Fifo ( Aux_Buf ,2   ,NULL );
     uartCallbackSet ( UART_USB ,UART_TRANSMITER_FREE ,uartUsbSendCallback ,NULL );
   }
}
