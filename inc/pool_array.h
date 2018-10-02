#ifndef POOL_ARRAY
#define POOL_ARRAY

#define MAX_POOL_SIZE    512     //el tamanio de cada pool (todos iguales)
#define MAX_REQUEST_SIZE 256     //lo maximo que se puede pedir
#define MIN_BLOCK_SIZE   16      //el pool mas chico de 16, el resto incrementa linealmente

void Init_Pool_Array( void         );
QMPool* Pool_Select ( uint8_t Size );
bool Pool_Get4Line  ( Line_t* L    );
void Pool_Put4Line  ( Line_t* L    );
bool Pool_Get4Token ( Line_t* L    );
void Pool_Put4Token ( Line_t* L    );
#endif
