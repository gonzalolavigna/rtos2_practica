#ifndef TEXT_PROCESS
#define TEXT_PROCESS

extern QueueHandle_t Upper_Queue;
extern QueueHandle_t Lower_Queue;
extern QueueHandle_t Performance_Queue;

void    Init_Text_Process   ( void      );
void    Upper_Task          ( void* nil );
void    Lower_Task          ( void* nil );
Line_t* To_Uppercase        (Line_t* L);
Line_t* To_Lowercase        (Line_t* L);

#endif
