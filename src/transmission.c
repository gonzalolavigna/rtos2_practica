#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "uart_driver.h"
#include "transmission.h"
#include "line_parser.h"
#include "pool_array.h"
#include "text_process.h"
#include "performance.h"
#include "utilities.h"

uint32_t transmissionEndT;

uint32_t readTransmissionEndT(void)
{
   return transmissionEndT;
}

// Callback de transmision proactiva de linea con medida de performance
void completionHandler ( void * Puart_tp )
{
   xSemaphoreGiveFromISR( waitingEndT, NULL);
   transmissionEndT = now4Isr();
}
void poolPut4DriverProactivo(proactiveDriver_t* D)
{
   poolPut(D->size,D->pBuffer,ISR_INSIDE);
}
void dynamicHeader2UartFifo(uint8_t size,uint8_t op)
{
   uint8_t auxBuf[3];
   auxBuf[0]          = STX_VALID; // header
   auxBuf[1]          = op;        // operacion
   auxBuf[2]          = size;      // tamanio
   dynamicData2UartFifo ( auxBuf ,3 );
}
void dynamicTrailer2UartFifo(void)
{
   uint8_t auxBuf = ETX_VALID;     // trailer
   dynamicData2UartFifo ( &auxBuf ,1 );
}
void data2UartFifoPlusHeader(uint8_t* data, uint8_t size,uint8_t op, callBackFuncPtr_t callback )
{
   dynamicHeader2UartFifo  ( size ,op            );
   data2UartFifo           ( data ,size,callback );
   dynamicTrailer2UartFifo (                     );
}
void dynamicData2UartFifoPlusHeader(uint8_t* data, uint8_t size,uint8_t op)
{
   dynamicHeader2UartFifo  ( size ,op   );
   dynamicData2UartFifo    ( data ,size );
   dynamicTrailer2UartFifo (            );
}
void dynamicData2UartFifo(uint8_t* data, uint8_t size)
{
   uint8_t* Buf=poolGet( size ,ISR_OUTSIDE);
   if(Buf!=NULL) {
      memcpy ( Buf,data,size ); // ssisi, copio pero alguien tienen que
                                // llenar el pool. en el peor casos copio 2
                                // veces, una en una local dentro de la funcion
                                // que genera los datos, por ejemplo con
                                // sprintf y luego aca al pedir el pool
                                // traspaso los datos. Cada funcion podria
                                // pedir el pool y evitar este cpy, pero
                                // deberia tambien manejar el error de que no
                                // hay pool y demas, con esto el codigo de
                                // envio queda de 1 linea y el manejo de
                                // errores en un solo lugar. Estas funciones
                                // ofrecen las 2 alternativas, cada funcion
                                // decidira cual le conviene
      data2UartFifo(Buf, size, (callBackFuncPtr_t )poolPut4DriverProactivo);
   }
   //si no hay espacio para enviar, se descarta el envio
}

