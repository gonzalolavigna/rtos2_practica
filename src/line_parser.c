#include <string.h>
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"
#include "sapi.h"

#include "qmpool.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "performance.h"

static  uint32_t lineBeginT;
static  uint32_t lineEndT;

void uartInitParser (void){
   uartCallbackSet ( UART_USB ,UART_RECEIVE ,parserCallback ,NULL );
}

bool parseByte(char input, line_t* l)
{
   bool                    ans         = false;
   static parserState_t    parserState = STX_STATE;
   static uint8_t          dataIndex;

   switch (parserState) {
      case STX_STATE:
         if(input==STX_VALID) {
            lineBeginT  = now();
            parserState = OP_STATE;
         }
         break;
      case OP_STATE:
         l->op       = input;
         parserState = ( input <= OP_PERFORMANCE )?T_STATE:STX_STATE;
         break;
      case T_STATE:
         l->len    = input;
         dataIndex = 0;
         poolGet4Line(l);
         parserState=l->data!=NULL?DATA_STATE:STX_STATE;
         break;
      case DATA_STATE:
        l->data[dataIndex++]=input;
        if(dataIndex>=l->len) {
           l->data[dataIndex] = '\0';
           parserState        = ETX_STATE;
        }
        break;
      case ETX_STATE:
        if(input==ETX_VALID) {
           lineEndT = now();
           ans      = true;
        }
        else {
           poolPut4Line  ( l );
        }
        parserState = STX_STATE;
        break;
      default:
        parserState = STX_STATE;
        break;
   }
   return ans;
}
//--------------------------------------------------------------------------------
void parserCallback( void* nil ) // Callback para la interrupcion.
{
   static line_t l;
   static BaseType_t xHigherPriorityTaskWoken= pdFALSE;

   while(uartRxReady(UART_USB)) {
      gpioToggle   ( LEDR );
      if ( parseByte(uartRxRead(UART_USB ), &l))
         switch(l.op) {
            case OP_TO_MAY:
               xQueueSendFromISR(upperQueue ,&l ,&xHigherPriorityTaskWoken);
               break;
            case OP_TO_MIN:
               xQueueSendFromISR(lowerQueue ,&l ,&xHigherPriorityTaskWoken);
               break;
            case OP_PERFORMANCE:
               poolGet4Token(&l);                          // Falta control de error
               l.token->id         = id++;
               l.token->payload    = l.data;
               l.token->lineBeginT = lineBeginT;
               l.token->lineEndT   = lineEndT;
               l.token->len        = l.len;
               l.token->mem        = l.len/MIN_BLOCK_SIZE; // TODO: No esta funcionando
               xQueueSendFromISR(performanceQueue ,&l ,&xHigherPriorityTaskWoken);
               break;
            default:
               poolPut4Line(&l);
               break;
         }
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------
