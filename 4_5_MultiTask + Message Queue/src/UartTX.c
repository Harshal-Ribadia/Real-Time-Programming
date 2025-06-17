/// \file		UartTX.c
/// \brief		Realizes UART Transmission as FreeRTOS task.
/// \author		Wolfgang Schulter
/// \license	for educational purposes only, no warranty, see license.txt
/// \date		10.11.2015 ws:  initial version
// ----------------------------------------------------------------------------

#include "mcommon.h"  		// commons for all modules
#include <MultiTask.h>

#include "uart.h"			// UART module
#include "UartTX.h"  		// UartTX module header

#define UART_TX_PERIOD_MS				(250)

// ----------------------------------------------------------------------------
// UART TX PDO
// ----------------------------------------------------------------------------
PDO_T pdo;

// ----------------------------------------------------------------------------
void vUartTX_Task(void * pvParameters)
{
	portTickType xLastWakeTime = xTaskGetTickCount();

	while(1) {
		vTaskDelayUntil(&xLastWakeTime, UART_TX_PERIOD_MS/portTICK_RATE_MS );

		pdo.sig = 0xCAFE;					// some signature
		pdo.team_nr = app_struct.team_nr;	// the team number
		pdo.poti_val = app_struct._d[7];
		pdo.js_state = app_struct.js_state;

		pdo.wcet_ping = pst[ST_PING].bwcet.wcet;
		pdo.wcet_pong = pst[ST_PONG].bwcet.wcet;
		pdo.queue = controlCharacter;

		uart_send(&pdo, sizeof(pdo)); // UART transmit		// UART transmit

	}
}
