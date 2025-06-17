// ----------------------------------------------------------------------------
/// \file		ws_strfun.c
/// \brief		small string functions (atoi, htoi,..)
/// \author		Wolfgang Schulter
/// \license	for educational purposes only, no warranty, see license.txt
/// \date		12.04.2013 ws:  buxfixes with ws_dtoa, ws_ftoa (1.00)
/// \date		17.05.2012 ws:  initial version
// ----------------------------------------------------------------------------

#include "ws_strfun.h"		// small string functions (atoi, htoi, ...)

// ws: for strtod.c
// #include <errno.h>
// #include <ctype.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
// #include <newlib_inc/stdlib.h>

// ----------------------------------------------------------------------------
int ws_isdigit(char c)
{
	if ((c >= '0') && (c <= '9'))
		return 1;
	else
		return 0;
}

// ----------------------------------------------------------------------------
int ws_isspace(char c)
{
	if ((c == ' ') || (c == '\t') || (c == '\n'))
		return 1;
	else
		return 0;
}

// ----------------------------------------------------------------------------
// ws: own simple memcpy
// ----------------------------------------------------------------------------
char *memcpy(char *s1, const char *s2, int n)
{
	char *ret = s1;  // default
	int i;
	if ((s1 == 0) || (s2 == 0)) return 0;

	if ((( (unsigned int)s1 & 3)==0) && (( (unsigned int)s2 & 3)==0) ) {	// & 3 == % 4
		int n4 = (n >> 2) << 2;
		int *pi1 = (int *)s1, *pi2 = (int *)s2;
		for (i = 0; i < n4; i += 4) {
			*pi1++ = *pi2++;
		}

		n = n - n4;
		if (n) {
			s1 += n4;
			s2 += n4;
			for (i = 0; i < n; i++) {
				*s1++ = *s2++;	// rest
			}
		}

	} else if ((( (unsigned int)s1 & 1)==0) && (( (unsigned int)s2 & 1)==0) ) {  // & 1 == % 2
		int n2 = (n >> 1) << 1;
		short *ps1 = (short *)s1, *ps2 = (short *)s2;
		for (i = 0; i < n2; i += 2) {
			*ps1++ = *ps2++;
		}

		n = n - n2;
		if (n) {
			s1 += n2;
			s2 += n2;
			// for (i = 0; i < n; i++) {
			*s1++ = *s2++;	// just once
			//}
		}
	} else {
		for (i = 0; i < n; i++) {
			*s1++ = *s2++;	// bytewise
		}
	}
	return ret;
	// return __builtin_memcpy(s1, s2, n);
}
/*char *memcpy(char *s1, const char *s2, int n)
{
	char *ret = s1;  // default
	int i;
	if ((s1 == 0) || (s2 == 0)) return 0;

	for (i = 0; i < n; i++) {
		*s1++ = *s2++;
	}
	return ret;
}*/

// ----------------------------------------------------------------------------
// ws: own simple memset
// ----------------------------------------------------------------------------
void * memset( void* ptr, int value, unsigned int num)
{
	void *ret = ptr;  // default
	char *p = (char *)ptr;

	unsigned int  i;
	if (ptr == 0) return 0;

	for (i = 0; i < num; i++) {
		*p++ = value;
	}
	return ret;
}

// ----------------------------------------------------------------------------
// ws: web found strstr O(n):
// http://leetcode.com/2010/10/implement-strstr-to-find-substring-in.html
// ----------------------------------------------------------------------------
char* strstr(char* haystack, char* needle)
{
	char *n, *h;
	for (;; ++haystack) {
		h = haystack;
		for (n = needle;; ++n, ++h) {
			if (!*n)
				return haystack;
			if ((*h) != (*n))
				break;
		}
		if (!*h)
			return 0;
	}
}

// ----------------------------------------------------------------------------
// ws: own simple strlen
// ----------------------------------------------------------------------------
#define STRLEN_MAX	1024
int strlen(char *s1)
{
	int ret = 0;  // default
	int i;
	if (s1 == 0) return 0;

	for (i = 0; i < STRLEN_MAX; i++) {
		if (*s1++)
			ret++;
		else
			break;
	}
	return ret;
}

// ----------------------------------------------------------------------------
// ws: own simple strncpy
// ----------------------------------------------------------------------------
char *strncpy(char *s1, char *s2, int n)
{
	char *ret = s1;  // default
	int i;
	if ((s1 == 0) || (s2 == 0)) return 0;

	for (i = 0; i < n; i++) {
		if (*s2)
			*s1++ = *s2++;
		else
			break;
	}
	*s1 = 0;	// delimiter
	return ret;
}

// ----------------------------------------------------------------------------
// ws: own simple strcpy
// ----------------------------------------------------------------------------
char *strcpy(char *s1, char *s2)
{
	char *ret = s1;  // default
	int i;
	if ((s1 == 0) || (s2 == 0)) return 0;

	for (i = 0; i < STRLEN_MAX; i++) {
		if (*s2)
			*s1++ = *s2++;
		else
			break;
	}
	*s1 = 0;	// delimiter
	return ret;
}

// ----------------------------------------------------------------------------
// ws: own simple strcmp
// ----------------------------------------------------------------------------
int strcmp(const char *s1, const char *s2)
{
	int i, cmp = 0;
	if ((s1 == 0) || (s2 == 0)) return -1;

	for (i = 0; i < STRLEN_MAX; i++) {
		char c1 = *s1++;
		char c2 = *s2++;
		if (!c1 || !c2) break;
		if (c1 != c2) return i+1;
	}
	return cmp;
}

// ----------------------------------------------------------------------------
// ws: web found strcat
// http://stackoverflow.com/questions/2488563/strcat-implementation
// ----------------------------------------------------------------------------
char *strcat(char *dest, const char *src)
{
    unsigned int i,j;
    for (i = 0; dest[i] != '\0'; i++) ;

    for (j = 0; src[j] != '\0'; j++)
        dest[i+j] = src[j];

    dest[i+j] = '\0';
    return dest;
}

// ----------------------------------------------------------------------------
// web-found version of htoa (http://johnsantic.com/comp/htoi.html)
// ----------------------------------------------------------------------------
unsigned int htoi (const char *ptr)
{
	unsigned int value = 0, i;
	char ch = *ptr;

    while (ch == ' ' || ch == '\t' || ch == '0' || ch == 'x') // ws: added 0x
        ch = *(++ptr);

    for (i=0;i<30;i++) {  // ws: added limit of 30 chars

        if (ch >= '0' && ch <= '9')
            value = (value << 4) + (ch - '0');
        else if (ch >= 'A' && ch <= 'F')
            value = (value << 4) + (ch - 'A' + 10);
        else if (ch >= 'a' && ch <= 'f')
            value = (value << 4) + (ch - 'a' + 10);
        else
            return value;
        ch = *(++ptr);
    }
    return value;
}

// ----------------------------------------------------------------------------
// ws: simple atoi, derived from htoa
// ----------------------------------------------------------------------------
int atoi (const char *ptr)
{
	int value = 0, i, s = 1;
	char ch = *ptr;

    while (ch == ' ' || ch == '\t' || ch == '0')  // || ch == 'x') // ws: added 0x
        ch = *(++ptr);

    if (ch == '-') {
    	s = -1;
        ch = *(++ptr);
    }

    for (i=0;i<30;i++) {  // ws: added limit of 30 chars

        if (ch >= '0' && ch <= '9')
            value = (value * 10) + (ch - '0');
        else
            goto fin; //return value;
        ch = *(++ptr);
    }
fin:
    if (s < 0)
    	value = -value;
    return value;
}

// ----------------------------------------------------------------------------
// web-found version of itoa (http://www.jb.man.ac.uk/~slowe/cpp/itoa.html)
// ----------------------------------------------------------------------------
char* ws_itoa(int val, int base)	// ws: renamed with GCC 4.9
{
	static char buf[32] = {0};
	int i = 30;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	return &buf[i+1];
}

// ----------------------------------------------------------------------------
// ws version of itoa, with optional cat
// ----------------------------------------------------------------------------
/*char* itoa_cat(int val, int base, char *str)
{
	static char buf[64];
	int i = 30;
	char c;
	buf[31] = 0;
	for(; val && i ; --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	if (str)
		for (val = 0; val < 32; val++) {
			if (c = *str++)
				buf[val+31] = c;
			if (c == 0) break;
		}
	return &buf[i+1];
}*/

// ----------------------------------------------------------------------------
// web-found version of itoa (http://interviewcracker.wordpress.com/2008/06/18/strtok/)
// ----------------------------------------------------------------------------
char* ws_strtok(char *input, char *tokenizer) {
	static char *memorizeInput;
	char *inp = input;

	int i, tokenFound = 0;

	if (input)
		memorizeInput = input;
	else
		input = memorizeInput;

	if (!(*memorizeInput))
		return 0;

	while (*memorizeInput) {
		for (i = 0; i < strlen(tokenizer) && !tokenFound; i++)
			if (*memorizeInput == tokenizer[i]) {
				*memorizeInput = 0;
				tokenFound = 1;
			}

		if (tokenFound) {
			memorizeInput++;
			break;
		} else {
			if (memorizeInput == inp)  // ws: added for dia_cmd: ac 0 or ac0
				return input;
		}

		memorizeInput++;
	}

	return input;
}

// ----------------------------------------------------------------------------
// ws: own simple strncmp
// ----------------------------------------------------------------------------
int ws_strncmp(char *s1, char *s2, int n)
{
	int i, cmp = 0;
	if ((s1 == 0) || (s2 == 0)) return -1;

	for (i = 0; i < n; i++) {
		char c1 = *s1++;
		char c2 = *s2++;
		if (!c1 || !c2) break;
		if (c1 != c2) return i+1;
	}
	return cmp;
}


/* some idea:
// converts a double to an ascii string
void __dtoa( double d, char* buff )
{
   // a float type
   union dpresc {
      struct sdat_t
      {
         unsigned long long mantissa : 52;
         unsigned long long exp : 11;
         unsigned long long sign : 1;
      } sdat __attribute((packed));
      double ddat;
   };

   union dpresc doub;
   doub.ddat = d;

   if( doub.sdat.sign == 0x0 )
      printf( "POSITIVE" );
   else
	  printf( "NEGATIVE" );
} */

// ----------------------------------------------------------------------------
// ws: own simple without exponent handling, fixed
// ----------------------------------------------------------------------------
void ws_dtoa(double val, char* buf)
{
	int sgn = 1;  	// positive or 0
	unsigned int ipart, fpart, base = 1000000000;

	if (val < 0) {
		sgn = -1;
		val = -val;
	}
	ipart = (int)val;
	fpart = (val - ipart)*base + 0.5;
	if (sgn == 1) {
		// ipart = (int)val;
		sprintf(buf, "%u.%09u", ipart, fpart);
	} else {
		// ipart = (int)-val;
		sprintf(buf, "-%u.%09u", ipart, fpart);
	}
}

// ----------------------------------------------------------------------------
// ws: own simple without exponent handling, fixed
// ----------------------------------------------------------------------------
void ws_ftoa(float val, char* buf)
{
	int sgn = 1;  	// positive or 0
	unsigned int ipart, fpart, base = 10000000;

	if (val < 0) {
		sgn = -1;
		sgn = -1;
		val = -val;
	}
	ipart = (int)val;
	fpart = (val - ipart)*base + 0.5;
	if (sgn == 1) {
		// ipart = (int)val;
		sprintf(buf, "%u.%07u", ipart, fpart);
	} else {
		// ipart = (int)-val;
		sprintf(buf, "-%u.%07u", ipart, fpart);
	}
}

// ----------------------------------------------------------------------------
// ws: own simple without exponent handling, fixed
// ----------------------------------------------------------------------------
void ws_ftoa3(float val, char* buf)
{
	int sgn = 1;  	// positive or 0
	unsigned int ipart, fpart, base = 1000;

	if (val < 0) {
		sgn = -1;
		sgn = -1;
		val = -val;
	}
	ipart = (int)val;
	fpart = (val - ipart)*base + 0.5;
	if (sgn == 1) {
		// ipart = (int)val;
		sprintf(buf, "%u.%03u", ipart, fpart);
	} else {
		// ipart = (int)-val;
		sprintf(buf, "-%u.%03u", ipart, fpart);
	}
}

// ws: http://www.jbox.dk/sanos/source/lib/strtod.c.html
// ----------------------------------------------------------------------------
//
// strtod.c
//
// Convert string to double
//
// Copyright (C) 2002 Michael Ringgaard. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. Neither the name of the project nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//

double strtod(const char *str, char **endptr)
{
  double number;
  int exponent;
  int negative;
  char *p = (char *) str;
  double p10;
  int n;
  int num_digits;
  int num_decimals;

  // Skip leading whitespace
  while (ws_isspace(*p)) p++;

  // Handle optional sign
  negative = 0;
  switch (*p)
  {
    case '-': negative = 1; // Fall through to increment position
    case '+': p++;
  }

  number = 0.;
  exponent = 0;
  num_digits = 0;
  num_decimals = 0;

  // Process string of digits
  while (ws_isdigit(*p))
  {
    number = number * 10. + (*p - '0');
    p++;
    num_digits++;
  }

  // Process decimal part
  if (*p == '.')
  {
    p++;

    while (ws_isdigit(*p))
    {
      number = number * 10. + (*p - '0');
      p++;
      num_digits++;
      num_decimals++;
    }

    exponent -= num_decimals;
  }

  if (num_digits == 0)
  {
    // errno = ERANGE;
    return 0.0;
  }

  // Correct for sign
  if (negative) number = -number;

  // Process an exponent string
  if (*p == 'e' || *p == 'E')
  {
    // Handle optional sign
    negative = 0;
    switch (*++p)
    {
      case '-': negative = 1;   // Fall through to increment pos
      case '+': p++;
    }

    // Process string of digits
    n = 0;
    while (ws_isdigit(*p))
    {
      n = n * 10 + (*p - '0');
      p++;
    }

    if (negative)
      exponent -= n;
    else
      exponent += n;
  }

  if (exponent < DBL_MIN_EXP  || exponent > DBL_MAX_EXP)
  {
    // errno = ERANGE;
    return 0; //HUGE_VAL;
  }

  // Scale the result
  p10 = 10.;
  n = exponent;
  if (n < 0) n = -n;
  while (n)
  {
    if (n & 1)
    {
      if (exponent < 0)
        number /= p10;
      else
        number *= p10;
    }
    n >>= 1;
    p10 *= p10;
  }

  // if (number == HUGE_VAL) errno = ERANGE;
  if (endptr) *endptr = p;

  return number;
}

float strtof(const char *str, char **endptr)
{
  return (float) strtod(str, endptr);
}


long double strtold(const char *str, char **endptr)
{
  return strtod(str, endptr);
}

/*double atof(const char *str)
{
  return strtod(str, 0);
}*/
