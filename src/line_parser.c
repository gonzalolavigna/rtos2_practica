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
#include "performance.h"

static  uint32_t tiempo_de_llegada;
static  uint32_t tiempo_de_recepcion;

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
    	    tiempo_de_llegada = now();
    	    L->Token = NULL;
            Ans=false;
            if(B==STX_VALID)
               Parser_State = OP_STATE;
            break;
      case OP_STATE:
            if(B>= 0 && B<= 5) { //GLAVIGNA: Se cambia para poder recibir segun protocolo de la consigna.
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
        Pool_Get4Line(L);
        if(L->Data!=NULL)
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
        	{
        	tiempo_de_recepcion = now();
        	Ans=true;
        	}
        else
            {
            Pool_Put4Line(L);
            Pool_Put4Token(L);
            }
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
         switch(L.Op) { // Agrego un switch case por si hay mas opciones y
                        // acciones que realizar
         case OP_TO_MAY:
            xQueueSendFromISR(Upper_Queue,(void *)&L,&xHigherPriorityTaskWoken);
            break;
         case OP_TO_MIN:
            xQueueSendFromISR(Lower_Queue,(void *)&L,&xHigherPriorityTaskWoken);
            break;
         case OP_PERFORMANCE:
        	Pool_Get4Token(&L);		// Falta control de error
        	L.Token->id_de_paquete = id_de_paquete++;
        	L.Token->payload = L.Data;
        	L.Token->tiempo_de_llegada = tiempo_de_llegada;
        	L.Token->tiempo_de_recepcion = tiempo_de_recepcion;
        	L.Token->largo_del_paquete = L.T;
        	L.Token->memoria_alojada = L.T/MIN_BLOCK_SIZE;	//TODO: No esta funcionando
            xQueueSendFromISR(Performance_Queue,(void *)&L,&xHigherPriorityTaskWoken);
            break;
         default:
            //debugPrintlnString ( "PARSER:OPERACION NO IMPLEMENTADA\r\n" );
                        // pslavkin aca hay que devolver el pool
            Pool_Put4Line(&L);
            break;
         }
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------
