/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: v1db_p.h 1.8 1998/11/12 13:19:16 johng Exp $
 * $Locker: $
 *
 * Public function prototypes for BRender V1 database
	Last change:  TN    9 Apr 97    4:40 pm
 */
#ifndef _V1DB_P_H_
#define _V1DB_P_H_

#if BASED_FIXED
#define BrV1dbBeginWrapper BrV1dbBeginWrapper_Fixed
#endif

#if BASED_FLOAT
#define BrV1dbBeginWrapper BrV1dbBeginWrapper_Float
#endif

/*
 * Setup - overide framework BrBegin/End with our own wrappers
 */
#define BrBegin BrV1dbBeginWrapper
#define BrEnd BrV1dbEndWrapper

#define BR_BOUNDS_MIN_X	0
#define BR_BOUNDS_MIN_Y	1
#define BR_BOUNDS_MAX_X	2
#define BR_BOUNDS_MAX_Y	3

/*
 * Callback function invoked when an actor is
 * rendered
 */
typedef void BR_CALLBACK br_renderbounds_cbfn(
	br_actor *actor,
	br_model *model,
	br_material *material,
	void *render_data,
	br_uint_8	style,
	br_matrix4 *model_to_screen,
	br_int_32 bounds[4]);

/*
 * Callback function invoked when a z-sort primitive is generated
 */
typedef void BR_CALLBACK br_primitive_cbfn(
	br_primitive *primitive,
	br_actor *actor,
	br_model *model,
	br_material *material,
	br_order_table *order_table,
	br_scalar *z);

#ifndef _NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

br_error BR_PUBLIC_ENTRY BrV1dbBeginWrapper(void);
br_error BR_PUBLIC_ENTRY BrV1dbEndWrapper(void);

br_error BR_PUBLIC_ENTRY BrV1dbRendererBegin(struct br_device_pixelmap *destination, struct br_renderer *renderer);
struct br_renderer * BR_PUBLIC_ENTRY BrV1dbRendererQuery(void);
br_error BR_PUBLIC_ENTRY BrV1dbRendererEnd(void);

/*
 * XXX - All the Add/Remove/Find/Count/Enum calls could ( when !(DEBUG | PARANOID))
 * be #defined in terms of the core Registry fns. with appropriate casts.
 */
/*
 * Material Handling
 */
br_material * BR_PUBLIC_ENTRY BrMaterialAdd(br_material *material);
br_material * BR_PUBLIC_ENTRY BrMaterialRemove(br_material *material);
br_material * BR_PUBLIC_ENTRY BrMaterialFind(char *pattern);
typedef br_material * BR_CALLBACK br_material_find_cbfn(char *name);
br_material_find_cbfn * BR_PUBLIC_ENTRY BrMaterialFindHook(br_material_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialAddMany(br_material **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialRemoveMany(br_material **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialFindMany(char *pattern, br_material **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_material_enum_cbfn(br_material *item, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialEnum(char *pattern,
		br_material_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrMaterialUpdate(br_material *material, br_uint_16 flags);

br_material * BR_PUBLIC_ENTRY BrMaterialAllocate(char *name);
void BR_PUBLIC_ENTRY BrMaterialFree(br_material *m);

/*
 * Model Handling
 */
br_model * BR_PUBLIC_ENTRY BrModelAdd(br_model *model);
br_model * BR_PUBLIC_ENTRY BrModelRemove(br_model *model);
br_model * BR_PUBLIC_ENTRY BrModelFind(char *pattern);
typedef br_model * BR_CALLBACK br_model_find_cbfn(char *name);
br_model_find_cbfn * BR_PUBLIC_ENTRY BrModelFindHook(br_model_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrModelAddMany(br_model **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrModelRemoveMany(br_model **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrModelFindMany(char *pattern, br_model **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrModelCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_model_enum_cbfn(br_model *item, void *arg);

br_uint_32 BR_PUBLIC_ENTRY BrModelEnum(char *pattern,
		br_model_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrModelUpdate(br_model *model, br_uint_16 flags);

void BR_PUBLIC_ENTRY BrModelApplyMap(br_model *model,int map_type, br_matrix34 *xform);
br_matrix34 * BR_PUBLIC_ENTRY BrModelFitMap(br_model *model, int axis_0, int axis_1, br_matrix34 *transform);

br_model * BR_PUBLIC_ENTRY BrModelAllocate(char *name, int nvertices, int nfaces);
void BR_PUBLIC_ENTRY BrModelFree(br_model *m);

/*
 * New primitive handling
 */

br_primitive_list * BR_PUBLIC_ENTRY BrPrimitiveListAllocate(br_uint_32 prim_type, br_uint_16 num_prims);
br_uint_32 BR_PUBLIC_ENTRY BrModelAddPrimitiveList(br_model *model, br_primitive_list *primitive_list);

/*
 * Texture handling
 */
br_pixelmap * BR_PUBLIC_ENTRY BrMapAdd(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrMapRemove(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrMapFind(char *pattern);
typedef br_pixelmap * BR_CALLBACK br_map_find_cbfn(char *name);
br_map_find_cbfn * BR_PUBLIC_ENTRY BrMapFindHook(br_map_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrMapAddMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMapRemoveMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrMapFindMany(char *pattern, br_pixelmap **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrMapCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_map_enum_cbfn(br_pixelmap *item, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrMapEnum(char *pattern,
		br_map_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrMapUpdate(br_pixelmap *item, br_uint_16 flags);

/*
 * Index lighting table handling
 */
br_pixelmap * BR_PUBLIC_ENTRY BrTableAdd(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrTableRemove(br_pixelmap *pixelmap);
br_pixelmap * BR_PUBLIC_ENTRY BrTableFind(char *pattern);
typedef br_pixelmap * BR_CALLBACK br_table_find_cbfn(char *name);
br_table_find_cbfn *BR_PUBLIC_ENTRY BrTableFindHook(br_table_find_cbfn *hook);
br_uint_32 BR_PUBLIC_ENTRY BrTableAddMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrTableRemoveMany(br_pixelmap **items, int n);
br_uint_32 BR_PUBLIC_ENTRY BrTableFindMany(char *pattern, br_pixelmap **items, int max);
br_uint_32 BR_PUBLIC_ENTRY BrTableCount(char *pattern);

typedef br_uint_32 BR_CALLBACK br_table_enum_cbfn(br_pixelmap *item, void *arg);

br_uint_32 BR_PUBLIC_ENTRY BrTableEnum(char *pattern,
		br_table_enum_cbfn *callback, void *arg);

void BR_PUBLIC_ENTRY BrTableUpdate(br_pixelmap *item, br_uint_16 flags);

/*
 * Actor Handling
 */
typedef br_uint_32 BR_CALLBACK br_actor_enum_cbfn(br_actor *mat, void *arg);
br_uint_32 BR_PUBLIC_ENTRY BrActorEnum(br_actor *parent, br_actor_enum_cbfn *callback , void *arg);

br_actor * BR_PUBLIC_ENTRY BrActorAdd(br_actor *parent, br_actor *a);
br_actor * BR_PUBLIC_ENTRY BrActorRemove(br_actor *a);
void BR_PUBLIC_ENTRY BrActorRelink(br_actor *parent,br_actor *actor);
br_uint_16 BR_PUBLIC_ENTRY BrActorToActorMatrix34(br_matrix34 *m, br_actor *a, br_actor *b);
void BR_PUBLIC_ENTRY BrActorToScreenMatrix4(br_matrix4 *m, br_actor *a, br_actor *camera);

br_actor * BR_PUBLIC_ENTRY BrActorAllocate(br_uint_8 actor_type, void * type_data);
void BR_PUBLIC_ENTRY BrActorFree(br_actor *a);

br_uint_32 BR_PUBLIC_ENTRY BrActorSearchMany(br_actor *root, char *pattern, br_actor **actors, int max);
br_actor * BR_PUBLIC_ENTRY BrActorSearch(br_actor *root, char *pattern);

br_bounds * BR_PUBLIC_ENTRY BrActorToBounds( br_bounds *b, br_actor *ap);

/*
 * File operations
 */
br_model * BR_PUBLIC_ENTRY BrModelLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrModelSave(char *filename,br_model *model);
br_uint_32 BR_PUBLIC_ENTRY BrModelLoadMany(char *filename,br_model **models,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrModelSaveMany(char *filename,br_model **models,br_uint_16 num);
br_error BR_PUBLIC_ENTRY BrModelFileCount(const char *filename, br_uint_16 *num);

br_material * BR_PUBLIC_ENTRY BrMaterialLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialSave(char *filename,br_material *material);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialLoadMany(char *filename,br_material **materials,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrMaterialSaveMany(char *filename,br_material **materials,br_uint_16 num);
br_error BR_PUBLIC_ENTRY BrMaterialFileCount(const char *filename, br_uint_16 *num);

br_actor * BR_PUBLIC_ENTRY BrActorLoad(char *filename);
br_uint_32 BR_PUBLIC_ENTRY BrActorSave(char *filename, br_actor *actor);
br_uint_32 BR_PUBLIC_ENTRY BrActorLoadMany(char *filename,br_actor **actors,br_uint_16 num);
br_uint_32 BR_PUBLIC_ENTRY BrActorSaveMany(char *filename,br_actor **actors,br_uint_16 num);
br_error BR_PUBLIC_ENTRY BrActorFileCount(const char *filename, br_uint_16 *num);

/*
 * Lights
 */
void BR_PUBLIC_ENTRY BrLightEnable(br_actor *l);
void BR_PUBLIC_ENTRY BrLightDisable(br_actor *l);

/*
 * Environment
 */
br_actor * BR_PUBLIC_ENTRY BrEnvironmentSet(br_actor *a);

/*
 * Clip planes
 */
void BR_PUBLIC_ENTRY BrClipPlaneEnable(br_actor *cp);
void BR_PUBLIC_ENTRY BrClipPlaneDisable(br_actor *cp);

/*
 * Horizon planes
 */
void BR_PUBLIC_ENTRY BrHorizonPlaneEnable(br_actor *h);
void BR_PUBLIC_ENTRY BrHorizonPlaneDisable(br_actor *h);

/*
 * Picking
 */
typedef int BR_CALLBACK br_pick2d_cbfn(
		br_actor *a,
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t_near, br_scalar t_far,
		void *arg);

int BR_PUBLIC_ENTRY BrScenePick2D(
		br_actor *world,
		br_actor *camera,
		br_pixelmap *viewport,
		int pick_x, int pick_y,
		br_pick2d_cbfn *callback, void *arg );

typedef int BR_CALLBACK br_pick3d_cbfn(
		br_actor *a,
		br_model *model,
		br_material *material,
		br_matrix34 *transform,
		br_bounds *bounds,
		void *arg);

int BR_PUBLIC_ENTRY BrScenePick3D(
		br_actor *world,
		br_actor *actor,
		br_bounds *bounds,
		br_pick3d_cbfn *callback, void *arg );

typedef int BR_CALLBACK br_modelpick2d_cbfn(
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t,
		int face,
		int edge,
		int vertex,
		br_vector3 *p,
		br_vector2 *map,
		void *arg);

int BR_PUBLIC_ENTRY BrModelPick2D(
		br_model *model,
		br_material *material,
		br_vector3 *ray_pos, br_vector3 *ray_dir,
		br_scalar t_near, br_scalar t_far,
		br_modelpick2d_cbfn *callback,
		void *arg);

/*
 * Custom calback support
 */
br_uint_32 BR_PUBLIC_ENTRY BrOnScreenCheck(br_bounds3 *bounds);

br_uint_8 BR_PUBLIC_ENTRY BrOriginToScreenXY(br_vector2 *screen);
br_uint_32 BR_PUBLIC_ENTRY BrOriginToScreenXYZO(br_vector3 *screen);

br_uint_8 BR_PUBLIC_ENTRY BrPointToScreenXY(br_vector2 *screen, br_vector3 *point);
br_uint_32 BR_PUBLIC_ENTRY BrPointToScreenXYZO(br_vector3 *screen, br_vector3 *point);

void BR_PUBLIC_ENTRY BrPointToScreenXYMany(br_vector2 *screens, br_vector3 *points, br_uint_32 npoints);
void BR_PUBLIC_ENTRY BrPointToScreenXYZOMany(br_vector3 *screens, br_uint_32 *outcodes, br_vector3 *points, br_uint_32 npoints);

void BR_PUBLIC_ENTRY BrSceneModelLight(br_model *model, br_material *default_material, br_actor *root, br_actor *a);

void BR_PUBLIC_ENTRY BrModelToScreenQuery(br_matrix4 *dest);
void BR_PUBLIC_ENTRY BrModelToViewQuery(br_matrix34 *dest);

br_scalar BR_PUBLIC_ENTRY BrZbDepthToScreenZ(br_uint_32 depth_z,const br_camera* camera);
br_uint_32 BR_PUBLIC_ENTRY BrZbScreenZToDepth(br_scalar sz,const br_camera* camera);

br_scalar BR_PUBLIC_ENTRY BrZsDepthToScreenZ(br_scalar depth_z,const br_camera* camera);
br_scalar BR_PUBLIC_ENTRY BrZsScreenZToDepth(br_scalar sz,const br_camera* camera);

br_scalar BR_PUBLIC_ENTRY BrScreenZToCamera(const br_actor* camera, br_scalar sz);
void BR_PUBLIC_ENTRY BrScreenXYZToCamera(br_vector3* point,const br_actor* camera,
	const br_pixelmap* screen_buffer, br_int_16 x, br_int_16 y,br_scalar sz);

br_error BR_PUBLIC_ENTRY BrLightModelCull(br_actor *light);

/*
 * Utility "FindFailed" callbacks that can be used to automaticaly load
 * models/materials/maps/tables from the filesystem
 */
br_pixelmap * BR_CALLBACK BrMapFindFailedLoad(char *name);
br_pixelmap * BR_CALLBACK BrTableFindFailedLoad(char *name);
br_model * BR_CALLBACK BrModelFindFailedLoad(char *name);
br_material * BR_CALLBACK BrMaterialFindFailedLoad(char *name);

/*
 * Backwards comaptibility
 */
#define BrModelPrepare BrModelUpdate
#define BrMaterialPrepare BrMaterialUpdate
#define BrMapPrepare BrMapUpdate
#define BrTablePrepare BrTableUpdate

/*
 * Rendering - General
 */
void BR_PUBLIC_ENTRY BrRendererBegin(br_pixelmap *destination,
	struct br_renderer_facility *renderer_facility, struct br_primitive_library *primitive_library,
	void *primitive_heap, br_uint_32 primitive_heap_size);

void BR_PUBLIC_ENTRY BrRendererEnd(void);

void BR_PUBLIC_ENTRY BrRendererFrameBegin();
void BR_PUBLIC_ENTRY BrRendererFrameEnd();

void BR_PUBLIC_ENTRY BrRendererFocusLossBegin();
void BR_PUBLIC_ENTRY BrRendererFocusLossEnd();

/*
 * Renderering - Z Buffer
 */
void BR_PUBLIC_ENTRY BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type);
void BR_PUBLIC_ENTRY BrZbEnd(void);

void BR_PUBLIC_ENTRY BrZbSceneRenderBegin(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer);

void BR_PUBLIC_ENTRY BrZbSceneRenderContinue(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer);

void BR_PUBLIC_ENTRY BrZbSceneRenderAdd(br_actor *tree);
void BR_PUBLIC_ENTRY BrZbSceneRenderEnd(void);

void BR_PUBLIC_ENTRY BrZbSceneRender(
					br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer);

/*
 * Used within custom model callbacks to render other models
 */
void BR_PUBLIC_ENTRY BrZbModelRender(br_actor *actor,
				br_model *model,
				br_material *material,
				br_uint_8 style,
				int on_screen,
				int use_custom);

br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrZbRenderBoundsCallbackSet(br_renderbounds_cbfn *new_cbfn);


/*
 * Renderering - Z Sort
 */
void BR_PUBLIC_ENTRY BrZsBegin(br_uint_8 colour_type, void *primitive, br_uint_32 size);
void BR_PUBLIC_ENTRY BrZsEnd(void);

void BR_PUBLIC_ENTRY BrZsSceneRenderBegin(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer);

void BR_PUBLIC_ENTRY BrZsSceneRenderContinue(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer);

void BR_PUBLIC_ENTRY BrZsSceneRenderAdd(br_actor *tree);
void BR_PUBLIC_ENTRY BrZsSceneRenderEnd(void);

void BR_PUBLIC_ENTRY BrZsSceneRender(
					br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer);

/*
 * Used within custom model callbacks to render other models
 */
void BR_PUBLIC_ENTRY BrZsModelRender(br_actor *actor,
				br_model *model,
				br_material *material,
				br_order_table *order_table,
				br_uint_8 style,
				int on_screen,
				int use_custom);

br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrZsRenderBoundsCallbackSet(br_renderbounds_cbfn *new_cbfn);

br_primitive_cbfn * BR_PUBLIC_ENTRY BrZsPrimitiveCallbackSet(br_primitive_cbfn *new_cbfn);

struct br_order_table * BR_PUBLIC_ENTRY BrZsOrderTableAllocate(br_uint_16 size,br_uint_32 flags,br_uint_16 type);
void BR_PUBLIC_ENTRY BrZsOrderTableFree(br_order_table *order_table);
struct br_order_table * BR_PUBLIC_ENTRY BrZsActorOrderTableSet(struct br_actor *actor, struct br_order_table *order_table);
struct br_order_table * BR_PUBLIC_ENTRY BrZsActorOrderTableGet(struct br_actor *actor);
struct br_order_table * BR_PUBLIC_ENTRY BrZsOrderTableClear(struct br_order_table *order_table);

void BR_PUBLIC_ENTRY BrZsOrderTablePrimitiveInsert(struct br_order_table *order_table,
												   struct br_primitive *primitive,
												   br_uint_16 bucket);

br_uint_16 BR_PUBLIC_ENTRY BrZsPrimitiveBucketSelect(br_scalar *z,
													 br_uint_16 type,
													 br_scalar min_z,
													 br_scalar max_z,
													 br_uint_16 size,
													 br_uint_16 sort_type);
void BR_PUBLIC_ENTRY BrZsOrderTablePrimaryEnable(struct br_order_table *order_table);
void BR_PUBLIC_ENTRY BrZsOrderTablePrimaryDisable(void);

/*
 * Backwards compatibility
 */
#define BrZbSetRenderBoundsCallback BrZbRenderBoundsCallbackSet
#define BrZsSetRenderBoundsCallback BrZsRenderBoundsCallbackSet
#define BrZsSetPrimitiveCallback BrZsPrimitiveCallbackSet


/*
 * Order table traversal
 */

typedef void BR_CALLBACK zs_order_table_traversal_cbfn(int primitive_type,ot_vertex *v0,ot_vertex *v1,ot_vertex *v2);
void BR_PUBLIC_ENTRY ZsOrderTableTraversal(zs_order_table_traversal_cbfn *cbfn);


/*
 * Callback function invoked when a renderer facility is enumerated
 */
typedef struct br_rendfcty_desc {

	br_boolean uses_primitive_library;

	/*
	 * A pointer to the renderer facility.  Note that if the enumeration
	 * was performed within an output facility enumeration, this is only
	 * guaranteed to remain valid within the callback routine.
	 */
	struct br_renderer_facility *renderer_facility;

} br_rendfcty_desc;

typedef br_boolean BR_CALLBACK br_rendfcty_enum_cbfn(char *identifier,
    br_rendfcty_desc *desc, void *args);

/*
 * Callback function invoked when a primitive library is enumerated
 */
typedef struct br_primlib_desc {

	/*
	 * A pointer to the primitive library.  Note that if the enumeration
	 * was performed within an output facility enumeration, this is only
	 * guaranteed to remain valid within the callback routine.
	 */
	struct br_primitive_library *primitive_library;

} br_primlib_desc;

typedef br_boolean BR_CALLBACK br_primlib_enum_cbfn(char *identifier,
    br_primlib_desc *desc, void *args);

/*
 * Enumeration routines.
 */
br_error BR_PUBLIC_ENTRY BrRendererFacilityEnum(br_pixelmap *destination, br_rendfcty_enum_cbfn *cbfn, void *args);
br_error BR_PUBLIC_ENTRY BrPrimitiveLibraryEnum(br_pixelmap *destination, br_primlib_enum_cbfn *cbfn, void *args);

#ifndef NO_PROTOTYPES

#if defined(_MSC_VER) && !defined(_USE_INTEL_COMPILER)

__inline br_actor * BR_PUBLIC_ENTRY BrActorAddNoRenumber(br_actor *parent, br_actor *a)
{
//	UASSERT_MESSAGE("NULL pointer to the parent's new child", a != NULL);
//	UASSERT_MESSAGE("NULL pointer to the parental actor", parent != NULL);
//	UASSERT(a->prev == NULL);

	/*
	 * Link this actor into sibling list of parent
	 */
	BR_SIMPLEADDHEAD(&parent->children,a);
	a->parent = parent;

	return a;
}

__inline br_actor * BR_PUBLIC_ENTRY BrActorRemoveNoRenumber(br_actor *a)
{
//	UASSERT_MESSAGE("NULL pointer to the hierrachy to remove", a != NULL);
//	UASSERT(a->prev != NULL);
	
	BR_SIMPLEREMOVE(a);

	a->parent = NULL;

	return a;
}


#endif

#endif

#ifdef __cplusplus
};
#endif

#endif /* _NO_PROTOTYPES */

#endif


