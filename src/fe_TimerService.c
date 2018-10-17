#include "FreeRTOS.h"
#include "queue.h"
#include "sapi.h"
#include "FrameworkEventos.h"
#include "utilities.h"

enum {
   TIMER_EXPIRED  = 0,
   TIMER_DISABLED = -1
};

static bool_t timerHabilitado ( Modulo_t * m );
static bool_t timerCorriendo  ( Modulo_t * m );

void     vApplicationTickHook       ( void )
{
   int nro_modulo;
   Modulo_t * m;
   portBASE_TYPE cambiarCtx = pdFALSE;

   for( nro_modulo = 0; nro_modulo < ultimoModulo; ++nro_modulo ) {
      m = &modulos[nro_modulo];
      if(timerHabilitado(m)){
         if(--m->timeout_tick == TIMER_EXPIRED) {
            cambiarCtx = EncolarEventoFromISR(m, SIG_TIMEOUT, 0);
            m->periodo != TIMER_DISABLED ? timerRecargar ( m,ISR_INSIDE ): timerDesarmar(m,ISR_INSIDE);
         }
      }
   }
   portEND_SWITCHING_ISR(cambiarCtx);
}

void timerArmarUnico       ( Modulo_t * modulo, unsigned int timeout, uint8_t enIsr )
{
   uint32_t basepri = 0;
   basepri= seccionCriticaEntrar(enIsr);
   modulo->timeout_tick = timeout;
   modulo->periodo      = TIMER_DISABLED;
   seccionCriticaSalir(enIsr,basepri);
   return;
}
void timerArmarRepetitivo  ( Modulo_t * modulo, unsigned int timeout, uint8_t enIsr )
{
   uint32_t basepri = 0;
   basepri= seccionCriticaEntrar(enIsr);
   modulo->periodo      = timeout;
   modulo->timeout_tick = modulo->periodo;
   seccionCriticaSalir(enIsr,basepri);
   return;
}
void timerRecargar         ( Modulo_t * modulo , uint8_t enIsr)
{
   uint32_t basepri = 0;
   basepri= seccionCriticaEntrar(enIsr);
   modulo->timeout_tick = modulo->periodo;
   seccionCriticaSalir(enIsr,basepri);
   return;
}
void timerDesarmar         ( Modulo_t * modulo , uint8_t enIsr)
{
   uint32_t basepri = 0;
   basepri= seccionCriticaEntrar(enIsr);
   modulo->timeout_tick = TIMER_DISABLED;
   seccionCriticaSalir(enIsr,basepri);
   return;
}
//-----------------------------------------------------------------------------
static bool_t timerHabilitado  ( Modulo_t * m )
{
   return (m->timeout_tick != TIMER_DISABLED);
}
static bool_t timerCorriendo   ( Modulo_t * m )
{
   return (m->timeout_tick != TIMER_DISABLED && m->timeout_tick != TIMER_EXPIRED);
}
