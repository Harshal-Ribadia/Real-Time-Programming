// ----------------------------------------------------------------------------
/// \file		ipc_app.h
/// \brief		Inter Processor Communication(IPC)  app communication,
///					derived from lpcopen/ipc_example.h
/// \author		Wolfgang Schulter
/// \license	for educational purposes only, no warranty, see license.txt
/// \date		23.07.2013 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef _IPC_APP_H_
#define _IPC_APP_H_

#include "ipc_msg.h"		// IPC module (M0/M4 comm)

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EXAMPLE_DUALCORE_CMN_IPC_EX LPC43xx Inter Processor Communication(IPC) example
 * @ingroup EXAMPLE_DUALCORE_CMN_IPC
 *
 * Build procedure:
 * <a href="http://www.lpcware.com/content/project/lpcopen-platform-nxp-lpc-microcontrollers/lpcopen-build-procedures/lpc18xx/43xx-lpco">LPCOpen 18xx/43xx build instructions</a>
 *
 * Submit bug reports for LPCOpen code <a href="http://www.lpcware.com/content/bugtrackerproject/lpcopen">here.</a>
 * @{
 */

/**
 * \def IPCEX_QUEUE_SZ
 * Size of the IPC Queue used by this example implimentation, the current
 * implementation uses same queue size for both M0 and M4, application
 * developer can override it.
 */
#define IPCEX_QUEUE_SZ        512	// 32

/**
 * \def IPCEX_MAX_IDS
 * Maximum number of message IDs that this IPC examples uses
 */
#define IPCEX_MAX_IDS         20
//
//#define IPCEX_ID_BLINKY        1  /*!< IPC ID used by BLINKY example */
//#define IPCEX_ID_USBHOST       2  /*!< IPC ID used by USB HOST example */
//#define IPCEX_ID_USBDEVICE     3  /*!< IPC ID used by USB Device example */
//#define IPCEX_ID_LWIP          4  /*!< IPC ID used by LWIP example */
//#define IPCEX_ID_EMWIN         5  /*!< IPC ID used by EMWIN example */
//#define IPCEX_ID_USER1         10 /*!< IPC ID that can be used by other user examples */
//#define IPCEX_ID_USER2         11 /*!< IPC ID that can be used by other user examples */


#define IPCEX_ID_ADDA_TIC_STARTED	1
#define IPCEX_ID_ADDA_FINISHED		2
#define IPCEX_ID_M4APP_FINISHED		3
#define IPCEX_ID_INIT_PST			4
#define IPCEX_ID_INIT_PDM			5

#define IPCEX_ID_INIT_PFGEN_PAR		6
#define IPCEX_ID_INIT_PFILT_PAR		7

#define IPCEX_ID_SEND_CHAR			8
#define IPCEX_ID_PRINTF				9

// #define IPCEX_ID_COMM_ANALYSIS		10

#define IPCEX_ID_COMM_NEWCHAR		11
#define IPCEX_ID_BUFFER_STATUS		12
#define IPCEX_ID_UPDATE_SUPER_STACK 13

#define IPCEX_ID_INIT_PDM_INF		14	// since v.11
#define IPCEX_ID_INIT_P_D			15	// since v.14
#define IPCEX_ID_INIT_APP_STRUCT	16  // since 4.1.0

typedef struct __ipcex_msg {
	uint32_t id;
	uint32_t data;
} ipcex_msg_t;

// ret = IPC_pushMsgTout(&msg_m4fin, 0);
// ret = IPC_pushMsg(&msg_m4fin);

// ws: simple IPC push macros
#define IPC_PUSH(ID, DATA)	{ipcex_msg_t msg; msg.id = (ID); msg.data = (DATA); IPC_pushMsg(&msg);}
#define IPC_PUSH_TO(ID, DATA, TO)	{ipcex_msg_t msg; msg.id = (ID); msg.data = (DATA); IPC_pushMsgTout(&msg, (TO));}

int ipcex_msgPush(uint32_t id, uint32_t data);

int ipcex_register_callback(uint32_t id, void (*func)(uint32_t));

extern xSemaphoreHandle semIPC_dispatch;	// ws: for dispatching high-level messages

#ifdef USE_CALLBACKS
/**
 * @brief	IPC register callback function pointer
 *
 * This function registers a callback function pointer to a
 * message \a id, whenever a message with \a id is received the
 * register call-back function will be invoked.
 *
 * @param	id		: ID of message to which the callback \a func be associated
 * @param	func	: pointer to callback function
 * @return	0 on failure [given \a id is greater than 
 * @note	#IPCEX_MAX_IDS], !0 on success
 */
int ipcex_register_callback(uint32_t id, void (*func)(uint32_t));

/**
 * @brief	Push data on the queue
 * @param	id		: Task ID of the destination task
 * @param	data	: Data containing the message
 * @return	#QUEUE_ERROR or #QUEUE_FULL on error, #QUEUE_INSERT on success
 */
int ipcex_msgPush(uint32_t id, uint32_t data);

/**
 * @}
 */
#endif

#ifdef __cplusplus
}
#endif

#endif // _IPC_APP_H_
