#include <string.h>
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "rx_parser.h"
#include "pool.h"
#include "process.h"

extern DEBUG_PRINT_ENABLE;

Line_t L;
Parser_t Parser_State;

void Init_Parser(void)
{
   Parser_State=STX_STATE;
}

bool Parse_Next_Byte(char B, Line_t* L)
{
   bool           Ans=false;
   static uint8_t Data_Index;

   switch (Parser_State) {
      case STX_STATE:
            Ans=false;
            if(B==STX_VALID)
               Parser_State = OP_STATE;
            break;
      case OP_STATE:
            if(B>='0' && B<='4') {
               L->Op        = B;
               Parser_State = T_STATE;
            }
            else
               Parser_State=STX_STATE;
            break;
      case T_STATE:
            if(B>='0' && B<='9') {
               L->T       = B-'0';
               Data_Index = 0    ;
               if(Pool_Get4Line(L))
                  Parser_State=DATA_STATE;
               else
                  Parser_State=STX_STATE;
            }
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
               debugPrintlnString("trama ok");
            }
            else
               Pool_Put4Line(L);
            Parser_State=STX_STATE;
            break;
      defaul:
            Parser_State=STX_STATE;
            break;
   }
   return Ans;
}


void Print_Line(Line_t* L)
{
   char S[100];
   sprintf( S, "Line: |%c|%c|%s|\r\n",
               L->Op,
               L->T,
               L->Data);
   debugPrintlnString(S);
}

void Parser_Task( void* nil )
{
   char Buff[2]="";
   Init_Parser ( );
   Init_Pools  ( );
   while(TRUE) {
      if(uartReadByte( UART_USB, Buff)) {
         debugPrintlnString(Buff);
         if(Parse_Next_Byte(Buff[0], &L)) {
            Print_Line(&L);
            xQueueSend(L.Op=='0'?Upper_Queue:Lower_Queue,&L,portMAX_DELAY);
         }
      }
      vTaskDelay         ( 100 / portTICK_RATE_MS ) ; // Envia la tarea al estado bloqueado durante 500ms
   }
}


