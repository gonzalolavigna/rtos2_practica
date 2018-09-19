#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "rx_parser.h"
#include "pool.h"

extern DEBUG_PRINT_ENABLE;

#define MIN_BLOCK_SIZE 16

char Mem_Pool_Sto[ 8][256];
QMPool Mem_Pool  [ 8];

void Init_Pools(void)
{
   uint8_t i;
   for(i=0;i<sizeof(Mem_Pool)/sizeof(Mem_Pool[0]);i++)
      QMPool_init(&Mem_Pool[i],
                   Mem_Pool_Sto[i],
                   sizeof(Mem_Pool_Sto[0]),
                   MIN_BLOCK_SIZE*(i+1));
}

QMPool* Pool_Select(uint8_t Size)
{
   return &Mem_Pool[Size/16];
}

bool Pool_Get4Line(Line_t* L)
{
   L->Pool=Pool_Select ( L->T      );
   L->Data=QMPool_get  ( L->Pool,0 );
   return L->Data!=NULL;
}
void Pool_Put4Line(Line_t* L)
{
   QMPool_put ( L->Pool,L->Data );
}
