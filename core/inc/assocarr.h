/*
 * Copyright (c) 1992,1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: assocarr.h 1.3 1998/02/27 18:10:59 jon Exp $
 * $Locker: $
 *
 * Structures describing associative arrays
 */
#ifndef _ASSOCARR_H_
#define _ASSOCARR_H_

typedef struct br_associative_array {

	br_token_value *tv;
	br_uint_16 num_elements;
	br_uint_16 max_elements;

} br_associative_array;

#endif
