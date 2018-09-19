#ifndef POOL
#define POOL

void Init_Pools     ( void         );
QMPool* Pool_Select ( uint8_t Size );
bool Pool_Get4Line  ( Line_t* L    );
void Pool_Put4Line  ( Line_t* L    );

#endif
