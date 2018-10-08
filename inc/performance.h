#ifndef SYS_PERFORMANCE
#define SYS_PERFORMANCE

uint32_t                id                ;
volatile uint32_t       transmissionBeginT;
volatile uint32_t       transmissionEndT  ;
QueueHandle_t           performanceQueue  ;
SemaphoreHandle_t       waitingEndT;

uint32_t now             (           );
void     performanceTask ( void* nil );
void     initPerformance ( void      );

#endif
