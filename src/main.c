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

DEBUG_PRINT_ENABLE;

int main(void)
{
   boardConfig();
   debugPrintConfigUart ( UART_USB, 115200                                );
   debugPrintlnString   ( "RTOS2 ***TP1*** [ Lavignia - Moya - Slavkin ]" );
   gpioWrite            ( LED3, ON                                        );

   Init_Pool_Array   ( ); // define los arreglos de pools para luego usar
   Init_Text_Process ( ); // inicializa las colas de frtos que se usaran y alguna otra cosa
   uartInitParser    ( );
   uartInitTx        ( );
   xTaskCreate ( Upper_Task                ,"uppercasing"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( Lower_Task                ,"lowercasing"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+2 ,0 );
   xTaskCreate ( Transmit_Task             ,"proactiveTx"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );
   xTaskCreate ( Performance_Task          ,"performeter"  ,configMINIMAL_STACK_SIZE*3 ,0 ,tskIDLE_PRIORITY+1 ,0 );

   debugPrintlnString   ( "StartSchedule" );
   vTaskStartScheduler();
   debugPrintlnString   ( "StartSchedule" );
   while( TRUE ) {
   }
   return 0;
}
