#ifndef TEXT_PROCESS
#define TEXT_PROCESS

extern QueueHandle_t upperQueue;
extern QueueHandle_t lowerQueue;

void    initTextProcess ( void      );
void    upperTask       ( void* nil );
void    lowerTask       ( void* nil );
line_t* toUppercase     ( line_t* l );
line_t* toLowercase     ( line_t* l );

#endif
