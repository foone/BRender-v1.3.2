/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: regsupt.c 1.3 1998/01/15 12:58:24 jon Exp $
 * $Locker: $
 *
 * Public registry functions
 */
#include <string.h>

#include "v1db.h"
#include "brassert.h"
#include "datafile.h"

BR_RCS_ID("$Id: regsupt.c 1.3 1998/01/15 12:58:24 jon Exp $")

/*
 * Declare static registry lists for evaluation build
 */
#if EVAL
extern br_registry_entry modelRegistryList[];
extern br_registry_entry materialRegistryList[];
extern br_registry_entry mapRegistryList[];
extern br_registry_entry tableRegistryList[];
#endif

/**
 ** Specific versions of registry calls for each class
 **/

/*
 * Models
 */
br_model * BR_PUBLIC_ENTRY BrModelAdd(br_model *model)
{
	/*
	 * Check that model was allocated
	 */
	UASSERT(BrResCheck(model, 1));
	UASSERT(BrResClass(model) == BR_MEMORY_MODEL);

#if EVAL
	if(v1db.reg_models.count >= MODEL_LIMIT) {
		BR_FAILURE("Model registry limit exceeded");
		return NULL;
	}
	BrRegistryAddStatic(&v1db.reg_models, modelRegistryList, model);
#else
	BrRegistryAdd(&v1db.reg_models, model);
#endif

	/*
	 * Preprocess model and generate stored info
	 */
	BrModelUpdate(model, BR_MODU_ALL);

	return model;
}

br_model * BR_PUBLIC_ENTRY BrModelRemove(br_model *model)
{
    /*
	 * Remove stored info.
	 */
    UASSERT_MESSAGE("Invalid BrModelRemove pointer", model != NULL);
	BrModelClear(model);

#if EVAL
	return BrRegistryRemoveStatic(&v1db.reg_models, model);
#else
	return BrRegistryRemove(&v1db.reg_models, model);
#endif
}

br_model * BR_PUBLIC_ENTRY BrModelFind(char *pattern)
{ return BrRegistryFind(&v1db.reg_models,pattern); }

br_model_find_cbfn * BR_PUBLIC_ENTRY BrModelFindHook(br_model_find_cbfn *hook)
{
    br_model_find_cbfn * old = (br_model_find_cbfn *) v1db.reg_models.find_failed_hook;

    UASSERT_MESSAGE("Invalid BrModelFindHook pointer", hook != NULL);

	v1db.reg_models.find_failed_hook = (br_find_failed_cbfn *)hook;

	return old;
}

br_uint_32 BR_PUBLIC_ENTRY BrModelAddMany(br_model **items, int n)
{
	int i,r=0;
    UASSERT_MESSAGE("Invalid BrModelAddMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrModelAddMany pointer", *items != NULL);
		if(BrModelAdd(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrModelRemoveMany(br_model **items, int n)
{
	int i,r=0;
    UASSERT_MESSAGE("Invalid BrModelRemoveMany pointer to an array of pointers", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrModelRemoveMany pointer", *items != NULL);
		if(BrModelRemove(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrModelFindMany(char *pattern, br_model **items, int max)
{ return BrRegistryFindMany(&v1db.reg_models, pattern, (void **)items, max); }

br_uint_32 BR_PUBLIC_ENTRY BrModelCount(char *pattern)
{ return BrRegistryCount(&v1db.reg_models,pattern); }

br_uint_32 BR_PUBLIC_ENTRY BrModelEnum(char *pattern,
		br_model_enum_cbfn *callback, void *arg)
{ return BrRegistryEnum(&v1db.reg_models,pattern,(br_enum_cbfn *)callback,arg); }


/*
 * Materials
 */
br_material * BR_PUBLIC_ENTRY BrMaterialAdd(br_material *material)
{
	/*
	 * Check that material was allocated
	 */
	UASSERT(BrResCheck(material, 1));
	UASSERT(BrResClass(material) == BR_MEMORY_MATERIAL);
    UASSERT(material != NULL);
#if EVAL
	if(v1db.reg_materials.count >= MATERIAL_LIMIT) {
		BR_FAILURE("Material registry limit exceeded");
		return NULL;
	}
	BrRegistryAddStatic(&v1db.reg_materials, materialRegistryList, material);
#else
	BrRegistryAdd(&v1db.reg_materials, material);
#endif

	BrMaterialUpdate(material, BR_MATU_ALL);

	return material;
}

br_material * BR_PUBLIC_ENTRY BrMaterialRemove(br_material *material)
{
    UASSERT(material != NULL);
	BrMaterialClear(material);

#if EVAL
	return BrRegistryRemoveStatic(&v1db.reg_materials, material);
#else
	return BrRegistryRemove(&v1db.reg_materials, material);
#endif
}

br_material * BR_PUBLIC_ENTRY BrMaterialFind(char *pattern)
{
	return BrRegistryFind(&v1db.reg_materials,pattern);
}

br_material_find_cbfn * BR_PUBLIC_ENTRY BrMaterialFindHook(br_material_find_cbfn *hook)
{
	br_material_find_cbfn * old = (br_material_find_cbfn *) v1db.reg_materials.find_failed_hook;

	v1db.reg_materials.find_failed_hook = (br_find_failed_cbfn *)hook;

	return old;
}

br_uint_32 BR_PUBLIC_ENTRY BrMaterialAddMany(br_material **items, int n)
{
	int i,r=0;
    UASSERT_MESSAGE("Invalid BrMaterialAddMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrMaterialAddMany pointer", *items != NULL);
		if(BrMaterialAdd(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrMaterialRemoveMany(br_material **items, int n)
{
	int i,r=0;

    UASSERT_MESSAGE("Invalid BrMaterialRemoveMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrMaterialRemoveMany pointer", *items != NULL);
        if(BrMaterialRemove(*items++) != NULL)
			r++;
    }

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrMaterialFindMany(char *pattern, br_material **items, int max)
{
	return BrRegistryFindMany(&v1db.reg_materials, pattern, (void **)items, max);
}

br_uint_32 BR_PUBLIC_ENTRY BrMaterialCount(char *pattern)
{
	return BrRegistryCount(&v1db.reg_materials,pattern);
}

br_uint_32 BR_PUBLIC_ENTRY BrMaterialEnum(char *pattern,
		br_material_enum_cbfn *callback, void *arg)
{
	return BrRegistryEnum(&v1db.reg_materials,pattern,(br_enum_cbfn *)callback,arg);
}

/*
 * Textures
 */
br_pixelmap * BR_PUBLIC_ENTRY BrMapAdd(br_pixelmap *pixelmap)
{
    UASSERT_MESSAGE("Invalid BrMapAdd pointer", pixelmap != NULL);
#if EVAL
	if(v1db.reg_textures.count >= MAP_LIMIT) {
		BR_FAILURE("Map registry limit exceeded");
		return NULL;
	}
	BrRegistryAddStatic(&v1db.reg_textures, mapRegistryList, pixelmap);
#else
	BrRegistryAdd(&v1db.reg_textures, pixelmap);
#endif

	BrMapUpdate(pixelmap, BR_MAPU_ALL);

	return pixelmap;
}

br_pixelmap * BR_PUBLIC_ENTRY BrMapRemove(br_pixelmap *pixelmap)
{
    UASSERT_MESSAGE("Invalid BrMapRemove pointer", pixelmap != NULL);
	BrBufferClear(pixelmap);

#if EVAL
	return BrRegistryRemoveStatic(&v1db.reg_textures, pixelmap);
#else
	return BrRegistryRemove(&v1db.reg_textures, pixelmap);
#endif
}

br_pixelmap * BR_PUBLIC_ENTRY BrMapFind(char *pattern)
{
	return BrRegistryFind(&v1db.reg_textures,pattern);
}

br_map_find_cbfn * BR_PUBLIC_ENTRY BrMapFindHook(br_map_find_cbfn *hook)
{
	br_map_find_cbfn * old = (br_map_find_cbfn *) v1db.reg_textures.find_failed_hook;

    UASSERT_MESSAGE("Invalid BrMapFindHook pointer", hook != NULL);

	v1db.reg_textures.find_failed_hook = (br_find_failed_cbfn *)hook;

	return old;
}

br_uint_32 BR_PUBLIC_ENTRY BrMapAddMany(br_pixelmap **items, int n)
{
	int i,r=0;

    UASSERT_MESSAGE("Invalid BrMapAddMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrMapAddMany pointer", *items != NULL);
		if(BrMapAdd(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrMapRemoveMany(br_pixelmap **items, int n)
{
	int i,r=0;

    UASSERT_MESSAGE("Invalid BrMapRemoveMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrMapRemoveMany pointer", *items != NULL);
		if(BrMapRemove(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrMapFindMany(char *pattern, br_pixelmap **items, int max)
{
	return BrRegistryFindMany(&v1db.reg_textures, pattern, (void **)items, max);
}

br_uint_32 BR_PUBLIC_ENTRY BrMapCount(char *pattern)
{
	return BrRegistryCount(&v1db.reg_textures,pattern);
}

br_uint_32 BR_PUBLIC_ENTRY BrMapEnum(char *pattern,
		br_map_enum_cbfn *callback, void *arg)
{
	return BrRegistryEnum(&v1db.reg_textures,pattern,(br_enum_cbfn *)callback,arg);
}

/*
 * Tables
 */
br_pixelmap * BR_PUBLIC_ENTRY BrTableAdd(br_pixelmap *pixelmap)
{

	UASSERT_MESSAGE("Invalid BrTableAdd pointer", pixelmap != NULL);

#if EVAL
	if(v1db.reg_tables.count >= TABLE_LIMIT) {
		BR_FAILURE("Table registry limit exceeded");
		return NULL;
	}
	BrRegistryAddStatic(&v1db.reg_tables, tableRegistryList, pixelmap);
#else
	BrRegistryAdd(&v1db.reg_tables, pixelmap);
#endif

	BrTableUpdate(pixelmap, BR_TABU_ALL);

	return pixelmap;
}

br_pixelmap * BR_PUBLIC_ENTRY BrTableRemove(br_pixelmap *pixelmap)
{
    UASSERT_MESSAGE("Invalid BrTableRemove pointer", pixelmap != NULL);

	BrBufferClear(pixelmap);

#if EVAL
	return BrRegistryRemoveStatic(&v1db.reg_tables, pixelmap);
#else
	return BrRegistryRemove(&v1db.reg_tables, pixelmap);
#endif
}

br_pixelmap * BR_PUBLIC_ENTRY BrTableFind(char *pattern)
{
	return BrRegistryFind(&v1db.reg_tables,pattern);
}

br_table_find_cbfn * BR_PUBLIC_ENTRY BrTableFindHook(br_table_find_cbfn *hook)
{
	br_table_find_cbfn * old = (br_table_find_cbfn *) v1db.reg_tables.find_failed_hook;

    UASSERT_MESSAGE("Invalid BrTableFindHook pointer", hook != NULL);

	v1db.reg_tables.find_failed_hook = (br_find_failed_cbfn *)hook;

	return old;
}

br_uint_32 BR_PUBLIC_ENTRY BrTableAddMany(br_pixelmap **items, int n)
{
	int i,r=0;
    UASSERT_MESSAGE("Invalid BrTableAddMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrTableAddMany pointer", *items != NULL);
		if(BrTableAdd(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrTableRemoveMany(br_pixelmap **items, int n)
{
	int i,r=0;
    UASSERT_MESSAGE("Invalid BrTableRemoveMany pointer", items != NULL);

	for(i=0; i < n; i++) {
        UASSERT_MESSAGE("Invalid BrTableRemoveMany pointer", *items != NULL);
		if(BrTableRemove(*items++) != NULL)
			r++;
	}

	return r;
}

br_uint_32 BR_PUBLIC_ENTRY BrTableFindMany(char *pattern, br_pixelmap **items, int max)
{
	return BrRegistryFindMany(&v1db.reg_tables, pattern, (void **)items, max);
}

br_uint_32 BR_PUBLIC_ENTRY BrTableCount(char *pattern)
{
	return BrRegistryCount(&v1db.reg_tables,pattern);
}

br_uint_32 BR_PUBLIC_ENTRY BrTableEnum(char *pattern,
		br_table_enum_cbfn *callback, void *arg)
{

	return BrRegistryEnum(&v1db.reg_tables,pattern,(br_enum_cbfn *)callback,arg);
}

