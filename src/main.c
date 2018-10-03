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
#include "uart_driver.h"
#include "performance.h"


int main(void)
{
   boardConfig (          );
   gpioWrite   ( LED3, ON );

   Init_Pool_Array   ( ); // define los arreglos de pools para luego usar
   Uart_Driver_Init  ( );
   Init_Text_Process ( ); // inicializa las colas de frtos que se usaran y alguna otra cosa
   uartInitParser    ( );
   xTaskCreate ( Upper_Task      	,"uppercasing"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( Lower_Task      	,"lowercasing"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( Transmit_Task   	,"proactiveTx"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( Performance_Task   ,"performeter"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );

   vTaskStartScheduler();
   while( TRUE )
      ;
   return 0;
}
