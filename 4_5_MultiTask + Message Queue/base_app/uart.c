// ----------------------------------------------------------------------------
/// \file		uart.c
/// \brief		Realizes 3 UART communication.
/// \author		Wolfgang Schulter
/// \date		01.05.2015 ws:  initial version
// ----------------------------------------------------------------------------

#include "FreeRTOS.h"
#include "board.h"

//#include "scu_18xx_43xx.h"	// pin functions
#include "uart.h"			// UART module

#include "mcommon.h"  		// commons for all modules

// ----------------------------------------------------------------------------
// NXP uart.c example: Initialize Interrupt for UART
// ----------------------------------------------------------------------------
#define UARTNum 0	// with Keil 4357/1857

#if (UARTNum == 0)
#define LPC_UART LPC_USART0
#define UARTx_IRQn  USART0_IRQn
#define UARTx_IRQHandler UART0_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART0_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART0_Rx
#elif (UARTNum == 1)
#define LPC_UART LPC_UART1
#define UARTx_IRQn  UART1_IRQn
#define UARTx_IRQHandler UART1_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART1_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART1_Rx
#elif (UARTNum == 2)
#define LPC_UART LPC_USART2
#define UARTx_IRQn  USART2_IRQn
#define UARTx_IRQHandler UART2_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART2_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART2_Rx
#elif (UARTNum == 3)
#define LPC_UART LPC_USART3
#define UARTx_IRQn  USART3_IRQn
#define UARTx_IRQHandler UART3_IRQHandler
#define _GPDMA_CONN_UART_Tx GPDMA_CONN_UART3_Tx
#define _GPDMA_CONN_UART_Rx GPDMA_CONN_UART3_Rx
#endif

void App_Interrupt_Init(void)
{
	/* Enable UART Rx interrupt */
	Chip_UART_IntConfig(LPC_UART, UART_INTCFG_RBR, ENABLE);
	/* Enable UART line status interrupt */
	Chip_UART_IntConfig(LPC_UART, UART_INTCFG_RLS, ENABLE);
	/*
	 * Do not enable transmit interrupt here, since it is handled by
	 * UART_Send() function, just to reset Tx Interrupt state for the
	 * first time
	 */
	Chip_UART_InitRingBuffer(LPC_UART);
	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	NVIC_SetPriority(UARTx_IRQn, 1);
	/* Enable Interrupt for UART channel */
	NVIC_EnableIRQ(UARTx_IRQn);
}

// ----------------------------------------------------------------------------
// UART RX ISR
// ----------------------------------------------------------------------------
void UARTx_IRQHandler(void)
{
	char c;
	Chip_UART_Receive(LPC_UART, &c, 1, BLOCKING);

	// TODO: realize uart_receive to process the received char
	uart_receive(c);
	set_display_var(4, c);
}

// ----------------------------------------------------------------------------
// UART3 TX a buffer
// ----------------------------------------------------------------------------
void uart_send(const char *buf, unsigned int n)
{
	Chip_UART_Send(LPC_UART, buf, n, BLOCKING);
}

// ----------------------------------------------------------------------------
void uart_init()
{
	Board_UART_Init(LPC_UART);
	Chip_UART_Init(LPC_UART);
	Chip_UART_SetBaud(LPC_UART, 115200);
	Chip_UART_ConfigData(LPC_UART, UART_DATABIT_8, UART_PARITY_NONE, UART_STOPBIT_1);	/* Default 8-N-1 */

	/* Enable UART Transmit */
	Chip_UART_TxCmd(LPC_UART, ENABLE);

	// enable UART RX interrupt
	App_Interrupt_Init();

}
