/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: depth.c 1.1 1997/12/10 16:51:41 jon Exp $
 * $Locker: $
 *
 * Generation of Alpha component
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

void SURFACE_CALL SurfaceLinearDepth(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	/*
	 * N.B. should cache (yon+hither)/2 and 2/(yon-hither)
	 */
	comp[C_SW] = BR_ADD(BR_MUL(self->state.cache.comp_scales[C_SW],
		BR_DIV(BR_SUB(BR_MUL(comp[C_W], -2),
		BR_ADD(self->state.matrix.yon_z, self->state.matrix.hither_z)),
		BR_SUB(self->state.matrix.yon_z, self->state.matrix.hither_z))),
 		self->state.cache.comp_offsets[C_SW]);
}

