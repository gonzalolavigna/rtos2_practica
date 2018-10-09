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


int main(void)
{
   boardConfig (          );
   gpioWrite   ( LED3, ON );

   initPoolArray   ( ); // define los arreglos de pools para luego usar
   initUartDriver  ( );
   initTextProcess ( ); // inicializa las colas de frtos que se usaran y alguna otra cosa
   initPerformance ( ); // cola de performance
//   initTransmit    ( );
   uartInitParser  ( );
   xTaskCreate ( upperTask       ,"uppercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( lowerTask       ,"lowercasing" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
//   xTaskCreate ( transmitTask    ,"proactiveTx" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( performanceTask ,"performeter" ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );

   vTaskStartScheduler ( );
   while( TRUE )
      ;
   return 0;
}
