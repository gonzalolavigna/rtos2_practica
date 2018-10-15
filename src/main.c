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

//Manejadores de Modulos
Modulo_t * moduloBroadcast;
Modulo_t * moduloPulsadores;
Modulo_t * moduloMedicionPulsadores;

void eventosInit (void);


int main(void)
{
   boardConfig (          );

   initPoolArray   ( ); // define los arreglos de pools para luego usar
   initUartDriver  ( );
   initTextProcess ( ); // inicializa las colas de frtos que se usaran y alguna otra cosa
   initPerformance ( ); // cola de performance
//   initTransmit    ( );
   uartInitParser  ( );
   eventosInit 	   ( );

   xTaskCreate ( upperTask       ,"uppercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( lowerTask       ,"lowercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
//   xTaskCreate ( transmitTask    ,"proactiveTx" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( performanceTask ,"performeter" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );

   vTaskStartScheduler ( );
   while( TRUE )
      ;
   return 0;
}

void eventosInit (void){
	queEventosBaja = xQueueCreate(15, sizeof(Evento_t));
	xTaskCreate(
			taskDespacharEventos,					//Funcion de la tarea a ejecutar
			(const char *) "Despachador Eventos",   //Nombre de la tarea como String
			configMINIMAL_STACK_SIZE*3,				//Cantidad de stack de la tarea
			(void*) queEventosBaja,					//Parametros de la tarea
			tskIDLE_PRIORITY+1,						//Prioridad de la tarea
			NULL									//Puntero a la tarea creada en el sistema
			);
	moduloBroadcast = RegistrarModulo(manejadorEventosBroadcast,PRIORIDAD_BAJA);
	moduloPulsadores = RegistrarModulo(manejadorEventosPulsadores,PRIORIDAD_BAJA);
	moduloMedicionPulsadores = RegistrarModulo(manejadorEventosMedicionPulsadores,PRIORIDAD_BAJA);
	IniciarTodosLosModulos();
}
