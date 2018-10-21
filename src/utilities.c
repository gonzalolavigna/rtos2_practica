#include "FreeRTOS.h"
#include "task.h"

//workaround para resolver la entrada a seccino critica desde dentro o fuera de una ISR
uint32_t seccionCriticaEntrar(uint8_t enIsr){
   uint32_t basepri = 0;
   if ( enIsr ) {
      basepri = taskENTER_CRITICAL_FROM_ISR ( );
   }
   else {
      portENTER_CRITICAL ( );
   }
   return basepri;
}
void seccionCriticaSalir(uint8_t enIsr, uint32_t basepri){
   if(enIsr){
      taskEXIT_CRITICAL_FROM_ISR(basepri);
   }
   else{
      portEXIT_CRITICAL();
   }
}
