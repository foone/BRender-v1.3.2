/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: printf.c 1.1 1997/12/10 16:41:09 jon Exp $
 * $Locker: $
 *
 * Various printf's that are exported
 */
#include <stdarg.h>

#include "fw.h"

BR_RCS_ID("$Id: printf.c 1.1 1997/12/10 16:41:09 jon Exp $")

br_int_32 BR_RESIDENT_ENTRY BrSprintf(char * buf, char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	n = BrVSprintf(buf, fmt, args);
	va_end(args);

	return n;
}

br_int_32 BR_RESIDENT_ENTRY BrSprintfN(char * buf, br_size_t buf_size, char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	n = BrVSprintf(buf, fmt, args);
	va_end(args);

	return n;
#if 0
	unsigned int n;
	char tmp[512];
	va_list args;

	va_start(args, fmt);
	n = BrVSprintf(tmp, fmt, args);
	va_end(args);

	if(n > buf_size-1) {
		n = buf_size-1;
	}

	BrStrNCpy(buf, tmp, n);
	buf[n] = '\0';

	return n;
#endif
}

int BR_RESIDENT_ENTRY BrLogPrintf(char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	n = BrVSprintf(BrScratchString(), fmt, args);
	va_end(args);

	BrLogWrite(BrScratchString(), 1, n);

	return n;
}

br_int_32 BR_RESIDENT_ENTRY BrSScanf(char *str, char *fmt, ...)
{
	int n;
	va_list args;

	va_start(args, fmt);
	n = BrVSScanf(str, fmt, args);
	va_end(args);

	return n;
}



