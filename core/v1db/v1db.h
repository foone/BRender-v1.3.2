/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: v1db.h 1.1 1997/12/10 16:41:33 jon Exp $
 * $Locker: $
 *
 * Internal structures for V1 database
 */
#ifndef _V1DB_H_
#define _V1DB_H_

/*
 * Pull in all the framework definitions/declarations
 */
#ifndef _BRDDI_H_
#include "brddi.h"
#endif

/*
 * Hard-wired registry limits for evaluation build
 */
#if EVAL
#define MODEL_LIMIT		5
#define MATERIAL_LIMIT	10
#define MAP_LIMIT		10
#define TABLE_LIMIT		5
#endif

/*
 * Structure for maintaining enabled actors
 */
typedef struct br_v1db_enable {
	br_int_32	max;
	br_int_32	count;
	br_int_32	type;
	char 		*name;
	br_actor	**enabled;
} br_v1db_enable;

/*
 * Private state of database
 */
typedef struct br_v1db_state {
	/*
	 * Flag to indicate that database is set up
	 */
	br_boolean active;

	/*
	 * Flags for old renderer types being active
	 */
	br_boolean zs_active;
	br_boolean zb_active;

	/*
	 * Flag that is !0 if rendering is going on
	 */
	br_int_32 rendering;

	/*
	 * Renderer being used
	 */
	struct br_renderer *renderer;

	/*
     * Renderer being queried
	 */
    struct br_renderer *query_renderer;

	/*
	 * Geometry formats
	 */
	struct br_geometry *format_model;
	struct br_geometry *format_buckets;
    struct br_geometry_lighting *format_lighting;

	/*
	 * Copies of transforms for use by callbacks
	 */
	br_matrix4 model_to_screen;
	br_matrix34 model_to_view;
	br_boolean model_to_screen_valid;

	/*
	 * Combined transform type for initial model_to_view
	 */
	br_uint_32 ttype;

	/*
	 * Current rendering root
	 */
	br_actor *render_root;

	/*
	 * List of transforms from camera to root with associated actor address
	 */
	struct {
		br_matrix34 m;
		br_actor *a;
		br_uint_8 transform_type;
	} camera_path[MAX_CAMERA_DEPTH];

	/*
	 * Table of pointers to enabled actors of various sorts
	 */
	br_v1db_enable enabled_lights;
	br_v1db_enable enabled_clip_planes;
	br_v1db_enable enabled_horizon_planes;

	/*
	 * Number of renderer parts used
	 */
	br_int_32 max_light;
	br_int_32 max_clip;

	/*
	 * A pointer to the current environment - if NULL, then the model's
	 * local frame will be used
	 */
	br_actor *enabled_environment;

	/*
	 * Various lists of registered items
	 */
	br_registry reg_models;
	br_registry reg_materials;
	br_registry reg_textures;
	br_registry reg_tables;

	/*
	 * Base resource of which everything else is
	 * a descendant
	 */
	void *res;

	/*
	 * Default model
	 */
	br_model *default_model;

	/*
	 * Default material
	 */
	br_material *default_material;

	/*
	 * Default render data
	 */
	void *default_render_data;

	/*
	 * Default order table
	 */
	br_order_table *default_order_table;

	/*
	 * Primary order table
	 */
	br_order_table *primary_order_table;

	/*
	 * Pointer to linked list of order tables
	 */
	br_order_table *order_table_list;

	/*
	 * Primitive heap
	 */
	br_primitive_heap heap;

	/*
	 * Primitive calback fr Z Sort
	 */
	br_primitive_cbfn *primitive_call;

	/*
	 * Callback for visible actors
	 */
	br_renderbounds_cbfn *bounds_call;

	/*
	 * Position in view of origin as a fraction of viewport size
	 */
	br_vector2 origin;

	/*
	 * Local copies of viewport centre and size
	 */
	br_scalar vp_ox, vp_oy, vp_width, vp_height;

	/*
	 * Local copy of colour_buffer pointer
	 */
	br_pixelmap *colour_buffer;

} br_v1db_state;

/*
 * Values for v1db.rendering
 */
enum {
	RENDERING_NONE = 0,
	RENDERING_ZB,
	RENDERING_ZS
};
/*
 * Global renderer state
 */
#ifdef __cplusplus
extern "C" {
#endif

extern br_v1db_state BR_ASM_DATA v1db;

#ifdef __cplusplus
}:
#endif

/*
 * Private model flag to indicate when model is using default material
 */
#define MODF_USES_DEFAULT			0x8000


/*
 * Define some of the device methods for scalars
 */
#if BASED_FLOAT
#define RendererModelMul				RendererModelMulF
#define RendererModelPopPushMul			RendererModelPopPushMulF
#define RendererBoundsTest				RendererBoundsTestF
#define RendererCoverageTest			RendererCoverageTestF
#define RendererViewDistance			RendererViewDistanceF

#define GeometryV1ModelRenderOnScreen	GeometryV1ModelRenderOnScreenF
#define GeometryV1ModelRender			GeometryV1ModelRenderF
#define GeometryV1ModelStoredNew		GeometryV1ModelStoredNewF

#define GeometryLightingRender                  GeometryLightingRenderF
#endif

#if BASED_FIXED
#define RendererModelMul				RendererModelMulX
#define RendererModelPopPushMul			RendererModelPopPushMulX
#define RendererBoundsTest				RendererBoundsTestX
#define RendererCoverageTest			RendererCoverageTestX
#define RendererViewDistance			RendererViewDistanceX

#define GeometryV1ModelRenderOnScreen	GeometryV1ModelRenderOnScreenX
#define GeometryV1ModelRender			GeometryV1ModelRenderX
#define GeometryV1ModelStoredNew		GeometryV1ModelStoredNewX

#define GeometryLightingRender                  GeometryLightingRenderX
#endif

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _V1DB_IP_H_
#include "v1db_ip.h"
#endif

#ifndef _V1DB_IX_H_
#include "v1db_ix.h"
#endif

#endif

#ifdef __cplusplus
};
#endif
#endif

