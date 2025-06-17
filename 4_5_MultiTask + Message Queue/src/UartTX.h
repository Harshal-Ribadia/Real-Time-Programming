// ----------------------------------------------------------------------------
/// \file		UartTX.h
/// \brief		Realizes UART Transmission as FreeRTOS task.
/// \author		Wolfgang Schulter
/// \license	for educational purposes only, no warranty, see license.txt
/// \date		10.11.2015 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __UART_TX_H__
#define __UART_TX_H__  	  	// only once ..

#include "mcommon.h"  	// commons for all modules
#include "multitask.h"
#include "msgQueue.h"
extern char controlCharacter;
// ----------------------------------------------------------------------------
// UART TX PDO
// ----------------------------------------------------------------------------
typedef struct _pdo_t {
	uint16_t sig;		// some signature
	uint8_t team_nr;	// some signature
	uint16_t wcet_ping;
	uint16_t wcet_pong;
	uint16_t poti_val;
	uint16_t js_state;
	char queue;
} PDO_T;
extern PDO_T pdo;

void vUartTX_Task(void * pvParameters);

#endif // __UART_TX_H__
