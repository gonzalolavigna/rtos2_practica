#ifndef TIMEOUT_H_
#define TIMEOUT_H_

//Operaciones que se pueden hacer con un timer
void timerArmarUnico      ( Modulo_t * modulo, unsigned int timeout , uint8_t enIsr);
void timerArmarRepetitivo ( Modulo_t * modulo, unsigned int timeout , uint8_t enIsr);
void timerRecargar        ( Modulo_t * modulo                       , uint8_t enIsr);
void timerDesarmar        ( Modulo_t * modulo                       , uint8_t enIsr);

#endif /* TIMEOUT_H_ */
