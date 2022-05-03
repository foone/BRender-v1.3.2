/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.h 1.1 1997/12/10 16:50:20 jon Exp $
 * $Locker: $
 *
 * Bits used to match renderer state to primitives
 */
#ifndef _MATCH_H_
#define _MATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Local version of brp_info extended with information used to match primitve with current state
 */
struct local_block {
	brp_block	p;

	/*
	 * Mask of match flags
	 */
	br_uint_32	flags_mask;
	br_uint_32	flags_cmp;

	/*
	 * Work buffer
	 */
	struct prim_work *work;

	/*
	 * Next block for chained blocks (FORCE_FRONT, DECAL etc.)
	 */
	struct local_prim_info *chain;
};


#ifdef __cplusplus
};
#endif
#endif

