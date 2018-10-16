#include "FreeRTOS.h"
#include "queue.h"
#include "FrameworkEventos.h"
#include "utilities.h"

#define  MAX_MODULOS    20
int      ultimoModulo = 0;
Modulo_t modulos[MAX_MODULOS];

xQueueHandle queEventosBaja, queEventosMedia, queEventosAlta;

Modulo_t * RegistrarModulo (fsm_ptr manejadorEventos, int prioridad)
{
   if(ultimoModulo >= MAX_MODULOS)
      return (Modulo_t *) NULL;

   Modulo_t * pModulo        = &modulos[ultimoModulo];
   pModulo->manejadorEventos = manejadorEventos;
   pModulo->prioridad        = prioridad       ;
   timerDesarmar( pModulo ,ISR_OUTSIDE);
   ultimoModulo++;

   return pModulo;
}

void IniciarTodosLosModulos ( void )
{
   int modulo;
   Evento_t evn;
   for (modulo = 0; modulo < ultimoModulo; ++modulo) {
      EncolarEvento( &modulos[modulo], SIG_MODULO_INICIAR, 0 );
   }
}
