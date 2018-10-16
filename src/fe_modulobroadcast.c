#include "FrameworkEventos.h"
#include "sapi.h"
#include "fe_modulopulsadores.h"
#include "qmpool.h"
#include "uart_driver.h"
#include "transmission.h"

extern Modulo_t * moduloBroadcast;

typedef enum estadoModuloBroadcastEnum {
   sBROADCAST_IDLE       = 0   ,
   sBROADCAST_CONTROLANDO_LEDS ,
} estadoModuloBroadcastEnum;

gpioMap_t ledsArray[] = {LEDB,LED1,LED2,LED3};
#define LED_ARRAY_LENGTH  (sizeof(ledsArray)/sizeof(ledsArray[0]))

static int estado = sBROADCAST_IDLE;

static void moduloBroadcastInit ( Modulo_t * pmodulo );
static bool isValorValid        ( Evento_t * evn     );
static void ledOn               ( Evento_t * evn     );
static void ledOff              ( Evento_t * evn     );

void manejadorEventosBroadcast (Evento_t * evn)
{
   switch ( estado ) {
      case sBROADCAST_IDLE:
         switch(evn->signal){
            case SIG_MODULO_INICIAR:
               moduloBroadcastInit(moduloBroadcast);
               estado=sBROADCAST_CONTROLANDO_LEDS;
               break;
            default:
               break;
         }
         break;
      case sBROADCAST_CONTROLANDO_LEDS:
         if(isValorValid(evn))
            switch(evn->signal){
               case SIG_PULSADOR_APRETADO:
                  ledOn(evn);
                  break;
               case SIG_PULSADOR_LIBERADO:
                  ledOff(evn);
                  break;
               default:
                  break;
            }
      default:
         break;
   }
}

static void moduloBroadcastInit(Modulo_t * pmodulo){
   int i;
   for(i=0;i<LED_ARRAY_LENGTH;i++)
      gpioWrite(ledsArray[i],OFF);
}

static bool isValorValid(Evento_t * evn){
   return evn->valor < LED_ARRAY_LENGTH && evn->valor >= 0;
}
static void ledOn (Evento_t * evn){
   gpioWrite(ledsArray[evn->valor],ON);
}
static void ledOff (Evento_t * evn){
   gpioWrite(ledsArray[evn->valor],OFF);
}
