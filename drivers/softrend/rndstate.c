/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: rndstate.c 1.2 1998/08/03 13:16:53 jon Exp $
 * $Locker: $
 *
 * State manipulation methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "brassert.h"

BR_RCS_ID("$Id: rndstate.c 1.2 1998/08/03 13:16:53 jon Exp $");


/*
 * This is a kludge to reduce the state copy overhead.  Only the 
 * model to view matrix information is copied, since we know that
 * that is the only bit that changes from model to model in the
 * current higher level system
 */
#define REDUCED_MATRIX_COPY 1


/*
 * Function to generate primitive state object on demand
 */
br_error CheckPrimitiveState(struct br_renderer *self)
{
	br_error r;

	if(self->state.pstate == NULL) {
		r = PrimitiveLibraryStateNew(self->plib, &self->state.pstate);
		if(r != BRE_OK)
			   return r;

		BrResAdd(self,self->state.pstate);
	}

	return BRE_OK;
}

#define CHECK_PSTATE { \
		r = CheckPrimitiveState(self);	\
		if(r != BRE_OK)					\
			return r;					\
}

/*
 * Setting current state
 */
br_error BR_CMETHOD_DECL(br_renderer_soft,partSet)
		(struct br_renderer *self, br_token part, br_int_32 index, br_token t, br_uint_32 value)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);
	br_uint_32 m = 0;

	if(tp != NULL) {
		r = BrTokenValueSet(sp, &m, t, value, tp);
		if(m)
			TemplateActions(&self->state, part, index, m);
	} else {
		/*
		 * Hand over to primitives
		 */
		CHECK_PSTATE;

		r = PrimitiveStatePartSet(self->state.pstate, part, index, t, value);
	}

	return r;
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partSetMany)
		(struct br_renderer *self, br_token part, br_int_32 index, br_token_value * tv, br_int_32 *pcount)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);
	br_uint_32 m = 0;
	br_int_32 c;

	if(tp != NULL) {
		r = BrTokenValueSetMany(sp, pcount, &m, tv, tp);
		if(m)
			TemplateActions(&self->state, part, index, m);
	} else {
		/*
		 * Hand over to primitives
		 */
		CHECK_PSTATE;

		c = 0;
		r = PrimitiveStatePartSetMany(self->state.pstate, part, index, tv, &c);

		if(pcount)
			*pcount = c;
	}

	return r;
}

/*
 * Reading current state
 */
br_error BR_CMETHOD_DECL(br_renderer_soft,partQuery)
		(struct br_renderer *self, br_token part, br_int_32 index, br_uint_32 *pvalue, br_token t)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQuery(pvalue, NULL, 0, t,  sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQuery(self->state.pstate, part, index, pvalue, t);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryBuffer)
		(struct br_renderer *self, br_token part, br_int_32 index, br_uint_32 *pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQuery(pvalue, buffer, buffer_size, t,  sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQueryBuffer(self->state.pstate, part, index, pvalue, buffer, buffer_size, t);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryMany)(struct br_renderer *self,
		br_token part, br_int_32 index,
		br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQueryMany(tv, extra, extra_size, pcount,  sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQueryMany(self->state.pstate, part, index, tv, extra, extra_size, pcount);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryManySize)
		(struct br_renderer *self, br_token part, br_int_32 index, br_size_t *pextra_size, br_token_value *tv)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQueryManySize(pextra_size, tv, sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQueryManySize(self->state.pstate, part, index, pextra_size, tv);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryAll)
		(struct br_renderer *self, br_token part, br_int_32 index, br_token_value *buffer, br_size_t buffer_size)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQueryAll(buffer, buffer_size, sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQueryAll(self->state.pstate, part, index, buffer, buffer_size);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryAllSize)
		(struct br_renderer *self, br_token part, br_int_32 index, br_size_t *psize)
{
	br_error r;
	struct state_all *sp = &self->state;
	struct br_tv_template *tp = FindStateTemplate(self,&sp,part,index);

	if(tp != NULL) {
		return BrTokenValueQueryAllSize(psize, sp, tp);
	} else {
		CHECK_PSTATE;
		return PrimitiveStatePartQueryAllSize(self->state.pstate, part, index, psize);
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft, partQueryCapability)(
        struct br_renderer *self,
        br_token part,
        br_int_32 index,
        br_token_value *buffer,
        br_size_t buffer_size)
{
    return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_renderer_soft, partIndexQuery)(
		struct br_renderer *self,
		br_token part,
		br_int_32 *pnindex)
{
	br_int_32 n;
	br_error r;

	switch(part) {
	case BRT_CULL:
	case BRT_SURFACE:
	case BRT_MATRIX:
	case BRT_ENABLE:
		n = 1;
		break;

	case BRT_LIGHT:
		n = BR_ASIZE(self->state.light);
		break;

	case BRT_CLIP:
		n = BR_ASIZE(self->state.clip);
		break;

	default:
		n = 0;

		CHECK_PSTATE;
		r = PrimitiveStatePartIndexQuery(self->state.pstate, part, &n);
		if(r != BRE_OK)
			return r;
	}

	if(pnindex) {
		*pnindex = n;
		return BRE_OK;
	} else {
		return BRE_FAIL;
	}
}

br_error BR_CMETHOD_DECL(br_renderer_soft, stateQueryPerformance)(
        struct br_renderer *self,
        br_fixed_lu *speed)
{
    return BRE_FAIL;
}

/*
 * Utility function to copy all or part of a state from one place to another
 *
 * The destination recalc. bitmasks are updated appropiately
 */
br_error StateCopy(struct state_all *dest, struct state_all *src, br_uint_32 copy_mask, void *res)
{
	int i;

	if (copy_mask & MASK_CACHED_STATES)
		copy_mask |= MASK_STATE_CACHE;

	/*
	 * Restrict copy_mask to valid source parts
	 */
	copy_mask &= src->valid;

	/*
	 * Merge valid mask into destination
	 */
	dest->valid |= copy_mask;

	/*
	 * See if any parts of primitive state need copying
	 */
	if(copy_mask & ~MASK_STATE_LOCAL) {
		if(dest->pstate) {
			if(src->pstate)
				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			else
				PrimitiveStateStateDefault(dest->pstate, copy_mask);
			
		} else if(src->pstate) {
			/*
			 * Create new dest. state and copy into it
			 */
			if(PrimitiveLibraryStateNew(src->renderer->plib, &dest->pstate) == BRE_OK) {
				/*
				 * Attach new primitive state to given resource parent
				 */
				if(res)
					BrResAdd(res,dest->pstate);

				PrimitiveStateStateCopy(dest->pstate, src->pstate, copy_mask);
			}
		}
	}

	if(copy_mask & MASK_STATE_CULL && dest->cull.timestamp != src->cull.timestamp)
		dest->cull = src->cull;

	if(copy_mask & MASK_STATE_SURFACE && dest->surface.timestamp != src->surface.timestamp)
		dest->surface = src->surface;

	if(copy_mask & MASK_STATE_MATRIX && dest->matrix.timestamp != src->matrix.timestamp) {

		if (dest->matrix.timestamp_m2v != src->matrix.timestamp_m2v) {

			dest->matrix.model_to_view = src->matrix.model_to_view;
			dest->matrix.model_to_view_hint = src->matrix.model_to_view_hint;
			dest->matrix.timestamp_m2v = src->matrix.timestamp_m2v;
		}

		if (dest->matrix.timestamp_v2s != src->matrix.timestamp_v2s) {

			dest->matrix.view_to_screen = src->matrix.view_to_screen;
			dest->matrix.timestamp_v2s = src->matrix.timestamp_v2s;
		}

		dest->matrix.view_to_screen_hint = src->matrix.view_to_screen_hint;
		dest->matrix.timestamp_v2s_hint = src->matrix.timestamp_v2s_hint;

		if (dest->matrix.timestamp_v2e != src->matrix.timestamp_v2e) {

			dest->matrix.view_to_environment = src->matrix.view_to_environment;
			dest->matrix.view_to_environment_hint = src->matrix.view_to_environment_hint;
			dest->matrix.timestamp_v2e = src->matrix.timestamp_v2e;
		}

		dest->matrix.hither_z = src->matrix.hither_z;
		dest->matrix.yon_z = src->matrix.yon_z;

		dest->matrix.timestamp = src->matrix.timestamp;
	}

	if(copy_mask & MASK_STATE_ENABLE && dest->enable.timestamp != src->enable.timestamp)
		dest->enable = src->enable;

	if(copy_mask & MASK_STATE_BOUNDS && dest->bounds.timestamp != src->bounds.timestamp)
		dest->bounds = src->bounds;

	if(copy_mask & MASK_STATE_LIGHT && dest->timestamp_lights != src->timestamp_lights) {
		for(i=0; i < BR_ASIZE(src->light); i++) {
			dest->light[i] = src->light[i];
		}
		dest->timestamp_lights = src->timestamp_lights;
	}

	if(copy_mask & MASK_STATE_CLIP && dest->timestamp_clips != src->timestamp_clips) {
		for(i=0; i < BR_ASIZE(src->clip); i++) {
			dest->clip[i] = src->clip[i];
		}
		dest->timestamp_clips = src->timestamp_clips;
	}

	/*
	 * Copy cache, or mark destination's cache as invalid as necessary
	 */
	if (copy_mask & MASK_STATE_CACHE)
		
		if (src->cache.valid && dest->cull.timestamp == src->cull.timestamp &&
			dest->surface.timestamp == src->surface.timestamp) {
			
			if (dest->timestamp_cache != src->timestamp_cache) {

				dest->cache = src->cache;
				dest->timestamp_cache = src->timestamp_cache;
			}

		} else 

			dest->cache.valid = BR_FALSE;

	return BRE_OK;
}

/*
 * Convertions routines for matrices fixed<->float
 */
#if BASED_FIXED
static void convertM34FloatToFixed(br_matrix34_x *dest, br_matrix34_f *src)
{
	int i,j;

	for(i=0; i < 4; i++)
		for(j=0; j < 3; j++)
			dest->m[i][j] = BrFloatToFixed(src->m[i][j]);
}
#endif

#if BASED_FLOAT
static void convertM34FixedToFloat(br_matrix34_f *dest, br_matrix34_x *src)
{
	int i,j;

	for(i=0; i < 4; i++)
		for(j=0; j < 3; j++)
			dest->m[i][j] = BrFixedToFloat(src->m[i][j]);
}
#endif

/*
 * Special case state manipulation for ease of use
 */

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulX)
		(struct br_renderer *self, br_matrix34_x *m)
#endif
#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulF)
		(struct br_renderer *self, br_matrix34_f *m)
#endif
{
	br_matrix34 om = self->state.matrix.model_to_view;

	BrMatrix34Mul(&self->state.matrix.model_to_view, (br_matrix34 *)m, &om);

	self->state.matrix.model_to_view_hint = BRT_NONE;
	
	TouchModelToView(self);

	return BRE_OK;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulF)
		(struct br_renderer *self, br_matrix34_f *m)
#define CONV(d,s) convertM34FloatToFixed((br_matrix34_x *)(d),(br_matrix34_f *)(s))
#endif

#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulX)
		(struct br_renderer *self, br_matrix34_x *m)
#define CONV(d,s) convertM34FixedToFloat((br_matrix34_f *)(d),(br_matrix34_x *)(s))
#endif
{
	br_matrix34 om = self->state.matrix.model_to_view;
	br_matrix34 cm;
	
	CONV(&cm,m);
	
	BrMatrix34Mul(&self->state.matrix.model_to_view, &cm, &om);

	self->state.matrix.model_to_view_hint = BRT_NONE;

	TouchModelToView(self);

	return BRE_OK;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulF)
		(struct br_renderer *self, br_matrix34_f *m)
#endif
#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulX)
		(struct br_renderer *self, br_matrix34_x *m)
#endif
{
	br_matrix34 cm;
	
	if(self->stack_top == 0)
		return BRE_UNDERFLOW;

	CONV(&cm,m);

	BrMatrix34Mul(
		&self->state.matrix.model_to_view,
		&cm,
		&self->state_stack[0].matrix.model_to_view);

	self->state.matrix.model_to_view_hint = BRT_NONE;

	TouchModelToView(self);

	return BRE_OK;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulX)
		(struct br_renderer *self, br_matrix34_x *m)
#endif
#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulF)
		(struct br_renderer *self, br_matrix34_f *m)
#endif
{
	if(self->stack_top == 0)
		return BRE_UNDERFLOW;

	BrMatrix34Mul(
		&self->state.matrix.model_to_view,
		(br_matrix34 *)m,
		&self->state_stack[0].matrix.model_to_view);

	self->state.matrix.model_to_view_hint = BRT_NONE;

	TouchModelToView(self);

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_renderer_soft,modelInvert)
		(struct br_renderer *self)
{
	br_matrix34 old;

	BrMatrix34Copy(&old,&self->state.matrix.model_to_view);

	if(self->state.matrix.model_to_view_hint == BRT_LENGTH_PRESERVING)
		BrMatrix34LPInverse(&self->state.matrix.model_to_view, &old);
	else
		BrMatrix34Inverse(&self->state.matrix.model_to_view, &old);

	TouchModelToView(self);

	return BRE_OK;
}

br_error BR_CMETHOD_DECL(br_renderer_soft,statePush)
		(struct br_renderer *self, br_uint_32 mask)
{
	struct state_all *sp;

	if(self->stack_top >= MAX_STATE_STACK)
		return BRE_OVERFLOW;

	sp = self->state_stack + self->stack_top;

	self->stack_top++;

	return StateCopy(sp, &self->state, mask, self);
}

br_error BR_CMETHOD_DECL(br_renderer_soft,statePop)
		(struct br_renderer *self, br_uint_32 mask)
{
	struct state_all *sp;
	br_error r;

	if(self->stack_top <= 0)
		return BRE_UNDERFLOW;

	self->stack_top--;

	sp = self->state_stack + self->stack_top;

	r = StateCopy(&self->state, sp, mask, self);
	sp->valid = 0;
	return r;
}

/*
 * Convertions routines for bounds fixed<->float
 */
#if BASED_FIXED
static void convertBounds3FloatToFixed(br_bounds3_x *dest, br_bounds3_f *src)
{
	int i;

	for(i=0; i < 3; i++) {
		dest->min.v[i] = BrFloatToFixed(src->min.v[i]);
		dest->max.v[i] = BrFloatToFixed(src->max.v[i]);
	}
}
#endif

#if BASED_FLOAT
static void convertBounds3FixedToFloat(br_bounds3_f *dest, br_bounds3_x *src)
{
	int i;

	for(i=0; i < 3; i++) {
		dest->min.v[i] = BrFixedToFloat(src->min.v[i]);
		dest->max.v[i] = BrFixedToFloat(src->max.v[i]);
	}
}
#endif

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestF)
	(struct br_renderer *self, br_token *r, br_bounds3_f *bounds_in)
#define BOUNDS_CONV(a,b) convertBounds3FloatToFixed((struct br_bounds3_x *)(a),(struct br_bounds3_f *)(b))
#endif
#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestX)
	(struct br_renderer *self, br_token *r, br_bounds3_x *bounds_in)
#define BOUNDS_CONV(a,b) convertBounds3FixedToFloat((struct br_bounds3_f *)(a),(struct br_bounds3_x *)(b))
#endif
{
	br_bounds3 bounds;

	BOUNDS_CONV(&bounds, bounds_in);
	/*
	 * XXX cache check
	 */
	ModelToScreenUpdate(self);
	
	*r = OnScreenCheck(self, &scache.model_to_screen, &bounds);

	return BRE_OK;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestX)
	(struct br_renderer *self, br_token *r, br_bounds3_x *bounds)
#endif
#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestF)
	(struct br_renderer *self, br_token *r, br_bounds3_f *bounds)
#endif
{
	/*
	 * XXX Cache check
	 */
	if(!scache.valid_m2s) {
		ModelToScreenUpdate(self);
		scache.valid_m2s = BR_TRUE;
	}
	
	*r = OnScreenCheck(self, &scache.model_to_screen, (br_bounds3 *)bounds);

	return BRE_OK;
}


br_error BR_CMETHOD_DECL(br_renderer_soft, coverageTestF)
	(struct br_renderer *self, br_float *r, br_bounds3_f *bounds)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_renderer_soft, coverageTestX)
	(struct br_renderer *self, br_fixed_ls *r, br_bounds3_x *bounds)
{
	return BRE_FAIL;
}


br_error BR_CMETHOD_DECL(br_renderer_soft,viewDistanceF)
	(struct br_renderer *self, br_float *r)
{
	return BRE_FAIL;
}

br_error BR_CMETHOD_DECL(br_renderer_soft,viewDistanceX)
	(struct br_renderer *self, br_fixed_ls *r)
{
	return BRE_FAIL;
}


br_error BR_CMETHOD_DECL(br_renderer_soft,stateSave)
		(struct br_renderer *self, struct br_renderer_state_stored *save, br_uint_32 mask)
{
	return StateCopyToStored(save, &self->state, mask, save);
}

br_error BR_CMETHOD_DECL(br_renderer_soft,stateRestore)
		(struct br_renderer *self, struct br_renderer_state_stored *save, br_uint_32 mask)
{
	/*
	 * Remember the state
	 */
	self->last_restored = save;

	return StateCopyFromStored(&self->state, save, mask, self);
}

br_error BR_CMETHOD_DECL(br_renderer_soft,stateDefault)
		(struct br_renderer *self, br_uint_32 mask)
{
	/*
	 * Set primitive state defaults
	 */
	if(self->state.pstate)
		PrimitiveStateStateDefault(self->state.pstate,mask);

	/*
	 * Copy our default entries
	 */
	return StateCopy(&self->state, self->default_state, mask & MASK_STATE_LOCAL, self);
}

br_error BR_CMETHOD_DECL(br_renderer_soft,stateMask)
		(struct br_renderer *self, br_uint_32 *mask, br_token *parts, int n_parts)
{
	int i;
	br_uint_32 m = 0;

	/*
	 * Get mask from primitives
	 */
	PrimitiveLibraryMask(self->plib, &m,parts, n_parts);

	/*
	 * Add out own bits
	 */
	for(i=0; i < n_parts; i++) {
		switch(parts[i]) {
		case BRT_CULL:
			m |= MASK_STATE_CULL;
			break;

		case BRT_SURFACE:
			m |= MASK_STATE_SURFACE;
			break;

		case BRT_MATRIX:
			m |= MASK_STATE_MATRIX;
			break;

		case BRT_ENABLE:
			m |= MASK_STATE_ENABLE;
			break;

		case BRT_LIGHT:
			m |= MASK_STATE_LIGHT;
			break;

		case BRT_CLIP:
			m |= MASK_STATE_CLIP;
			break;
		}
	}

	*mask = m;

	return BRE_OK;
}

