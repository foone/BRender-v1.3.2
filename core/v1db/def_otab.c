/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: def_otab.c 1.1 1997/12/10 16:41:30 jon Exp $
 * $Locker: $
 *
 * Default order_table for bucket Z-sort renderer
 */
#include "v1db.h"

BR_RCS_ID("$Id: def_otab.c 1.1 1997/12/10 16:41:30 jon Exp $")

#define _default_order_table_size 256

static void *_default_table[_default_order_table_size];

br_order_table _BrDefaultOrderTable={
	/*
	 * Default order table
	 */
        (br_primitive**)_default_table,                                 /* Pointer to table     */
	_default_order_table_size,		/* Size				*/
	NULL,							/* Next				*/
        BR_SCALAR(1.0),                                /* Minimum Z            */
	BR_SCALAR(10.0),				/* Maximum Z		*/
	BR_SCALAR(0.0),					/* Sort Z			*/
	BR_SCALAR(1.0/9.0),				/* Scale			*/
	BR_ORDER_TABLE_LEAVE_BOUNDS,	/* Flags			*/
	BR_SORT_AVERAGE,				/* Sort type		*/
	0								/* Visit count		*/

};

