/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: object.c 1.1 1997/12/10 16:41:09 jon Exp $
 * $Locker: $
 *
 * Some generic methods for object support
 */
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: object.c 1.1 1997/12/10 16:41:09 jon Exp $")

/*
 * Token/Value support methods (via private templateQuery() method)
 */
br_error BR_CMETHOD_DECL(br_object,query)(br_object *self, br_uint_32 *pvalue, br_token t)
{
	return BrTokenValueQuery(pvalue, NULL, 0, t, self, ObjectTemplateQuery(self));
}

br_error BR_CMETHOD_DECL(br_object,queryBuffer)
	(br_object *self, br_uint_32 *pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t)
{
	return BrTokenValueQuery(pvalue, buffer, buffer_size, t, self, ObjectTemplateQuery(self));
}

br_error BR_CMETHOD_DECL(br_object,queryMany)
	(br_object *self, br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount)
{
	return BrTokenValueQueryMany(tv, extra, extra_size, pcount, self, ObjectTemplateQuery(self));
}

br_error BR_CMETHOD_DECL(br_object,queryManySize)
	(br_object *self, br_size_t *pextra_size, br_token_value *tv)
{
	return BrTokenValueQueryManySize(pextra_size, tv, self, ObjectTemplateQuery(self));
}

br_error BR_CMETHOD_DECL(br_object,queryAll)
	(br_object *self, br_token_value *buffer, br_size_t buffer_size)
{
	return BrTokenValueQueryAll(buffer, buffer_size, self, ObjectTemplateQuery(self));
}

br_error BR_CMETHOD_DECL(br_object,queryAllSize)
	(br_object *self, br_size_t *psize)
{
	return BrTokenValueQueryAllSize(psize, self, ObjectTemplateQuery(self));
}

/*
 * Resource destructor for BR_MEMORY_OBJECT
 */
void BR_CALLBACK _BrObjectFree(void *res, br_uint_8 res_class, br_size_t size)
{
	br_object *o = res;

	/*
	 * Assume the data is a br_object, and if a vtable is attached, call object destructor
	 */
	if(o && o->dispatch)
		ObjectFree(o);
}

