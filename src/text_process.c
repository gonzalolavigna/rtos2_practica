#include <string.h>
#include <ctype.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"
#include "performance.h"

QueueHandle_t upperQueue;       //cola para mensajes que seran mayusculizados
QueueHandle_t lowerQueue;       //para los que seran pasados a minuscula
QueueHandle_t performanceQueue; //cola para mensajes a medir performance

void initTextProcess(void)
{
   upperQueue       = xQueueCreate ( 10,sizeof(line_t ));
   lowerQueue       = xQueueCreate ( 10,sizeof(line_t ));
   performanceQueue = xQueueCreate ( 10,sizeof(line_t ));
}

line_t* toUppercase(line_t* l)
{
   uint8_t i;
   for(i=0;i<l->len;i++)
      l->data[i]=toupper(l->data[i]);     //aprovechando libc
  return l;
}
line_t* toLowercase(line_t* l)
{
   uint8_t i;
   for(i=0;i<l->len;i++)
      l->data[i]=tolower(l->data[i]);     //aprovechando a libc
  return l;
}
void upperTask( void* nil )
{
   line_t l;
   while(TRUE) {
      while(xQueueReceive(upperQueue,&l,portMAX_DELAY)== pdFALSE)
         ;
      toUppercase ( &l                              );
      xQueueSend  ( processedQueue,&l,portMAX_DELAY );
   }
}
void lowerTask( void* nil )
{
   line_t l;
   while(TRUE) {
      while( xQueueReceive(lowerQueue,&l,portMAX_DELAY )== pdFALSE)
         ;
      toLowercase ( &l                              );
      xQueueSend  ( processedQueue,&l,portMAX_DELAY );
   }
}

