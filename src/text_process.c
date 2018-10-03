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


QueueHandle_t Upper_Queue;       //cola para mensajes que seran mayusculizados
QueueHandle_t Lower_Queue;       //para los que seran pasados a minuscula

QueueHandle_t Performance_Queue; //cola para mensajes a medir performance

void Init_Text_Process(void)
{
   //TODO: definir el largo apropiado.. por ahora 10
   Upper_Queue     = xQueueCreate ( 10,sizeof(Line_t ));
   Lower_Queue     = xQueueCreate ( 10,sizeof(Line_t ));
   Performance_Queue = xQueueCreate ( 10,sizeof(Line_t  ));
}

Line_t* To_Uppercase(Line_t* L)
{
   uint8_t i;
   for(i=0;i<L->T;i++)
      L->Data[i]=toupper(L->Data[i]);     //aprovechando libc
  return L;
}
Line_t* To_Lowercase(Line_t* L)
{
   uint8_t i;
   for(i=0;i<L->T;i++)
      L->Data[i]=tolower(L->Data[i]);     //aprovechando a libc
  return L;
}
void Upper_Task( void* nil )
{
   Line_t L;
   while(TRUE) {
      if (xQueueReceive(Upper_Queue,&L,portMAX_DELAY)== pdTRUE){
         To_Uppercase ( &L );
         xQueueSend(Processed_Queue,&L,portMAX_DELAY);
      }
   }
}
void Lower_Task( void* nil )
{
   Line_t L;
   while(TRUE) {
      if( xQueueReceive(Lower_Queue,&L,portMAX_DELAY )== pdTRUE){
         To_Lowercase(&L);
         xQueueSend(Processed_Queue,&L,portMAX_DELAY);
      }
   }
}

