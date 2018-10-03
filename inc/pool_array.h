#ifndef POOL_ARRAY
#define POOL_ARRAY

void        Init_Pool_Array ( void                       );
QMPool*     Pool_Select     ( uint8_t Size               );
void*       Pool_Get        ( uint8_t Size               );
void        Pool_Put        ( uint8_t Size,uint8_t* Data );
void        Pool_Get4Line   ( Line_t* L                  );
void        Pool_Put4Line   ( Line_t* L                  );

#endif
