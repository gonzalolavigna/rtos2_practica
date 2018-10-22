#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "uart_driver.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"
#include "performance.h"

#include "FrameworkEventos.h"
#include "fe_modulobroadcast.h"
#include "fe_modulopulsadores.h"
#include "fe_modulomedicion.h"
#include "fe_modulomensajes.h"

//Manejadores de Modulos
Modulo_t* moduloBroadcast;
Modulo_t* moduloPulsadores;
Modulo_t* moduloMedicionPulsadores;
Modulo_t* moduloMensajesAsincronicos;

void eventosInit (void);

int main(void)
{
   boardConfig     ( );
   initPoolArray   ( ); // define los arreglos de pools para luego usar
   initUartDriver  ( ); // inicializa la uart segun sapi
   initTextProcess ( ); // inicializa las colas de frtos que se usaran y alguna otra cosa
   initPerformance ( ); // cola de performance
   uartInitParser  ( ); // setea el callback para rx de uart
   eventosInit     ( );

   xTaskCreate ( upperTask       ,"uppercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( lowerTask       ,"lowercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( performanceTask ,"performeter" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );

   vTaskStartScheduler ( );
   while( TRUE )
      ;
   return 0;
}

void eventosInit (void){
   queEventosBaja = xQueueCreate(15, sizeof(Evento_t));
   xTaskCreate ( taskDespacharEventos ,"Despachador Eventos" ,configMINIMAL_STACK_SIZE*3 ,(void* )queEventosBaja ,tskIDLE_PRIORITY+1 ,NULL);
   //Se colocan todos los evento en la cola  de prioridad baja porque no tenemos nada que exija mas prioridad
   //Se observa el funcionamiento por testing de esta asignacion y no se observo ningun problema
   //Por deformacion queda esdte nombre pero este handler de evento maneja los leds
   moduloBroadcast          = RegistrarModulo(manejadorEventosBroadcast          ,PRIORIDAD_BAJA);
   moduloPulsadores         = RegistrarModulo(manejadorEventosPulsadores         ,PRIORIDAD_BAJA);
   moduloMedicionPulsadores = RegistrarModulo(manejadorEventosMedicionPulsadores ,PRIORIDAD_BAJA);
   //Esto surge para implimentar los mensajes del heap que salen una vez. Lo manejamos con signal
   //y eventos. Claramente esto se implemento al final. La potencia que vemos a esto a que mensajes
   //que querramos reportar una vez quedan atados a este handler.
   moduloMensajesAsincronicos= RegistrarModulo(manejadorEventosMensajesAsincronicos ,PRIORIDAD_BAJA);
   IniciarTodosLosModulos();
}
