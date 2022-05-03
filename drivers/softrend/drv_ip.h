/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.4 1998/11/13 16:23:35 jon Exp $
 * $Locker: $
 *
 * Prototypes for functions internal to driver
 */
#ifndef _DRV_IP_H_
#define _DRV_IP_H_

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_soft, identifier)( br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_soft, device)( br_object *self);

/*
 * device.c
 */
br_device * DeviceSoftAllocate(char *identifier);

/*
 * rendtype.c
 */
br_renderer_facility * RendererFacilitySoftAllocate(br_device *dev, char *identifier);

/*
 * renderer.c
 */
extern const br_token RendererPartsTokens[];

br_renderer * RendererSoftAllocate(br_device *dev, struct br_renderer_facility *type, struct br_primitive_library *prims);

br_error BR_CMETHOD_DECL(br_renderer_soft,commandModeSet)
	(struct br_renderer *self, br_token mode);
br_error BR_CMETHOD_DECL(br_renderer_soft,commandModeQuery)
	(struct br_renderer *self, br_token *mode);
br_error BR_CMETHOD_DECL(br_renderer_soft,commandModeDefault)
	(struct br_renderer *self);
br_error BR_CMETHOD_DECL(br_renderer_soft,commandModePush)
	(struct br_renderer *self);
br_error BR_CMETHOD_DECL(br_renderer_soft,commandModePop)
	(struct br_renderer *self);
br_error BR_CMETHOD_DECL(br_renderer_soft, flush)
	(struct br_renderer *self, br_boolean wait);
br_error BR_CMETHOD_DECL(br_renderer_soft, synchronise)
	(struct br_renderer *self, br_token sync_type, br_boolean block);

/*
 * rendstat.c
 */
br_error CheckPrimitiveState(struct br_renderer *self);

br_error BR_CMETHOD_DECL(br_renderer_soft,partSet)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_token t, br_uint_32 value);
br_error BR_CMETHOD_DECL(br_renderer_soft,partSetMany)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_token_value * tv, br_int_32 *pcount);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQuery)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_uint_32 *pvalue, br_token t);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryBuffer)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_uint_32 *pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryMany)(struct br_renderer *self,
		br_token part, br_int_32 index,
		br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryManySize)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_size_t *pextra_size, br_token_value *tv);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryAll)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_token_value *buffer, br_size_t buffer_size);
br_error BR_CMETHOD_DECL(br_renderer_soft,partQueryAllSize)
		(struct br_renderer *self, br_token part, br_int_32 index,
		br_size_t *psize);

br_error BR_CMETHOD_DECL(br_renderer_soft, partQueryCapability)(
        struct br_renderer *self,
        br_token part,
        br_int_32 index,
        br_token_value *buffer,
        br_size_t buffer_size);

br_error BR_CMETHOD_DECL(br_renderer_soft, partIndexQuery)(
		struct br_renderer *self,
		br_token part,
		br_int_32 *pnindex);

br_error BR_CMETHOD_DECL(br_renderer_soft, stateQueryPerformance)(
        struct br_renderer *self,
        br_uint_32 *speed);

br_error BR_CMETHOD_DECL(br_renderer_soft,stateMask)
		(struct br_renderer *self, br_uint_32 *mask, br_token *parts, int n_parts);
br_error BR_CMETHOD_DECL(br_renderer_soft,stateDefault)
		(struct br_renderer *self, br_uint_32 mask);

br_error StateCopy(struct state_all *dest, struct state_all *src, br_uint_32 copy_mask, void *res);

br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulF)
		(struct br_renderer *self, br_matrix34_f *m);
br_error BR_CMETHOD_DECL(br_renderer_soft,modelMulX)
		(struct br_renderer *self, br_matrix34_x *m);
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulF)
		(struct br_renderer *self, br_matrix34_f *m);
br_error BR_CMETHOD_DECL(br_renderer_soft,modelPopPushMulX)
		(struct br_renderer *self, br_matrix34_x *m);
br_error BR_CMETHOD_DECL(br_renderer_soft,modelInvert)
		(struct br_renderer *self);

br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestF)
	(struct br_renderer *self, br_token *r, br_bounds3_f *bounds);
br_error BR_CMETHOD_DECL(br_renderer_soft,boundsTestX)
	(struct br_renderer *self, br_token *r, br_bounds3_x *bounds);

br_error BR_CMETHOD_DECL(br_renderer_soft, coverageTestF)
	(struct br_renderer *self, br_float *r, br_bounds3_f *bounds);
br_error BR_CMETHOD_DECL(br_renderer_soft, coverageTestX)
	(struct br_renderer *self, br_fixed_ls *r, br_bounds3_x *bounds);

br_error BR_CMETHOD_DECL(br_renderer_soft,viewDistanceF)
	(struct br_renderer *self, br_float *r);
br_error BR_CMETHOD_DECL(br_renderer_soft,viewDistanceX)
	(struct br_renderer *self, br_fixed_ls *r);

br_error BR_CMETHOD_DECL(br_renderer_soft,statePush)
		(struct br_renderer *self, br_uint_32 mask);
br_error BR_CMETHOD_DECL(br_renderer_soft,statePop)
		(struct br_renderer *self, br_uint_32 mask);
br_error BR_CMETHOD_DECL(br_renderer_soft,stateSave)
		(struct br_renderer *self, struct br_renderer_state_stored *save, br_uint_32 mask);
br_error BR_CMETHOD_DECL(br_renderer_soft,stateRestore)
		(struct br_renderer *self, struct br_renderer_state_stored *save, br_uint_32 mask);

br_error BR_CMETHOD_DECL(br_renderer_soft,stateDefault)
		(struct br_renderer *self, br_uint_32 mask);

br_error BR_CMETHOD_DECL(br_renderer_soft,stateMask)
		(struct br_renderer *self, br_uint_32 *mask, br_token *parts, int n_parts);

br_error BR_CMETHOD_DECL(br_renderer_soft, testRender)(
		struct br_renderer *self,
		br_token type,
		union brp_vertex *v0,
		union brp_vertex *v1,
		union brp_vertex *v2);

br_error BR_CMETHOD_DECL(br_renderer_soft, frameBegin)
		(void);

br_error BR_CMETHOD_DECL(br_renderer_soft, frameEnd)
		(void);

/*
 * sstate.c
 */
br_renderer_state_stored * RendererStateStoredSoftAllocate(br_renderer *renderer,
	struct state_all *base_state, br_uint_32 m, br_token_value *tv);

br_error StateCopyToStored(struct br_renderer_state_stored *dest, struct state_all *src, br_uint_32 copy_mask, void *res);

br_error StateCopyFromStored(struct state_all *dest, struct br_renderer_state_stored *src, br_uint_32 copy_mask, void *res);

/*
 * state.c
 */
br_error StateInitialise(struct state_all *state);
struct br_tv_template * FindStateTemplate(struct br_renderer *self, struct state_all **state, br_token part, br_int_32 index);
void TemplateActions(struct state_all *state, br_token part, br_int_32 index, br_uint_32 mask);
void TouchModelToView(br_renderer *self);

/*
 * genrend.c
 */
void GEOMETRY_CALL Vertex_ClearFlags(struct br_geometry *self, struct br_renderer *renderer);
void GEOMETRY_CALL ScratchFree(struct br_geometry *self, struct br_renderer *renderer);

void VertexGeometryFns(struct br_renderer *renderer, geometry_fn *prim_outcode);

/*
 * gprim.c
 */
br_geometry_primitives * GeometryPrimitivesAllocate(br_renderer_facility *type, char *id);

/*
 * gv1model.c
 */
struct br_geometry_v1_model * GeometryV1ModelAllocate(br_renderer_facility *type, char *id);

/*
 * gv1buckt.c
 */
struct br_geometry_v1_buckets * GeometryV1BucketsAllocate(br_renderer_facility *type, char *id);

/*
 * lighting.c
 */
struct br_geometry_lighting * GeometryLightingAllocate(br_renderer_facility *type, char *id);

/*
 * v1model.c
 */
br_error BR_CMETHOD_DECL(br_geometry_v1_model_soft, render)
		(struct br_geometry *self, struct br_renderer *renderer, struct v11model *model,
			struct br_renderer_state_stored *default_state,
			br_token type);

br_error BR_CMETHOD_DECL(br_geometry_v1_model_soft, renderOnScreen)
		(struct br_geometry *self, struct br_renderer *renderer, struct v11model *model,
			struct br_renderer_state_stored *default_state,
			br_token type);

/*
 * convert.c
 */
void BR_ASM_CALL RenderConvert1(struct brp_block *block,
	brp_vertex *v0);
void BR_ASM_CALL RenderConvert2(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1);
void BR_ASM_CALL RenderConvert3(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1,brp_vertex *v2);
void BR_ASM_CALL RenderConvert4(struct brp_block *block,
	brp_vertex *v0, brp_vertex *v1,brp_vertex *v2,brp_vertex *v3);

void ConvertVertex(brp_vertex *dest, brp_vertex *src);

/*
 * clip.c
 */
union brp_vertex *FaceClip(br_renderer *self, union brp_vertex *clip_in, br_uint_32 mask, br_uint_32 codes, int n, int *n_out);
void ClippedRenderTriangles(struct br_renderer *renderer, brp_block *block, union brp_vertex *cp_in, int n,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3]);
br_boolean ClipLine(br_renderer *self, union brp_vertex *out, union brp_vertex *v0, union brp_vertex *v1, br_uint_32 mask, br_uint_32 codes);
void ClippedRenderLine(struct br_renderer *renderer, brp_block *block, union brp_vertex *cp_in);

/*
 * setup.c
 */
br_error CacheUpdate(br_renderer *self);
void StaticCacheUpdate_PerScene(br_renderer *self);
void StaticCacheUpdate_PerModel(br_renderer *self);
br_error CacheClear(void);
void ViewToModelUpdate(br_renderer *self);
void ModelToScreenUpdate(br_renderer *self);

void ActiveLightsFind(br_renderer *self);
void ActiveLightsUpdate(br_renderer *self);

br_int_32 GenerateSurfaceFunctions(br_renderer *self, surface_fn **fns, br_uint_32 mask);

br_uint_32 ComponentMaskToSlots(br_uint_32 cm);

void GeometryFunctionReset(br_renderer *renderer);
void GeometryFunctionAdd(br_renderer *renderer, geometry_fn *fn);
void GeometryFunctionOnScreenAdd(br_renderer *renderer, geometry_fn *fn);
void GeometryFunctionBothAdd(br_renderer *renderer, geometry_fn *fn);

void PrimBlockReset(br_renderer *renderer);
void PrimBlockAdd(br_renderer *renderer, brp_render_fn *fn);
void PrimBlockOnScreenAdd(br_renderer *renderer, brp_render_fn *fn);
void PrimBlockAddBoth(br_renderer *renderer, brp_render_fn *fn);

/*
 * onscreen.c
 */
br_token OnScreenCheck(br_renderer *self, br_matrix4 *model_to_screen, br_bounds3 *bounds);

/*
 * light8.c
 */
void SURFACE_CALL SurfaceIndexZero(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceIndexUnlit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceIndexLit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceIndexLitSimple(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void ActiveLightAccumulateIndexSet(struct active_light *alp);

void SURFACE_CALL SurfaceIndexLit1MD(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);

/*
 * light24.c
 */
void SURFACE_CALL SurfaceColourZero(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceColourUnlit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceColourLit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceColourLitPrelit(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void SURFACE_CALL SurfaceColourLitSimple(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour gcolour, br_scalar *comp);
void ActiveLightAccumulateColourSet(struct active_light *alp);

/*
 * mapping.c
 */
void SURFACE_CALL SurfaceMapEnvironmentInfinite(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *normal, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapEnvironmentLocal(br_renderer *self,
	br_vector3 *p, br_vector2 *map, br_vector3 *normal, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryX(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryY(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryZ(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryMap(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryMapScale(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryMapScaleTranslate(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryMapCopy(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceMapGeometryMapShift(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);

void BR_ASM_CALL OpTriangleMapQuad(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

/*
 * alpha.c
 */
void SURFACE_CALL SurfaceAlpha(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);
void SURFACE_CALL SurfaceAlphaPrealpha(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);

/*
 * depth.c
 */
void SURFACE_CALL SurfaceLinearDepth(br_renderer *self, br_vector3 *p, br_vector2 *map, br_vector3 *n, br_colour colour, br_scalar *comp);

/*
 * faceops.c
 */
void BR_ASM_CALL OpTriangleClip(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);
void BR_ASM_CALL OpTriangleClipConstantSurf(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);
void BR_ASM_CALL OpTriangleConstantSurf(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);
void BR_ASM_CALL OpTriangleTwoSidedConstantSurf(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

void BR_ASM_CALL OpTriangleMappingWrapFix(struct brp_block *block, union brp_vertex *v0, union brp_vertex *v1, union brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

void BR_ASM_CALL OpTriangleRelightTwoSided(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

void BR_ASM_CALL OpTriangleToLines(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3]);
void BR_ASM_CALL OpTriangleReplicateConstant(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3]);
void BR_ASM_CALL OpTriangleReplicateConstantI(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3]);
void BR_ASM_CALL OpTriangleReplicateConstantRGB(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3]);
void BR_ASM_CALL OpTriangleToPoints(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3]);
void BR_ASM_CALL OpTriangleToPoints_OS(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3]);
void BR_ASM_CALL OpLineClip(struct brp_block *block, brp_vertex *v0, brp_vertex *v1);

void BR_ASM_CALL OpTriangleSubdivide(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

void BR_ASM_CALL OpTriangleSubdivideOnScreen(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);

void SubdivideSetThreshold(br_int_32 subdivide_tolerance);

/*
 * heap.c
 */
void BR_ASM_CALL OpHeapAddPoint(struct brp_block *block, brp_vertex *v0);
void BR_ASM_CALL OpHeapAddLine(struct brp_block *block, brp_vertex *v0, brp_vertex *v1);
void BR_ASM_CALL OpHeapAddTriangle(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

void BR_ASM_CALL OpHeapAddPointConvert(struct brp_block *block, brp_vertex *v0);
void BR_ASM_CALL OpHeapAddLineConvert(struct brp_block *block, brp_vertex *v0, brp_vertex *v1);
void BR_ASM_CALL OpHeapAddTriangleConvert(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);

/*
 * Platform dependant downcoded geometry support
 */
#ifdef __386__
#include "v1m_386.h"
#include "gen_386.h"
#endif

/* Rampant optimism ...
 */

#ifdef __MIPS__
#endif

#ifdef __ALPHA__
#endif

#ifdef __PPC__
#endif

#ifdef __680X0__
#endif

#ifdef __cplusplus
};
#endif

#endif
#endif

