#ifndef TRANSMISSION
#define TRANSMISSION

volatile uint32_t       transmissionBeginT;
volatile uint32_t       transmissionEndT;
QueueHandle_t           processedQueue;

void   initTransmit             ( void      );
void   transmitTask             ( void* nil );
void   Transmit_performanceTask ( void* nil );
void   completionHandler        ( void* nil );


void poolPut4DriverProactivo        ( proactiveDriver_t* D                                               );
void dynamicHeader2UartFifo         ( uint8_t size,uint8_t op                                            );
void dynamicTrailer2UartFifo        ( void                                                               );
void data2UartFifoPlusHeader        ( uint8_t* data, uint8_t size,uint8_t op, callBackFuncPtr_t callback );
void dynamicData2UartFifoPlusHeader ( uint8_t* data, uint8_t size,uint8_t op                             );
void dynamicData2UartFifo           ( uint8_t* data, uint8_t size                                        );


#endif
