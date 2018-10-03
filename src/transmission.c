#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "transmission.h"
#include "line_parser.h"
#include "pool_array.h"
#include "text_process.h"
#include "uart_driver.h"

QueueHandle_t Processed_Queue;   //una vez procesada la linea, viene a esta cola


//TODO: borrar todos los comentarios y lo que sobra una vez que se entienda

void Transmit_Task ( void* nil )
{
   Line_t L;
   uint8_t Aux_Buf[10];    //auziliar para armar la trama, con 3 alcanza
   Processed_Queue = xQueueCreate ( 10,sizeof(Line_t ));

   while (TRUE) {
     while(xQueueReceive ( Processed_Queue, &L, portMAX_DELAY )==pdFALSE)
        ;
     //estrategia M, el header y el trailer no viajan con el payload. Se
     //generan nuevamente y de manera dinamica para enviar
     //estrategia K, podria enviar payload y header y trailer todo en la misma
     //pool y que viaje por todos los procesos asi no tengo que rearmar el
     //paquete para enviar
     //Se decide M, dado que resalta el modelo de capas, o modelo cebolla,
     //aunque para este caso en particualr sea mas ineficiente
     Aux_Buf[0] = STX_VALID; // header
     Aux_Buf[1] = L.Op;      // operacion
     Aux_Buf[2] = L.T;       // tamanio
     Dynamic_Data2Uart_Fifo ( Aux_Buf ,3 );

     Data2Uart_Fifo ( L.Data  ,L.T ,(callBackFuncPtr_t )Pool_Put4Driver_Proactivo );

     Aux_Buf[0] = ETX_VALID; // trailer
     Dynamic_Data2Uart_Fifo ( Aux_Buf ,1 );

     //DEBUG para generar un corte de linea por cada tramau
     //memcpy(Aux_Buf,"<--line\r\n",9);
     //Dynamic_Data2Uart_Fifo ( Aux_Buf ,9);
     //memcpy(Aux_Buf,"lo piso\r\n",9);     //demostracion de que aunque lo pise no pasa nada
     //Dynamic_Data2Uart_Fifo ( "y con esto puedo mandar cualquier cosa dinamica\r\n" ,49);
     //Data2Uart_Fifo ( "y estatica tambien (esto es estaico)\r\n" ,38,NULL);

   }
}
