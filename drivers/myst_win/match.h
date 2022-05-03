/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: match.h 1.1 1997/12/10 16:46:10 jon Exp $
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

typedef void BR_ASM_CALL render_begin_fn(struct brp_block *block);
typedef void BR_ASM_CALL render_end_fn(struct brp_block *block);

struct local_block {
	brp_block	p;

	/* Mask of match flags */
	br_uint_32	flags_mask;
	br_uint_32	flags_cmp;

	/* Copy of no-rendering flag */
	br_boolean no_render;
};

/*
 * Valid range flags
 */
enum {
	RF_DECAL		= 0x0008,
	RF_RGB_SHADE	= 0x0010,
};

#ifdef __cplusplus
};
#endif
#endif

