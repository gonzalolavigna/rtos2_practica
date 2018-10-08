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

QueueHandle_t     processedQueue;   //una vez procesada la linea, viene a esta cola
volatile uint32_t transmissionBeginT;
volatile uint32_t transmissionEndT;

void transmitTask ( void* nil )
{
   line_t l;
   uint8_t auxBuf[10];    //auziliar para armar la trama, con 3 alcanza
   processedQueue = xQueueCreate ( 10,sizeof(line_t ));

   while (TRUE) {
     while(xQueueReceive ( processedQueue, &l, portMAX_DELAY )==pdFALSE)
        ;
     auxBuf[0]          = STX_VALID; // header
     auxBuf[1]          = l.op;      // operacion
     auxBuf[2]          = l.len;     // tamanio
     transmissionBeginT = now();
     dynamicData2UartFifo ( auxBuf ,3 );
     if(l.token == NULL){
        data2UartFifo ( l.data ,l.len ,(callBackFuncPtr_t )poolPut4DriverProactivo );
     }
     else {
        data2UartFifo ( l.data ,l.len ,(callBackFuncPtr_t )completionHandler );
     }
     auxBuf[0] = ETX_VALID;       // trailer
     dynamicData2UartFifo ( auxBuf ,1 );
   }
}

// Callback de transmision proactiva de linea con medida de performance
void completionHandler ( void * Puart_tp )
{
       transmissionEndT = now();
}

