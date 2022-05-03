/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dbsetup.c 1.6 1998/03/10 15:18:54 jon Exp $
 * $Locker: $
 *
 */
#include "v1db.h"
#include "host.h"
#include "datafile.h"
#include <brassert.h>


/*
 * Global database state
 */
br_v1db_state BR_ASM_DATA v1db;

/*
 * Various bits of default data
 */
extern br_model _BrDefaultModel;
extern br_order_table _BrDefaultOrderTable;

extern br_material *SetupDefaultMaterial();



#if EVAL
/*
 * Static registry lists for evaluation build
 */
br_registry_entry modelRegistryList[MODEL_LIMIT];
br_registry_entry materialRegistryList[MATERIAL_LIMIT];
br_registry_entry mapRegistryList[MAP_LIMIT];
br_registry_entry tableRegistryList[TABLE_LIMIT];
#endif

/*
 * System resource classes
 */
static br_resource_class resourceClasses[] = {
	{0,"VERTICES",			BR_MEMORY_VERTICES, 		NULL },
	{0,"FACES",				BR_MEMORY_FACES, 			NULL },
	{0,"MODEL",				BR_MEMORY_MODEL, 			NULL },
	{0,"MATERIAL",			BR_MEMORY_MATERIAL, 		NULL },
	{0,"MATERIAL_INDEX",	BR_MEMORY_MATERIAL_INDEX,	NULL },
	{0,"ACTOR",				BR_MEMORY_ACTOR, 			NULL },
	{0,"LIGHT",				BR_MEMORY_LIGHT, 			NULL },
	{0,"CAMERA",			BR_MEMORY_CAMERA, 			NULL },
	{0,"BOUNDS",			BR_MEMORY_BOUNDS, 			NULL },
	{0,"CLIP_PLANE",		BR_MEMORY_CLIP_PLANE,		NULL },
	{0,"TRANSFORM",			BR_MEMORY_TRANSFORM, 		NULL },
	{0,"ENABLED_ACTORS",	BR_MEMORY_ENABLED_ACTORS,	NULL },
	{0,"PREPARED_MODEL",	BR_MEMORY_PREPARED_MODEL,	NULL,	32 },
	{0,"ORDER_TABLE",		BR_MEMORY_ORDER_TABLE,		NULL,	32 },
};

br_error BR_PUBLIC_ENTRY BrV1dbBegin(void)
{
	int i;
	char *devstr;
	br_size_t s;

	if(v1db.active)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Zero out whole static structure (paranoid)
	 */
	BrMemSet(&v1db, 0, sizeof(v1db));

	/*
	 * Mark as active
	 */
	v1db.active = BR_TRUE;

	/*
	 * Initialise all registries
	 */
#if EVAL
	BrRegistryNewStatic(&v1db.reg_models, modelRegistryList, MODEL_LIMIT);
	BrRegistryNewStatic(&v1db.reg_materials, materialRegistryList, MATERIAL_LIMIT);
	BrRegistryNewStatic(&v1db.reg_textures, mapRegistryList, MAP_LIMIT);
	BrRegistryNewStatic(&v1db.reg_tables, tableRegistryList, TABLE_LIMIT);
#else
	BrRegistryNew(&v1db.reg_models);
	BrRegistryNew(&v1db.reg_materials);
	BrRegistryNew(&v1db.reg_textures);
	BrRegistryNew(&v1db.reg_tables);
#endif

	/*
	 * Allocate the zero sized base resource instance
	 */
	v1db.res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);

	/*
	 * Register all our resource classes
	 */
	for(i=0; i < BR_ASIZE(resourceClasses); i++)
		BrResClassAdd(resourceClasses+i);

	/*
	 * Create default model
	 */
	v1db.default_model =
		BrResAllocate(v1db.res, sizeof(br_model), BR_MEMORY_MODEL);
		
	*v1db.default_model = _BrDefaultModel;

	/*
	 * Create default material
	 */
	v1db.default_material = SetupDefaultMaterial();

	/*
	 * Set up 'enabled' lists
	 */
	v1db.enabled_lights.max = BR_MAX_LIGHTS;
	v1db.enabled_lights.name = "light";
	v1db.enabled_lights.type = BR_ACTOR_LIGHT;

	v1db.enabled_clip_planes.max = BR_MAX_CLIP_PLANES;
	v1db.enabled_clip_planes.name = "clip plane";
	v1db.enabled_clip_planes.type = BR_ACTOR_CLIP_PLANE;

	v1db.enabled_horizon_planes.max = BR_MAX_HORIZON_PLANES;
	v1db.enabled_horizon_planes.name = "horizon plane";
	v1db.enabled_horizon_planes.type = BR_ACTOR_HORIZON_PLANE;

	return BRE_OK;
}

br_error BR_PUBLIC_ENTRY BrV1dbEnd(void)
{
	br_device *dev;

	if(!v1db.active)
		return BRE_NOT_ACTIVE;

	v1db.active = BR_FALSE;

	/*
	 * Free all resources ...
	 */
	BrResFree(v1db.res);

	/*
	 * Zero out whole static structure
	 */
	BrMemSet(&v1db, 0, sizeof(v1db));

	return BRE_OK;
}

static br_uint_32 BR_CALLBACK updateTable(br_pixelmap *item, void *arg)
{

    ASSERT_MESSAGE("BR_CALLBACK updateTable NULL pointer", item !=NULL);
	BrTableUpdate(item, BR_TABU_ALL);

	return 0;
}

static br_uint_32 BR_CALLBACK updateMap(br_pixelmap *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK updateMap NULL pointer", item !=NULL);
	BrMapUpdate(item, BR_MAPU_ALL);

	return 0;
}

static br_uint_32 BR_CALLBACK updateMaterial(br_material *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK updateMaterial NULL pointer", item !=NULL);
	BrMaterialUpdate(item, BR_MATU_ALL);

	return 0;
}

static br_uint_32 BR_CALLBACK updateModel(br_model *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK updateModel NULL pointer", item !=NULL);

	if (item->prepared != NULL)
		BrModelUpdate(item, _BR_MODU_RESERVED);
	else
		BrModelUpdate(item, BR_MODU_ALL);

	return 0;
}

static br_uint_32 BR_CALLBACK clearTable(br_pixelmap *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK clearTable NULL pointer", item !=NULL);
	BrBufferClear(item);

	return 0;
}

static br_uint_32 BR_CALLBACK clearMap(br_pixelmap *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK clearMap NULL pointer", item !=NULL);
	
	BrBufferClear(item);

	return 0;
}

static br_uint_32 BR_CALLBACK clearMaterial(br_material *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK clearMaterial NULL pointer", item !=NULL);
	
	BrMaterialClear(item);

	return 0;
}

static br_uint_32 BR_CALLBACK clearModel(br_model *item, void *arg)
{
    ASSERT_MESSAGE("BR_CALLBACK clearModel NULL pointer", item !=NULL);
	
	BrModelClear(item);

	return 0;
}

br_error BR_PUBLIC_ENTRY BrV1dbRendererBegin(br_device_pixelmap *destination, br_renderer *renderer)
{
	br_renderer_facility *renderer_facility = NULL;
	br_error r;
	br_token_value tv[] = {
		{0,	0},
		{0},
	};

    /*
	 * Fire up renderer
	 */
	if(renderer == NULL) {
        /*
         * Find a renderer if none is specified
         */
		r = BrRendererFacilityFind(&renderer_facility, destination, BR_SCALAR_TOKEN);
		if(r != BRE_OK)
			return r;

        if(destination != NULL) {
            /*
             * If a destination pixelmap is givem, pass it on to the new method
             * so that it can be more specfic
             */
	    	tv[0].t = BRT_DESTINATION_O;
		    tv[0].v.o = (br_object *)destination;
    	}

	    r = RendererFacilityRendererNew(renderer_facility, &renderer, tv);

		if(r != BRE_OK)
			return r;

	} else {

		/*
		 * Find the renderer facility for the provided renderer
		 */
		r = ObjectQuery(renderer, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_O);
		if (r != BRE_OK)
			return r;
	}

	/*
	 * Remember renderer to use
	 */
	v1db.renderer = renderer;

	/*
	 * Look up geometry format objects
	 */
	r = BrGeometryFormatFind((br_geometry **)&v1db.format_model,
		renderer,renderer_facility,
		BR_SCALAR_TOKEN, BRT_GEOMETRY_V1_MODEL);

	if(r != BRE_OK)
		return r;

	r = BrGeometryFormatFind((br_geometry **)&v1db.format_buckets,
		renderer,renderer_facility,
		BR_SCALAR_TOKEN, BRT_GEOMETRY_V1_BUCKETS);

	if(r != BRE_OK)
		v1db.format_buckets = NULL;

    r = BrGeometryFormatFind((br_geometry **)&v1db.format_lighting,
        renderer,renderer_facility,
        BR_SCALAR_TOKEN, BRT_GEOMETRY_LIGHTING);

    if(r != BRE_OK)
        return r;

	/*
	 * Update default model and material
	 */
	BrModelUpdate(v1db.default_model,BR_MODU_ALL);

	// STEVE : For some bizzare reason that I don't have time to look
	// at now, we need to do two material updates on the default material
	// to get it to work. Weird.
   
	BrMaterialUpdate(v1db.default_material, BR_MATU_ALL);
	BrMaterialUpdate(v1db.default_material, BR_MATU_ALL); 

	/*
	 * Set the default order table
	 */
	v1db.default_order_table = &_BrDefaultOrderTable;

	/*
	 * Disable the primary order table
	 */
	v1db.primary_order_table = NULL;

	/*
	 * Update any registered maps, tables and materials
	 */
	BrTableEnum(NULL, updateTable, NULL);
	BrMapEnum(NULL, updateMap, NULL);
	BrMaterialEnum(NULL, updateMaterial, NULL);
	BrModelEnum(NULL, updateModel, NULL);

	return BRE_OK;
}


br_renderer * BR_PUBLIC_ENTRY BrV1dbRendererQuery(void)
{
	return v1db.renderer;
}

br_error BR_PUBLIC_ENTRY BrV1dbRendererEnd(void)
{
	if(v1db.renderer == NULL)
		return BRE_FAIL;

	/*
	 * Make sure none of the registered items have
	 * references to stored buffers
	 */
	BrTableEnum(NULL, clearTable, NULL);
	BrMapEnum(NULL, clearMap, NULL);
	BrMaterialEnum(NULL, clearMaterial, NULL);
	BrModelEnum(NULL, clearModel, NULL);
	
	/*
	 * Clear down the defaults
	 */
	BrMaterialClear(v1db.default_material);
	BrModelClear(v1db.default_model);

	v1db.default_order_table = NULL;
	v1db.default_render_data = NULL;
	v1db.primary_order_table = NULL;

	v1db.format_model = NULL;

    /*
     * Release the renderer
     */
	ObjectFree(v1db.renderer);
	v1db.renderer = NULL;
	
	return BRE_OK;
}

/*
 * General purpose renderer handling
 */
void BR_PUBLIC_ENTRY BrRendererBegin(br_pixelmap *destination,
	struct br_renderer_facility *renderer_facility, struct br_primitive_library *primitive_library,
	void *primitive_heap, br_uint_32 primitive_heap_size)
{
	br_renderer *renderer = NULL;
	br_token_value tv[3];
	br_uint_32 tokens = 0;

	if (v1db.zb_active || v1db.zs_active)
		BR_ERROR0("Renderer already started\n");

	/*
	 * Start up a renderer if either the renderer facility or the primitive
	 * library was specified
	 */
	if (renderer_facility != NULL || primitive_library != NULL) {

		/*
		 * Find a renderer if none is specified
		 */
		if (renderer_facility == NULL)
                        if (BrRendererFacilityFind(&renderer_facility,(br_device_pixelmap*) destination, BR_SCALAR_TOKEN) != BRE_OK)
				BR_ERROR0("Could not find renderer facility\n");

		if (destination != NULL) {

			/*
			 * If a destination pixelmap is givem, pass it on to the new method
			 * so that it can be more specfic
			 */
			tv[tokens].t = BRT_DESTINATION_O;
			tv[tokens].v.o = (br_object *)destination;
			tokens++;
		}

		if (primitive_library != NULL) {

			/*
			 * If a primitive library is given, pass it on to the new method
			 */
			tv[tokens].t = BRT_PRIMITIVE_LIBRARY_O;
			tv[tokens].v.o = (br_object *)primitive_library;
			tokens++;
		}

		tv[tokens].t = BR_NULL_TOKEN;

		if (RendererFacilityRendererNew(renderer_facility, &renderer, tv) != BRE_OK)
			BR_ERROR0("Failed to load renderer\n");
	}

	if (BrV1dbRendererBegin((br_device_pixelmap *)destination, renderer) != BRE_OK)
		BR_ERROR0("Failed to load renderer\n");

	v1db.zb_active = v1db.zs_active = BR_TRUE;

	/*
	 * Save primitive root and size
	 */
	v1db.heap.base = primitive_heap;
	v1db.heap.size = primitive_heap_size;
}

void BR_PUBLIC_ENTRY BrRendererEnd(void)
{
	v1db.zb_active = v1db.zs_active = BR_FALSE;

	if (v1db.renderer)
		BrV1dbRendererEnd();
}

/*
 * V1.1 compatibility entry points
 */
void BR_PUBLIC_ENTRY BrZbBegin(br_uint_8 colour_type, br_uint_8 depth_type)
{
	if(!v1db.zs_active && !v1db.zb_active) 
		if(BrV1dbRendererBegin((br_device_pixelmap *)BrDevLastBeginQuery(),NULL) != BRE_OK)
			BR_ERROR0("Failed to load renderer\n");

	v1db.zb_active = BR_TRUE;
}

void BR_PUBLIC_ENTRY BrZsBegin(br_uint_8 colour_type, void *primitive, br_uint_32 size)
{
        UASSERT_MESSAGE("BrZsBegin NULL pointer to an allocated block of memory", primitive != NULL);

	if(!v1db.zs_active && !v1db.zb_active) 
		if(BrV1dbRendererBegin((br_device_pixelmap *)BrDevLastBeginQuery(),NULL) != BRE_OK)
			BR_ERROR0("Failed to load renderer\n");

	v1db.zs_active = BR_TRUE;

	/*
	 * Save primitive root and size
	 */
	v1db.heap.base = primitive;
	v1db.heap.size = size;
}

void BR_PUBLIC_ENTRY BrZbEnd(void)
{
	v1db.zb_active = BR_FALSE;

	/*
	 * If both renderers are inactive then close down renderer
	 */
	if(!v1db.zs_active && v1db.renderer)
		BrV1dbRendererEnd();
}

void BR_PUBLIC_ENTRY BrZsEnd(void)
{
	v1db.zs_active = BR_FALSE;

	if(!v1db.zb_active && v1db.renderer)
		BrV1dbRendererEnd();
}


/*
 * Wrappers that are used to replace BrBegin() and BrEnd()
 */
#undef BrBegin
#undef BrEnd

br_error BR_PUBLIC_ENTRY BrV1dbBeginWrapper(void)
{
	br_error error;

	error = BrBegin();
	if (error != BRE_OK)
		return error;

	error = BrV1dbBegin();
	if (error != BRE_OK)
		return error;

	return BRE_OK;
}

br_error BR_PUBLIC_ENTRY BrV1dbEndWrapper(void)
{
	br_error result = BRE_OK;
	br_error error;

	error = BrV1dbEnd();
	if (error != BRE_OK)
		result = error;

	error = BrEnd();
	if (error != BRE_OK)
		result = error;

	return result;
}

