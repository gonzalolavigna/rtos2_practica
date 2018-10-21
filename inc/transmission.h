#ifndef TRANSMISSION
#define TRANSMISSION

uint32_t readTransmissionEndT           ( void                                                               );
void     completionHandler              ( void* nil                                                          );
void     poolPut4DriverProactivo        ( proactiveDriver_t* D                                               );
void     dynamicHeader2UartFifo         ( uint8_t size,uint8_t op                                            );
void     dynamicTrailer2UartFifo        ( void                                                               );
void     data2UartFifoPlusHeader        ( uint8_t* data, uint8_t size,uint8_t op, callBackFuncPtr_t callback );
void     dynamicData2UartFifoPlusHeader ( uint8_t* data, uint8_t size,uint8_t op                             );
void     dynamicData2UartFifo           ( uint8_t* data, uint8_t size                                        );


#endif
