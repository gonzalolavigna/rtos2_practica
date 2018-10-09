#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "uart_driver.h"
#include "transmission.h"
#include "line_parser.h"
#include "pool_array.h"
#include "text_process.h"
#include "performance.h"

#define TRANSMISION_QUEUE_SIZE 10
QueueHandle_t     processedQueue;   //una vez procesada la linea, viene a esta cola
volatile uint32_t transmissionBeginT;
volatile uint32_t transmissionEndT;

void initTransmit ( void)
{
   processedQueue = xQueueCreate ( TRANSMISION_QUEUE_SIZE,sizeof(line_t ));
}

void transmitTask ( void* nil )
{
//    callBackFuncPtr_t callback;
   line_t l;
   while (TRUE) {
     while(xQueueReceive ( processedQueue, &l, portMAX_DELAY )==pdFALSE)
        ;
   }
}

// Callback de transmision proactiva de linea con medida de performance
void completionHandler ( void * Puart_tp )
{
      xSemaphoreGiveFromISR( waitingEndT, NULL);
      transmissionEndT = now4Isr();
}
void poolPut4DriverProactivo(proactiveDriver_t* D)
{
   poolPut(D->size,D->pBuffer);
}
void dynamicHeader2UartFifo(uint8_t size,uint8_t op)
{
   uint8_t auxBuf[3];
   auxBuf[0]          = STX_VALID; // header
   auxBuf[1]          = op;        // operacion
   auxBuf[2]          = size;      // tamanio
   dynamicData2UartFifo ( auxBuf ,3 );
}
void dynamicTrailer2UartFifo(void)
{
   uint8_t auxBuf = ETX_VALID;     // trailer
   dynamicData2UartFifo ( &auxBuf ,1 );
}
void data2UartFifoPlusHeader(uint8_t* data, uint8_t size,uint8_t op, callBackFuncPtr_t callback )
{
   dynamicHeader2UartFifo  ( size ,op   );
          data2UartFifo    ( data ,size,callback);
   dynamicTrailer2UartFifo (            );
}
void dynamicData2UartFifoPlusHeader(uint8_t* data, uint8_t size,uint8_t op)
{
   dynamicHeader2UartFifo  ( size ,op   );
   dynamicData2UartFifo    ( data ,size );
   dynamicTrailer2UartFifo (            );
}
void dynamicData2UartFifo(uint8_t* data, uint8_t size)
{
   uint8_t* Buf=poolGet( size );
   memcpy   ( Buf,data,size ); // ssisi, copio pero alguien tienen que llenar
                               // el pool. en el peor caso copio 2 veces, una
                               // en una local y de la local aca, pero el
                               // codigo queda mas fresco
   data2UartFifo(Buf, size, (callBackFuncPtr_t )poolPut4DriverProactivo);
}

