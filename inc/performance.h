#ifndef SYS_PERFORMANCE
#define SYS_PERFORMANCE

QueueHandle_t           performanceQueue  ;
SemaphoreHandle_t       waitingEndT       ;

uint32_t now                    (           );
uint32_t now4Isr                (           );
void     performanceTask        ( void* nil );
void     initPerformance        ( void      );
void     printPerformanceReport ( line_t* l );

#endif
