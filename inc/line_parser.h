#ifndef LINE_PARSER
#define LINE_PARSER

#define HEADER 3 // Cantidad de bytes antes del payload
#define TAIL   1 // Cantidad de bytes despues del payload

enum opCodes {
   OP_TO_MAY      =0,
   OP_TO_MIN      =1,
   OP_STACK       =2,
   OP_HEAP        =3,
   OP_STATUS      =4,
   OP_PERFORMANCE =5
};

enum lineHeadTrailer{
   STX_VALID = 0x55,
   ETX_VALID = 0xAA
};

typedef struct {
   uint32_t id                ;
   uint8_t* payload           ;
   uint32_t lineBeginT        ;
   uint32_t lineEndT          ;
   uint32_t proccessBeginT    ;
   uint32_t proccessEndT      ;
   uint32_t transmissionBeginT;
   uint32_t transmissionEndT  ;
   uint16_t len               ;
   uint16_t mem               ;
} token_t;

typedef struct {
   uint8_t  op;    // tipo de operacion
   uint8_t  len;   // tamanio del payload;
   char*    data;  // puntero a los datos
   token_t* token; // puntero al token
} line_t;

typedef enum {
   STX_STATE  = 0 ,
   OP_STATE       ,
   T_STATE        ,
   DATA_STATE     ,
   ETX_STATE
} parserState_t;

void uartInitParser       ( void      );
void parserCallback       ( void* nil );
void Print_Line           ( line_t* l );
bool_t uartInitLineParser ( void      );

#endif
