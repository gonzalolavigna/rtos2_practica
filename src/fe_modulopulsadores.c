#include "FrameworkEventos.h"
#include "sapi.h"

#include "qmpool.h"
#include "uart_driver.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"

extern Modulo_t * moduloPulsadores;
extern Modulo_t * moduloBroadcast;
extern Modulo_t * moduloMedicionPulsadores;

#define BUTTONS_PERIOD          50                                // los botones se samplean cada cierto tiempo regular
#define BOUNCE_TIMES            2                                 // numero de veces seguidas que se debera capturar el pulsador para darlo por valido
#define MAX_REPORT_BUTTONS_SIZE 255                               // Solo para realizar debugging.
#define BUTTONS_QTY        ( sizeof(buttons)/sizeof(buttons[0]) ) // shortcut a cant. de botones

typedef enum estadoModuloPulsadoresEnum {
   sPULSADORES_IDLE      = 0 ,
   sPULSADORES_SENSANDO      ,
} estadoModuloPulsadoresEnum;

typedef enum {
   UP,                           //boton liberado
   FALLING,                      //parece que apretaron, pruebo varias veces
   DOWN,                         //ante de cambiara a DOWN
   RISING,                       //parece que liberaron botn..pruebo algunas veces
} buttonState_t;

typedef struct {
   gpioMap_t      tec;           //es el numero de la tecla
   bool_t         elec;          //estado electrico del pin en cada sampleo
   uint8_t        bounce;        //veces que se repite el mismo estado electrico
   buttonState_t  state;         //estado segun buttonState_t
} button_t;


static bool_t     hardwarePulsadoresInicializado = FALSE;
static int        estado = sPULSADORES_IDLE;
static button_t   buttons []  = {
      // inicializo en orden segun la estructura para que quepa en pantalla.
      //   .tec    .elec, .bounce      .state .ack .ackTout
      {TEC1, HIGH, BOUNCE_TIMES, UP},
      {TEC2, HIGH, BOUNCE_TIMES, UP},
      {TEC3, HIGH, BOUNCE_TIMES, UP},
      {TEC4, HIGH, BOUNCE_TIMES, UP},
};

static void moduloPulsadoresInit ( Modulo_t * pModulo                        );
static void buttonsFsm           ( void                                      );
static void debugPrintTeclaEvent ( uint8_t teclaIndice, buttonState_t estado );

void manejadorEventosPulsadores (Evento_t * evn){
   switch ( estado ) {
      case sPULSADORES_IDLE:
         switch(evn->signal){
            case SIG_MODULO_INICIAR:
               moduloPulsadoresInit(moduloPulsadores);
               estado=sPULSADORES_SENSANDO;
               break;
            default:
               break;
         }
         break;
      case sPULSADORES_SENSANDO:
         switch(evn->signal){
            case SIG_TIMEOUT:
               buttonsFsm();
               timerArmarUnico ( moduloPulsadores,BUTTONS_PERIOD );
               break;
            default:
               break;
         }
         break;
      default:
         break;
   }
}

static void moduloPulsadoresInit (Modulo_t * pModulo){
   hardwarePulsadoresInicializado = TRUE;
   timerArmarUnico ( moduloPulsadores,BUTTONS_PERIOD );
}

static void buttonsFsm (void){
   uint8_t i;
   for( i = 0 ; i < BUTTONS_QTY  ; i ++ ) {     // recorre toda la lista de botones
      button_t* b = &buttons[ i ];              // una local para no tener que indexar cada vez y que el codigo quede mas legible
      b->elec = gpioRead( b->tec );             // leo la tecla y guardo el estado en la estructura, es el estado electrico del pin, no el esstado de boton.
      switch( b->state ) {
      case UP:
         if( !b->elec )  {                      // si pulsaron, me aseguro contando bounce
            b->state    = FALLING;              // y para eso reseteo el contador
            b->bounce      = 0;                 // de bounce
         }
         break;
      case FALLING:
         if( !b->elec ) {
            if( ++b->bounce >= BOUNCE_TIMES ) { // suficientes veces lo mismo?
               b->state    = DOWN;              // lo doy por valido y espero
               EncolarEvento(moduloBroadcast          ,SIG_PULSADOR_APRETADO ,i);
               EncolarEvento(moduloMedicionPulsadores ,SIG_PULSADOR_APRETADO ,i);
               //debugPrintTeclaEvent(i,b->state); --> Only for DEBUG
            }
         }
         else {                              //en donde falle alguno
            b->state = UP;                   //vuelvo a empezar
         }
         break;
      case DOWN:
         if( b->elec )  {                    //liberaron?
            b->state    = RISING;            //probemos un par de veces
            b->bounce   = 0;                 //reseto para contarlas
         }
         break;
      case RISING:
         if( b->elec ) {                        //sigue libeado?>
            if( ++b->bounce >= BOUNCE_TIMES ) {    //ok, sufi. doy por liberado
               b->state = UP;                //
               EncolarEvento(moduloBroadcast          ,SIG_PULSADOR_LIBERADO ,i);
               EncolarEvento(moduloMedicionPulsadores ,SIG_PULSADOR_LIBERADO ,i);
               //debugPrintTeclaEvent(i,b->state); --> ONly for DEBUG
            }
         }
         else {
            b->state = DOWN;                 //en donde uno solo falle, reseteo
         }
         break;
      default:
         b->state = UP;                      //mmm.. doy por liberado
         break;
      }
   }
}
static void debugPrintTeclaEvent(uint8_t teclaIndice, buttonState_t estado){
    uint8_t auxBuf[MAX_REPORT_BUTTONS_SIZE];
    uint8_t len;
    len = snprintf(auxBuf,MAX_REPORT_BUTTONS_SIZE,
                  "\r\n TEC%d %s\r\n"
                  ,teclaIndice+1,estado==DOWN?"PULSADA":"LIBERADA");
    dynamicData2UartFifoPlusHeader(auxBuf,len,OP_BOTONES);
}
