#include <string.h>
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
#include "performance.h"

QueueHandle_t     Processed_Queue;   //una vez procesada la linea, viene a esta cola
volatile uint32_t tiempo_de_salida;
volatile uint32_t tiempo_de_transmision;

void Transmit_Task ( void* nil )
{
   Line_t L;
   uint8_t Aux_Buf[10];    //auziliar para armar la trama, con 3 alcanza
   Processed_Queue = xQueueCreate ( 10,sizeof(Line_t ));

   while (TRUE) {
     while(xQueueReceive ( Processed_Queue, &L, portMAX_DELAY )==pdFALSE)
        ;
     Aux_Buf[0]       = STX_VALID; // header
     Aux_Buf[1]       = L.Op;      // operacion
     Aux_Buf[2]       = L.T;       // tamanio
     tiempo_de_salida = now();
     Dynamic_Data2Uart_Fifo ( Aux_Buf ,3 );
     if(L.Token == NULL){
        Data2Uart_Fifo ( L.Data  ,L.T ,(callBackFuncPtr_t )Pool_Put4Driver_Proactivo );
     }
     else {
        Data2Uart_Fifo ( L.Data  ,L.T ,(callBackFuncPtr_t )completionHandler );
     }
     Aux_Buf[0] = ETX_VALID;       // trailer
     Dynamic_Data2Uart_Fifo ( Aux_Buf ,1 );
   }
}

// Callback de transmision proactiva de linea con medida de performance
void completionHandler ( void * Puart_tp )
{
       tiempo_de_transmision = now();
}

