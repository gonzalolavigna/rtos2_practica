#include <string.h>
#include <ctype.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"
#include "performance.h"
#include "uart_driver.h"

QueueHandle_t     performanceQueue; //cola para mensajes a medir performance
SemaphoreHandle_t waitingEndT;

uint32_t id = 0;

void initPerformance(void)
{
   performanceQueue = xQueueCreate           ( 10,sizeof(line_t ));
   waitingEndT      = xSemaphoreCreateBinary (                  ) ;
}

void performanceTask( void* nil )
{
   line_t l;
   line_t lPerformance;
   while(TRUE) {
      while(xQueueReceive(performanceQueue,&l,portMAX_DELAY)== pdFALSE)
         ;
      l.token->proccessBeginT = now();
      toUppercase ( &l );
      l.token->proccessEndT = now();
      transmissionEndT      = 0;
      xQueueSend(processedQueue ,&l ,portMAX_DELAY);

      while(xSemaphoreTake( waitingEndT, portMAX_DELAY)==pdFALSE)
         ;

      l.token->transmissionBeginT = transmissionBeginT;
      l.token->transmissionEndT   = transmissionEndT;

      //Armo un paquete que reconoza el transmisor este va a tener la data util
      lPerformance.op    = OP_PERFORMANCE;
      lPerformance.token = NULL;
      lPerformance.len   = 255;
      poolGet4Line(&lPerformance);

      uint8_t len;
      ///BUG: Cuando me paso de 100000 ticks colapsa el size es mas que 255.
      len = sprintf( lPerformance.data, "\r\n"
            "ID de paquete:%d\r\n"
            "Largo de paquete:      %d\r\n"
            "Memoria alojada:       %d\r\n"
            "Tiempo de llegada:     %d\r\n"
            "Tiempo de recepcion:   %d\r\n"
            "Tiempo de inicio:      %d\r\n"
            "Tiempo de fin:         %d\r\n"
            "Tiempo de salida:      %d\r\n"
            "Tiempo de transmision: %d\r\n",
            l.token->id,
            l.token->len,
            l.token->mem,
            l.token->lineBeginT,
            l.token->lineEndT,
            l.token->proccessBeginT,
            l.token->proccessEndT,
            l.token->transmissionBeginT,
            l.token->transmissionEndT
            );
      lPerformance.len = len;
      xQueueSend(processedQueue,&lPerformance,portMAX_DELAY);
      poolPut4Token ( &l );
      poolPut4Line  ( &l );
   }
}

uint32_t now()
{
   return (uint32_t) xTaskGetTickCount();
}
