/*
 * Copyright (c) 1992,1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: scalar.c 1.1 1997/12/10 16:41:22 jon Exp $
 * $Locker: $
 *
 * Symbols that are defined to make sure apps. link to the right library
 */
#include "brender.h"

BR_RCS_ID("$Id: scalar.c 1.1 1997/12/10 16:41:22 jon Exp $")

#if BASED_FIXED
int _BR_Fixed_Point_Scalar = 0;
#endif

#if BASED_FLOAT
int _BR_Floating_Point_Scalar = 0;
#endif
