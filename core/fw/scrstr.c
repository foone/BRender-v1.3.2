/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: scrstr.c 1.1 1997/12/10 16:41:10 jon Exp $
 * $Locker: $
 *
 * A global scratchpad string used for printf and error operations
 */
#include "fw.h"

BR_RCS_ID("$Id: scrstr.c 1.1 1997/12/10 16:41:10 jon Exp $")

/*
 * Initialised to a value because IBM Cset++ LIB hides the symbol
 * otherwise ???
 */
char _br_scratch_string[512] = "SCRATCH";
br_size_t _br_scratch_string_size = BR_ASIZE(_br_scratch_string);
