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
#include "utilities.h"

static uint32_t lineBeginT;
static uint32_t lineEndT  ;
static uint32_t id = 0    ; //lleva cuenta del numero de trama correcta

void uartInitParser (void){
   uartCallbackSet ( UART_USB ,UART_RECEIVE ,parserCallback ,NULL );
}

//recibe byte a byte desde la uart y armar la trama descartando header y
//trailer, quedandose solo con el payload. En la respuesta se agregara
//nuevamente el header y trailer. Eso preserva una arquitectura de capas a
//costa de un minimo overhead
bool parseByte(char input, line_t* l)
{
   bool                    ans         = false;
   static parserState_t    parserState = STX_STATE;
   static uint8_t          dataIndex;

   switch (parserState) {
      case STX_STATE:
         if(input==STX_VALID) {
            lineBeginT  = now4Isr(); // se captura el tiempo de inicio de trama,
                                     // podria usarse luego
            parserState = OP_STATE;
         }
         break;
      case OP_STATE:
         l->op       = input;
         //verifica que el op sea valido, y como son correlativos se pregunta
         //si es menor que el maximo
         parserState = ( input <= OP_PERFORMANCE )?T_STATE:STX_STATE;
         break;
      case T_STATE:
         l->len    = input;
         dataIndex = 0;
         poolGet4Line(l,ISR_INSIDE);      // pide memoria dinamica verificando luego
                                          // que haya espacio, sino se descarta
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
           lineEndT = now4Isr();
           ans      = true;               // trama correcta, devuelve true.. en otro caso false
        }
        else {
           poolPut4Line ( l,ISR_INSIDE ); // trama incorrecta, devuelvo lo pedido
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
//es llamada desde la irq de tx de la uart, parsea byte a byte hasta armara una trama
//valida y luego decide
void parserCallback( void* nil )
{
   static line_t l;
   static BaseType_t xHigherPriorityTaskWoken= pdFALSE;

   while(uartRxReady(UART_USB)) {
      gpioToggle   ( LEDR );                     // ayuda a visualizar que se reviben datos
      if ( parseByte(uartRxRead(UART_USB ), &l)) // devuelve true solo ante trama valida
         switch(l.op) {
            case OP_TO_MAY:
               //Nota1: importante.. si hay que encolar la trama y la cola no tiene
               //espacio al estar en una irq hay que validar manualmente, ya
               //que no se puede esperar dentro de la irq. Si no hay espacio se
               //devuelve lo pedido al pool y la trama se descarta
               if(xQueueSendFromISR(upperQueue ,&l ,&xHigherPriorityTaskWoken)!=pdTRUE)
                  poolPut4Line(&l,ISR_INSIDE);
               break;
            case OP_TO_MIN:
               //ver Nota1
               if(xQueueSendFromISR(lowerQueue ,&l ,&xHigherPriorityTaskWoken)!=pdTRUE)
                  poolPut4Line(&l,ISR_INSIDE);
               break;
            case OP_PERFORMANCE:
               poolGet4Token(&l,ISR_INSIDE);
               if(l.token!=NULL) {
               l.token->id         = id++;
               l.token->lineBeginT = lineBeginT;
               l.token->lineEndT   = lineEndT;
               l.token->len        = l.len;
               l.token->mem        = poolGetUsedMem4Line(&l);
               //ver Nota1
               if(xQueueSendFromISR(performanceQueue ,&l ,&xHigherPriorityTaskWoken)!=pdTRUE) 
                  poolPut4Token(&l,ISR_INSIDE);
               }
               else
                  poolPut4Line(&l,ISR_INSIDE); //si no queda espacio en el pool se descarta
               break;
            default:
               poolPut4Line(&l,ISR_INSIDE);
               break;
         }
   }
   portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
//--------------------------------------------------------------------------------
