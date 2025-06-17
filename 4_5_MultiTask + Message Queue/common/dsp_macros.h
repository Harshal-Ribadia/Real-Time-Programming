/// \file		 dsp_macros.h
/// \brief		 some DSP macros.
/// \author		 Wolfgang Schulter 
/// \date		 30.08.2013 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __DSP_MACROS_H__
#define __DSP_MACROS_H__  	  	// only once ..

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


#endif // __DSP_MACROS_H__
