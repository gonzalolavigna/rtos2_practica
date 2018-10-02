#ifndef LINE_PARSER
#define LINE_PARSER

#define HEADER	3	//Cantidad de bytes antes del payload
#define TAIL    1   //Cantidad de bytes despues del payload

enum Op_Codes {
   OP_TO_MAY      =0,
   OP_TO_MIN      =1,
   OP_STACK       =2,
   OP_HEAP        =3,
   OP_STATUS      =4,
   OP_PERFORMANCE =5
};

enum Line_Head_Trailer{
   STX_VALID = 0x55, //Cambiado GLAVIGNA para recibir los comienzos y fin de paquetes especificados en el protocolo
   ETX_VALID = 0xAA
};

typedef struct Token_Struct {
	uint32_t id_de_paquete;
	uint8_t * payload;
	uint32_t tiempo_de_llegada;
	uint32_t tiempo_de_recepcion;
	uint32_t tiempo_de_inicio;
	uint32_t tiempo_de_fin;
	uint32_t tiempo_de_salida;
	uint32_t tiempo_de_transmision;
	uint16_t largo_del_paquete;
	uint16_t memoria_alojada;
} Token_t;

typedef struct Line_Struct {
	uint8_t Op;     //0: Convertir los datos recibidos a mayúsculas.
					//1: Convertir los datos recibidos a minúsculas.
					//2: Reportar stack disponible.
                  	//3: Reportar heap disponible.
                  	//4: Mensajes de estado de la aplicación.
  	                //5: Medir performance.
   uint8_t  T;      // tamanio del payload;
   char*    Data;   // puntero a los datos
   QMPool*  Pool;   // pool al que pertenece el contenido de Data
   Token_t* Token;  // puntero al token
} Line_t;

typedef enum Parser_States {
   STX_STATE  = 0 ,
   OP_STATE       ,
   T_STATE        ,
   DATA_STATE     ,
   ETX_STATE
} Parser_t;

bool_t uartInitParser (void);
void   parserCallback ( void* nil );
void   Print_Line  ( Line_t* L );
bool_t uartInitLineParser (void);

#endif
