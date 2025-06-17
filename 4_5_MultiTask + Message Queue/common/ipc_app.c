/*
 * @brief Example implementation of IPC using IPC library
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "lpc43xx_dualcore_config.h"
#include "ipc_msg.h"		// IPC module (M0/M4 app comm)
#include "ipc_app.h"		// IPC module (M0/M4 app comm)

xSemaphoreHandle semIPC_dispatch;	// ws: for dispatching high-level messages

/** @ingroup EXAMPLE_DUALCORE_CMN_IPC_EX
 * @{
 */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Array of IPC Event callback functions */
static void(*ipcex_callback_lookup[IPCEX_MAX_IDS]) (uint32_t);

/* Message QUEUE used by IPC library to exchange message between cores */
static ipcex_msg_t ipcex_queue[IPCEX_QUEUE_SZ];

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* This task will receive the message from the other core
 * and will invoke the appropriate callback with the message
 */
static void ipcex_dispatch_task(void *loop)
{
/*	int ret;
	ipcex_msg_t msg;
	extern ipcex_msg_t msg_rx;
	void app_dispatch_msg(ipcex_msg_t *msg);
	while (1) {

		xSemaphoreTake(semIPC_dispatch, portMAX_DELAY);		// <--- sync AD mutex/sync semaphore --> 0
		// ret = IPC_popMsg(&msg);
		// if (ret == QUEUE_VALID)
		msg.id = msg_rx.id;
		msg.data = msg_rx.data;
	// app_dispatch_msg(&msg);
	};
}

#ifdef ___ORIG */
	int ret;
	ipcex_msg_t msg;
	do {
		if (loop) {
			ret = IPC_popMsg(&msg);
		}
		else {
			ret = IPC_tryPopMsg(&msg);
		}
		if ((ret == QUEUE_VALID) && (msg.id < IPCEX_MAX_IDS)) {
			if (ipcex_callback_lookup[msg.id]) {
				ipcex_callback_lookup[msg.id](msg.data);
#ifdef CORE_M0
				//Board_LED_Toggle(7);
#endif
			}
		}
	} while (loop);
}
// #endif

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* IPC register callback function pointer */
int ipcex_register_callback(uint32_t id, void (*func)(uint32_t))
{
	if (id >= IPCEX_MAX_IDS) {
		return 0;
	}

	ipcex_callback_lookup[id] = func;
	return 1;
}

/* IPC example implementation task */
uint32_t ipcex_tasks(void)
{
	uint32_t ret;	// ws: added

	vSemaphoreCreateBinary(semIPC_dispatch);		// ws add: semaphore for dispatch task
	xSemaphoreTake(semIPC_dispatch,portMAX_DELAY);	// <---- init to 0

	ret = xTaskCreate(ipcex_dispatch_task, (signed char *) "IPC Ex",
				configMINIMAL_STACK_SIZE, (void *) 1, TASK_PRIO_IPC_DISPATCH,
				(xTaskHandle *) NULL);

	return ret;
}

/* Initialize the IPC queue */
void IPCEX_Init(void)
{
	IPC_initMsgQueue(ipcex_queue, sizeof(ipcex_msg_t), IPCEX_QUEUE_SZ);
}

/* Push data on the queue */
int ipcex_msgPush(uint32_t id, uint32_t data)
{
	ipcex_msg_t msg;
	msg.id = id;
	msg.data = data;
#if (defined(OS_FREE_RTOS) || defined(OS_UCOS_III))
	return IPC_pushMsg(&msg);
#else
	return IPC_tryPushMsg(&msg);
#endif
}

/**
 * @}
 */
