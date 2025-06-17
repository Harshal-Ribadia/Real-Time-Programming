// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
/// \file		 ws_strfun.h
/// \brief		 small string functions (atoi, htoi,..)
/// \author		 Wolfgang Schulter 
/// \date		 12.04.2013 ws:  buxfixes with ws_dtoa, ws_ftoa
/// \date		 26.12.2011 ws:  initial version
// ----------------------------------------------------------------------------

#ifndef __WS_STRFUN_H__
#define __WS_STRFUN_H__  	  	// only once ..

char *strncpy(char *s1, char *s2, int n);
char *strcpy(char *s1, char *s2);
int strcmp(const char *s1, const char *s2);
int strlen(char *s1);
char* strstr(char* haystack, char* needle);
char *strcat(char *dest, const char *src);

void * memset( void* ptr, int value, unsigned int num);
char *memcpy(char *s1, const char *s2, int n);

int atoi (const char *ptr);
unsigned int htoi (const char *ptr);
char* ws_itoa(int val, int base);			// ws: renamed with GCC 4.9
char* ws_strtok(char *input, char *tokenizer);
int ws_strncmp(char *s1, char *s2, int n);
void ws_dtoa(double val, char* buf);
void ws_ftoa(float val, char* buf);
void ws_ftoa3(float val, char* buf);		// since 4.0.9
// double atof(const char *str);

#endif // __WS_STRFUN_H__
