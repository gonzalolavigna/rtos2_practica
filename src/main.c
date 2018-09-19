#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "rx_parser.h"
#include "process.h"

DEBUG_PRINT_ENABLE;

int main(void)
{
   boardConfig();
   debugPrintConfigUart ( UART_USB, 115200                   );
   debugPrintlnString   ( "TP1 - RTOS2 - Lavignia - Slavkin" );
   gpioWrite( LED3, ON );

   Init_Process();
   xTaskCreate ( Upper_Task  ,"upper"  ,configMINIMAL_STACK_SIZE*2 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( Lower_Task  ,"lower"  ,configMINIMAL_STACK_SIZE*2 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( Parser_Task ,"parser" ,configMINIMAL_STACK_SIZE*2 ,0 ,tskIDLE_PRIORITY+1 ,0   );

   vTaskStartScheduler();
   while( TRUE ) {
   }
   return 0;
}


