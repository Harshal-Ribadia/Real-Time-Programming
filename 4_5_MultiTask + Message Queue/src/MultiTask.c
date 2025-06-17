// ----------------------------------------------------------------------------
/// \file		 PingPong.c
/// \brief		 RTP-Lab intro.
/// \author		 Wolfgang Schulter
/// \license	 for educational purposes only, no warranty, see license.txt
/// \date		 14.01.2013 ws:  initial version
// ----------------------------------------------------------------------------

#include <MultiTask.h>
#include <MsgQueue.h>
#include "queue.h"// PingPong module header

// ----------------------------------------------------------
// --- variables
// ----------------------------------------------------------
xQueueHandle xQueueRX;

xSemaphoreHandle semPing; // semaphore Ping
xSemaphoreHandle semPong; // semaphore Pong

uint16_t count = 0;					// count variable incremented by ping
uint16_t count_print = 0;			// count variable printed by PrintTask

#define TICK_RATE_KHZ	(configTICK_RATE_HZ/1000)
uint16_t delay1 = 100*TICK_RATE_KHZ;		// number of ticks <=> ms delay
uint16_t delay2 = 50*TICK_RATE_KHZ;
uint16_t delay3 = 80*TICK_RATE_KHZ;

uint8_t do_printf = 0;				// print task: printf cout variable to stdout, if 1

// Pattern for shifting LEDs 2…7 (10 states)
static const uint8_t shiftPattern[10][2] = {
    {2,3}, {3,4}, {4,5}, {5,6}, {6,7},
    {7,6}, {6,5}, {5,4}, {4,3}, {3,2}
};

// ----------------------------------------------------------
// --- void init_Multitasking(){} - create semaphores
// ----------------------------------------------------------
void init_Multitasking(void) {
	// vSemaphoreCreateBinary(semPing);
     //     vSemaphoreCreateBinary(semPong);
//  xSemaphoreTake(semPing, 0); // prevent Pong from starting first
}

// ----------------------------------------------------------
// --- void tBlinkingTask(void * pvParameters){} - toggle_LED0/1
// ----------------------------------------------------------
void tBlinkingTask(void *pvParameters) {

	    _Bool toggle = 0;
	    for (;;)
	    {
	        // Alternate LED0/LED1
	        Board_LED_Set(0, toggle);
	        Board_LED_Set(1, !toggle);
	        toggle = !toggle;
	        vTaskDelay(delay1);
	    }
}
// ----------------------------------------------------------
// --- void tShiftingTask(void * pvParameters){} - shift LEDs 2–7
// ----------------------------------------------------------
void tShiftingTask(void *pvParameters) {
    uint16_t idx = 0;
    uint16_t i; //
    for (;;) {
        // Turn all LEDs 2–7 off
        for (i = 2; i <= 7; i++) {
            Board_LED_Set(i, false);
        }

        // Light exactly two LEDs as per pattern
        Board_LED_Set(shiftPattern[idx][0], true);
        Board_LED_Set(shiftPattern[idx][1], true);

        vTaskDelay(delay2);
        idx = (idx + 1) % 10;
    }
}

// ----------------------------------------------------------
// --- void tControlTask(void * pvParameters){} - control via joystick
// ----------------------------------------------------------
void tWorkerTask(void *pvParameters) {
    char c;

    while (1) {
        if (xQueueReceive(xQueueRX, &c, portMAX_DELAY) == pdTRUE) {
            switch (c) {
                case 'B':
                    vTaskSuspend(hTasksM4[ST_PING]);
                    break;
                case 'b':
                    vTaskResume(hTasksM4[ST_PING]);
                    break;
                case 's':
                    vTaskSuspend(hTasksM4[ST_PONG]);
                    break;
                case 'S':
                    vTaskResume(hTasksM4[ST_PONG]);
                    break;
                case '+':
                    delay1 = MIN(10000, delay1 + 10);
                    delay2 = MIN(10000, delay2 + 10);
                    break;
                case '-':
                    delay1 = MAX(1, delay1 - 10);
                    delay2 = MAX(1, delay2 - 10);
                    break;
                default:
                    break;
            }
        }
        vTaskDelay(delay3);
    }
}
