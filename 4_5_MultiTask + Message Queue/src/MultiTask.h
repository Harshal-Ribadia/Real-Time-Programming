// ----------------------------------------------------------------------------
/// \file		 PingPong.h
/// \brief		 RTP-Lab intro.
/// \author		 Wolfgang Schulter 
/// \license	 for educational purposes only, no warranty, see license.txt
/// \date		 14.01.2013 ws:  initial version

#ifndef __PINGPONG_H__
#define __PINGPONG_H__  	  	  // only once ..

#include "mcommon.h"  	// commons for all modules

// ----------------------------------------------------------------------------
// --- variables to be accessed from outside (extern)
// ----------------------------------------------------------------------------
extern xSemaphoreHandle semPing;			// semaphore Ping
extern xSemaphoreHandle semPong;			// semaphore Pong

extern uint16_t count_print;				// count variable printed

// ----------------------------------------------------------------------------

// ----------------------------------------------------------
// --- 4 functions to be accessed
// ----------------------------------------------------------
void init_Multitasking(void);
void tBlinkingTask(void *pvParameters);
void tShiftingTask(void *pvParameters);
void tWorkerTask(void *pvParameters);


// ----------------------------------------------------------------------------

#endif // __PINGPONG_H__
