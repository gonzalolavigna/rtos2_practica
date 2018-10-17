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
#include "utilities.h"

#define MAX_REPORT_MENSAJES_ASINCRONICOS     255

extern Modulo_t * moduloMensajesAsincronicos;

typedef enum estadoModuloMedicionPulsadoresEnum {
   sMENSAJES_ASINCRONICOS_IDLE     = 0		,
   sMENSAJES_ASINCRONICOS_REPORTANDO	,
} estadoModuloMensajesAsincronicosnum;

static int        estado = sMENSAJES_ASINCRONICOS_IDLE;

static void printEstadoAplicacion(void);

void manejadorEventosMensajesAsincronicos (Evento_t * evn){
	   switch ( estado ) {
	      case sMENSAJES_ASINCRONICOS_IDLE:
	         switch ( evn->signal ) {
	            case SIG_MODULO_INICIAR:
	               estado=sMENSAJES_ASINCRONICOS_REPORTANDO;
	               timerArmarUnico(moduloMensajesAsincronicos,1000,ISR_OUTSIDE); //Programo mensaje del heap 1 segundo despues
	               break;
	            default:
	               break;
	         }
	         break;
	      case sMENSAJES_ASINCRONICOS_REPORTANDO:
	         switch ( evn->signal ) {
	            case SIG_TIMEOUT:
	            	printEstadoAplicacion();
	               break;
	            default:
	               break;
	         }
	         break;
	      default:
	         break;
	   }
}

static void printEstadoAplicacion(void){
	size_t minimumHeapSize;
	uint8_t auxBuf[MAX_REPORT_MENSAJES_ASINCRONICOS];
	uint8_t len;
	minimumHeapSize = xPortGetFreeHeapSize();
	len = snprintf(auxBuf,MAX_REPORT_MENSAJES_ASINCRONICOS,"MINIMO HEAP DISPONIBLE HISTORICO: %d",minimumHeapSize);
	dynamicData2UartFifoPlusHeader(auxBuf,len,OP_STATUS);

}
