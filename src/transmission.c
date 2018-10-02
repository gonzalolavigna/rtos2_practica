// Requiere el cambio propuesto por Leonardo Urrego para sapi_uart.c que es:
// Despues de la linea:
// uint8_t status = Chip_UART_ReadLineStatus( lpcUarts[uart].uartAddr );
// agregar esta:
// uint32_t pendingInterrupt = Chip_UART_ReadIntIDReg( lpcUarts[ uart ].uartAddr );
// y cambiar la linea:
// if( ( status & UART_LSR_THRE ) && // uartTxReady
// por esta:
// if( ( pendingInterrupt & UART_IIR_INTID_THRE ) && // uartTxReady

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "sapi.h"

#include "transmission.h"
#include "qmpool.h"
#include "line_parser.h"
#include "pool_array.h"
#include "text_process.h"
#include "performance.h"

#define CANTIDAD_ITEMS_COLA_TXPRO      8

extern DEBUG_PRINT_ENABLE;

circularBufferNew ( cola_tx_proactivas,
      sizeof(Driver_proactivo),
      CANTIDAD_ITEMS_COLA_TXPRO );

volatile uint32_t tiempo_de_salida;
volatile uint32_t tiempo_de_transmision;

void Transmit_Task ( void* nil )
{
   Line_t L;

   while (TRUE) {
	   xQueueReceive(Processed_Queue,&L,portMAX_DELAY);
	   UART_TX_Proact ( L.Data,L.T,(L.Token==NULL)?txCallback:completionHandler);
   }
}

// Callback de transmision proactiva de linea
void txCallback ( void * Puart_tp )
{
   Driver_proactivo * uart_tp = (Driver_proactivo *) Puart_tp;
   QMPool_put (Pool_Select(uart_tp->largo),uart_tp->pBuffer);
}

// Callback de transmision proactiva de linea con medida de performance
void completionHandler ( void * Puart_tp )
{
       tiempo_de_transmision = now();
}


// Inicializacion de IRQ para UART TX
bool_t uartInitTx (void){
   uartTxInterruptCallbackSet ( UART_USB, uart_TX_ISR  );
   debugPrintlnString         ( "TX: IRQ INICIALIZADA" );
   uartTxInterruptSet( UART_USB, TRUE );
   return TRUE;
}


void UART_TX_Proact( uint8_t * pBuffer, uint32_t largo, callBackFuncPtr_t callback)
{
	static bool_t primera_vez = TRUE;
	Driver_proactivo uart_txpro;
	if (primera_vez) {
		circularBufferInit( cola_tx_proactivas, sizeof(Driver_proactivo), 8 );
	}

    uart_txpro.pBuffer  = pBuffer;
    uart_txpro.largo    = largo;
    uart_txpro.callback = callback;
    circularBufferWrite ( &cola_tx_proactivas, (uint8_t *) &uart_txpro);

    	if( uartTxReady(UART_USB) ) {
    		uart_TX_ISR ();
    	}
    primera_vez = FALSE;
}


// Handler IRQ FIFO de TX de UART USB vacia
static void uart_TX_ISR (void * nil)
{
   static Driver_proactivo txpro;
   static int32_t          faltan_transmitir        = 0;
   static uint32_t         i                        = 0;
   uint8_t                 byte_a_enviar;
   circularBufferStatus_t  estado_cola              = CIRCULAR_BUFFER_EMPTY;

   if ( faltan_transmitir > 0) {
      byte_a_enviar = txpro.pBuffer[i++];
      uartTxWrite (UART_USB, byte_a_enviar);
      faltan_transmitir--;
      if (faltan_transmitir==0)
      {
         ( * txpro.callback )((void*)&txpro); // Llamo al Callback apenas
                                              // despacho el ultimo dato
      }
   }
   else
   {
      i = 0;
      faltan_transmitir = 0;
      estado_cola = circularBufferRead( &cola_tx_proactivas, (uint8_t *) &txpro);
      if( estado_cola != CIRCULAR_BUFFER_EMPTY) {
         faltan_transmitir = txpro.largo-1;
         byte_a_enviar     = txpro.pBuffer[i++];
         uartTxWrite (UART_USB, byte_a_enviar);
         tiempo_de_salida = now();
      }
   }
}
