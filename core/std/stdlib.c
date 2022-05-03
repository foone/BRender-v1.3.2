/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: stdlib.c 1.1 1997/12/10 16:41:28 jon Exp $
 * $Locker: $
 *
 * Glue to stdlib functions
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <ctype.h>
#include <stdarg.h>

#include "brender.h"

BR_RCS_ID("$Id: stdlib.c 1.1 1997/12/10 16:41:28 jon Exp $")

int BR_RESIDENT_ENTRY BrMemCmp( const void *s1, const void *s2, size_t n )
{
	return memcmp(s1,s2,n);
}


void * BR_RESIDENT_ENTRY BrMemCpy( void *s1, const void *s2, size_t n )
{
	return memcpy(s1,s2,n);
}


void * BR_RESIDENT_ENTRY BrMemSet( void *s, int c, size_t n )
{
	return memset(s,c,n);
}



char * BR_RESIDENT_ENTRY BrStrCat( char *s1, const char *s2 )
{
	return strcat(s1,s2);
}


int BR_RESIDENT_ENTRY BrStrCmp( const char *s1, const char *s2 )
{
	return strcmp(s1,s2);
}


int BR_RESIDENT_ENTRY BrStrICmp( const char *s1, const char *s2 )
{
	return stricmp(s1,s2);
}


char * BR_RESIDENT_ENTRY BrStrCpy( char *s1, const char *s2 )
{
	return strcpy(s1,s2);
}


br_size_t BR_RESIDENT_ENTRY BrStrLen( const char *s )
{
	return strlen(s);
}


int BR_RESIDENT_ENTRY BrStrNCmp( const char *s1, const char *s2, size_t n )
{
	return strncmp(s1,s2,n);
}


int BR_RESIDENT_ENTRY BrStrNICmp( const char *s1, const char *s2, size_t n )
{
	return strnicmp(s1,s2,n);
}


char * BR_RESIDENT_ENTRY BrStrNCpy( char *s1, const char *s2, size_t n )
{
	return strncpy(s1,s2,n);
}

char * BR_RESIDENT_ENTRY BrStrChr( char *s1, char c)
{
	return strchr(s1,c);
}

char * BR_RESIDENT_ENTRY BrStrRChr( char *s1, char c)
{
	return strrchr(s1,c);
}

void BR_RESIDENT_ENTRY BrAbort( void )
{
	abort();
}

char * BR_RESIDENT_ENTRY BrGetEnv( const char *name )
{
	return getenv(name);
}

float BR_RESIDENT_ENTRY BrStrToF( const char *nptr, char **endptr )
{
	return (float)strtod(nptr, endptr);
}

double BR_RESIDENT_ENTRY BrStrToD( const char *nptr, char **endptr )
{
	return strtod(nptr, endptr);
}


long int BR_RESIDENT_ENTRY BrStrToL( const char *nptr, char **endptr, int base )
{
	return strtol(nptr, endptr, base);
}


unsigned long BR_RESIDENT_ENTRY BrStrToUL( const char *nptr, char **endptr, int base )
{
	return strtoul(nptr, endptr, base);
}



br_boolean	BR_RESIDENT_ENTRY BrIsAlpha(int c)
{
	return isalpha(c);
}


br_boolean	BR_RESIDENT_ENTRY BrIsDigit(int c)
{
	return isdigit(c);
}


br_boolean	BR_RESIDENT_ENTRY BrIsSpace(int c)
{
	return isspace(c);
}


br_boolean	BR_RESIDENT_ENTRY BrIsPrint(int c)
{
	return isprint(c);
}

br_int_32 BR_RESIDENT_ENTRY BrVSprintf(char * buf, char *fmt, va_list args)
{
	return vsprintf(buf, fmt, args);
}

br_int_32 BR_RESIDENT_ENTRY BrVSprintfN(char * buf, br_size_t buf_size, char *fmt, va_list args)
{
	unsigned int n;
	char tmp[512];

	n = vsprintf(tmp, fmt, args);

	if(n > buf_size-1) {
		n = buf_size-1;
	}

	strncpy(buf, tmp, n);
	buf[n] = '\0';

	return n;
}

br_int_32 BR_RESIDENT_ENTRY BrVSScanf(char * buf, char *fmt, va_list args)
{

#if defined(_MSC_VER)
	int i;
	unsigned long arg_block[16];

	/*
	 * Microsoft do not provide vsscanf, so fake one by passing 16 longs from args
	 * to the normal sscanf
	 */
	for(i=0; i < 16; i++)
		arg_block[i] = va_arg(args,unsigned long);

	return sscanf(buf, fmt,
		arg_block[0],
		arg_block[1],
		arg_block[2],
		arg_block[3],
		arg_block[4],
		arg_block[5],
		arg_block[6],
		arg_block[7],
		arg_block[8],
		arg_block[9],
		arg_block[10],
		arg_block[11],
		arg_block[12],
		arg_block[13],
		arg_block[14],
		arg_block[15]);
#else
	return vsscanf(buf, fmt, args);
#endif
}

