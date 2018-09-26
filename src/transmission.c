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

#define CANTIDAD_ITEMS_COLA_TXPRO		8

extern DEBUG_PRINT_ENABLE;

typedef struct Driver_proactivo_st {
	uint8_t *			pBuffer;
	uint32_t 			largo;
	callBackFuncPtr_t	callback;
}Driver_proactivo;

circularBufferNew ( cola_tx_proactivas,
		sizeof(Driver_proactivo),
		CANTIDAD_ITEMS_COLA_TXPRO );


// Inicializacion de IRQ para UART TX
bool_t uartInitTx (void){
	uartTxInterruptCallbackSet( UART_USB, uart_TX_ISR );
	debugPrintlnString("TX: IRQ INICIALIZADA");
//	uartTxInterruptSet( UART_USB, TRUE );
	return TRUE;
}

void Transmit_Task ( void* nil )
{
	Line_t L;
	int i;
	Driver_proactivo uart_txpro;

	circularBufferInit( cola_tx_proactivas, sizeof(Driver_proactivo), 8 );

// Repetir por siempre
	while (TRUE) {

	  xQueueReceive ( Processed_Queue, &L, portMAX_DELAY );

	  uart_txpro.pBuffer = L.Data;
	  uart_txpro.largo = L.T;
	  uart_txpro.callback = txCallback;
	  circularBufferWrite ( &cola_tx_proactivas, (uint8_t *) &uart_txpro);
	  uartTxInterruptSet( UART_USB, TRUE );		// Habilito THRE IRQ sólo mientras hayan datos para transmitir
	  if( uartTxReady(UART_USB) ) {
		  uart_TX_ISR ();
	  }

	}

}

// Callback de transmision proactiva
static void txCallback ( void * Puart_tp )
{
    uartTxInterruptSet( UART_USB, FALSE );		// Deshabilito THRE IRQ porque
												// ya no tengo más para transmitir
	Driver_proactivo * uart_tp = (Driver_proactivo *) Puart_tp;
	QMPool_put (Pool_Select(uart_tp->largo),uart_tp->pBuffer);
}

// Handler IRQ FIFO de TX de UART USB vacia
void uart_TX_ISR (void * nil)
{
	static BaseType_t xHigherPriorityTaskWoken= pdFALSE;
	static Driver_proactivo txpro;
	static int32_t faltan_transmitir = 0;
	static uint32_t i = 0;
	uint8_t byte_a_enviar;
	circularBufferStatus_t estado_cola = CIRCULAR_BUFFER_EMPTY;

	if ( faltan_transmitir > 0) {
		byte_a_enviar = txpro.pBuffer[i++];
		uartTxWrite (UART_USB, byte_a_enviar);
		faltan_transmitir--;
		if (faltan_transmitir==0)
		{
			(* txpro.callback ) ((void*)&txpro);	// Llamo al Callback apenas
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
			byte_a_enviar = txpro.pBuffer[i++];
			uartTxWrite (UART_USB, byte_a_enviar);
		}
	}

//	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
