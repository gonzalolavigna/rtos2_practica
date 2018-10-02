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

extern DEBUG_PRINT_ENABLE; // no encontre otra manera de poder mandar mensajes de debug desde varios
                           // archivos.  solo poniendo esto como externo y volando static en la sapi...

static  uint32_t tiempo_de_llegada;
static  uint32_t tiempo_de_recepcion;

bool_t uartInitParser (void){
   uartConfig                 ( UART_USB,115200            );
   uartRxInterruptCallbackSet ( UART_USB, parserCallback   );
   uartRxInterruptSet         ( UART_USB, TRUE             );
   debugPrintlnString         ( "PARSER:UART INICIALIZADA" );
   //TODO: Pensar en el Init que pasa si no esta habilitado el scheduler y hay
   //una IRQ. Amerita un testing? o un esquema mas inteligente?
   gpioInit  ( GPIO0,GPIO_OUTPUT ); // GLAVIGNA:Agregado para medir tiempos de atencion de la interrupcion.
   gpioWrite ( GPIO0,OFF         );
   gpioInit  ( GPIO1,GPIO_OUTPUT ); // GLAVIGNA:Agregado para medir tiempos de atencion de la interrupcion.
   gpioWrite ( GPIO1,OFF         );
   return TRUE;
}

bool Parse_Next_Byte(char B, Line_t* L)
{
   bool              Ans          = false;
   static Parser_t   Parser_State = STX_STATE;
   static uint8_t    Data_Index;

   gpioWrite(GPIO1,ON);
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
    	L->T       = B ;// TODO:Test con tamaños con numero entero de pool.Habria que verificar
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
        if(Data_Index>= L->T) {
            L->Data[Data_Index] = '\0';
            Parser_State               = ETX_STATE;
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
   gpioWrite(GPIO1,OFF);
   return Ans;
}
//--------------------------------------------------------------------------------
//funcion para imprimir la line, para debug.. despues volar
void Print_Line(Line_t* L)
{
   char S[100];
   sprintf( S, "Line: |0x%X|0x%X|%s|\r\n",
               L->Op,
               L->T,
               L->Data);
   debugPrintlnString(S);
}

//Callback para la interrupcion.
void parserCallback( void* nil )
{
   static Line_t L;
   static uint8_t byteReceived;
   static BaseType_t xHigherPriorityTaskWoken= pdFALSE;

                         // Tiempo medido con Analizador logico atencion de la
                         // interrucion de 3 us.  Para mandar a la cola le
                         // lleva 6 us.
   gpioWrite (GPIO0,ON); // GLAVIGNA: Para medir tiempo de atencion ISR, lo
                         // mido con Analizador Logico

   while(uartRxReady(UART_USB)) {
      byteReceived = uartRxRead(UART_USB);
      if(Parse_Next_Byte(byteReceived, &L)) {
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
            debugPrintlnString ( "PARSER:OPERACION NO IMPLEMENTADA\r\n" );
                        // pslavkin aca hay que devolver el pool
            Pool_Put4Line(&L);
            break;
         }
      }
   }
   gpioWrite (GPIO0,OFF); //GLAVIGNA: Para medir tiempo de atencion ISR, lo
                          //mido con Analizador Logico
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------
