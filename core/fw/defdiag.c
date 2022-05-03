/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: defdiag.c 1.1 1997/12/10 16:41:05 jon Exp $
 * $Locker: $
 *
 * Default diagnostic handler that does nothing
 */
#include "brender.h"

BR_RCS_ID("$Id: defdiag.c 1.1 1997/12/10 16:41:05 jon Exp $")

static void BrNullWarning(char *message)
{
}

static void BrNullFailure(char *message)
{
}

/*
 * DiagHandler structure
 */
br_errorhandler BrNullDiagHandler = {
	"Null DiagHandler",
	BrNullWarning,
	BrNullFailure,
};

/*
 * Global variable that can be overridden by linking something first
 */
br_diaghandler *_BrDefaultDiagHandler = &BrNullDiagHandler;

