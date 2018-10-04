#include <string.h>
#include <ctype.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"
#include "performance.h"
#include "uart_driver.h"

uint32_t id_de_paquete = 0;

void Performance_Task( void* nil )
{
   Line_t L;
   Line_t L_metricas;
   while(TRUE) {
      if (xQueueReceive(Performance_Queue,&L,portMAX_DELAY)== pdTRUE){
   	      L.Token->tiempo_de_inicio = now();
          To_Uppercase ( &L );
          L.Token->tiempo_de_fin = now();
          xQueueSend(Processed_Queue,&L,portMAX_DELAY);
          tiempo_de_transmision = 0;
   	      while (tiempo_de_transmision == 0)	//TODO: Usar un semaforo y mutex
   		   ;
   	      L.Token->tiempo_de_salida = tiempo_de_salida;
   	      L.Token->tiempo_de_transmision = tiempo_de_transmision;

   	      //Armo un paquete que reconoza el transmisor este va a tener la data util
   	      L_metricas.Op = OP_PERFORMANCE;
   	      L_metricas.Token = NULL;
   	      L_metricas.T = 255;
   	      Pool_Get4Line(&L_metricas);

   	      uint8_t len;
   	      ///BUG: Cuando me paso de 100000 ticks colapsa el largo es mas que 255.
    	  len = sprintf( L_metricas.Data, "\r"
    			            "ID de paquete:%d\r"
    			            "Largo de paquete:      %d\r"
    			            "Memoria alojada:       %d\r"
    			            "Tiempo de llegada:     %d\r"
    		                "Tiempo de recepcion:   %d\r"
					        "Tiempo de inicio:      %d\r"
					        "Tiempo de fin:         %d\r"
					        "Tiempo de salida:      %d\r"
					        "Tiempo de transmision: %d\r\n",
							L.Token->id_de_paquete,
							L.Token->largo_del_paquete,
							L.Token->memoria_alojada,
    			            L.Token->tiempo_de_llegada,
   				            L.Token->tiempo_de_recepcion,
   				            L.Token->tiempo_de_inicio,
   				            L.Token->tiempo_de_fin,
   				            L.Token->tiempo_de_salida,
   				            L.Token->tiempo_de_transmision
    				);
    	  L_metricas.T = len;
          xQueueSend(Processed_Queue,&L_metricas,portMAX_DELAY);
    	  Pool_Put4Token(&L);
    	  Pool_Put4Line(&L);
      }
   }
}

uint32_t now()
{
	return (uint32_t) xTaskGetTickCount();
}
