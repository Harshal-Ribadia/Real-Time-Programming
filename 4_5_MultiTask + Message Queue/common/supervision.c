// ----------------------------------------------------------------------------
/// \file		 supervision.c
/// \brief		 supervision functions (stack/execution time)
/// \author		 Wolfgang Schulter 
/// \license	 for educational purposes only, no warranty, see license.txt
/// \date		 06.03.2012 ws:  initial version
/// \date		 18.05.2012 ws:  struct for stack/bwcet supervision, vSuperTask
/// \date		 23.05.2012 ws:  selectable timer resolution 8,16,32 bit
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// FreeRTOS includes
// ----------------------------------------------------------------------------
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "supervision.h" 	// supervision module (stack, exec time)
#include "mcommon.h"  		// commons for all modules

#include "ipc_app.h"		// IPC module (M0/M4 app comm)

uint32_t idle_tick_count;	// ws: added, idle tick count

#ifdef CORE_M0				// includes for buffer checks
SUPER_TASK st[ST_NUM];		// array of stack/bwcet supervised tasks
const char *st_task_names[ST_NUM] = INIT_TASK_NAME_STRINGS;
#include "dia_comm.h"		// ASCII dia comm module (M0)
#elif CORE_M4
#include "dia_comm4.h"		// ASCII dia comm module (M4)
SUPER_TASK *pst = 0;		// pointer to array of stack/bwcet supervised tasks (CORE_M0)
xTaskHandle hTasksM4[ST_NUM];		// array of M4 task handles
#endif

#ifdef CORE_M0				//

volatile unsigned char do_print_stack = 0; // 1;	// cyclic print of stack supervision
volatile unsigned char do_print_bwcet = 0; // 1;	// cyclic print of b/wcet   "


#ifdef configCHECK_FOR_STACK_OVERFLOW
#if (configCHECK_FOR_STACK_OVERFLOW > 0)

#include "StackMacros.h"

void init_SuperTask()
{
	int i;
	// set traceable task numbers
	for (i=0; i < ST_NUM; i++) {
		st[i].hTask = 0;  // ws: new, M4 tasks must have handle 0 !
		// vTaskSetTaskNumber(st[i].hTask, i+1);
	}
}

#define BWCET_TIME_US	1000000		// new in 4.0: fixed measure time for B/WCET

// ----------------------------------------------------------------------------
// the task supervision task
// ----------------------------------------------------------------------------
void vSuperTask(void *pvParameters)
{
	portTickType xLastWakeTime;
	static unsigned char tic = 0;
	// unsigned portBASE_TYPE stack_hBlinkTask, stack_hADDATask, stack_hEMACTask, stack_hUARTRXTask; //stack_hUIPTask;

	xLastWakeTime = xTaskGetTickCount();

	// init_SuperTask();  done in main !

	init_wcet();
	wcet_init(&BWCET_SUPER);		// <---- wcet init

	vTaskDelayUntil(&xLastWakeTime, (60000/portTICK_RATE_US) );

	while (1) {

		vTaskDelayUntil(&xLastWakeTime, (100000/portTICK_RATE_US) );  // new in 4.0: wake up each 100 ms

		// wcet_t1(&BWCET_SUPER);	// <---- wcet measure
		if (do_print_bwcet) {

			// -------------------------------------
			// execution time supervision
			// -------------------------------------
			// if (do_print_bwcet) {  //  && ((tic & 7) == 4)) { // every .. s
			short i;
			// print_super_bwcet(st, ST_NUM);

			for (i = 0; i < ST_NUM; i++) {
				if (st[i].bwcet.n >= st[i].bwcet.n_re_init)
					if (st[i].bwcet.binit)  {
						wcet_init(&st[i].bwcet);	// <---- wcet re-init
					}
			}
			idle_tick_count = 0;		//
			// }
			wcet_t1(&BWCET_SUPER);		// <---- wcet measure (for SUPER_TASK)
			vTaskDelay(BWCET_TIME_US/portTICK_RATE_US);  		// new in 4.0: wait for 1s
			print_super_bwcet(st, ST_NUM);
			do_print_bwcet = MAX(0, do_print_bwcet-1);			// new in 4.0: do it a limited time only
		}

#ifdef configCHECK_FOR_STACK_OVERFLOW
#if (configCHECK_FOR_STACK_OVERFLOW > 0)
		// -------------------------------------
		// stack supervision
		// -------------------------------------
		if (do_print_stack) { // every .. s
			// unsigned int do_print;
			update_super_stack(); // st, ST_NUM);
			if (do_print_stack) {
				print_super_stack(); //st, ST_NUM);
			}
			do_print_stack = MAX(0, do_print_stack-1);			// new in 4.0: do it a limited time only

			print_buffer_status();
		}
#endif
#endif
		wcet_t2(&BWCET_SUPER);	// <---- wcet measure
		tic++;
	}
}

// ----------------------------------------------------------------------------
// the task supervision task --> in FreeRTOSCommonHooks.c
// ----------------------------------------------------------------------------
/* void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	err_blink(1);
	/* This function will get called if a task overflows its stack. *

	( void ) pxTask;
	( void ) pcTaskName;
	long i, n=100000;

	while (1) 
	{
		// LEDs blinking the task with stack overflow !
		for (i=0; i<n; i++)
			;//LPC_GPIO2->FIOPIN = ~((unsigned char)pcTaskName[0]);
		for (i=0; i<n; i++)
			;//LPC_GPIO2->FIOPIN = 0;     // LEDs off
	}
}*/
#endif
#endif

// -----------------------------------------------------------
// stack supervision 
// -----------------------------------------------------------
/*unsigned int compare_stack(unsigned portBASE_TYPE act_stack, unsigned portBASE_TYPE *stored_stack)
{
	unsigned int do_print = 0;
	if (act_stack != *stored_stack) {
		do_print = 1;
		*stored_stack = act_stack;
	}
	return do_print;
}*/

// -----------------------------------------------------------
// stack supervision for a set of n tasks in the sst[] array
// -----------------------------------------------------------
int update_super_stack() // SUPER_TASK st[], int n)
{
	int i, change = 0;

	IPC_PUSH(IPCEX_ID_UPDATE_SUPER_STACK, ST_TIC41);	// new in 4.0: notify M4 (app)
	vTaskDelay(10000/portTICK_RATE_US);

	for (i = 0; i < ST_NUM; i++) {
		if (st[i].hTask) {					// M0 task
			if (st[i].bwcet.binit) {		// FreeRTOS Task ?
				st[i].stack_left_last = st[i].stack_left_act;
				st[i].stack_left_act = uxTaskGetStackHighWaterMark(st[i].hTask);
				if (st[i].stack_left_last != st[i].stack_left_act) change = 1;
			}
		}
	}
	return change;
}

// -----------------------------------------------------------
// print stack bytes left for a set of n tasks in the sst[] array
// -----------------------------------------------------------
void print_super_stack()	// SUPER_TASK st[], int n)
{
	int i;
	PRINTF("\nStack\n"); //_DBD(tic); _DBG("\n");
	for (i = 0; i < ST_NUM; i++) {
		char *pTaskName = st_task_names[i]; // pcTaskGetTaskName(st[i].hTask);
		//_DBG(pTaskName); _DBG(":");
		//_DBD(st[i].stack_left_act); _DBG(" ");
		// DEBUGOUT("%d ",st[i].stack_left_act);
		// PRINTF("%s[%x,%d]:%d ",pTaskName, st[i].hTask, st[i].bwcet.binit, st[i].stack_left_act);
		PRINTF("%s:%d ",pTaskName, st[i].stack_left_act);
	}
	// _DBG("\nHeap: "); _DBD16(xPortGetFreeHeapSize());
	PRINTF("\n");
	// PRINTF("\nHeap: %d\n", xPortGetFreeHeapSize());
}

// -----------------------------------------------------------
// print bytes left on Heap, important buffers
// -----------------------------------------------------------
/*typedef struct _t_buffer_status {
	uint16_t heap;
	uint16_t nQprint;
	uint16_t n_db;
	uint16_t n_rxBuf;
} BUFFER_STATUS;

BUFFERSTATUS bufstat;*/

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

// -----------------------------------------------------------
// print stack bytes left for a set of n tasks in the sst[] array
// -----------------------------------------------------------
// 204/256 * 20/16 ~ 1
// #define  portGET_RUN_TIME_COUNTER_VALUE()	((20*(Chip_RIT_GetCounter(LPC_RITIMER) >> 4)) >> 8)
unsigned int get_us(unsigned int T)
{
	//static float kt = 1.0/204;		// SysTick at 204 MHz
	static float kt = 1.0;			// TIMER at 1 MHz
	return T*kt;
	// return T;

}

void print_super_bwcet()	// SUPER_TASK st[], int n)
{
	unsigned int i;
	PRINTF("\nBWCET(%d) - M0 idle_tick_count = %u\n", ST_NUM, idle_tick_count); //_DBD(tic); _DBG("\n");
	for (i = 0; i < ST_NUM; i++) {
		char *pTaskName = st_task_names[i];// pcTaskGetTaskName(st[i].hTask);
		unsigned int H = 0, prio = 0, Tp = 0;
//		if (!st[i].bwcet.binit) /* && !st[i].bwcet.dt)*/ continue;	// no deactivated tasks

		if (st[i].hTask) // FreeRTOS Task ?
			prio = uxTaskPriorityGet(st[i].hTask);
		// if (st[i].bwcet.dt1 && st[i].bwcet.wcet)
		// 	H = st[i].bwcet.wcet*100/st[i].bwcet.dt1;
		if (st[i].bwcet.n && st[i].bwcet.wcet) {
			Tp = BWCET_TIME_US / st[i].bwcet.n;
			H = 100*st[i].bwcet.wcet / Tp;
		}
		PRINTF("%02d:%s(%u): n=%u dt=%d b=%d w=%d Tp=%d us --> H%s = %d %%\n", i, pTaskName, prio,
				st[i].bwcet.n, get_us(st[i].bwcet.dt), get_us(st[i].bwcet.bcet), get_us(st[i].bwcet.wcet),
				Tp,  pTaskName, H);
	}
	/*	sprintf(_db,"%02d:%s(%u): n=%u dt=%d b=%d w=%d d1=%d --> H%s = %d %%\n", i, pTaskName, prio,
				st[i].bwcet.n,st[i].bwcet.dt, st[i].bwcet.bcet, st[i].bwcet.wcet, st[i].bwcet.dt1,
				pTaskName, H); DB;
	}*/
	// _DBG("\n");
}
#endif // CORE_M0

// -----------------------------------------------------------
void print_buffer_status()
{
#ifdef CORE_M0
	extern short nQmax;
	const char *cpu = "M0";
	ipcex_msg_t msg = {IPCEX_ID_BUFFER_STATUS, 0};	// 1 = AD_READY

	IPC_pushMsg(&msg);			// new in 4.0: notify M4 (app, dia_comm4)
	// PRINTF("\n%s\n",buf);
	vTaskDelay(1);
	PRINTF("\nBuffers free bytes (%s): \nHeap = %d, n_db = %d, n_rxBuf = %d, nQdbg = %d\n", cpu,
		xPortGetFreeHeapSize(), get_free_str(_db, NRXBUF), get_free_str(_rxbuf, N_DB), nQmax);
#elif CORE_M4
	const char *cpu = "M4";
	vTaskDelay(1);
	PRINTF("\nBuffers free bytes (%s): \nHeap = %d, n_db = %d, n_rxBuf = %d\n", cpu,
		xPortGetFreeHeapSize(), get_free_str(_db, NRXBUF), get_free_str(_rxbuf, N_DB));
#endif
}

// -----------------------------------------------------------
void wcet_init(BWCET *pbwcet)
{
	pbwcet->t1 = pbwcet->dt = pbwcet->wcet = 0;
	pbwcet->n = 0;
	pbwcet->bcet = 0; //65535;
	pbwcet->brunning = 0;
	pbwcet->binit = 1;
}

// -----------------------------------------------------------
void wcet_t1(BWCET *pbwcet)
{
	pbwcet->brunning = 1;  // stopwatch started

#if (USE_TIMER_BITS==16)
	// stopwatch function: start watch at time t1
	unsigned short t1 = portGET_RUN_TIME_COUNTER_VALUE();  // free-running timer
	int dt1;
	if (pbwcet->n > 0) {		// calc delta t1
		dt1 = t1 - pbwcet->t1;
		if (dt1 < 0) dt1 += 65536;
		pbwcet->dt1 = dt1;
	}
	pbwcet->t1 = t1;
#elif (USE_TIMER_BITS==8)
	// stopwatch function: start watch at time t1
	unsigned char t1 = portGET_RUN_TIME_COUNTER_VALUE();  // free-running timer
	int dt1;
	if (pbwcet->n > 0) {		// calc delta t1
		dt1 = t1 - pbwcet->t1;
		if (dt1 < 0) dt1 += 256;
		pbwcet->dt1 = dt1;
	}
	pbwcet->t1 = t1;
#elif (USE_TIMER_BITS==32)
	// stopwatch function: start watch at time t1
	unsigned int t1;
	long long dt1;
	//vPortEnterCritical();
	t1 = (unsigned int)portGET_RUN_TIME_COUNTER_VALUE();  // free-running timer
	//vPortExitCritical();
	if (pbwcet->n > 0) {		// calc delta t1
		dt1 = (long long)t1 - pbwcet->t1;
		if (dt1 < 0) dt1 += ((long long)1 << 32);
		pbwcet->dt1 = dt1;
	}
	pbwcet->t1 = t1;
#endif
}

// -----------------------------------------------------------
void wcet_t2(BWCET *pbwcet)
{
	if (!pbwcet->brunning) return;

	pbwcet->brunning = 0;  // stopwatch stop
	// stopwatch function: stop watch at time t2

#if (USE_TIMER_BITS==16)
	int dt, t1, t2;
	unsigned short bc, wc;
	
	t1 = pbwcet->t1;
	t2 = portGET_RUN_TIME_COUNTER_VALUE();  // free-running timer
	
	if (t2 >= t1) {
		dt = t2-t1;
	} else {
		// dt = 256+t2-t1;	// 16-bit counter
		dt = 65536+t2-t1;	// 16-bit counter
	}
	pbwcet->dt = dt;
	if (pbwcet->n > 0) {		// calc delta t1
		bc = MIN(pbwcet->bcet, (unsigned short)dt);  // best-case exec time
		wc = MAX(pbwcet->wcet, (unsigned short)dt);  // worst-case exec time
	} else
		bc = wc = dt;
	pbwcet->bcet = bc; //pbwcet->bcet - (pbwcet->bcet >> 4) + (bc >> 4);
	pbwcet->wcet = wc; //pbwcet->wcet - (pbwcet->wcet >> 4) + (wc >> 4);
	pbwcet->n = MIN(pbwcet->n+1,65535);
#elif (USE_TIMER_BITS==8)
#error "not implemented yet"
#elif (USE_TIMER_BITS==32)
	long long dt;
	unsigned int t1, t2;
	unsigned int bc, wc;

	t1 = pbwcet->t1;
	//vPortEnterCritical();
	t2 = (unsigned int)portGET_RUN_TIME_COUNTER_VALUE();  // crasht hier !!
	//vPortExitCritical();

	if (t2 >= t1) {
		dt = t2-t1;
	} else {
		dt = t2-t1+((long long)1 << 32);	//32-bit counter
	}
	pbwcet->dt = dt;
	if (pbwcet->n > 0) {		// calc delta t1
		bc = MIN(pbwcet->bcet, (unsigned int)dt);  // best-case exec time
		wc = MAX(pbwcet->wcet, (unsigned int)dt);  // worst-case exec time
	} else
		bc = wc = dt;
	pbwcet->bcet = bc; //pbwcet->bcet - (pbwcet->bcet >> 4) + (bc >> 4);
	pbwcet->wcet = wc; //pbwcet->wcet - (pbwcet->wcet >> 4) + (wc >> 4);
	pbwcet->n = MIN(pbwcet->n+1,0xFFFFFFFF);
#endif
}

// -----------------------------------------------------------
void init_wcet()
{
	uint32_t tc = 0; //Chip_Timer_GetClock(LPC_TIMER0);
	uint32_t ps = Chip_TIMER_ReadPrescale(LPC_TIMER1);

	// power-up timer 0 for portGET_RUN_TIME_COUNTER_VALUE() macro
	Chip_TIMER_Init(LPC_TIMER1);
	Chip_TIMER_PrescaleSet(LPC_TIMER1, 204);
	Chip_TIMER_Enable(LPC_TIMER1);

	// PRINTF("tc = %u, ps = %u\n", tc,ps);

#ifdef __USE_OWN_TIMER
	// Initialize timer, prescale count time of 1 uS
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	// TIM_MATCHCFG_Type TIM_MatchConfigStruct;

	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1; //10;//

	//TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_Init(LPC_TIM1, TIM_TIMER_MODE,&TIM_ConfigStruct);
	//TIM_Init(LPC_TIM2, TIM_TIMER_MODE,&TIM_ConfigStruct);

	// use channel 0, MR0
	/*
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = FALSE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	// Set Match value
	TIM_MatchConfigStruct.MatchValue   = T1*10;
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	toggle=TRUE;
	// preemption = 1, sub-priority = 1
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));
	// Enable interrupt for timer 0
	NVIC_EnableIRQ(TIMER0_IRQn);
	NVIC_SetPriority(TIMER0_IRQn, ((0x01<<3)|0x01));*/

	//NVIC_DisableIRQ(TIMER1_IRQn);
	//TIM_ResetCounter(LPC_TIM0);

	//TIM_Cmd(LPC_TIM0,ENABLE);
	TIM_Cmd(LPC_TIM1,ENABLE);
	//TIM_Cmd(LPC_TIM2,ENABLE);

	//LPC_TIM0->IR = 0x00;  // disable Timer0 IRQ

	/*LPC_SC->PCONP |= 0x02UL;			// power up timer 0
	LPC_SC->PCLKSEL0 = ( LPC_SC->PCLKSEL0 & (~(0x03<<2) ) ) | (0x01 << 2);

	LPC_TIM0->TCR = 0x02;				// reset timer
	//LPC_TIM0->CTCR = CTCR_CTM_TIMER;	// run continuously

	LPC_TIM0->PR = ( configCPU_CLOCK_HZ/50000 ) - 1UL;	// 50 kHz -> 20 us
	LPC_TIM0->TCR = 0x01;				// start timer
	*/
#endif
}

