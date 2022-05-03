/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brstdlib.h 1.1 1997/12/10 16:41:05 jon Exp $
 * $Locker: $
 *
 * Wrappers around standard C library functions that are provided by the STD library
 */

#ifndef _BRSTDLIB_H_
#define _BRSTDLIB_H_

#include <stdarg.h>

#ifndef _NO_PROTOTYPES
#ifdef __cplusplus
extern "C" {
#endif

int			BR_PUBLIC_ENTRY BrMemCmp( const void *s1, const void *s2, br_size_t n );
void *		BR_PUBLIC_ENTRY BrMemCpy( void *s1, const void *s2, br_size_t n );
void *		BR_PUBLIC_ENTRY BrMemSet( void *s, int c, br_size_t n );

char *		BR_PUBLIC_ENTRY BrStrCat( char *s1, const char *s2 );
int 		BR_PUBLIC_ENTRY BrStrCmp( const char *s1, const char *s2 );
int 		BR_PUBLIC_ENTRY BrStrICmp( const char *s1, const char *s2 );
char *		BR_PUBLIC_ENTRY BrStrCpy( char *s1, const char *s2 );
br_size_t	BR_PUBLIC_ENTRY BrStrLen( const char *s );
int 		BR_PUBLIC_ENTRY BrStrNCmp( const char *s1, const char *s2, br_size_t n );
int 		BR_PUBLIC_ENTRY BrStrNICmp( const char *s1, const char *s2, br_size_t n );
char *		BR_PUBLIC_ENTRY BrStrNCpy( char *s1, const char *s2, br_size_t n );

char *		BR_PUBLIC_ENTRY BrStrChr( char *s1, char c);
char *		BR_PUBLIC_ENTRY BrStrRChr( char *s1, char c);

void 		BR_PUBLIC_ENTRY BrAbort( void );

char * 		BR_PUBLIC_ENTRY BrGetEnv( const char *name );

float 		BR_PUBLIC_ENTRY BrStrToF( const char *nptr, char **endptr );
double 		BR_PUBLIC_ENTRY BrStrToD( const char *nptr, char **endptr );
long int	BR_PUBLIC_ENTRY BrStrToL( const char *nptr, char **endptr, int base );
unsigned long BR_PUBLIC_ENTRY BrStrToUL( const char *nptr, char **endptr, int base );

br_boolean	BR_PUBLIC_ENTRY BrIsAlpha(int c);
br_boolean	BR_PUBLIC_ENTRY BrIsDigit(int c);
br_boolean	BR_PUBLIC_ENTRY BrIsSpace(int c);
br_boolean	BR_PUBLIC_ENTRY BrIsPrint(int c);

br_int_32 BR_RESIDENT_ENTRY BrVSScanf(char * str, char *fmt, va_list args);
br_int_32 BR_RESIDENT_ENTRY BrVSprintf(char * buf, char *fmt, va_list args);
br_int_32 BR_RESIDENT_ENTRY BrVSprintfN(char * buf, br_size_t buf_size, char *fmt, va_list args);

#ifdef __cplusplus
};
#endif
#endif


/*
 * Compiler specific overrides
 */
#if defined(__USE_CLIB__)

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

	/*
	 * Force stlib operations to be used directly
	 */
#define BrMemCmp	memcmp
#define BrMemCpy	memcpy
#define BrMemSet	memset

#define BrStrCat	strcat
#define BrStrCmp	strcmp
#define BrStrICmp	stricmp
#define BrStrCpy	strcpy
#define BrStrLen	strlen
#define BrStrNCmp	strncmp
#define BrStrNICmp	strnicmp
#define BrStrNCpy	strncpy

#define BrStrChr	strchr
#define BrStrRChr	strrchr

#define BrAbort		abort

#define BrGetEnv	getenv

#define BrStrToF	strtod
#define BrStrToD	strtod
#define BrStrToL	strtol
#define BrStrToUL	strtoul

#define BrIsAlpha	isalpha
#define BrIsDigit	isdigit
#define BrIsSpace	isspace
#define BrIsPrint	isprint

#define BrVSScanf	vsscanf
#define BrVSprintf	vsprintf

#elif defined(__WATCOMC__)

#elif defined(_MSC_VER)

#elif defined (__BORLANDC__)

#elif defined(__PROTON__)

#elif defined(__GNUC__) && defined(i386)

#endif

#endif


