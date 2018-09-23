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

extern DEBUG_PRINT_ENABLE; // no encontre otra manera de poder
                           // mandar mensajes de debug desde varios archivos.
                           // solo poniendo esto como externo y volando static
                           // en la sapi...

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
            if(B>= 0 && B<= 4) { //Cambiado GLAVIGNA para poder recibir scripts y formato ya que es en binario y no ASCII
               L->Op        = B;
               Parser_State = T_STATE;
            }
            else
               Parser_State=STX_STATE;
            break;
      case T_STATE:
    	  L->T       = B; //TODO:Test con tamaños con numero entero de pool.Habria que verificar los tamaños pero como el tamño enviado siempre es 1 mas grande, alcanza para poner el \0.
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
    		  Parser_State = ETX_STATE;
    	  }
    	  break;
      case ETX_STATE:
    	  if(B==ETX_VALID) {
    		  Ans=true;
    		  debugPrintlnString("trama ok");  //debug
    		  //aca se deberia enviar la L a la cola. Por lo pronto uso ANS
    		  //para que el que llame a esta funcion sepa si termino o no, pero
    		  //si se envia desde aca mismo, no hace falta que devuelva nada
    	  }
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
//funcion para imprimir la line, para debug.. despues volar
void Print_Line(Line_t* L)
{
   char S[100];
   sprintf( S, "Line: |%c|%c|%s|\r\n",
               L->Op,
               L->T,
               L->Data);
   debugPrintlnString(S);
}
//tarea de debug para recibir desde uart y llamar al parser.. pero se
//reemplazaria con la etapa de recepcion por irq.
void Parser_Task( void* nil )
{
   Line_t L;
   char Buff[2]="";
   while(TRUE) {
      if(uartReadByte( UART_USB, Buff)) {
         debugPrintlnString(Buff);           //eco de debug
         if(Parse_Next_Byte(Buff[0], &L)) {
            Print_Line(&L);                  //debug
            xQueueSend(L.Op=='0'?Upper_Queue:Lower_Queue,&L,portMAX_DELAY);
         }
      }
      gpioToggle ( LEDB                   ); //que parezca que estoy haciendo algo
      vTaskDelay ( 100 / portTICK_RATE_MS );
   }
}
//--------------------------------------------------------------------------------

