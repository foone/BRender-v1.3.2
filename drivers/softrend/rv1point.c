/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: rv1point.c 1.1 1997/12/10 16:52:37 jon Exp $
 * $Locker: $
 *
 * Geometry format methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: rv1point.c 1.1 1997/12/10 16:52:37 jon Exp $");


br_error BR_CMETHOD_DECL(br_geometry_format_soft, render_v1points)
		(struct br_geometry *self, struct br_renderer *r,
			struct v11model *model)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_geometry_format_soft, storedNew_v1points)
		(struct br_geometry *self, struct br_renderer *r,
		struct br_geometry_stored **psg, br_token_value *tv, struct v11model *model)
{
	return BRE_FAIL;
}


