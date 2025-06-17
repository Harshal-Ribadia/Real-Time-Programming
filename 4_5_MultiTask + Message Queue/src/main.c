// ----------------------------------------------------------------------------
// @file	main.c
// @brief   RTP_PingPong application (Keil MCB4357 Board), UDP DataMonitor
// @version 4.0.14 	(derived from FR_WSFilt 4.0.13)
// @date    26. Apr 2014
//
// @license	for educational purposes only, no warranty, see license.txt
// @note	Copyright (C) W. Schulter
//
// @par
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// general includes
// ----------------------------------------------------------------------------
#include <MultiTask.h>  	// PingPong module header
#include <MsgQueue.h>
#include <stdio.h>
#include "lpc43xx_dualcore_config.h"

#include "FreeRTOS.h"
#include "task.h"

#include "queue.h"
#include "semphr.h"

// ----------------------------------------------------------------------------
// project module includes
// ----------------------------------------------------------------------------
#include "mcommon.h"  	// commons for all modules

#include "app.h"		// app (filter) module
#include "dia_comm4.h"	// ASCII diagnostic comm module (M4)

#include "uart.h"		// UART module
#include "UartTX.h"  	// UartTX module header, since 4.3


const char IDstr[] = ABOUT1 "," __DATE__ " " __TIME__ "\n";

unsigned short fg1_Hz = FS_HZ/2;		// since 4.0.3: default cut-off frequency fg1 in Hz

char _db[N_DB];			// string for debugging

// ----------------------------------------------------------------------------
int _d[N_D];			// general purpose debug var's
int *p_d_m0 = 0;		// pointer to _d[] array in M0, since .14
int _d_1[N_D];			// general purpose debug var's, prev values
// ----------------------------------------------------------------------------

// Macro that calculates the start address of M0 image
#define M0_IMAGE_ADDR  (IMAGE_BASE_ADDR + M0_IMAGE_OFFSET)

/*****************************************************************************
 * Public functions
 ****************************************************************************/
/* Milli-second sleep function */
void MSleep(int32_t msecs)
{
	int32_t curr = (int32_t) Chip_RIT_GetCounter(LPC_RITIMER);
	int32_t final = curr + ((SystemCoreClock / 1000) * msecs);

	/* If the value is zero let us not worry about it */
	if (!msecs || (msecs < 0)) {
		return;
	}

	if ((final < 0) && (curr > 0)) {
		while (Chip_RIT_GetCounter(LPC_RITIMER) < (uint32_t) final) {}
	}
	else {
		while ((int32_t) Chip_RIT_GetCounter(LPC_RITIMER) < final) {}
	}
}

// ----------------------------------------------------------------------------
// Micro-second sleep function (ws)
// ----------------------------------------------------------------------------
void uSleep(int32_t usecs)
{
	int32_t curr = (int32_t) Chip_RIT_GetCounter(LPC_RITIMER);
	int32_t final = curr + ((SystemCoreClock / 1000000) * usecs);

	/* If the value is zero let us not worry about it */
	if (!usecs || (usecs < 0)) {
		return;
	}

	if ((final < 0) && (curr > 0)) {
		while (Chip_RIT_GetCounter(LPC_RITIMER) < (uint32_t) final) {}
	}
	else {
		while ((int32_t) Chip_RIT_GetCounter(LPC_RITIMER) < final) {}
	}
}

// ----------------------------------------------------------------------------
void err_blink(uint32_t bitmask)
{
	volatile uint32_t i;
	while(1)  {  // forever
		Board_LED_Toggle(0);
		if (bitmask)	Board_LED_Toggle(3);
		// Board_LED_Toggle(1);
		//Board_LED_Toggle(6);
		Board_LED_Toggle(7);
		for (i=0; i < 1000000; i++);
		Board_LED_Toggle(0);
		if (bitmask)	Board_LED_Toggle(3);
		//Board_LED_Toggle(1);
		//Board_LED_Toggle(6);
		Board_LED_Toggle(7);
		for (i=0; i < 1000000; i++);
	}
}

// ----------------------------------------------------------------------------
void __error__(char *pcFilename, unsigned long ulLine)
{
	err_blink(0xF0);  // raw debugging
}


/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* take SLAVE processor out of reset */
/*int IPC_startSlave(void)
{
	volatile uint32_t u32REG, u32Val;
	int ret = 0;

	// Release Slave from reset, first read status
	u32REG = LPC_RGU->RESET_ACTIVE_STATUS1;

	// If the M0 is being held in reset, release it...
	// 1 = no reset, 0 = reset
	while(!(u32REG & (1u << 24)))
	{
		ret = 1;
		u32Val = (~(u32REG) & (~(1 << 24)));
		LPC_RGU->RESET_CTRL1 = u32Val;
		u32REG = LPC_RGU->RESET_ACTIVE_STATUS1;
	};
	return ret;
}*/

// #define T3_MATCHVAL1	(204000/4)
#define T3_MATCHVAL2	(204000/2)

/*void TIMER3_IRQHandler(void)
{
	// static uint32_t t_ts_next = FREQ_TICK_HZ/2;	// initial match
	// extern xSemaphoreHandle sem_tic;			// basic tick semaphore

	// Clear interrupt
	Chip_TIMER_ClearMatch(LPC_TIMER3, 0);
	// NVIC_ClearPendingIRQ(TIMER0_IRQn);
	LPC_TIMER3->MR[0] += T3_MATCHVAL1;
	LPC_TIMER3->MR[1] += T3_MATCHVAL2;
}*/

// ----------------------------------------------------------------------------
// @brief	TIMER0 init (Tick Timer for M0/M4)
// @return	Nothing
// ----------------------------------------------------------------------------
void init_Tick_Timer(void)
{
	uint32_t clock_rate = SystemCoreClock; //204000000; //Chip_Clock_GetRate(CLK_MX_MXCORE); // CLK_MX_RITIMER);
	uint32_t div = clock_rate / FS_HZ;

	// Initialize Basic Tick Timer, TIMER1 since 4.0.3
	Chip_TIMER_Init(LPC_TIMER0);
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, div);  // ftick: TIMER0, compare reg 0
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);

	// seit 4.0.3 fg2 = fs/2 Erzeugung Alternative
	Chip_TIMER_SetMatch(LPC_TIMER0, 1, div);  		// TIMER0, compare reg 1
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 1);
	LPC_TIMER0->EMR |= ((uint32_t)3 << 6);			// --> toggle output T0_MAT1, pin P8_1
	Chip_SCU_PinMuxSet(8, 1, SCU_MODE_FUNC7);		// T0_MAT1 � Match output 1 of timer 0.

	Chip_TIMER_Enable(LPC_TIMER0);

	NVIC_SetPriority(TIMER0_IRQn, IRQ_PRIO_TIMER0); // ws: higher prio than EMAC-IRQ
	NVIC_EnableIRQ(TIMER0_IRQn);
}

// ----------------------------------------------------------------------------
// @brief	TIMER0 re-init (Tick Timer for M0/M4)
// @return	Nothing
// ----------------------------------------------------------------------------
int re_init_Tick_Timer(uint16_t _fs_Hz)
{
	uint32_t div;

	div = SystemCoreClock / _fs_Hz;

	// Chip_TIMER_DeInit(LPC_TIMER0);

	// Chip_TIMER_Disable(LPC_TIMER0);
	Chip_TIMER_Reset(LPC_TIMER0);
	// Initialize Basic Tick Timer
	// Chip_TIMER_Init(LPC_TIMER0);
	Chip_TIMER_SetMatch(LPC_TIMER0, 0, div);  // ftick: TIMER0, compare reg 0

	Chip_TIMER_SetMatch(LPC_TIMER0, 1, div);  		// TIMER0, compare reg 1

	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER0, 0);
	Chip_TIMER_MatchEnableInt(LPC_TIMER0, 0);
	Chip_TIMER_Enable(LPC_TIMER0);

	NVIC_SetPriority(TIMER0_IRQn, IRQ_PRIO_TIMER0); // ws: higher prio than EMAC-IRQ
	NVIC_EnableIRQ(TIMER0_IRQn);
	return 1;
}

// ----------------------------------------------------------------------------
// @brief	TIMER init (fg1 Timer for SC-Filter)
// @return	Nothing
// ----------------------------------------------------------------------------
void init_fg1_Timer(void)
{
	uint32_t div;

	div = SystemCoreClock / ((fg1_Hz*100) << 1);  // 100 fg1 (MAX74114) --> Pin PC_7

	// init fg1 Timer, TIMER0, Match 1 --> toggle output T3_MAT0, pin PC_8
	Chip_TIMER_Init(LPC_TIMER3);
	Chip_TIMER_SetMatch(LPC_TIMER3, 1, div);  				// TIMER1, compare match reg 1
	Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 1);
	//Chip_TIMER_ExtMatchControlSet(LPC_TIMER3, 0, 3, 1);  	// see Table 736 in UM10503.pdf
	LPC_TIMER3->EMR |= ((uint32_t)3 << 6);						// --> toggle output T3_MATx, pin PC_7
	Chip_SCU_PinMuxSet(0x0c, 8, SCU_MODE_FUNC6);				// T3_MAT1 � Match output 0 of timer 3.

	// init fg1 Timer, TIMER3, Match 0 --> toggle output T3_MAT1, pin PC_7
	/*Chip_TIMER_SetMatch(LPC_TIMER3, 0, T3_MATCHVAL1);  					// TIMER1, compare match reg 1
	// Chip_TIMER_ResetOnMatchEnable(LPC_TIMER3, 0);
	// Chip_TIMER_ExtMatchControlSet(LPC_TIMER3, 0, 3, 0);  		// see Table 736 in UM10503.pdf
	LPC_TIMER3->EMR |= (uint32_t)3 << 4;						// --> toggle output T3_MATx, pin PC_8
	Chip_SCU_PinMuxSet(0x0c, 7, SCU_MODE_FUNC6);				// T3_MAT0 � Match output 1 of timer 3.
	Chip_TIMER_MatchEnableInt(LPC_TIMER3, 0);*/

	Chip_TIMER_Enable(LPC_TIMER3);
	// NVIC_EnableIRQ(TIMER3_IRQn);
}

// @brief	TIMER0 re-init (Tick Timer for M0/M4)
// @return	Nothing
// ----------------------------------------------------------------------------
int re_init_fg1_Timer(uint16_t _fg1_Hz)
{
	uint32_t div;

	if (_fg1_Hz < 1) return 0;

	div = SystemCoreClock / ((_fg1_Hz*100) << 1);  // 100 fg1 (MAX74114) --> Pin PC_7

	Chip_TIMER_Reset(LPC_TIMER3);

	Chip_TIMER_SetMatch(LPC_TIMER3, 1, div);  // ftick: TIMER1, compare reg 1
}

/* initialization routine for dual core examples */
static void prvSetupHardware(void)
{
#ifdef CORE_M4
	int i;

	SystemCoreClockUpdate();	// seen in eeprom 1.03a lpcopen
	/* Re-initialize CGU for proper operation */
	Board_Init();

	uart_init();

	// ws: basic tick via common TIMER0 IRQ
	// init_Tick_Timer();

	/* Initialize the IPC Queue */
	IPCEX_Init();

	app_init();

	// ----------------------------------------------------------

	// ----------------------------------------------------------
	// --- init the PingPong module
	// ----------------------------------------------------------

	init_Multitasking();
	Init_MessageQueue();

	// ----------------------------------------------------------

	// ws: basic tick via common TIMER0 IRQ
	init_Tick_Timer();

	init_fg1_Timer();  // since 4.0.3: init fg1 timer for SC-Filter

	/* Time to Start M0 */
	if (M0Image_Boot((uint32_t) M0_IMAGE_ADDR) < 0) {
		Board_LED_Set(7, 1);
		while (1) {
			__WFI();
		}
	}
	// for (i=0; i < 1000000; i++); //
	//MSleep(5);

	// if (IPC_startSlave()) Board_LED_Set(6, 1);

#elif defined(CORE_M0)
	extern void prvSetupTimerInterrupt(void);

	/* Needs to be called coz durinig initializtion the
	 * global variable would have initialized to 0
	 */
	SystemCoreClockUpdate();

	#ifdef OS_FREE_RTOS
	/* Disable global interrupts */
	taskDISABLE_INTERRUPTS();
	prvSetupTimerInterrupt();
	#endif
#endif

	// MSleep(50);
	// init_i2s_audio();  // HQ Audio
	// Board_Audio_Init(LPC_I2S0, UDA1380_LINE_IN);
	// BLINKY_Init();
}

#define vAppTask_PRIO  (configMAX_PRIORITIES-1)			// high
#define vdia_commTask_PRIO 	(tskIDLE_PRIORITY + 8)   	// med

// ----------------------------------------------------------
#define vPingTask_PRIO  (tskIDLE_PRIORITY + 7)		// low
#define vPongTask_PRIO  (tskIDLE_PRIORITY + 7)		// low
#define vPrintTask_PRIO  (tskIDLE_PRIORITY + 7)		// low
// ----------------------------------------------------------
#define vUartTX_Task_PRIO	(tskIDLE_PRIORITY + 3)			// low
#define vUartRX_Task_PRIO	(tskIDLE_PRIORITY + 7)			// med

// ----------------------------------------------------------------------------
// Main for M4 app
// ----------------------------------------------------------------------------
int main()
{
	uint32_t ret = 1;
	// extern xSemaphoreHandle sem_tic;			// basic tick semaphore

	//vSemaphoreCreateBinary(sem_tic);		// the basic tick semaphore
	//xSemaphoreTake(sem_tic,portMAX_DELAY);	// <---- init to 0


	prvSetupHardware();

	// ------------------------------------------------------
	// create the Tasks Ping, Pong and Print
	// ------------------------------------------------------
	ret &= xTaskCreate(tBlinkingTask, 0, configMINIMAL_STACK_SIZE, 0, vPingTask_PRIO, &hTasksM4[ST_PING]);
	ret &= xTaskCreate(tShiftingTask, 0, configMINIMAL_STACK_SIZE, 0, vPongTask_PRIO, &hTasksM4[ST_PONG]);
	ret &= xTaskCreate(tControlTask, 0, configMINIMAL_STACK_SIZE, 0, vPrintTask_PRIO, &hTasksM4[ST_PRINT]);
	ret &= xTaskCreate(tWorkerTask, 0, configMINIMAL_STACK_SIZE, 0, vPrintTask_PRIO, &hTasksM4[ST_PRINT]);



	// ------------------------------------------------------
	// Start the basic application tasks ...
	// ------------------------------------------------------
	ret &= ipcex_tasks();
	// ret &= xTaskCreate(vAppTask, 0, configMINIMAL_STACK_SIZE, 0, vAppTask_PRIO, &hTasksM4[0]);
	ret &= xTaskCreate(vdia_commTask, 0, configMINIMAL_STACK_SIZE, 0, vdia_commTask_PRIO, &hTasksM4[ST_DIA4]);
	// ------------------------------------------------------
	// Start the UART tasks, if needed
	// ------------------------------------------------------
	ret &= xTaskCreate(vUartTX_Task, 0, configMINIMAL_STACK_SIZE, 0, vUartTX_Task_PRIO, 0);
	//ret &= xTaskCreate(vUartRX_Task, 0, configMINIMAL_STACK_SIZE, 0, vUartRX_Task_PRIO, 0);

	// ---- Start the scheduler
	if (ret) {

		printf("\n" ABOUT1 "\n");
		printf(ABOUT2 " " __DATE__ " " __TIME__ "\n");

		vTaskStartScheduler();

		// Control should never come here
		// DEBUGSTR("Schedule Failure\r\n");
	}

	err_blink(1);
	return 1;
}

