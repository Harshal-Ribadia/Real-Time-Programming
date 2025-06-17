// ----------------------------------------------------------------------------
/// \file		 dia_comm.h
/// \brief		 realizes diagnostic serial communication.
/// \author		 Wolfgang Schulter 
/// \date		 17.05.2012 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __DIA_COMM_H__
#define __DIA_COMM_H__  	  	// only once ..


extern unsigned short comm_analyze_command_working;

#define NRXBUF	1024 	//64

#define NTXT (4*32)
extern char txt[NTXT];

extern char _rxbuf[NRXBUF];
extern xSemaphoreHandle semDIAC;	// semaphore dia_comm task activation

// void comm_tic();			// comm tick function
void comm_newchar(unsigned char c);
void comm_analyze_command(char *buf, int nbuf);
void vdia_commTask(void * pvParameters);

#endif // __DIA_COMM_H__
