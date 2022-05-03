/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: lighting.c 1.1 1997/12/10 16:52:12 jon Exp $
 * $Locker: $
 *
 * Geometry format for rendering the order table buckets spat out by a prior hidden surface render
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: lighting.c 1.1 1997/12/10 16:52:12 jon Exp $");

/*
 * Default dispatch table for renderer type (defined at end of file)
 */								 
static const struct br_geometry_lighting_dispatch geometryLightingDispatch;

/*
 * Geometry format info. template
 */
#define F(f)    offsetof(struct br_geometry_lighting, f)

static const struct br_tv_template_entry geometryLightingTemplateEntries[] = {
	{BRT_IDENTIFIER_CSTR,	0,	F(identifier),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_RENDERER_FACILITY_O,0,	F(renderer_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_FACILITY_O,			0,	F(renderer_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
};
#undef F

/*
 * Allocate a geometry format
 */
br_geometry_lighting * GeometryLightingAllocate(br_renderer_facility *type, char *id)
{
	br_geometry_lighting * self;

	self = BrResAllocate(type->device, sizeof(*self), BR_MEMORY_OBJECT_DATA);

	if(self == NULL)
		return NULL;

	self->dispatch = (struct br_geometry_lighting_dispatch *)&geometryLightingDispatch;
 	self->identifier = id;
        self->device = type->device;
 	self->renderer_facility = type;

	ObjectContainerAddFront(type, (br_object *)self);
	return self;
}

static void BR_CMETHOD_DECL(br_geometry_lighting_soft, free)(br_geometry_lighting *self)
{
	ObjectContainerRemove(self->renderer_facility, (br_object *)self);

	BrResFreeNoCallback(self);
}

static br_token BR_CMETHOD_DECL(br_geometry_lighting_soft, type)(br_geometry_lighting *self)
{
        return BRT_GEOMETRY_LIGHTING;
}

static br_boolean BR_CMETHOD_DECL(br_geometry_lighting_soft, isType)(br_geometry_lighting *self, br_token t)
{
        return (t == BRT_GEOMETRY_LIGHTING) || (t == BRT_GEOMETRY) || (t == BRT_OBJECT);
}

static br_int_32 BR_CMETHOD_DECL(br_geometry_lighting_soft, space)(br_geometry_lighting *self)
{
        return sizeof(br_geometry_lighting);
}

static struct br_tv_template * BR_CMETHOD_DECL(br_geometry_lighting_soft, templateQuery)
        (br_geometry_lighting *self)
{
    if(self->device->templates.geometryLightingTemplate == NULL)
        self->device->templates.geometryLightingTemplate = BrTVTemplateAllocate(self->device,
            (struct br_tv_template_entry *)geometryLightingTemplateEntries,
			BR_ASIZE(geometryLightingTemplateEntries));

    return self->device->templates.geometryLightingTemplate;
}

/*
 * Lighting function calling facility
 */

br_error BR_CMETHOD_DECL(br_geometry_lighting_soft, render)
		(struct br_geometry *self, struct br_renderer *renderer, 
                 br_vector3 *points, br_vector3 *normals,
                 br_colour *colour_in, br_colour *colour_out,
                 br_uint_16 *redirect, int pstride, int nstride,
                 int cinstride, int coutstride, int nvertices)
{
        int i, j;
        br_vector2 map = {BR_SCALAR(0.0), BR_SCALAR(0.0)};
        br_scalar comp[NUM_COMPONENTS];
        br_error r;

        if(renderer->state.pstate == NULL) {
            r = PrimitiveLibraryStateNew(renderer->plib, &renderer->state.pstate);
            if(r != BRE_OK)
                return r;
        }

        if(!scache.valid_per_scene){
            StaticCacheUpdate_PerScene(renderer);
            scache.valid_per_scene = BR_TRUE;
        }

        if(!scache.valid_per_model){
            StaticCacheUpdate_PerModel(renderer);
            scache.valid_per_model = BR_TRUE;
        }

		r = PrimitiveStateRenderBegin(renderer->state.pstate,
            &rend.block, &rend.block_changed, &rend.range_changed, 0,
            BRT_TRIANGLE);

        if(r != BRE_OK)
            return r;

        CacheUpdate(renderer);

        renderer->state.cache.comp_offsets[C_I] = BR_SCALAR(0.0);
        renderer->state.cache.comp_offsets[C_R] = BR_SCALAR(0.0);
        renderer->state.cache.comp_offsets[C_G] = BR_SCALAR(0.0);
        renderer->state.cache.comp_offsets[C_B] = BR_SCALAR(0.0);
        renderer->state.cache.comp_scales[C_I] = BR_SCALAR(256);
        renderer->state.cache.comp_scales[C_R] = BR_SCALAR(256);
        renderer->state.cache.comp_scales[C_G] = BR_SCALAR(256);
        renderer->state.cache.comp_scales[C_B] = BR_SCALAR(256);

        if(redirect)
                colour_out = (br_colour *)(((char *)colour_out) + coutstride*(*redirect));

        for(j = 0; j < nvertices; j++){
                for(i = 0; i < renderer->state.cache.nvertex_fns; i++)
                        renderer->state.cache.vertex_fns[i](renderer, points,
                                &map, normals, *colour_in, comp);
                for(i = 0; i < renderer->state.cache.nconstant_fns; i++)
                    renderer->state.cache.constant_fns[i](renderer, points,
                        &map, normals, *colour_in, comp);

                *((char *)colour_out) = BrScalarToInt(comp[C_I]);
                *(((char *)colour_out)+1) = BrScalarToInt(comp[C_R]);
                *(((char *)colour_out)+2) = BrScalarToInt(comp[C_G]);
                *(((char *)colour_out)+3) = BrScalarToInt(comp[C_B]);

                points = (br_vector3 *)((char *)points + pstride);
                normals = (br_vector3 *)((char *)normals + nstride);
                colour_in = (br_colour *)((char *)colour_in + cinstride);
                if(redirect){
                        colour_out = (br_colour *)(((char *)colour_out) + coutstride*((*(redirect+1))-(*redirect)));
                        redirect++;
                }
                else
                        colour_out = (br_colour *)(((char *)colour_out) + coutstride);
        }

		PrimitiveStateRenderEnd(renderer->state.pstate,rend.block);

        return BRE_OK;
}

#if BASED_FIXED
br_error BR_CMETHOD_DECL(br_geometry_lighting_soft, renderFloatToFixed)
		(struct br_geometry *self, struct br_renderer *renderer, 
                 br_vector3 *points, br_vector3 *normals,
                 br_colour *colour_in, br_colour *colour_out,
                 br_uint_16 *redirect, int pstride, int nstride,
                 int cinstride, int coutstride, int nvertices)
{
	return BRE_FAIL;
}
#endif

#if BASED_FLOAT
br_error BR_CMETHOD_DECL(br_geometry_lighting_soft, renderFixedToFloat)
		(struct br_geometry *self, struct br_renderer *renderer, 
                 br_vector3 *points, br_vector3 *normals,
                 br_colour *colour_in, br_colour *colour_out,
                 br_uint_16 *redirect, int pstride, int nstride,
                 int cinstride, int coutstride, int nvertices)
{
	return BRE_FAIL;
}
#endif

/*
 * Default dispatch table for renderer type
 */
static const struct br_geometry_lighting_dispatch geometryLightingDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
	BR_CMETHOD_REF(br_geometry_lighting_soft,     free),
	BR_CMETHOD_REF(br_object_soft,				identifier),
	BR_CMETHOD_REF(br_geometry_lighting_soft,     type),
	BR_CMETHOD_REF(br_geometry_lighting_soft,     isType),
	BR_CMETHOD_REF(br_object_soft,				device),
	BR_CMETHOD_REF(br_geometry_lighting_soft,     space),

	BR_CMETHOD_REF(br_geometry_lighting_soft,     templateQuery),
	BR_CMETHOD_REF(br_object,					query),
	BR_CMETHOD_REF(br_object,					queryBuffer),
	BR_CMETHOD_REF(br_object,					queryMany),
	BR_CMETHOD_REF(br_object,					queryManySize),
	BR_CMETHOD_REF(br_object,					queryAll),
	BR_CMETHOD_REF(br_object,					queryAllSize),

#if BASED_FIXED
	BR_CMETHOD_REF(br_geometry_lighting_soft,     renderFloatToFixed),
	BR_CMETHOD_REF(br_geometry_lighting_soft,     render),
#endif

#if BASED_FLOAT
	BR_CMETHOD_REF(br_geometry_lighting_soft,     render),
	BR_CMETHOD_REF(br_geometry_lighting_soft,     renderFixedToFloat),
#endif
};

