// ----------------------------------------------------------------------------
/// \file		 app.c
/// \brief		 basic application.
/// \author		 Wolfgang Schulter 
/// \license	 for educational purposes only, no warranty, see license.txt
/// \date		 17.07.2013 ws:  initial version for FR_base01 (M4 part)
/// \date		 12.07.2012 ws:  initial version
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "mcommon.h"  		// commons for all modules
// #include "adda.h"		// AD/DA module

#include "app.h"			// application module
#include "dia_comm4.h"		// ASCII dia comm module (M4)

#include "ipc_app.h"		// IPC module (M0/M4 app comm)
#include "lpc43xx_dualcore_config.h"	// SHARED_MEM_M0

#include "data_monitor.h"	// data monitor struct

// #include "emac_Task.h"	// EMAC module

// #include "i2s_audio.h"		// HQ I2C-Audio module

#include "supervision.h" 	// supervision module (stack, exec time)

// #include "pwm.h"			// PWM with multiple lines

#ifdef USE_M4_DSP_LIB		// DSP library functions, if needed
#include "fgen.h"			// FGen module
#include "filter.h"  		// filter module header
#include "my_filter.h"		// my_filter module  (external filter functions, not in the lib)
short DTMF_detect(short x) { return x; }
#endif // #ifdef USE_M4_DSP_LIB

// #include "lms.h"			// LMS module

// #include "fft_filter.h"		// FFT-filter module

// APP_STRUCT *papp_struct = 0;	// ws: seit 4.1.0, def'd in M4, shared struct for M0/M4
// ----------------------------------------------------------------------------
const char proj_ID[] =  PROJ_ID;
const char proj_Auth[] =  PROJ_AUTHOR;
// ----------------------------------------------------------------------------
APP_STRUCT app_struct =	{		// ws: seit 4.1.0, def'd in M4, shared struct for M0/M4
	0,							//  short itick_m4;			// new in 4.1
	15,							//	short team_nr;		//!< new in 4.1
	proj_ID,					//  const char *pproj_ID;	//!< new in 4.1.0
	proj_Auth,					//  const char *pproj_Auth;	//!< new in 4.1.0
	// &rx_msg						//  void *pcan_rx_msg;		//!< new in 4.2.x
};
// ----------------------------------------------------------------------------

DMON_STRUCT *pdm = 0;			// pointer to DM struct, DM task/struct is on M0
DMON_INF_STRUCT *pdm_inf = 0;	// pointer to DM_INF struct	(on M0)

//xSemaphoreHandle semADDA_tic;		// semaphore AD tic --> app M4
//xSemaphoreHandle semADDA_fin;		// semaphore AD tic --> app M0

unsigned char js_state = 0;

unsigned char cmd_state, cmd_state_1;	//!< cmd_state, up/down/off

// short pwm_duty_sc = 0.9*PWM_PERIOD;			// signed pwm_output, affected by speed control

unsigned short fs_Hz = FS_HZ;				// default sampling frequency in Hz, prescaled configTICK_RATE_HZ
unsigned int Ts_ns = (1UL << 30)/FS_HZ;	// default sampling period in ns (since 4.0.11) was ~us before

// ----------------------------------------------------------------------------
// external functions (not within lib) since lib .17
// ----------------------------------------------------------------------------
#ifdef USE_M4_DSP_LIB		// DSP library functions, if needed
const char *filter_mode_ext_str[FILTER_NUM_MODES_EXT] = {	// extern. function strings
	"My Filter",
	"",
	"",
	"", "",
	"", "", "", "", "",
	"", "", "", "", "",
	"", "", "", "", ""
};

const void (*filter_mode_ext_dia_comm_fcn[FILTER_NUM_MODES_EXT])(char *, int , double[] ) = {
	my_filter_dia_comm,
	0,
	0,
	0,0,
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0
};

const short (*filter_mode_ext_tic_fcn[FILTER_NUM_MODES_EXT])( short ) = {	// extern. function pointers
	my_filter_tic,
	0,
	0,
	0,0,
	0,0,0,0,0,
	0,0,0,0,0,
	0,0,0,0,0
};
#endif // #ifdef USE_M4_DSP_LIB		// DSP library functions, if needed

// ----------------------------------------------------------------------------
// set display debug _d[0] .. _d[7] vars
// ----------------------------------------------------------------------------
void set_display_var(int ind, int val)
{
	if ((ind >= 0) && (ind < N_D))
		app_struct._d[ind] = val;
}

// ----------------------------------------------------------------------------
// assign digital signal monitor signals
// ----------------------------------------------------------------------------
char binit_inf = 0;
void app_assign_debug_vars()
{
	// extern uint8_t ha_nrx_buf, ha_ntx_buf;		// counter for num of buffers reveived/trans'ed
	int i;

	if (!pdm) return;

	// --------------------------------
	// set dsm-signals (for RTWin)
	// --------------------------------
	// SET_DSM_I16(0, ADC_in, 	"ADC_in");

	// if (ha_enable)  pdm->s[2] = ha_in;  // since 4.0.6: HQ Audio, if enabled (--> s command)

	// pdm->s[0 .. 2] assigned directliy in adda module (M0)

	/*pdm->s[3] = fgen_out[0];	// fgen_output;
	// pdm->s[4] = fgen_par.f0;
	pdm->s[5] = filt_out[0];	// filter_output;
	pdm->s[6] = filt_out_inv;	// Ginv = 1 - G

	pdm->s[8] = fgen_out[1];	// fgen_output1;
	pdm->s[9] = fgen_out[2];	// fgen_output2;

	pdm->i[0] = fgen_par[0].f0;	// since 4.0.10

	/ *pdm->i[1] = filt_out[2];	// since 4.0.11
	pdm->i[2] = filt_out[3];	//
	pdm->i[3] = filt_out[4];	//
	pdm->i[4] = filt_out[5];	//
	pdm->i[5] = filt_out[6];	//
	pdm->i[6] = filt_out[7];*/	//

	//pdm->i[11] = ha_nrx_buf;
	//pdm->i[12] = ha_ntx_buf;

	// since 4.0.14: copy lower 4 debug var's to M0's _d[] array
	/*if (p_d_m0) {
		for (i=0; i < (N_D/2); i++)
			p_d_m0[i] = _d[i];
		for (i=(N_D/2); i < N_D; i++)
			_d[i] = p_d_m0[i];
	}*/
	//_d[1] = count;

 	//_d[1] = app_auto_state;
	// _d[1] = phi;			// L1

 	//_d[2] = mot_dIa;
	//_d[2] = fgen_output; // mot_UmHs;

	//_d[3] = mot_UmHs;

	//_d[4] = app_auto_state;
	//_d[4] = mot_IaML;
	//_d[4] = (int)RXMsg.dataA;

	// _d[5] = cmd_state;
	//_d[5] = (int)RXMsg.dataB;

	//_d[6] = fgen_output;
	//_d[7] =
	{
		//extern short n_Qchars;
		//_d[7] = n_Qchars;
	}
}

// ----------------------------------------------------------------------------
void proc_comm_newchar(uint32_t data)
{
	comm_newchar((char)data);	// pass to EMAC_dbg
}

// ----------------------------------------------------------------------------
void proc_buffer_status(uint32_t data)
{
	/*extern short nQmax;
	PRINTF("\nBuffers free bytes (M4): \nHeap = %d, n_db = %d, n_rxBuf = %d\n",
		xPortGetFreeHeapSize(), get_free_str(_db, NRXBUF), get_free_str(_rxbuf, N_DB));*/
	print_buffer_status();
}

// ----------------------------------------------------------------------------
// TODO: cleanup taskHandle array for M4
void proc_update_super_stack(uint32_t data)
{
	int i;
	SUPER_TASK *p = pst + data;
	for (i = FIRST_M4_TASK; i < ST_NUM; i++, p++)
		if (p && hTasksM4[i]) {
			p->stack_left_last = p->stack_left_act;
			p->stack_left_act = uxTaskGetStackHighWaterMark(hTasksM4[i]);
		}
}

// ----------------------------------------------------------------------------
void proc_adda_tic_started(uint32_t data)
{
	// pdm = (DMON_STRUCT *)data;				// pointer to data monitor struct
	// tic4_1();	// alternative: in timer ISR
}

// ----------------------------------------------------------------------------
void proc_adda_finished(uint32_t data)
{
	// pst = (SUPER_TASK *)data;				// pointer to super task struct
	tic4_2();
}

// ----------------------------------------------------------------------------
void proc_init_pst(uint32_t data)
{
	pst = (SUPER_TASK *)data;				// pointer to super task struct
}

// ----------------------------------------------------------------------------
void proc_init_pdm(uint32_t data)
{
	pdm = (DMON_STRUCT *)data;				// pointer to dm struct
}

// ----------------------------------------------------------------------------
void proc_init_pdm_inf(uint32_t data)
{
	pdm_inf = (DMON_INF_STRUCT *)data;			// pointer to dm_inf struct
}

// ----------------------------------------------------------------------------
/*void proc_init_p_d(uint32_t data)	// removed in 4.2.x
{
	p_d_m0 = (int *)data;			// pointer to _d[] array of M0
}*/

// ----------------------------------------------------------------------------
/*void proc_init_app_struct(uint32_t data)
{
	papp_struct = (APP_STRUCT *)data;			// pointer to app_struct of M0
}*/

bool b_app_init = 0;
// ----------------------------------------------------------------------------
void app_init()
{
#ifdef USE_TICM4_BIT_TOGGLE
	// ------------------------------------------------------------------------
	Chip_SCU_PinMuxSet(0xC, 3, (SCU_MODE_ZIF_DIS | SCU_MODE_MODE_INACT | SCU_MODE_FUNC4));
	Chip_GPIO_WriteDirBit(LPC_GPIO_PORT, 6, 2, true);	// M2 tick WCET toggle bit --> PC_3
	// ------------------------------------------------------------------------
#endif

#ifdef USE_M4_DSP_LIB		// DSP library functions, if needed
	init_fgen();			// frequency generator
	init_filter();			// filter
#endif
	app_struct.led_mode = 3; 	// 0: heartbeat blink mode, 3:do-nothing mode

	ipcex_register_callback(IPCEX_ID_COMM_NEWCHAR, proc_comm_newchar);
	ipcex_register_callback(IPCEX_ID_BUFFER_STATUS, proc_buffer_status);
	ipcex_register_callback(IPCEX_ID_UPDATE_SUPER_STACK, proc_update_super_stack);

	ipcex_register_callback(IPCEX_ID_ADDA_TIC_STARTED, proc_adda_tic_started);
	ipcex_register_callback(IPCEX_ID_ADDA_FINISHED, proc_adda_finished);

	ipcex_register_callback(IPCEX_ID_INIT_PST, proc_init_pst);
	ipcex_register_callback(IPCEX_ID_INIT_PDM, proc_init_pdm);
	ipcex_register_callback(IPCEX_ID_INIT_PDM_INF, proc_init_pdm_inf);	// since v.11
	// ipcex_register_callback(IPCEX_ID_INIT_P_D, proc_init_p_d);			// since v.14, removed in 4.2.x
	// ipcex_register_callback(IPCEX_ID_INIT_APP_STRUCT, proc_init_app_struct);	// since 4.1.0

	b_app_init = 1;
}

volatile float x = 0.1;

// ----------------------------------------------------------------------------
// send char to M0 (M4 printf(...))
// ----------------------------------------------------------------------------
void send_str(char *pc)
{
	ipcex_msg_t msg_str;
	msg_str.id = IPCEX_ID_PRINTF;
	msg_str.data = (uint32_t)pc;
	IPC_pushMsg(&msg_str);
}

// ----------------------------------------------------------------------------
// send char to M0 (M4 printf(...))
// ----------------------------------------------------------------------------
void send_char(char c)
{
	ipcex_msg_t msg_char;
	msg_char.id = IPCEX_ID_SEND_CHAR;
	msg_char.data = c;
	IPC_pushMsg(&msg_char);
}

// -----------------------------------------------------------
int get_free_str(char *buf, int siz)
{
	int i, numfree = 0;
	for (i = siz-1; i >= 0; i--)
		if (!buf[i])
			numfree++;
		else
			break;
	return numfree;
}

// ws: add shared semahore
// LOCATE_AT(SHARED_MEM_M0) xSemaphoreHandle semM4_fin;	// ws add: semaphore M4 fin

// ----------------------------------
// ws: new heartbeat, always 0.5 Hz
// ----------------------------------
void heartBeat_m4()
{
	// static short itick = 1;
	// ipcex_msg_t msg, msg_test = {IPCEX_ID_M4APP_FINISHED,0}; // {IPCEX_ID_USER1,0}; //
	// uint32_t ret;

	// if (--itick) return;
	if (app_struct.itick_m4) {
		app_struct.itick_m4--;
		// return;
	} else
		app_struct.itick_m4 = fs_Hz; 	// >> 1;
	if (app_struct.led_mode == 0) {
		// new in 4.1.: m4 heartbeat, can be sync'ed with m0
		if (app_struct.itick_m4 < (fs_Hz >> 1))
			SET_LED0(0);
		else
			SET_LED0(1);
	} else {
		// mask handled in heartbeat_m0
		/*SET_LED0(app_struct.led_mask & 0x01);
		SET_LED1(app_struct.led_mask & 0x02);
		SET_LED2(app_struct.led_mask & 0x04);
		SET_LED3(app_struct.led_mask & 0x08);
		SET_LED4(app_struct.led_mask & 0x10);
		SET_LED5(app_struct.led_mask & 0x20);
		SET_LED6(app_struct.led_mask & 0x40);
		SET_LED7(app_struct.led_mask & 0x80);*/	 // not aval. with pwm_init !
	}
	// SET_LED1(!s); //TOGGLE_LED1();

	// ret = IPC_pushMsg(&msg_test);
	// PRINTF("hi from M4 = %d ", itick);
}

// ----------------------------------------------------------------------------
// @brief	TIMER0 interrupt handler (Tick Timer for M0/M4)
// @return	Nothing
// ----------------------------------------------------------------------------
// uint16_t ftick = FREQ_TICK_HZ;		// tick frequency
uint32_t t_ts = 0;					// the tick counter
xSemaphoreHandle sem_tic;			// basic tick semaphore
uint8_t b_pointers_init = 0;
void TIMER0_IRQHandler(void)
{
	// static uint32_t t_ts_next = FREQ_TICK_HZ/2;	// initial match
	// extern xSemaphoreHandle sem_tic;			// basic tick semaphore

	// Clear interrupt
	Chip_TIMER_ClearMatch(LPC_TIMER0, 0);
	// NVIC_ClearPendingIRQ(TIMER0_IRQn);

#ifdef USE_TICM4_BIT_TOGGLE
	SET_BIT(6, 2, 1);			// set bit GPIO6[2] --> PC_3
#endif
	if (b_pointers_init < 2) {	// init more than once, to avoid critical races !
		//ipcex_msgPush(IPCEX_ID_INIT_PFGEN_PAR, &fgen_par);
		//ipcex_msgPush(IPCEX_ID_INIT_PFILT_PAR, &filt_par);
		ipcex_msgPush(IPCEX_ID_INIT_APP_STRUCT, &app_struct);	// since 4.1.0
		b_pointers_init = MIN(b_pointers_init+1, 100);
	}

	tic4_1();
#ifdef USE_TICM4_BIT_TOGGLE
	SET_BIT(6, 2, 0);			// set bit GPIO6[2] --> PC_3
#endif
}

#define TOGGLE_LED5() (SET_LED5())

void tic4_1()
{
	// xSemaphoreTakeFromISR(semADDA_tic, 0);		// <--- sync AD mutex/sync semaphore --> 0

	/*if (!pdm) {
	 	ipcex_msgPush(IPCEX_ID_M4APP_FINISHED, 0);
		return;
	}*/

	if (pst && !pst[ST_TIC41].bwcet.binit) BWCET_INIT(ST_TIC41);	// <---- wcet init (pst pointer via IPC)

	BWCET_T1(ST_TIC41);			// ----> wcet measure (IPC)

	// Toggle LED 0 each 0.5 s --> M4-Heartbeat
	heartBeat_m4();

	uSleep(5);		// since 4.0.13: wait some us until DM data is sent (2 us is not enough)

#ifdef USE_M4_DSP_LIB		// DSP library functions, if needed
	// while M0 does ADDA stuff, update the fgen()
	fgen_tic();
#endif

 	BWCET_T2(ST_TIC41);			// ----> wcet measure (IPC)
}

void tic4_2()
{
	if (pst && !pst[ST_TIC42].bwcet.binit) BWCET_INIT(ST_TIC42);	// <---- wcet init (pst pointer via IPC)

#ifdef USE_TICM4_BIT_TOGGLE
	SET_BIT(6, 2, 1);			// set bit GPIO6[2] --> PC_3
#endif
	BWCET_T1(ST_TIC42);			// ----> wcet measure (IPC)
	//SET_LED2(1);				// macro is much faster than the lib functions
	//SET_LED4(1);				// macro is much faster than the lib functions

	// ----- call filter tick function -----
#ifdef USE_M4_DSP_LIB		// DSP library functions, if needed
 	filter_tic();
#endif

 	// Board_LED_Toggle(4);

 	app_assign_debug_vars();

 // ipcex_msgPush(IPCEX_ID_M4APP_FINISHED, 0);   // ws: sehr kritisch (M4 output fehlt ohne ..), obwohl ohne fkt ?
 	BWCET_T2(ST_TIC42);			// ----> wcet measure (IPC)
#ifdef USE_TICM4_BIT_TOGGLE
	SET_BIT(6, 2, 0);			// set bit GPIO6[2] --> PC_3
#endif
	t_ts++;
}

