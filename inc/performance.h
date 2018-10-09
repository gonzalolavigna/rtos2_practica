#ifndef SYS_PERFORMANCE
#define SYS_PERFORMANCE

extern uint32_t         id                ;
volatile uint32_t       transmissionBeginT;
volatile uint32_t       transmissionEndT  ;
QueueHandle_t           performanceQueue  ;
SemaphoreHandle_t       waitingEndT       ;

uint32_t now                    (           );
uint32_t now4Isr                (           );
void     performanceTask        ( void* nil );
void     initPerformance        ( void      );
void     printPerformanceReport ( line_t* l );

#endif
