#include "FrameworkEventos.h"
#include "sapi.h"
#include "fe_modulopulsadores.h"

extern Modulo_t * moduloBroadcast;

typedef enum estadoModuloBroadcastEnum {
	sBROADCAST_IDLE		 = 0	,
	sBROADCAST_CONTROLANDO_LEDS ,
} estadoModuloBroadcastEnum;

gpioMap_t ledsArray[] = {LEDB,LED1,LED2,LED3};
#define LED_ARRAY_LENGTH  (sizeof(ledsArray)/sizeof(ledsArray[0]))

static int 			estado = sBROADCAST_IDLE;

static void moduloBroadcastInit(Modulo_t * pmodulo);
static void ledsFsm (Evento_t * evn);

void manejadorEventosBroadcast (Evento_t * evn){
	switch(estado){
	case sBROADCAST_IDLE:{
		switch(evn->signal){
		case SIG_MODULO_INICIAR:
			moduloBroadcastInit(moduloBroadcast);
			estado=sBROADCAST_CONTROLANDO_LEDS;
			break;
		default:
			break;
		}
		break;
	}
	case sBROADCAST_CONTROLANDO_LEDS:{
		switch(evn->signal){
		case SIG_PULSADOR_APRETADO:
		case SIG_PULSADOR_LIBERADO:
			ledsFsm(evn);
			break;
		default:
			break;
		}
		break;
	}
	default:
		//Este modulo no tiene mas
		break;
	}
}

static void moduloBroadcastInit(Modulo_t * pmodulo){
	int i;
	for(i=0;i<LED_ARRAY_LENGTH;i++)
		gpioWrite(ledsArray[i],OFF);
}

static void ledsFsm (Evento_t * evn){
	if(evn->valor >= LED_ARRAY_LENGTH || evn->valor < 0)
		return;
	if(evn->signal == SIG_PULSADOR_APRETADO){
		gpioWrite(ledsArray[evn->valor],ON);
	}
	else if(evn->signal == SIG_PULSADOR_LIBERADO){
		gpioWrite(ledsArray[evn->valor],OFF);
	}
}
