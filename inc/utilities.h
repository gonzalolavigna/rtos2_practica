#ifndef UTILITIES
#define UTILITIES

#include "sapi.h"

#define ISR_INSIDE   1
#define ISR_OUTSIDE  0

uint32_t seccionCriticaEntrar(uint8_t enIsr);
void seccionCriticaSalir(uint8_t enIsr, uint32_t basepri);

#endif
