// ----------------------------------------------------------------------------
/// \file		 app.h
/// \brief		 basic application.
/// \author		 Wolfgang Schulter 
/// \date		 12.07.2012 ws:  initial version

#ifndef __APP_H__
#define __APP_H__  	  	  // only once ..

enum CMD_STATES {CMD_STATE_0, CMD_STATE_UP, CMD_STATE_DOWN, CMD_STATE_RIGHT, CMD_STATE_LEFT};

enum APP_AUTO_STATES {APP_AUTO_STATE_0, APP_AUTO_STATE_1, APP_AUTO_STATE_UP, APP_AUTO_STATE_DOWN, APP_AUTO_STATE_WAIT};

// ----------------------------------------------------------------------------
// --- app variables (all elements appear in map-file) ----
// ----------------------------------------------------------------------------
extern unsigned char cmd_state, cmd_state_1;
extern unsigned char app_auto_state;		//!< app auto_mode on/off
extern short app_auto_pos[2];				//!< lo[0],hi[1] positions for auto_mode

#define FS_HZ			5000				// new in 4.0: initial sampling fs in Hz
#define TS_US 			((1UL << 20)/FS_HZ)	// initial sampling period in ~us

extern unsigned short fs_Hz;				//!< sampling frequency in Hz
extern unsigned int Ts_ns;					//!< sampling period in ~ns (since 4.0.11)
// ----------------------------------------------------------------------------

void app_init();		// --- init the application module
//short inc_filter_mode(short delta);

void tic4_1();			// new in 4.0: tic functions
void tic4_2();
void vAppTask(void * pvParameters);

#endif // __APP_H__
