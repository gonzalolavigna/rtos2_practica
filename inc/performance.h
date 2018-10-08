#ifndef SYS_PERFORMANCE
#define SYS_PERFORMANCE

extern uint32_t id;
extern volatile uint32_t                   transmissionBeginT;
extern volatile uint32_t                   transmissionEndT;

uint32_t now();

void performanceTask( void* nil );

#endif
