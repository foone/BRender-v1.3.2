/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: alpha.c 1.2 1998/11/13 16:23:21 jon Exp $
 * $Locker: $
 *
 * Generation of Alpha component
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

void SURFACE_CALL SurfaceAlpha(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_A] = BR_MUL(self->state.cache.comp_scales[C_A],
 		BR_CONST_DIV(BrIntToScalar(BR_ALPHA(colour)),256)) +
 		self->state.cache.comp_offsets[C_A];
}

void SURFACE_CALL SurfaceAlphaPrealpha(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp)
{
	comp[C_A] = BR_MUL(self->state.cache.comp_scales[C_A],
		BR_CONST_DIV(BrIntToScalar(BR_ALPHA(rend.prelit_colours[rend.current_index])), 256)) +
 		self->state.cache.comp_offsets[C_A];
}
