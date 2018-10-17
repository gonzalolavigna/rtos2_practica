#include <string.h>
#include <ctype.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"
#include "sapi.h"

#include "qmpool.h"
#include "uart_driver.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"
#include "performance.h"

#define PERFORMANCE_QUEUE_SIZE 10
#define MAX_REPORT_SIZE        255

QueueHandle_t     performanceQueue;
SemaphoreHandle_t waitingEndT;

uint32_t id = 0;

void initPerformance(void)
{
   performanceQueue = xQueueCreate           ( PERFORMANCE_QUEUE_SIZE,sizeof(line_t ));
   waitingEndT      = xSemaphoreCreateBinary (                                      ) ;
}

void printPerformanceReport(line_t* l)
{
   uint8_t auxBuf[MAX_REPORT_SIZE];
   uint8_t len;
   len = snprintf( auxBuf,MAX_REPORT_SIZE, ""
         "ID de paquete:%d "
         "Largo de paquete:      %d "
         "Memoria alojada:       %d "
         "Tiempo de llegada:     %d "
         "Tiempo de recepcion:   %d "
         "Tiempo de inicio:      %d "
         "Tiempo de fin:         %d "
         "Tiempo de salida:      %d "
         "Tiempo de transmision: %d ",
         l->token->id,
         l->token->len,
         l->token->mem,
         l->token->lineBeginT         - l->token->lineBeginT,
         l->token->lineEndT           - l->token->lineBeginT,
         l->token->proccessBeginT     - l->token->lineBeginT,
         l->token->proccessEndT       - l->token->lineBeginT,
         l->token->transmissionBeginT - l->token->lineBeginT,
         l->token->transmissionEndT   - l->token->lineBeginT
         );
   dynamicData2UartFifoPlusHeader(auxBuf,len,l->op);
}

void performanceTask( void* nil )
{
   line_t l;
   while(TRUE) {
      while(xQueueReceive(performanceQueue,&l,portMAX_DELAY)== pdFALSE)
         ;
      l.token->proccessBeginT = now();
      toUppercase ( &l );
      l.token->proccessEndT   = now();
      transmissionEndT        = 0;
      l.token->transmissionBeginT = now();
      data2UartFifoPlusHeader(l.data,l.len,l.op,completionHandler);

      while(xSemaphoreTake( waitingEndT, portMAX_DELAY)==pdFALSE)
         ;
      l.token->transmissionEndT   = transmissionEndT;
      printPerformanceReport ( &l );
      poolPut4Token          ( &l );
   }
}

uint32_t now     ( ) { return (uint32_t) xTaskGetTickCount()       ;}
uint32_t now4Isr ( ) { return (uint32_t) xTaskGetTickCountFromISR();}
