/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Stub functions to hook STDIO ops.
 */
#include "drv.h"
#include "stddef.h"
#include "brender.h"

#ifdef __DRIVER__

#if 0
/*
 * Fake file handle area
 */
long __iob[8*4];
char defenv[]={"0"};

void *malloc(size_t size)
{
   return(BrResAllocate(DRIVER_RESOURCE,size,BR_MEMORY_SCRATCH));
}
#endif


void main(void)
{
	return;
}


#if 0
void free(void *dummy)
{
   BrResFree(dummy);
}

void exit( int status )
{
	BR_FATAL1("Glide Exit: %d",status);
}

int printf(const char *format,
	long a0,
	long a1,
	long a2,
	long a3,
	long a4,
	long a5,
	long a6,
	long a7,
	long a8,
	long a9,
	long a10,
	long a11,
	long a12,
	long a13,
	long a14,
	long a15)
{
#if DEBUG
	return BrLogPrintf(format,
		a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
#else
	return 0;
#endif
}

int printasf_(const char *format,
	long a0,
	long a1,
	long a2,
	long a3,
	long a4,
	long a5,
	long a6,
	long a7,
	long a8,
	long a9,
	long a10,
	long a11,
	long a12,
	long a13,
	long a14,
	long a15)
{
#if DEBUG
	return BrLogPrintf(format,
		a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
#else
	return 0;
#endif
}

#if 0				 
char *getenv(const char *name)
{
#if DEBUG
	BrLogPrintf("getenv(%s)\n",name);
#endif
// 	return NULL;

 	return &defenv;
}
#endif
void *fopen( const char *filename, const char *mode )
{
	return NULL;
}

int fclose( void *fp )
{
	return 0;
}

unsigned int fwrite(const void *ptr, unsigned int size, unsigned int n, void *fp )
{
	return 0;
}

int fprintf( void *fp, const char *format, ... )
{
	return 0;
}

int fflush( void *fp )
{
	return 0;
}


int puts( const char *s )
{
#if DEBUG
	BrLogPrintf("%s\n",s);
#endif
	return 0;
}

void perror( const char *s)
{
}

int	sprintf( char *s, const char *format,
	long a0,
	long a1,
	long a2,
	long a3,
	long a4,
	long a5,
	long a6,
	long a7,
	long a8,
	long a9,
	long a10,
	long a11,
	long a12,
	long a13,
	long a14,
	long a15)
{
	return BrSprintf(s,format,
		a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
}

int	sscanf( const char *s, const char *format,
	long a0,
	long a1,
	long a2,
	long a3,
	long a4,
	long a5,
	long a6,
	long a7,
	long a8,
	long a9,
	long a10,
	long a11,
	long a12,
	long a13,
	long a14,
	long a15)
{
	return BrSScanf(s,format,
		a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15);
}

int atoi(const char *ptr)
	{
	return(0);
	}

int fputs(const char *buf, void *fp)
	{
	return(0);
	}
int fputc(int c, void *fp)
	{
	return(0);
	}

int vfprintf( void *fp, const char *format, ... )
{
	return 0;
}
#endif


#endif

