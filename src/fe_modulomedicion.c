#include "FrameworkEventos.h"
#include "sapi.h"
#include "fe_modulopulsadores.h"
#include "fe_modulomedicion.h"

#include "task.h"
#include "qmpool.h"
#include "uart_driver.h"
#include "line_parser.h"
#include "text_process.h"
#include "pool_array.h"
#include "transmission.h"

#define MAX_REPORT_MEDICION_PULSADORES_SIZE     255

//Posibles estados del handler de eventos
typedef enum estadoModuloMedicionPulsadoresEnum {
   sMEDICION_IDLE     = 0        ,
   sMEDICION_MIDIENDO_PULSADORES ,
} estadoModuloMedicionPulsadoresEnum;

//Estados de la maquina de estado de los botones que espera los eventos de distintos botones.
typedef enum {
   ESPERANDO_PULSACION = 0,
   ESPERANDO_LIBERACION
} estadoPulsadorMedicion_t;

typedef struct {
   buttonGpioIndex_t          botonIndice     ; // es el numero de la tecla
   TickType_t                 tiempoPulsacion ;
   TickType_t                 tiempoLiberacion;
   estadoPulsadorMedicion_t   state           ; // estado segun buttonState_t
} medicionPulsador_t;

static int        estado = sMEDICION_IDLE;
//Arreglo con la informacion de los distintos pulsadores con sus respectivas mediciones.
static medicionPulsador_t  medicionPulsadores []   = {
      // inicializo en orden segun la estructura para que quepa en pantalla.
      //   .tec    .elec, .bounce      .state .ack .ackTout
      {TEC1_INDEX, 0, 0, ESPERANDO_PULSACION},
      {TEC2_INDEX, 0, 0, ESPERANDO_PULSACION},
      {TEC3_INDEX, 0, 0, ESPERANDO_PULSACION},
      {TEC4_INDEX, 0, 0, ESPERANDO_PULSACION},
};

static void       medicionPulsadorFsm   ( Evento_t * evn                             );
static uint32_t   nowMedicionPulsadores ( void                                       );
static void       printPulsadoresEvent  ( const medicionPulsador_t* medicionPulsador );

//Se entra a la FSM de los pulsadores ya sea con las signal SIG_PULSADOR_APRETADO y SIG_PULSADOR_LIBERADO.
//No se considera necesario que este manejador de eventos tenga mas estados.
void manejadorEventosMedicionPulsadores (Evento_t * evn){
   switch ( estado ) {
      case sMEDICION_IDLE:
         switch ( evn->signal ) {
            case SIG_MODULO_INICIAR:
               estado=sMEDICION_MIDIENDO_PULSADORES;
               break;
            default:
               break;
         }
         break;
      case sMEDICION_MIDIENDO_PULSADORES:
         switch ( evn->signal ) {
            case SIG_PULSADOR_APRETADO:
            case SIG_PULSADOR_LIBERADO:
               medicionPulsadorFsm(evn);
               break;
            default:
               break;
         }
         break;
      default:
         break;
   }
}

//Recorre el vector con la informacion de la medicion de losp ulsadores y se encarga de distinguir para que pulsador corresponde la medicion.
//Actualiza los valores de medicion. Siempre se imprime el reporte cuando se detecta la liberacion de un pulsador.
static void medicionPulsadorFsm (Evento_t * evn){
   int i;
   for ( i=TEC_INDEX_INICIAL;i < TEC_INDEX_FINAL;i++ ) {
      if ( i==evn->valor ) {
         switch(medicionPulsadores[i].state){
            case ESPERANDO_PULSACION:
               if(evn->signal == SIG_PULSADOR_APRETADO){
                  medicionPulsadores[i].tiempoPulsacion = nowMedicionPulsadores();
                  medicionPulsadores[i].state = ESPERANDO_LIBERACION;
               }
               break;
            case ESPERANDO_LIBERACION:
               if(evn->signal == SIG_PULSADOR_LIBERADO){
                  medicionPulsadores[i].tiempoLiberacion = nowMedicionPulsadores();
                  medicionPulsadores[i].state = ESPERANDO_PULSACION;
                  printPulsadoresEvent(&medicionPulsadores[i]);
               }
               break;
            default:
               break;
         }
      }
   }
}

uint32_t nowMedicionPulsadores  (void){
   return (uint32_t) xTaskGetTickCount();
}
//Imprime utilizando el driver de UART de las practicas anteriores el tiempo en ticks que estuvo pulsada una tecla.
static void printPulsadoresEvent(const medicionPulsador_t* medicionPulsador){
   uint8_t auxBuf[MAX_REPORT_MEDICION_PULSADORES_SIZE];
   uint8_t len;
   len = snprintf(auxBuf,MAX_REPORT_MEDICION_PULSADORES_SIZE,
            "TEC%d PULSADA DURANTE:%d TICKS",
            medicionPulsador->botonIndice+1,
            medicionPulsador->tiempoLiberacion - medicionPulsador->tiempoPulsacion);
   dynamicData2UartFifoPlusHeader ( auxBuf,len,OP_BOTONES );
}
