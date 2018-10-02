#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"

void uartInitParser (void){
   uartCallbackSet ( UART_USB ,UART_RECEIVE ,parserCallback ,NULL );
}

bool Parse_Next_Byte(char B, Line_t* L)
{
   bool              Ans          = false;
   static Parser_t   Parser_State = STX_STATE;
   static uint8_t    Data_Index;

   switch (Parser_State) {
      case STX_STATE:
            Ans=false;
            if(B==STX_VALID)
               Parser_State = OP_STATE;
            break;
      case OP_STATE:
            if(B>= 0 && B<= 4) { // GLAVIGNA: Se cambia para poder recibir segun protocolo de la consigna.
               L->Op        = B;
               Parser_State = T_STATE;
            }
            else
               Parser_State=STX_STATE;
            break;
      case T_STATE:
        L->T       = B;          // TODO:Test con tamaños con numero entero de pool.Habria que verificar
                                 // los tamaños pero como el tamño enviado siempre es 1 mas grande,
                                 // alcanza para poner el \0.
        Data_Index = 0;
        if(Pool_Get4Line(L))
           Parser_State=DATA_STATE;
        else
           Parser_State=STX_STATE;
        break;
      case DATA_STATE:
        L->Data[Data_Index++]=B;
        if(Data_Index>=L->T) {
           L->Data[Data_Index] = '\0';
           Parser_State        = ETX_STATE;
        }
        break;
      case ETX_STATE:
        if(B==ETX_VALID)
           Ans=true;
        else
           Pool_Put4Line(L);
        Parser_State=STX_STATE;
        break;
      default:
        Parser_State=STX_STATE;
        break;
   }
   return Ans;
}
//--------------------------------------------------------------------------------
void parserCallback( void* nil ) // Callback para la interrupcion.
{
   static Line_t L;
   static BaseType_t xHigherPriorityTaskWoken= pdFALSE;

   while(uartRxReady(UART_USB)) {
      gpioToggle   ( LEDR );
      if(Parse_Next_Byte(uartRxRead(UART_USB), &L))
         switch(L.Op) {          // Agrego un switch case por si hay mas opciones
            case OP_TO_MAY:
               xQueueSendFromISR(Upper_Queue,(void *)&L,&xHigherPriorityTaskWoken);
               break;
            case OP_TO_MIN:
               xQueueSendFromISR(Lower_Queue,(void *)&L,&xHigherPriorityTaskWoken);
               break;
            default:
               Pool_Put4Line(&L);
               break;
         }
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------
