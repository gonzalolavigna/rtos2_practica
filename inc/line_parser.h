#ifndef LINE_PARSER
#define LINE_PARSER

enum Op_Codes {
   OP_TO_MAY =0,
   OP_TO_MIN =1,
   OP_STACK  =2,
   OP_HEAP   =3,
   OP_STATUS =4
};

enum Line_Head_Trailer{
   STX_VALID = 0x55,
   ETX_VALID = 0xAA
};

typedef struct Line_Struct {
//   uint8_t Stx;   //debe valer 0x55 para que se considere valida la trama
   uint8_t Op;   // 0: Convertir los datos recibidos a mayúsculas.
                 // 1: Convertir los datos recibidos a minúsculas.
                 // 2: Reportar stack disponible.
                 // 3: Reportar heap disponible.
                 // 4: Mensajes de estado de la aplicación.
   uint8_t T;    // tamanio del payload;
   char* Data;   // puntero a los datos
//   uint8_t Etx;   //0xAA para dar por valida la trama
} Line_t;

typedef enum Parser_States {
   STX_STATE  = 0 ,
   OP_STATE       ,
   T_STATE        ,
   DATA_STATE     ,
   ETX_STATE
} Parser_t;

void uartInitParser       ( void      );
void parserCallback       ( void* nil );
void Print_Line           ( Line_t* L );
bool_t uartInitLineParser ( void      );

#endif
