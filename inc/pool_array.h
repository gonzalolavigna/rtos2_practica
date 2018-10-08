#ifndef POOL_ARRAY
#define POOL_ARRAY

#define MAX_POOL_SIZE    512     //el tamanio de cada pool (todos iguales)
#define MAX_REQUEST_SIZE 256     //lo maximo que se puede pedir
#define MIN_BLOCK_SIZE   16      //el pool mas chico de 16, el resto incrementa linealmente

void     initPoolArray ( void                       );
QMPool*  poolSelect    ( uint8_t size               );
void*    poolGet       ( uint8_t size               );
void     poolPut       ( uint8_t size,uint8_t* data );
void     poolGet4Line  ( line_t* l                  );
void     poolPut4Line  ( line_t* l                  );
bool     poolGet4Token ( line_t* l                  );
void     poolPut4Token ( line_t* l                  );
#endif
