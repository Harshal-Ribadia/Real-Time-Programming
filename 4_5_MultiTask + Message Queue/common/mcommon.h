// ----------------------------------------------------------------------------
/// \file		 mcommon.h
/// \brief		 module common macros, and typedefs.
/// \author		 Wolfgang Schulter 
/// \date		 19.07.2013 ws:  LPC43xx version (M4/M0 common)
/// \date		 19.01.2013 ws:  board.h
/// \date		 28.11.2011 ws:  initial version

#ifndef __MCOMMON_H__
#define __MCOMMON_H__  		// include only once ..

// ----------------------------------------------------------------------------
// Software configuration
// ----------------------------------------------------------------------------
// #define  USE_M4_DSP_LIB		// DSP library functions, uncomment if needed
#define USE_TICM0_BIT_TOGGLE	// Toggle Bit GPIO6[1] --> PC_2
#define USE_TICM4_BIT_TOGGLE	// Toggle Bit GPIO6[2] --> PC_3
// ----------------------------------------------------------------------------
#ifdef USE_M4_DSP_LIB		// DSP library functions
#include "ws_m4dsp_lib.h"	// the lib, include since .16
#else
#define WS_M4DSP_LIB_VERSION	"0"
#endif
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "list.h"

#define VERSIONID 	"4.3."  	WS_M4DSP_LIB_VERSION 	// x.y.LIBVER
#define SHORTID		"RPP " 		VERSIONID 		// shortform ID, transmitted with dm_inf.sig0

#ifdef CORE_M4
#define ABOUT1 "RTP_PingPong_M4 " VERSIONID
#define ABOUT2 "(c) W. Schulter 2015"
#elif CORE_M0
#define ABOUT1 "RTP_PingPong_M0 " VERSIONID
#define ABOUT2 "(c) W. Schulter 2015"
#else
#error "CORE_M0 or CORE_M4 must be defined !"
#endif
extern const char IDstr[];

// ---------------------------------------------
#define PROJ_ID		"PINGPONG"
// ---------------------------------------------
#define PROJ_AUTHOR	"RTP-Lab Team "
// ---------------------------------------------

#include "supervision.h" 	// supervision module (stack, exec time)

// #include "debug_frmwrk.h"

// extern unsigned int T_ts;  		// the tick counter
// #define FREQ_TICK_HZ 	20000		// basic tick frequency
// #define FREQ_TICK_DIV 	4			// basic tick divider for sampling  fs = ftick / div

#ifndef MIN
#define MIN(A,B) (((A)<(B)) ? (A) : (B))
#endif
#ifndef MAX
#define MAX(A,B) (((A)>(B)) ? (A) : (B))
#endif
#ifndef ABS
#define ABS(A) (((A)<(0)) ? -(A) : (A))
#endif
#ifndef LIMIT
#define LIMIT(X, LO, HI)  ((X) >= (LO) ? ( MIN(X,HI) ) : (LO))
#endif
// the modulo 2^exp macro
#define MOD2(EXP, X) ( ((1 << (EXP))-1) & (X) )


#define N_D		8			// since 4.2.x part of app_struct

// ---------------------------------------------
// new in 4.1 app parameter structure
// ---------------------------------------------
typedef struct _app_struct_t
{
	short itick_m4;			// new in 4.1
	short team_nr;			//!< new in 4.1
	const char *pproj_ID;	//!< new in 4.1.0
	const char *pproj_Auth;	//!< new in 4.1.0
	void *pcan_rx_msg;		//!< new in 4.2.x
	unsigned char led_mode;	// new in 4.1: mode 0 = heartbeat, mode 1: led_mask -> LEDs
	unsigned char led_mask;	// new in 4.1
	short led_rl_start;		// new in 4.1: mode 2: running light
	short led_rl_delta;		// new in 4.1: mode 2: running light
	//short mot_state_1;
	//short mot_state_changed;
	//short mot_init;		//!< flag, 1, if motor_init_before_start() was executed
	//short pos_state;		//!< not used
	short js_state;
	short js_state_1;
	short js_changed;
	short js_menu_mode;
	short tmp_js_state;
	short cmd_state;		//!< cmd_state, up/down/off
	short cmd_state_1;
	short cmd_changed;
	//short button_up;		//!< string constant used instead
	//short button_down;	//!< string constant used instead
	int _d[N_D]; 			//!< general purpose debug variables
	int _d_1[N_D]; 			//!< debug variables, prev values
} APP_STRUCT;

void err_blink(uint32_t bitmask);
int re_init_Tick_Timer(uint16_t _fs_Hz);

//#if defined(BOARD_KEIL_MCB_18574357)
// ws: much faster than the Board_LED_Toggle()/Board_LED_Set() lib functions
#define SET_LED0(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 6, 24, (X)))
#define SET_LED1(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 6, 25, (X)))
#define SET_LED2(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 6, 26, (X)))
#define SET_LED3(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 6, 27, (X)))
#define SET_LED4(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 6, 28, (X)))
#define SET_LED5(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 4, 12, (X)))
#define SET_LED6(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 4, 13, (X)))
#define SET_LED7(X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, 4, 14, (X)))

#define GET_LED1() Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 6, 25)
#define GET_LED4() Chip_GPIO_ReadPortBit(LPC_GPIO_PORT, 6, 28)

#define TOGGLE_LED1() (SET_LED1(!GET_LED1()))
#define TOGGLE_LED4() (SET_LED4(!GET_LED4()))

// ----------------------------------------------------------------------------
// set any bit B on GPIO Port GB
// ----------------------------------------------------------------------------
#define SET_BIT(GP,B,X) (Chip_GPIO_WritePortBit(LPC_GPIO_PORT, GP, B, (X)))
// ----------------------------------------------------------------------------

// -------------------------------------
/// initialization structure
/// save/restored to EEPROM (in main)
// -------------------------------------
/*typedef struct tini {
	long divider,An,Bn,Cn,Dn,En,Fn;  // for GLCD
	int fgen_mode;
	int filter_mode;
} TINI;

extern TINI ini;*/

/*
#define N_D		8
extern int _d[N_D]; 		// general purpose debug variables
extern int *p_d_m0;			// pointer to _d[] array in M0, since .14
extern int _d_1[N_D]; 		// debug variables, prev values
*/
void set_display_var(int ind, int val);		// set display var

#define N_DB	1024
extern char _db[N_DB];

#ifdef CORE_M4
// -------------------------------------
extern void send_str(char *pc);				// send string to M0 via IPC
#define PRINTF(...)		sprintf(_db, __VA_ARGS__);	send_str(_db);
#define DB				send_str(_db)		// ws: for compatibility with lpc17xx
#define DEBUGSTR(str)  	(send_str(str))  	// ws: new in lpc43xx: add string from debugging to EMAC
// -------------------------------------
extern APP_STRUCT app_struct;				// ws: seit 4.1.0, def'd in M4, shared struct for M0/M4
// -------------------------------------
#elif CORE_M0
// -------------------------------------
#include "emac_Task.h"	// Ethernet MAC module
#undef DEBUGSTR			// Board_UARTPutSTR(str)  	from board_api.h
#define DEBUGSTR(str)  	(putStr_Emac_dbg(str))  	// ws: new in lpc43xx: add string from debugging to EMAC
#undef DEBUGOUT 		// printf(__VA_ARGS__)  	from board_api.h
#define DEBUGOUT(...) 	sprintf(_db, __VA_ARGS__);putStr_Emac_dbg(_db);
#define PRINTF(...)		sprintf(_db, __VA_ARGS__);putStr_Emac_dbg(_db);

#define DB				putStr_Emac_dbg(_db)	// ws: for compatibility with lpc17xx
#define _DBG(x)			DEBUGSTR(x)
// -------------------------------------
extern APP_STRUCT *papp_struct;			// ws: seit 4.1.0, def'd in M4, shared struct for M0/M4
// -------------------------------------
#endif

#endif // __MCOMMON_H__
