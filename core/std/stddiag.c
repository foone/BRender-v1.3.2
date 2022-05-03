/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: stddiag.c 1.1 1997/12/10 16:41:28 jon Exp $
 * $Locker: $
 *
 * Default diagnostic handler that reports through stderr
 */
#include <stdio.h>
#include <stdlib.h>

#include "brender.h"

BR_RCS_ID("$Id: stddiag.c 1.1 1997/12/10 16:41:28 jon Exp $")

static void BR_CALLBACK BrStdioWarning(char *message)
{
	fflush(stdout);
	fputs(message,stderr);
	fputc('\n',stderr);
	fflush(stderr);
}

static void BR_CALLBACK BrStdioFailure(char *message)
{
	/*
	 * Close down all devices etc.
	 */
	BrEnd();

	fflush(stdout);
	fputs(message,stderr);
	fputc('\n',stderr);
	fflush(stderr);
	
	exit(10);
}

/*
 * ErrorHandler structure
 */
br_diaghandler BrStdioDiagHandler = {
	"Stdio DiagHandler",
	BrStdioWarning,
	BrStdioFailure,
};

/*
 * Override default
 */
br_diaghandler * BR_ASM_DATA _BrDefaultDiagHandler = &BrStdioDiagHandler;


