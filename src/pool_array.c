#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "pool_array.h"

#define MAX_POOL_SIZE    512     //el tamanio de cada pool (todos iguales)
#define MAX_REQUEST_SIZE 256     //lo maximo que se puede pedir
#define MIN_BLOCK_SIZE   16      //el pool mas chico de 16, el resto incrementa linealmente

char Mem_Pool_Sto[ MAX_REQUEST_SIZE/MIN_BLOCK_SIZE][MAX_POOL_SIZE];
QMPool Mem_Pool  [ MAX_REQUEST_SIZE/MIN_BLOCK_SIZE];

void Init_Pool_Array(void)
{
   uint8_t i;
   for(i=0;i<sizeof(Mem_Pool)/sizeof(Mem_Pool[0]);i++)
      QMPool_init(&Mem_Pool           [ i ],
                   Mem_Pool_Sto       [ i ],
                   sizeof(Mem_Pool_Sto[ 0 ]),
                   MIN_BLOCK_SIZE* ( i+1 ));
}
//para no hacer la asignacino usando if/else, defino una asignacino lineal dividiendo 
//e indexando un vector de pools de diferentes tamanios, pero se puede cambiar a otro modelo 
QMPool* Pool_Select(uint8_t Size)
{
   return &Mem_Pool[Size/MIN_BLOCK_SIZE];
}
void* Pool_Get(uint8_t Size)
{
  return QMPool_get ( Pool_Select ( Size ),0 );
}
void Pool_Put(uint8_t Size,uint8_t* Data)
{
   QMPool_put ( Pool_Select(Size ),Data );
}
void Pool_Get4Line(Line_t* L)
{
   L->Data=Pool_Get ( L->T );
}
void Pool_Put4Line(Line_t* L)
{
   Pool_Put ( L->T,L->Data );
}

bool Pool_Get4Token(Line_t* L)
{
   L->Token=QMPool_get  ( Pool_Select(sizeof(Token_t)),0 );
   return L->Token!=NULL;
}
void Pool_Put4Token(Line_t* L)
{
   QMPool_put ( Pool_Select(sizeof(Token_t)),L->Token );
}
