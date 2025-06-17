// ----------------------------------------------------------------------------
/// \file		 Data_Monitor.h
/// \brief		 Data Monitor with higher speed links (UDP)
/// \author		 Wolfgang Schulter
/// \date		 19.07.2013 ws:  LPC43xx version (M4/M0 common)
/// \date		 21.04.2012 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef DATA_MONITOR_H
#define DATA_MONITOR_H

#include <stdint.h>

#pragma GCC diagnostic error "-Warray-bounds"  // ws: new in 4.0.2, above bounds -_> error, instead of just warning

// -------------------------------------------
// new data monitoring by struct dm
// -------------------------------------------
#define NSIG	 4
#define NSHORT	10
#define NINT	10
#define NDOUBLE	 5
#define NFLOAT	 5
#define NLLONG	 0
// -------------------------------------------
#define NDM_VAR	(NSHORT+NINT+NDOUBLE+NFLOAT+NLLONG)		// num of dm variables
#define NDM_INF	(5+NDM_VAR)								// num of dm_inf entries
// -------------------------------------------
#define NINF_DESC	16			// length of each descriptor string
// -------------------------------------------
typedef struct _dmon {
	uint8_t  sig[NSIG];			// stamp, num
	char   inf[NINF_DESC];		// new in 1.1 cyclic transmission of dm_inf
	int16_t  s[NSHORT];			// shorts
	int32_t	 i[NINT];			// integers
#if NLLONG>0
	int64_t	 l[NINT];			// long integers
#endif
#if NDOUBLE>0
	double	 d[NDOUBLE];		// doubles
#endif
#if NFLOAT>0
	float	 f[NFLOAT];			// floats
#endif
} DMON_STRUCT;

// -------------------------------------------
// new simple element descriptors by struct dm_inf
typedef struct _dmon_inf {
	char  desc[NDM_INF][NINF_DESC];	// inf entries, sent by cyclic transmisssion in dm
} DMON_INF_STRUCT;

// -------------------------------------------
// new in 4.0.6: dynamic DM pointer table (--> D command)
// -------------------------------------------
typedef struct _dmon_ptr {
	void *ps[NSHORT];		// shorts
	// char ps_txt[NSHORT][8];	// dynamic txt description for a short (since v.11)
	void *pi[NINT];			// integers
	// char pi_txt[NINT][8];	// dynamic txt description for an int (since v.11)
#if NLLONG>0
	void *pl[NLLONG];		// long integers
	// char pl_txt[NLLONG][8];	// dynamic txt description for a long (since v.11)
#endif
#if NDOUBLE>0
	void *pd[NDOUBLE];			// doubles
	// char pd_txt[NDOUBLE][8];	// dynamic txt description for a double (since v.11)
#endif
#if NFLOAT>0
	void *pf[NFLOAT];			// floats
	// char pf_txt[NFLOAT][8];		// dynamic txt description for a float (since v.11)
#endif
} DMON_PTR_STRUCT;

typedef struct _dmon_len {
	char ps[NSHORT];		// shorts
	char pi[NINT];			// integers
#if NLLONG>0
	char pl[NINT];			// long integers
#endif
#if NDOUBLE>0
	char pd[NDOUBLE];		// doubles
#endif
#if NFLOAT>0
	char pf[NFLOAT];		// floats
#endif
} DMON_LEN_STRUCT;

#ifdef CORE_M0
extern DMON_STRUCT dm;					// DM task/struct is on M0
// extern const DMON_INF_STRUCT dm_inf;	// new in 4.0/1.1: DM_INF struct
// !const								// in dm_inf RAM since v.11
extern DMON_INF_STRUCT dm_inf;			// new in 4.0/1.1: DM_INF struct
extern DMON_PTR_STRUCT dm_ptr;			// new in 4.0.6: dynamic DM pointer table (--> D command)
extern DMON_LEN_STRUCT dm_len;			// new in 4.0.6: dynamic DM pointer length table (--> D command)

// set dm_inf macros (v.18)
#define SET_DMS_INF(IND,TXT)  {{unsigned char *p = &dm_inf.desc[5+(IND)][5]; strncpy(p, (TXT), 9); }}  // since v.11
#define SET_DMI_INF(IND,TXT)  {{unsigned char *p = &dm_inf.desc[5+(IND)+NSHORT][5]; strncpy(p, (TXT), 9);}}  // since v.11
#define SET_DMD_INF(IND,TXT)  {{unsigned char *p = &dm_inf.desc[5+(IND)+NSHORT+NINT][5]; strncpy(p, (TXT), 9); }}  // since v.11
#define SET_DMF_INF(IND,TXT)  {{unsigned char *p = &dm_inf.desc[5+(IND)+NSHORT+NINT+NDOUBLE][5]; strncpy(p, (TXT), 9); }}  // since v.11
#elif CORE_M4
extern DMON_STRUCT *pdm;				// pointer to DM struct
extern DMON_INF_STRUCT *pdm_inf;
#define SET_DMS(IND,VAL)	{if (pdm) pdm->s[IND] = (VAL);}
#define SET_DMI(IND,VAL)	{if (pdm) pdm->i[IND] = (VAL);}
#define SET_DMF(IND,VAL)	{if (pdm) pdm->f[IND] = (VAL);}
#define SET_DMD(IND,VAL)	{if (pdm) pdm->d[IND] = (VAL);}
// set dm_inf macros (v.11)
#define SET_DMS_INF(IND,TXT)  {if (pdm_inf) {unsigned char *p = &pdm_inf->desc[5+(IND)][5]; strncpy(p, (TXT), 9); }}  // since v.11
#define SET_DMI_INF(IND,TXT)  {if (pdm_inf) {unsigned char *p = &pdm_inf->desc[5+(IND)+NSHORT][5]; strncpy(p, (TXT), 9);}}  // since v.11
#define SET_DMD_INF(IND,TXT)  {if (pdm_inf) {unsigned char *p = &pdm_inf->desc[5+(IND)+NSHORT+NINT][5]; strncpy(p, (TXT), 9); }}  // since v.11
#define SET_DMF_INF(IND,TXT)  {if (pdm_inf) {unsigned char *p = &pdm_inf->desc[5+(IND)+NSHORT+NINT+NDOUBLE][5]; strncpy(p, (TXT), 9); }}  // since v.11
#endif

// Funktionsprototypen
unsigned short DataMonitor_Send_Data(unsigned char *buf, unsigned short nbuf); // ws
void DataMonitor_Init(void);

extern unsigned short dmon_mode;	// Data Monitor Mode on/off
#endif

