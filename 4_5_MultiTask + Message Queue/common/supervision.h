// ----------------------------------------------------------------------------
/// \file		 supervision.h
/// \brief		 supervision functions (stack/execution time)
/// \author		 Wolfgang Schulter 
/// \date		 23.07.2014 ws:  initial version for M0/M4, added binit member
// ----------------------------------------------------------------------------

#ifndef __SUPERVISION_H__
#define __SUPERVISION_H__  	  	  // only once ..

// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "list.h"

// ---------------------------------------
#ifdef configCHECK_FOR_STACK_OVERFLOW
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
#include "StackMacros.h"
void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName );
#endif
#endif
// ---------------------------------------

void init_SuperTask();

void vSuperTask(void *pvParameters);

void supervise_stack();

void print_buffer_status();
int update_super_stack();
void print_super_stack();

// -------------------------------------
// timer resolution
// -------------------------------------
//#define USE_TIMER_BITS	16
#define USE_TIMER_BITS	32

// -------------------------------------
// struct for WCET/BCET task supervision
// -------------------------------------
#if (USE_TIMER_BITS==16)
typedef struct T_BWCET {
	unsigned short t1;		// last t1
	unsigned short dt;		// last dt = t2-t1
	unsigned short dt1;		// last dt1 = t1 - t1_
	unsigned short bcet;
	unsigned short wcet;
	unsigned short n;
} BWCET;
#elif (USE_TIMER_BITS==8)
typedef struct T_BWCET {
	unsigned char t1;		// last t1
	unsigned char dt;		// last dt = t2-t1
	unsigned char dt1;		// last dt1 = t1 - t1_
	unsigned char bcet;
	unsigned char wcet;
	unsigned short n;
} BWCET;
#elif (USE_TIMER_BITS==32)
typedef struct T_BWCET {
	unsigned int t1;		// last t1
	unsigned int dt;		// last dt = t2-t1
	unsigned int dt1;		// last dt1 = t1 - t1_
	unsigned int bcet;
	unsigned int wcet;
	unsigned int n;
	unsigned int n_re_init; 	// re-init after n has reached approx. this number
	unsigned short binit;		// 1 after wcet_init()
	unsigned short brunning;	// 1 after t1 started, 0 after t2 stopped
} BWCET;
#endif

void wcet_init(BWCET *pbwcet);
void wcet_t1(BWCET *pbwcet);
void wcet_t2(BWCET *pbwcet);
void init_wcet();

// -------------------------------------
// struct for stack/bwcet supervision of a task
// -------------------------------------
typedef struct T_SUPER_TASK {
	xTaskHandle hTask;				// task handle
	// const char *name;				// task name
	portBASE_TYPE stack_left_act;	// bytes left on stack, actual
	portBASE_TYPE stack_left_last;	// bytes left on stack, last call
	BWCET bwcet;					// WCET/BCET task supervision
} SUPER_TASK;

int update_super_stack(); 		// SUPER_TASK sst[], int n);

void print_super_stack(); 		// SUPER_TASK st[], int n);
void print_super_bwcet(); 		// SUPER_TASK st[], int n);
void print_buffer_status();		// new in 4.0

// ----------------------------------------------------------------------------
// Tasks with stack/exec time supervision
// ----------------------------------------------------------------------------
enum SUPER_TASK_INDEX {	ST_SUPER, 		ST_ADDA, 		ST_TIC0, 	ST_APP0, 	ST_EMAC_DM,  // M0 tasks
						ST_EMAC_DBG, 	ST_UIP, 		ST_UART, 	ST_SWIM,	ST_DIA0,
						ST_CAN,
						ST_TIC41,		ST_TIC42,		ST_DIA4,	ST_FFT4,	ST_SND4,	// the M4 tasks ..
						ST_PING,		ST_PONG,		ST_PRINT,	ST_NUM};

#define INIT_TASK_NAME_STRINGS {\
	"S", 			"AD", 		"TIC0",		"APP0",		"EDM",\
	"EDB",			"UIP",		"URT",		"SWIM",		"DIA0",\
	"CAN",\
	"TC41",			"TC42",		"DIA4",		"FFT4",		"SND4",\
 	"PING",			"PONG",		"PRNT"\
};

#define FIRST_M4_TASK	ST_TIC41		// since 4.0.14
// #define NTASKSM4	(ST_NUM-ST_TIC41)

extern const char *st_task_names[ST_NUM];

/*#define NAME_ST_SUPER	"S"
#define NAME_ST_ADDA	"A"
#define NAME_ST_APP		"AP"
#define NAME_ST_EMAC_DM		"EDM"
#define NAME_ST_EMAC_DBG	"EDB"
#define NAME_ST_UIP		"UI"
#define NAME_ST_UART	"U"
#define NAME_ST_GLCD	"G"
#define NAME_ST_CAN		"CAN"
// #define NAME_ST_FFT		"T"
*/

#ifdef CORE_M0
extern SUPER_TASK st[ST_NUM];				// array of stack/bwcet supervised tasks
#endif

// macros for wcet_init(), wcet_t1(), wcet_t2() --> wcet_init(BWCET_BLINK), ...
#define BWCET_SUPER		st[ST_SUPER].bwcet
#define BWCET_ADDA		st[ST_ADDA].bwcet
#define BWCET_APP0		st[ST_APP0].bwcet
#define BWCET_TIC0		st[ST_TIC0].bwcet
#define BWCET_EMAC_DM	st[ST_EMAC_DM].bwcet
#define BWCET_EMAC_DBG	st[ST_EMAC_DBG].bwcet
#define BWCET_DIA0		st[ST_DIA0].bwcet
#define BWCET_UIP		st[ST_UIP].bwcet
#define BWCET_UART		st[ST_UART].bwcet
#define BWCET_SWIM		st[ST_SWIM].bwcet
#define BWCET_CAN		st[ST_CAN].bwcet
#define BWCET_FFT4		st[ST_FFT4].bwcet
#define BWCET_TC41		st[ST_TC41].bwcet
#define BWCET_TC42		st[ST_TC42].bwcet
#define BWCET_DIA4		st[ST_DIA4].bwcet
#define BWCET_PING		st[ST_PING].bwcet
#define BWCET_PONG		st[ST_PONG].bwcet
#define BWCET_PRINT		st[ST_PRINT].bwcet

// cancel debug outputs
// #define sprintf
// #define DB
// #define _db  ""

// new: IPC macros for supervision/bwcet measures (on M0)
#ifdef CORE_M4
extern xTaskHandle hTasksM4[ST_NUM];		// array of M4 task handles
extern SUPER_TASK *pst;			// pointer to array of stack/bwcet supervised tasks (CORE_M0)
#define  BWCET_INIT(TASK_ID)	((pst) ? wcet_init(&((pst+TASK_ID)->bwcet)) : 0)
#define  BWCET_T1(TASK_ID)		((pst) ? wcet_t1(&((pst+TASK_ID)->bwcet)) : 0)
#define  BWCET_T2(TASK_ID)		((pst) ? wcet_t2(&((pst+TASK_ID)->bwcet)) : 0)
#endif

/*#include "ipc_app.h"			// IPC module (M0/M4 app comm)
#define  BWCET_INIT(TASK_ID)	IPC_PUSH(IPCEX_ID_BWCET_INIT, (TASK_ID))
#define  BWCET_T1(TASK_ID)		IPC_PUSH(IPCEX_ID_BWCET_T1, (TASK_ID))
#define  BWCET_T2(TASK_ID)		IPC_PUSH(IPCEX_ID_BWCET_T2, (TASK_ID))
#endif*/

#endif // __SUPERVISION_H__
