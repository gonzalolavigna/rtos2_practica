#ifndef POOL_ARRAY
#define POOL_ARRAY


void     initPoolArray       ( void                                       );
QMPool*  poolSelect          ( uint8_t size                               );
void*    poolGet             ( uint8_t size ,uint8_t inIsr                );
void     poolPut             ( uint8_t size ,uint8_t* data ,uint8_t inIsr );
uint16_t poolGetUsedMem4Line ( line_t* l                                  );
void     poolGet4Line        ( line_t* l    ,uint8_t inIsr                );
void     poolPut4Line        ( line_t* l    ,uint8_t inIsr                );
bool     poolGet4Token       ( line_t* l    ,uint8_t inIsr                );
void     poolPut4Token       ( line_t* l    ,uint8_t inIsr                );
#endif
