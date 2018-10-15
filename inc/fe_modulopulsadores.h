#ifndef _MODULO_PULSADORES_H_
#define _MODULO_PULSADORES_H_

#include "modulos.h"

void manejadorEventosPulsadores (Evento_t * evn);

typedef enum {
	TEC1_INDEX = 0,		//esta lista tiene que coincidr con la estrcutura de  buttons y me dice en que posicion esta cada tecla dentro de la estructura.. sino tendria que buscarla una por una.. y por ahora se supone fija, asi que no hace falta..
	TEC2_INDEX,
	TEC3_INDEX,
	TEC4_INDEX,
} buttonGpioIndex_t;

#endif  /*_MODULO_PULSADORES_H_*/
