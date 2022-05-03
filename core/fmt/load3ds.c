/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: load3ds.c 1.3 1998/07/28 14:33:23 jon Exp $
 * $Locker: $
 *
 * Loader for 3D-Studio .3DS format
 */
#include <stdarg.h>

#include "brender.h"
#include "load3ds.h"
#include "brassert.h"

BR_RCS_ID("$Id: load3ds.c 1.3 1998/07/28 14:33:23 jon Exp $")

/*
 * Default 3DS conversion options
 */
STATIC br_3ds_options _BrDefault3DSOptions = {
	/*
	 * Flags
	 */
	BR_3DS_CORRECT_AXES |
	BR_3DS_APPLY_MESH_MATRIX |
	BR_3DS_APPLY_PIVOT |
	BR_3DS_SET_HITHER |
	BR_3DS_SET_YON,

	/*
	 * Hither and yon
	 */
	BR_SCALAR(1.0),
	BR_SCALAR(10.0),

	/*
	 * Scale
	 */
	BR_SCALAR(1.0),

	/*
	 * No message reporting
	 */
	NULL
};

/*
 * Report a message
 */
STATIC void ReportMessage(br_fmt_report_cbfn *report, char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	BrVSprintf(BrScratchString(), fmt, args);
	va_end(args);

	report(BrScratchString());
}

/*
 * Read a character from a stream
 */
STATIC Bool_t ReadChar(void *stream, char *result)
{
	int c;

	UASSERT((stream != NULL) && (result != NULL));

	if((c = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (char)c;

	return TRUE;
}

/*
 * Read a byte from a stream
 * and cast to an 8 bit unsigned integer
 */
STATIC Bool_t ReadUInt8(void *stream, br_uint_8 *result)
{
	int b;

	UASSERT((stream != NULL) && (result != NULL));

	if((b = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_uint_8)b;

	return TRUE;
}

#if 0
/*
 * Read a byte from a stream
 * and cast to an 8 bit signed integer
 */
STATIC Bool_t ReadInt8(void *stream, br_int_8 *result)
{
	int b;

	UASSERT((stream != NULL) && (result != NULL));

	if((b = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_int_8)b;

	return TRUE;
}
#endif

/*
 * Read two bytes from a stream
 * and convert to a 16 bit unsigned integer
 */
STATIC Bool_t ReadUInt16(void *stream, br_uint_16 *result)
{
	int lsb,msb;

	UASSERT((stream != NULL) && (result != NULL));

	if((lsb = BrFileGetChar(stream)) == BR_EOF) return FALSE;
	if((msb = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_uint_16)((msb << 8) | lsb);

	return TRUE;
}

/*
 * Read two bytes from a stream
 * and convert to a 16 bit signed integer
 */
STATIC Bool_t ReadInt16(void *stream, br_int_16 *result)
{
	int lsb,msb;

	UASSERT((stream != NULL) && (result != NULL));

	if((lsb = BrFileGetChar(stream)) == BR_EOF) return FALSE;
	if((msb = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_int_16)((msb << 8) | lsb);

	return TRUE;
}

/*
 * Read four bytes from a stream
 * and convert to a 32 bit unsigned integer
 */
STATIC Bool_t ReadUInt32(void *stream, br_uint_32 *result)
{
	int i,b[4];

	UASSERT((stream != NULL) && (result != NULL));

	for(i=0; i<4; i++) if((b[i] = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_uint_32)((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);

	return TRUE;
}

/*
 * Read four bytes from a stream
 * and convert to a 32 bit signed integer
 */
STATIC Bool_t ReadInt32(void *stream, br_int_32 *result)
{
	int i,b[4];

	UASSERT((stream != NULL) && (result != NULL));

	for(i=0; i<4; i++) if((b[i] = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	*result = (br_int_32)((b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0]);

	return TRUE;
}

/*
 * Read four bytes from a stream
 * and convert to a 32 bit IEEE float
 */
STATIC Bool_t ReadFloat(void *stream, Float_t *result)
{
	br_uint_32 f;

	UASSERT((stream != NULL) && (result != NULL));

	if(!ReadUInt32(stream, &f)) return FALSE;

	*result = (Float_t)(*((float *)&f));

	return TRUE;
}

/*
 * Block copy
 */
STATIC void BlockCopy(void *dest, void *src, Int_t n_bytes)
{
	Int_t i;
	char *src_bp, *dest_bp;

	UASSERT((src != NULL) && (dest != NULL));

	src_bp = (char *)src;
	dest_bp = (char *)dest;

	for(i=0; i<n_bytes; i++) *dest_bp++ = *src_bp++;
}

/*
 * Skip bytes in a stream
 */
STATIC Bool_t SkipBytes(void *stream, int bytes)
{
	int i,c;

	UASSERT(stream != NULL);

	for(i=0; i<bytes; i++) if((c = BrFileGetChar(stream)) == BR_EOF) return FALSE;

	return TRUE;
}

/*
 * Skip all remaining bytes in the chunk
 * currently at the top of the stack
 */
STATIC Bool_t SkipRest(void *stream, Stack_t *top)
{
    UASSERT((stream != NULL) && (top != NULL));

	if(SkipBytes(stream, top->length - top->done)) {
		top->done = top->length;
		return TRUE;
	}	

	return FALSE;
}

/*
 * Read a chunk header from the input stream
 * into the top of the stack
 */
STATIC Bool_t ReadHeader(void *stream, Stack_t *top)
{
    br_uint_16 id_tag;
    br_uint_32 length;

    UASSERT((stream != NULL) && (top != NULL));

    if(ReadUInt16(stream, &id_tag) && ReadUInt32(stream, &length)) {

        top->id_tag = (ChunkIdTag_t)(id_tag);
        top->length = length;
        top->done   = sizeof(br_uint_16) + sizeof(br_uint_32);
        top->flags  = 0;

        return TRUE;
    }

    return FALSE;
}

/*
 * Read a NULL-terminated string of length
 * less than 'max_n_bytes' into 'buffer' from the input stream
 */
STATIC Bool_t ReadString(void *stream, Stack_t *top, Int_t max_n_bytes, char *buffer)
{
    Int_t i;

    UASSERT((stream != NULL) && (top != NULL));

    for (i=0; (i < max_n_bytes) && (top->done < top->length); i++) {
        if (ReadChar(stream,buffer+i)) {
            top->done += sizeof(char);
            if (buffer[i] == '\0') {
                return TRUE;
            }
        } else {
            return FALSE;
        }
    }

    return FALSE;
}

/*
 * Read an RGB triple of floating point values
 * into the top of the stack
 */
STATIC Bool_t ReadColorF(void *stream, Stack_t *top)
{
    Color_t *color;

    UASSERT((stream != NULL) && (top != NULL));

    color = &(top->data.color);

    if(ReadFloat(stream, &(color->red)) &&
	   ReadFloat(stream, &(color->green)) &&
	   ReadFloat(stream, &(color->blue)) &&
	   ((color->red   >= 0.0) && (color->red   <= 1.0)) &&
	   ((color->green >= 0.0) && (color->green <= 1.0)) &&
	   ((color->blue  >= 0.0) && (color->blue  <= 1.0))) {

		top->done += 3 * sizeof(Float_t);

        return TRUE;

    }

    return FALSE;
}

/*
 * Read an RGB triple of unsigned bytes, convert them
 * to floating point values, and copy into the top of the stack
 */
STATIC Bool_t ReadColor24(void *stream, Stack_t *top)
{
	br_uint_8 red,green,blue;
    Color_t *color;

    UASSERT((stream != NULL) && (top != NULL));

    color = &(top->data.color);

	if(ReadUInt8(stream, &red) && ReadUInt8(stream, &green) && ReadUInt8(stream, &blue)) {

		color->red   = ((Float_t)red)   / 255.0F;
		color->green = ((Float_t)green) / 255.0F;
		color->blue  = ((Float_t)blue)  / 255.0F;

		top->done += 3 * sizeof(br_uint_8);

        return TRUE;

    }

    return FALSE;
}

/*
 * Convert a colour from the 'Color_t' type to BRender's 'br_colour' type
 */
STATIC br_colour ConvertColor(Color_t *color)
{
    return BR_COLOUR_RGB((float)(255.0 * color->red),
                         (float)(255.0 * color->green),
                         (float)(255.0 * color->blue));
}

/*
 * Take a 'Color_t' and return it's grey-scale intensity
 */
STATIC br_ufraction GreyscaleOfColor(Color_t *color)
{
    return BrScalarToUFraction(
               BrFloatToScalar(
                   (float)((color->red   * RED_GREYSCALE_FACTOR)
                          +(color->green * GREEN_GREYSCALE_FACTOR)
                          +(color->blue  * BLUE_GREYSCALE_FACTOR))));
}

/*
 * Read an XYZ triple from the input stream, convert to a 'br_vector3',
 * and reflect in the planes y=z and z=0 if required
 */
STATIC Bool_t ReadPoint(void *stream, Stack_t *top, br_vector3 *vector, br_3ds_options *options)
{
    Int_t   i;
    Float_t coords[3];

    UASSERT((stream != NULL) && (top != NULL) && (vector != NULL));

    for (i=0; i<3; i++) {
        if (ReadFloat(stream,&coords[i])) {
            top->done += sizeof(Float_t);
        } else {
            return FALSE;
        }
    }

	if(options->flags & BR_3DS_CORRECT_AXES) {
	    /*
		 * Convert to right-handed coordinate space
		 */
	    vector->v[0] = BrFloatToScalar((float)(coords[0]));
	    vector->v[1] = BrFloatToScalar((float)(coords[2]));
    	vector->v[2] = BrFloatToScalar((float)(-coords[1]));
	} else {
	    vector->v[0] = BrFloatToScalar((float)(coords[0]));
    	vector->v[1] = BrFloatToScalar((float)(coords[1]));
	    vector->v[2] = BrFloatToScalar((float)(coords[2]));
	}

    return TRUE;
}

/*
 * Initialise a pixmap reference so that it refers
 * to neither a texture map nor a reflection map
 */
STATIC void InitialisePixmapRef(PixmapRef_t *pixmap_ref)
{
    UASSERT(pixmap_ref != NULL);

    pixmap_ref->mat_mapname       = NULL;
    pixmap_ref->is_reflection_map = FALSE;
}

/*
 * De-allocate any memory used by the name of a referenced pixmap
 */
STATIC void DismantlePixmapRef(PixmapRef_t *pixmap_ref)
{
    UASSERT(pixmap_ref != NULL);

    pixmap_ref->is_reflection_map = FALSE;

    if (pixmap_ref->mat_mapname != NULL) {
        BrMemFree(pixmap_ref->mat_mapname);
        pixmap_ref->mat_mapname = NULL;
    }
}

/*
 * If there is no existing referenced pixmap, or one with a
 * greater strength has been found, update the reference to
 * refer to the newly found pixmap
 */
STATIC void UpdatePixmapRef(PixmapRef_t *exist_ref, PixmapRef_t *new_ref)
{
    UASSERT((exist_ref != NULL) && (new_ref != NULL));

    if (exist_ref->mat_mapname == NULL
            || new_ref->strength > exist_ref->strength) {
        DismantlePixmapRef(exist_ref);
        BlockCopy(exist_ref,new_ref,sizeof(PixmapRef_t));
    } else {
        DismantlePixmapRef(new_ref);
    }
}

/*
 * This function allocates a new br_pixelmap, given its name. These
 * are used in textured materials. One holds the actual texture and one
 * the shade table.
 *
 * This function doesn't allocate any pixels. It just makes a dummy
 * br_pixelmap with the right identifier string.
 *
 * Each map that gets created this way gets threaded into a list, a
 * pointer to which is passed as the "maps" parameter.
 */
STATIC br_pixelmap * GetPixelmap(char *mat_mapname, PixmapList_t **pixmap_list)
{
    char *identifier, *suffix;
    br_pixelmap *pixelmap;
    PixmapList_t *pixmap_link;

    UASSERT((mat_mapname != NULL) && (pixmap_list != NULL));

    /*
	 * Make a copy of the name, without it's dot extension
	 */
    suffix = BrStrRChr(mat_mapname,'.');
    if (suffix != NULL) {
        *suffix = '\0';
    }

    identifier = (char*)BrResAllocate(NULL, (BrStrLen(mat_mapname)+1) * sizeof(char), BR_MEMORY_STRING);
    if (identifier == NULL) {
        if (suffix != NULL) {
            *suffix = '.';
        }
        return NULL;
    }

    BrStrCpy(identifier,mat_mapname);
    if (suffix != NULL) {
        *suffix = '.';
    }

    /*
	 * If we already have a map by the same name, share it
	 */
    for (pixmap_link = *pixmap_list; pixmap_link != NULL; pixmap_link = pixmap_link->next) {
        if (BrStrCmp (identifier, pixmap_link->pixelmap->identifier) == 0) {
            BrResFree(identifier);
            return pixmap_link->pixelmap;
        }
    }

    /*
	 * Otherwise, make one
	 */
    pixelmap = BrPixelmapAllocate(BR_PMT_INDEX_8,1,1,NULL,0);
    if (pixelmap == NULL) {
         BrResFree(identifier);
         return NULL;
    }

    pixmap_link = (PixmapList_t*) BrMemAllocate(sizeof(PixmapList_t), BR_MEMORY_SCRATCH);
    if (pixmap_link == NULL) {
        BrPixelmapFree(pixelmap);
        BrResFree(identifier);
        return NULL;
    }

	BrResAdd(pixelmap, identifier);
    pixelmap->identifier = identifier;

    /*
	 * Thread the map into the list at the head
	 */
    pixmap_link->pixelmap = pixelmap;
    pixmap_link->next = *pixmap_list;
    *pixmap_list = pixmap_link;

    return pixelmap;
}

/*
 * De-allocate pixmap list nodes
 */
STATIC void DeallocatePixmapListNodes(PixmapList_t *pixmap_list)
{
	PixmapList_t *dud;

	while(pixmap_list != NULL) {
		dud = pixmap_list;
		pixmap_list = pixmap_list->next;
		BrMemFree(dud);
	}
}

/*
 * De-allocate entire pixmap list
 */
STATIC void DeallocatePixmapList(PixmapList_t *pixmap_list)
{
	PixmapList_t *dud;

	while(pixmap_list != NULL) {
		dud = pixmap_list;
		pixmap_list = pixmap_list->next;
		BrPixelmapFree(dud->pixelmap);
		BrMemFree(dud);
	}
}

/*
 * Initialise a material entry so that all
 * of its optional properties are turned off
 */
STATIC void InitialiseMatEntry(MatEntry_t *mat_entry)
{
    UASSERT(mat_entry != NULL);

    mat_entry->mat_name     = NULL;
    mat_entry->mat_two_side = FALSE;
    mat_entry->mat_decal    = FALSE;
	mat_entry->mat_ambient.red	 = 0.0F;
	mat_entry->mat_ambient.green = 0.0F;
	mat_entry->mat_ambient.blue  = 0.0F;
	mat_entry->mat_diffuse.red	 = 0.75F;
	mat_entry->mat_diffuse.green = 0.75F;
	mat_entry->mat_diffuse.blue  = 0.75F;
	mat_entry->mat_specular.red	 = 0.0F;
	mat_entry->mat_specular.green = 0.0F;
	mat_entry->mat_specular.blue  = 0.0F;
	mat_entry->mat_transparency  = 0.0F;
	mat_entry->mat_shininess  = 0.0F;
	mat_entry->mat_shading  = FLAT;

    InitialisePixmapRef(&(mat_entry->pixmap_ref));
}

/*
 * De-allocate any memory used by the names of
 * a material entry and any pixmap it may refer to
 */
STATIC void DismantleMatEntry(MatEntry_t *mat_entry)
{
    UASSERT(mat_entry != NULL);

    if (mat_entry->mat_name != NULL) {
        BrMemFree(mat_entry->mat_name);
        mat_entry->mat_name = NULL;
    }

    DismantlePixmapRef(&(mat_entry->pixmap_ref));
}

/*
 * Convert a 3DS material to an equivalent BRender material.
 *
 * This function builds a BRender material from the information
 * contained in a 3DS material, and adds it to the registry
 *
 * Each material that is converted is threaded into a list.
 */
STATIC MaterialList_t * ConvertMaterial(MatEntry_t *mat_entry, PixmapList_t **pixmap_list, br_3ds_options *options)
{
    br_material *material;
    br_pixelmap *colour_map;
    br_pixelmap *index_shade;
    MaterialList_t *material_link;

    UASSERT((mat_entry != NULL) && (pixmap_list != NULL) && (options != NULL));

    material_link = (MaterialList_t*)BrMemAllocate(sizeof(MaterialList_t), BR_MEMORY_SCRATCH);
    if (material_link == NULL) {
        return NULL;
    }

    material_link->mat_shading = mat_entry->mat_shading;
    material_link->next = NULL;

    material = BrMaterialAllocate(mat_entry->mat_name);
    if (material == NULL) {
        BrMemFree(material_link);
        return NULL;
    }

    /* Set the RGB true colour. This will only be used when rendering
     * 24-bit, but a clever application could use this to generate its
     * colour ramps.
	 *	
     * As per Sam's instructions, the diffuse colour is used as the
     * material's colour. The Ambient and specular colours only affect
     * Ka and Ks.
	 */
    material->colour = ConvertColor(&(mat_entry->mat_diffuse));

    /*
	 * The Ambient, Diffuse and Specular components are used to do
     * lighting in all cases
	 */
    material->ka = GreyscaleOfColor(&(mat_entry->mat_ambient));
    material->kd = GreyscaleOfColor(&(mat_entry->mat_diffuse));
    material->ks = GreyscaleOfColor(&(mat_entry->mat_specular));

#if USE_MATERIAL_OPACITY
    /*
	 * We have discovered that this causes the material to be rejected
	 */
    material->opacity = (br_uint_8)(255.0 * mat_entry->mat_transparency);
#endif

    /*
	 * Set the 'sharpness' of the specular highlight, which normally shows
     * up in the lighting equation as cos(theta)^power
	 */
    material->power = BrFloatToScalar((float)(1.0 + (99.0 * mat_entry->mat_shininess)));

    /*
	 * Set the material flags. Most are direct BRender equivalents of
     * 3DS flags, except for perspective, which is a user choice
	 */
    material->flags = BR_MATF_LIGHT;

    if (mat_entry->mat_shading == GOURAUD) {
        material->flags |= BR_MATF_GOURAUD;
    }

#if USE_BR_MATF_PERSPECTIVE
    if (options->flags & BR_3DS_SET_PERSPECTIVE) {
        material->flags |= BR_MATF_PERSPECTIVE;
    }
#endif

#if USE_BR_MATF_TWO_SIDED
    /*
	 * Apparently, this crashes the renderer when set
	 */
    if (mat_entry->mat_two_side) {
        material->flags |= BR_MATF_TWO_SIDED;
    }
#endif

#if USE_BR_MATF_DECAL
    /*
	 * This is said to be unreliable
	 */
    if (mat_entry->mat_decal) {
        material->flags |= BR_MATF_DECAL;
    }
#endif


    if (mat_entry->pixmap_ref.mat_mapname == NULL) {

		/*
		 * Un-textured material
		 */
        material->index_base = 0;
        material->index_range = 63;
        material->colour_map = NULL;
        material->index_shade = NULL;

    } else {

		/*
		 * Textured material
		 */
        material->flags |= BR_MATF_MAP_COLOUR;

        if (mat_entry->pixmap_ref.is_reflection_map) {
            material->flags |= BR_MATF_ENVIRONMENT_I;
        }

        /*
		 * Set the texture map and shade table of the material.
         * This does not mean we load any pixel data. All we do is
         * allocate two 'br_pixelmap' structures, one for the texture and
         * one for the shade table, and set the identifier strings to
         * the material name. We leave the pixels pointer NULL in
         * both. The registry is supposed to match these up by name
         * when the materials are loaded.
		 *
         * The shade table name should be named "shade", Sam said.
		 */
        colour_map = GetPixelmap(mat_entry->pixmap_ref.mat_mapname, pixmap_list);
        index_shade = GetPixelmap("shade",pixmap_list);

        if ((colour_map != NULL) && (index_shade != NULL)) {
            material->colour_map = colour_map;
            material->index_shade = index_shade;
        } else {
            BrMaterialFree(material);
            BrMemFree(material_link);
            return NULL;
        }

        material->index_base = 64;
        material->index_range = 31;

    }

#if REGISTRY_ADD
    material_link->material = BrMaterialAdd(material);
#else
	material_link->material = material;
#endif

    if (material_link->material == NULL) {
        BrMaterialFree(material);
        BrMemFree(material_link);
        return NULL;
    }

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "Material: '%s'\n", material->identifier);
		ReportMessage(options->report, "  colour:   %3d %3d %3d\n",
			BR_RED(material->colour),
			BR_GRN(material->colour),
			BR_BLU(material->colour));
		ReportMessage(options->report, "  ka kd ks: %f %f %f\n",
			BrScalarToFloat(material->ka),
			BrScalarToFloat(material->kd),
			BrScalarToFloat(material->ks));
		ReportMessage(options->report, "  index_base:     %3d\n", material->index_base);
		ReportMessage(options->report, "  index_range:    %3d\n", material->index_range);
		if(mat_entry->pixmap_ref.mat_mapname != NULL) {
			ReportMessage(options->report, "  texture:  '%s'\n", colour_map->identifier);
			ReportMessage(options->report, "  shade:    '%s'\n", index_shade->identifier);
		}
	}
#endif

    return material_link;
}

/*
 * Given a list of converted materials, attempt to
 * find one with a specific name
 */
STATIC MaterialList_t * LookupMaterial(char *name, MaterialList_t *list)
{
    MaterialList_t *item;

    for (item = list; item != NULL; item = item->next) {
         if (BrStrCmp(name,item->material->identifier) == 0) {
             return item;
         }
    }

    return NULL;
}

/*
 * Return TRUE if a material specifies filled faces,
 * otherwise return FALSE
 */
STATIC Bool_t FillMat(MaterialList_t *material_link)
{
    return (material_link == NULL || material_link->mat_shading != WIREFRAME);
}

/*
 * Return TRUE if the material specifies wire-framed faces,
 * otherwise return FALSE
 */
STATIC Bool_t WireMat(MaterialList_t *material_link)
{
    return (material_link != NULL && material_link->mat_shading == WIREFRAME);
}

/*
 * De-allocate material list nodes
 */
STATIC void DeallocateMaterialListNodes(MaterialList_t *material_list)
{
	MaterialList_t *dud;

	while(material_list != NULL) {
		dud = material_list;
		material_list = material_list->next;
		BrMemFree(dud);
	}
}

/*
 * De-allocate entire material list
 */
STATIC void DeallocateMaterialList(MaterialList_t *material_list)
{
	MaterialList_t *dud;

	while(material_list != NULL) {
		dud = material_list;
		material_list = material_list->next;
		BrMaterialFree(dud->material);
		BrMemFree(dud);
	}
}

/*
 * De-allocate the memory used to assign
 * a specific material to a group of faces
 */
STATIC void DeallocateMshMatGroups(MshMatGroup_t *msh_mat_groups)
{
    MshMatGroup_t *dud;

    while (msh_mat_groups != NULL) {
         dud = msh_mat_groups;
         msh_mat_groups = msh_mat_groups->next;

         if(dud->indexes) BrMemFree(dud->indexes);

         BrMemFree(dud);
    }
}

/*
 * Clear all references to pixelmaps in converted materials
 */
STATIC void ClearMaterialPixelmaps(br_fmt_results *results)
{
	br_uint_32 i;
	br_material *material;

	UASSERT(results != NULL);

	for(i=0; i<results->nmaterials; i++) {
		material = results->materials[i];
		material->colour_map = NULL;
		material->index_shade = NULL;
	}
}

/*
 * Clear all references to materials in converted model faces
 */
STATIC void ClearModelMaterials(br_fmt_results *results)
{
	br_uint_16 f;
	br_uint_32 i;
	br_model *model;

	UASSERT(results != NULL);

	for(i=0; i<results->nmodels; i++) {
		model = results->models[i];
	 	for(f=0; f<model->nfaces; f++) model->faces[f].material = NULL;
	}
}

/*
 * Initialise a new point array
 */
STATIC void InitialisePointArray(PointArray_t *point_array)
{
    UASSERT(point_array != NULL);

    point_array->vertices = NULL;
}

/*
 * Read a list of vertices from the input stream
 * into the top of the stack, converting them
 * to BRender's 'br_vector3' structure
 */
STATIC State_t ReadPointArray(void *stream, Stack_t *top, br_3ds_options *options)
{
    br_uint_16 n_vertices;
    br_vector3 *vertices;
    Int_t      i;

    UASSERT((stream != NULL) && (top != NULL));

    if (!ReadUInt16(stream,&n_vertices) || n_vertices == 0) {
        return PARSE_ERROR;
    }

    top->done += sizeof(br_uint_16);

    vertices = (br_vector3*)BrMemAllocate(n_vertices * sizeof(br_vector3), BR_MEMORY_VERTICES);
    if (vertices == NULL) {
        return OUT_OF_MEMORY;
    }

    for (i=0; i < n_vertices; i++) {
        if (!ReadPoint(stream,top,vertices+i,options)) {
            BrMemFree(vertices);
            return PARSE_ERROR;
        }
    }

    top->data.point_array.n_vertices = n_vertices;
    top->data.point_array.vertices   = vertices;

    return OK;
}

/*
 * De-allocate the memory used by a point array
 */
STATIC void DismantlePointArray(PointArray_t *point_array)
{
    UASSERT(point_array != NULL);

    if (point_array->vertices != NULL) {
        BrMemFree(point_array->vertices);
        point_array->vertices = NULL;
    }
}

/*
 * Initialise a face array
 */
STATIC void InitialiseFaceArray(FaceArray_t *face_array)
{
    UASSERT(face_array != NULL);

    face_array->faces          = NULL;
    face_array->smooth_group   = NULL;
    face_array->msh_mat_groups = NULL;
}

/*
 * Read a list of faces from the input stream
 * into the top of the stack, converting the edge
 * visibility flags into BRender co-planarity flags
 */
STATIC State_t ReadFaceArray(void *stream, Stack_t *top)
{
    br_uint_16 n_faces;
    br_uint_16 visibility;
    Int_t      i,j;
    Face_t     *faces;

    UASSERT((stream != NULL) && (top != NULL));

    if (!ReadUInt16(stream,&n_faces)
          || n_faces == 0) {
        return PARSE_ERROR;
    }

    top->done += sizeof(br_uint_16);

    faces = (Face_t*)BrMemAllocate(n_faces * sizeof(Face_t), BR_MEMORY_FACES);
    if (faces == NULL) {
        return OUT_OF_MEMORY;
    }

    for (i=0; i<n_faces; i++) {

        for (j=0; j<3; j++) {
            if (ReadUInt16(stream,&(faces[i].vertices[j]))) {
                top->done += sizeof(br_uint_16);
            } else {
                BrMemFree(faces);
                return PARSE_ERROR;
            }
        }

        if (ReadUInt16(stream,&visibility)) {

            faces[i].flags = 0;
            if ((visibility & 0x0001) == 0) {
                faces[i].flags |= BR_FACEF_COPLANAR_2;
            }
            if ((visibility & 0x0002) == 0) {
                faces[i].flags |= BR_FACEF_COPLANAR_1;
            }
            if ((visibility & 0x0004) == 0) {
                faces[i].flags |= BR_FACEF_COPLANAR_0;
            }
            top->done += sizeof(br_uint_16);

        } else {

            BrMemFree(faces);
            return PARSE_ERROR;

        }
    }

    top->data.face_array.n_faces = n_faces;
    top->data.face_array.faces = faces;

    return OK;
}

/*
 * De-allocate any memory used by a face array,
 * their smoothing groups, and their material references
 */
STATIC void DismantleFaceArray(FaceArray_t *face_array)
{
    UASSERT(face_array != NULL);

    if (face_array->faces != NULL) {
        BrMemFree(face_array->faces);
        face_array->faces = NULL;
    }

    if (face_array->smooth_group != NULL) {
        BrMemFree(face_array->smooth_group);
        face_array->smooth_group = NULL;
    }

    DeallocateMshMatGroups(face_array->msh_mat_groups);

    face_array->msh_mat_groups = NULL;
}

/*
 * Read a list of material references for a set of faces
 * from the input stream, into the top of the stack,
 * remembering to make sure the material for those
 * faces has been defined
 */
STATIC State_t ReadMshMatGroup(void *stream, Stack_t *top, MaterialList_t *material_list)
{
    char           buffer[17];
    br_uint_16     n_indexes;
    br_uint_16     *indexes;
    Int_t          i;
    MaterialList_t *material_link;
    MshMatGroup_t  *msh_mat_group;

    UASSERT((stream != NULL) && (top != NULL));

    if (!ReadString(stream,top,17,buffer) || !ReadUInt16(stream,&n_indexes)) {
        return PARSE_ERROR;
    }

    top->done += sizeof(br_uint_16);

    material_link = LookupMaterial(buffer,material_list);

    if (material_link == NULL) {
        return PARSE_ERROR;
    }

    /*
     * Allocate BR_MEMORY_APPLICATION rather than BR_MEMORY_GROUPS. aac
     */
    msh_mat_group = (MshMatGroup_t*)BrMemAllocate(sizeof(MshMatGroup_t), BR_MEMORY_APPLICATION);
    if (msh_mat_group == NULL) {
        return OUT_OF_MEMORY;
    }

	if(n_indexes == 0) {
	    msh_mat_group->indexes = NULL;
	} else {

	    indexes = (br_uint_16*)BrMemAllocate(n_indexes * sizeof(br_uint_16), BR_MEMORY_MATERIAL_INDEX);

    	if (indexes == NULL) {
	        BrMemFree(msh_mat_group);
	        return OUT_OF_MEMORY;
	    }

    	for (i=0; i<n_indexes; i++) {
	        if (ReadUInt16(stream,&(indexes[i]))) {
	            top->done += sizeof(br_uint_16);
	        } else {
	            BrMemFree(indexes);
	            BrMemFree(msh_mat_group);
	            return OUT_OF_MEMORY;
	        }
	    }

	    msh_mat_group->indexes = indexes;
	}

    msh_mat_group->material_link = material_link;
    msh_mat_group->n_indexes = n_indexes;

    top->data.msh_mat_group = msh_mat_group;

    return OK;
}

/*
 * Read a smoothing group definition for each face
 * from the input stream, and convert them from
 * 32 bit to 16 bit smoothing groups before placing
 * them at the top of the stack
 */
STATIC State_t ReadSmoothGroup(void *stream, Stack_t *top, br_uint_16 n_faces, br_3ds_options *options)
{
    br_uint_16 i,j;
    br_uint_16 n_used;
    br_uint_16 *unused, *used, member;
    br_uint_16 *new_smooth_group, new_face_group;
    br_uint_32 *smooth_group, face_group;

    br_uint_16 set[32] = {
        0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,
        16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    };
    br_uint_16 map[32] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };

    UASSERT((stream != NULL) && (top != NULL) && (n_faces > 0) && (options != NULL));

    smooth_group = (br_uint_32*)BrMemAllocate(n_faces * sizeof(br_uint_32), BR_MEMORY_SCRATCH);
    if (smooth_group == NULL) {
        return OUT_OF_MEMORY;
    }

    new_smooth_group = (br_uint_16*)BrMemAllocate(n_faces * sizeof(br_uint_16), BR_MEMORY_SCRATCH);
    if (new_smooth_group == NULL) {
        BrMemFree(smooth_group);
        return OUT_OF_MEMORY;
    }

    for (i=0; i<n_faces; i++) {
        if (ReadUInt32(stream,&(smooth_group[i]))) {
            top->done += sizeof(br_uint_32);
        } else {
            BrMemFree(new_smooth_group);
            BrMemFree(smooth_group);
            return OUT_OF_MEMORY;
        }
    }

	i = n_used = 0;
    used = set + 32;

    do {
        face_group = smooth_group[i++];
        unused     = set;
        do {
            member = *unused;
            if ((face_group & (1 << member)) != 0) {
                map[member] = (br_uint_16)((1 << n_used) & 0xFFFF);
                n_used += 1;
                used -= 1;
                *unused = *used;
                *used = member;
            } else {
                unused += 1;
            }
        } while (unused < used);
    } while (i < n_faces && n_used < 32);

    if (n_used > 16) {

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "  %d smoothing group%s (arbitrary remapping)\n", n_used, (n_used != 1)?"s":"");
	}
#endif

        for (i=0; i<n_faces; i++) {
            face_group = smooth_group[i];
            new_smooth_group[i] = (br_uint_16)(face_group & 0xFFFF) | (br_uint_16)(face_group >> 16);
        }

    } else {

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "  %d smoothing group%s (sensible remapping)\n", n_used, (n_used != 1)?"s":"");
	}
#endif

        for (i=0; i<n_faces; i++) {
            face_group     = smooth_group[i];
            new_face_group = 0;
            for (j=0; j<n_used; j++) {
                member = used[j];
                if ((face_group & (1 << member)) != 0) {
                    new_face_group |= map[member];
                }
            }
            new_smooth_group[i] = new_face_group;
        }

    }

    BrMemFree(smooth_group);

    top->data.smooth_group = new_smooth_group;

    return OK;
}

/*
 * Initialise a list of texture coordinates
 */
STATIC void InitialiseTexVerts(TexVerts_t *tex_verts)
{
    UASSERT(tex_verts != NULL);

    tex_verts->texverts = NULL;
}

/*
 * Read a list of texture coordinates for every vertex
 * from the input stream, convert them to the 'br_vector2'
 * structure, and place them at the top of the stack
 */
STATIC State_t ReadTexVerts(void *stream, Stack_t *top)
{
    br_uint_16 n_texverts;
    br_vector2 *texverts;
    Int_t i;
    Float_t u,v;

    UASSERT((stream != NULL) && (top != NULL));

    if (!ReadUInt16(stream,&n_texverts)
          || n_texverts == 0) {
        return PARSE_ERROR;
    }

    top->done += sizeof(br_uint_16);

    texverts = (br_vector2*)BrMemAllocate(n_texverts * sizeof(br_vector2), BR_MEMORY_SCRATCH);
    if (texverts == NULL) {
        return OUT_OF_MEMORY;
    }

    for (i=0; i<n_texverts; i++) {
        if (ReadFloat(stream,&u) && ReadFloat(stream,&v)) {
            texverts[i].v[0] = BrFloatToScalar((float)(u));
            texverts[i].v[1] = BrFloatToScalar((float)(-v));
            top->done += 2 * sizeof(Float_t);
        } else {
            BrMemFree(texverts);
            return PARSE_ERROR;
        }
    }

    top->data.tex_verts.n_texverts = n_texverts;
    top->data.tex_verts.texverts   = texverts;

    return OK;
}

/*
 * De-allocate the memory used to store
 * texture coordinates for each vertex
 */
STATIC void DismantleTexVerts(TexVerts_t *tex_verts)
{
    UASSERT(tex_verts != NULL);

    if (tex_verts->texverts != NULL) {
        BrMemFree(tex_verts->texverts);
        tex_verts->texverts = NULL;
    }
}

/*
 * Read a mesh matrix from the input stream into the
 * top of the stack, converting it for a right-handed
 * coordinate system
 */
STATIC State_t ReadMeshMatrix(void *stream, Stack_t *top, br_3ds_options *options)
{
  	br_vector3 row;
    br_matrix34 *mesh_matrix;
    Int_t i;

    UASSERT((stream != NULL) && (top != NULL));

    mesh_matrix = &(top->data.mesh_matrix);

    for (i=0; i<4; i++) {

		if(!ReadPoint(stream, top, &row, options))
			return PARSE_ERROR;

		mesh_matrix->m[i][0] = row.v[0];
		mesh_matrix->m[i][1] = row.v[1];
		mesh_matrix->m[i][2] = row.v[2];
    }

    return OK;
}

/*
 * Allocate a new named object structure, setting
 * it's name to the one supplied
 */
STATIC NamedObj_t * AllocateNamedObj(char *name)
{
    NamedObj_t *obj;

    obj = (NamedObj_t*)BrMemAllocate(sizeof(NamedObj_t), BR_MEMORY_SCRATCH);

    if (obj != NULL) {

        if (name != NULL) {
            obj->name = BrMemStrDup(name);
            if (obj->name == NULL) {
                BrMemFree(obj);
                return NULL;
            }
        }

        obj->type = NONE;

        BrMatrix34Identity(&(obj->to_world));
        BrMatrix34Identity(&(obj->from_world));

        obj->next = NULL;
    }

    return obj;
}

/*
 * De-allocate a list of converted named objects,
 * but leave any attached BRender models alone
 */
STATIC void DeallocateNamedObjListNodes(NamedObj_t *objects)
{
    NamedObj_t *dud;

    while (objects != NULL) {

        dud = objects;
        objects = objects->next;

        BrMemFree(dud->name);

        if (dud->type == LIGHT && dud->data.light != NULL) {
            BrMemFree(dud->data.light);
		} else if (dud->type == CAMERA && dud->data.camera != NULL) {
		    BrMemFree(dud->data.camera);
        } else if (dud->type == MODEL) {
            BrMemFree(dud->data.model);
        }

        BrMemFree(dud);

    }
}

/*
 * De-allocate and entire list of named objects
 */
STATIC void DeallocateNamedObjList(NamedObj_t *objects)
{
    NamedObj_t *dud;

    while (objects != NULL) {

        dud = objects;
        objects = objects->next;

        BrMemFree(dud->name);

        if (dud->type == LIGHT && dud->data.light != NULL) {
            BrMemFree(dud->data.light);
		} else if (dud->type == CAMERA && dud->data.camera != NULL) {
		    BrMemFree(dud->data.camera);
        } else if (dud->type == MODEL) {
            if (dud->data.model->fill_model != NULL) {
                BrModelFree(dud->data.model->fill_model);
            }
            if (dud->data.model->wire_model != NULL) {
                BrModelFree(dud->data.model->wire_model);
            }
            BrMemFree(dud->data.model);
        }

        BrMemFree(dud);

    }
}



/*
 * Take a face map of materials and a vertex re-mapping array,
 * allocate a new 'br_model', and fill it up
 */
STATIC br_model * MakeModelFromMaps(NTriObj_t *n_tri_obj,
									Bool_t (*material_type)(MaterialList_t*),
									char *name,
									char *suffix,
									Int_t n_vertices,
									Int_t n_faces,
									br_uint_16 *vertex_map,
									MaterialList_t **face_map)
{
    char *full_name;
    br_model *result;
    br_model *model;
    br_vertex *vertex;
    br_face *face;
    Int_t i,j;

    UASSERT(n_tri_obj != NULL);
	UASSERT(material_type != NULL);
	UASSERT((name != NULL) && (suffix != NULL));
	UASSERT(vertex_map != NULL);
	UASSERT(face_map != NULL);
	UASSERT(((n_vertices == 0) && (n_faces == 0)) || ((n_vertices > 0) && (n_faces > 0)));

    if (n_vertices == 0) {
        return NULL;
    }

    full_name = BrMemAllocate((BrStrLen(name)+BrStrLen(suffix)+1) * sizeof(char), BR_MEMORY_STRING);
    if (full_name == NULL)
        return NULL;

    BrStrCpy(full_name,name);
    BrStrCat(full_name,suffix);

    model = BrModelAllocate(full_name,n_vertices,n_faces);
    
    /*
     * Set model updatable flags or BrSaveModelMany complains. aac
     */
    model->flags|=BR_MODF_UPDATEABLE;


    BrMemFree(full_name);

    if (model == NULL) {
        return NULL;
    }

    BrVector3SetFloat(&(model->pivot),0.0F,0.0F,0.0F);

    /*
	 * For all of the points in the point array....
	 */
    for (i=0; i<n_tri_obj->point_array.n_vertices; i++) {

        /*
		 * If a point has been remapped into the target vertex array....
		 */
        if (vertex_map[i] != FENCE) {

            /*
			 * Find the target vertex
			 */
            vertex = model->vertices + vertex_map[i];

            /*
			 * Copy its coordinates from the the current point
			 */
            BrVector3Copy(&(vertex->p), &(n_tri_obj->point_array.vertices[i]));

            /*
			 * And if texture mapping is specified....
			 */
            if (n_tri_obj->tex_verts.texverts != NULL) {

                /*
				 * Copy its texture coordinates from the point
				 */
                BrVector2Copy(&(vertex->map), &(n_tri_obj->tex_verts.texverts[i]));
            }
        }
    }

    /*
	 * Start the target face at the beginning of the target array
	 */
    face = model->faces;

    /*
	 * For all source faces in the mesh....
	 */
    for (i=0; i<n_tri_obj->face_array.n_faces; i++) {

        /*
		 * If a source face has the right type of material shading....
		 */
        if (material_type(face_map[i])) {

            /*
			 * Copy its vertex indices from the vertex remapping array
			 */
            for (j=0; j<3; j++) {
                face->vertices[j] = vertex_map[n_tri_obj->face_array.faces[i].vertices[j]];
            }

            /*
			 * Copy the co-planarity flags
			 */
            face->flags = (unsigned char)n_tri_obj->face_array.faces[i].flags;

            /*
			 * Find the faces material reference
			 */
            if (face_map[i] != NULL) {
                face->material = face_map[i]->material;
            } else {
                face->material = NULL;
            }

            /*
			 * If smoothing is specified, then copy the smooth group
			 */
            if (n_tri_obj->face_array.smooth_group != NULL) {
                face->smoothing = n_tri_obj->face_array.smooth_group[i];
            } else {
                face->smoothing = 0;
            }

            face += 1;
        }

    }

#if REGISTRY_ADD
    result = BrModelAdd(model);
#else
	result = model;
#endif

    if (result == NULL) {
        BrModelFree(model);
        return NULL;
    }

    return result;
}

/*
 * Apply a matrix to all vertices in a point array
 */
STATIC void ApplyMatrixToPointArray(PointArray_t *point_array, br_matrix34 *matrix)
{
    br_vector3 new_vertex;
    Int_t i;

    UASSERT((point_array != NULL) && (matrix != NULL));

    for (i = 0; i < point_array->n_vertices; i++) {
        BrMatrix34ApplyP(&new_vertex, &(point_array->vertices[i]), matrix);
        BrVector3Copy(&(point_array->vertices[i]), &new_vertex);
    }
}

/*
 * Find the inverse of the 'to_world' matrix
 * and set 'from_world' to it, unless it is singular
 * in which case both are set to the identity
 */
STATIC void CreateFromWorldMatrix(NamedObj_t *named_obj, br_3ds_options *options)
{
    br_scalar det;

    det = BrMatrix34Inverse(&(named_obj->from_world), &(named_obj->to_world));

    if (BrScalarToFloat(det) == 0.0) {

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "  Singluar matrix found, using identity instead\n");
	}
#endif

        BrMatrix34Identity(&(named_obj->from_world));
        BrMatrix34Identity(&(named_obj->to_world));
    }
}

/*
 * Make a 'Model_t' and put into a 'NamedObj_t'
 */
STATIC State_t ConvertNTriObj(NTriObj_t  *n_tri_obj, NamedObj_t *named_obj, br_3ds_options *options)
{
    char *name;
    br_uint_16 *fill_vertex_map;
    br_uint_16 *wire_vertex_map;
    Int_t i,j,k;
    Int_t n_vertices, n_faces;
    Int_t n_fill_faces, n_fill_vertices;
    Int_t n_wire_faces, n_wire_vertices;
    Face_t *faces;
    Model_t *model;
    MshMatGroup_t *group, *msh_mat_groups;
    MaterialList_t **face_map;

    UASSERT((n_tri_obj != NULL) && (named_obj != NULL));

    n_vertices     = n_tri_obj->point_array.n_vertices;
    n_faces        = n_tri_obj->face_array.n_faces;
    faces          = n_tri_obj->face_array.faces;
    msh_mat_groups = n_tri_obj->face_array.msh_mat_groups;
    name           = named_obj->name;

    named_obj->type = MODEL;

	if(options->flags & BR_3DS_APPLY_MESH_MATRIX) {
	    BrMatrix34Copy(&(named_obj->to_world), &(n_tri_obj->mesh_matrix));
	    CreateFromWorldMatrix(named_obj, options);
	} else {
        BrMatrix34Identity(&(named_obj->from_world));
        BrMatrix34Identity(&(named_obj->to_world));
	}

    /*
	 * Convert points to local coordinate space
	 */
    ApplyMatrixToPointArray(&(n_tri_obj->point_array), &(named_obj->from_world));

    /*
	 * If texture mapping is specified, make sure that the number of
	 * texture coordinates agrees with the number of vertices
	 */
    if (n_tri_obj->tex_verts.texverts != NULL) {
        if (n_tri_obj->tex_verts.n_texverts != n_vertices) {
            return PARSE_ERROR;
        }
    }

    model = BrMemAllocate(sizeof(Model_t), BR_MEMORY_MODEL);
    if (model == NULL) {
        return OUT_OF_MEMORY;
    }

    face_map = (MaterialList_t**)BrMemAllocate(n_faces * sizeof(MaterialList_t*), BR_MEMORY_SCRATCH);
    if (face_map == NULL) {
        BrMemFree(model);
        return OUT_OF_MEMORY;
    }

    fill_vertex_map = (br_uint_16*)BrMemAllocate(n_vertices * sizeof(br_uint_16), BR_MEMORY_SCRATCH);
    if (fill_vertex_map == NULL) {
        BrMemFree(face_map);
        BrMemFree(model);
        return OUT_OF_MEMORY;
    }

    wire_vertex_map = (br_uint_16*)BrMemAllocate(n_vertices * sizeof(br_uint_16), BR_MEMORY_SCRATCH);
    if (wire_vertex_map == NULL) {
        BrMemFree(fill_vertex_map);
        BrMemFree(face_map);
        BrMemFree(model);
        return OUT_OF_MEMORY;
    }

    /*
	 * Initialise the entire face mapping array to the default material
	 */
    for (i=0; i<n_faces; i++) {
        face_map[i] = NULL;
    }

    /*
	 * For all of the different material applied to faces....
	 */
    for (group = msh_mat_groups; group != NULL; group = group->next) {

         /*
		  * For all of the faces given this material....
		  */
         for (i=0; i<group->n_indexes; i++) {

			/*
			 * Find the specific face, and set its entry in the face
			 * mapping array to the material
			 */
             j = group->indexes[i];
             if (j >= n_faces) {
                 BrMemFree(wire_vertex_map);
                 BrMemFree(fill_vertex_map);
                 BrMemFree(face_map);
                 BrMemFree(model);
                 return PARSE_ERROR;
             } else if (face_map[j] == NULL) {
                 face_map[j] = group->material_link;
             }

         }

    }

    /*
	 * Initialise both vertex re-mapping arrays to remap no vertices
	 */
    for (i=0; i<n_vertices; i++) {
        fill_vertex_map[i] = FENCE;
        wire_vertex_map[i] = FENCE;
    }

    n_fill_faces    = 0;
    n_wire_faces    = 0;
    n_fill_vertices = 0;
    n_wire_vertices = 0;

    /*
	 * For all faces in the face mapping array....
	 */
    for (i=0; i<n_faces; i++) {

		/*
		 * If it uses a filled material....
		 */
        if (FillMat(face_map[i])) {

            n_fill_faces += 1;

            /*
			 * For all three vertices referred to by the face....
			 */
            for (j=0; j<3; j++) {

				/*
				 * Find the location of the vertex in the point array,
				 * and if no entry for it exists in the filled vertex
				 * re-mapping array, set its entry to the current number
				 * of filled faces
				 */
                k = faces[i].vertices[j];
                if (k >= n_vertices) {
                    BrMemFree(wire_vertex_map);
                    BrMemFree(fill_vertex_map);
                    BrMemFree(face_map);
                    BrMemFree(model);
                    return PARSE_ERROR;
                } else if (fill_vertex_map[k] == FENCE) {
                    fill_vertex_map[k] = (br_uint_16)n_fill_vertices++;
                }

            }

        } else {

			/*
			 * If it uses a wireframed material....
			 */
            n_wire_faces += 1;

            /*
			 * For all three vertices referred to by the face....
			 */
            for (j=0; j<3; j++) {

				/*
				 * Find the location of the vertex in the point array,
				 * and if no entry for it exists in the filled vertex
				 * re-mapping array, set its entry to the current number
				 * of wireframed faces
				 */
                k = faces[i].vertices[j];
                if (k >= n_vertices) {
                    BrMemFree(wire_vertex_map);
                    BrMemFree(fill_vertex_map);
                    BrMemFree(face_map);
                    BrMemFree(model);
                    return PARSE_ERROR;
                } else if (wire_vertex_map[k] == FENCE) {
                    wire_vertex_map[k] = (br_uint_16) n_wire_vertices++;
                }

            }

        }

    }

	if(n_fill_faces && n_wire_faces) {
	    model->fill_model = MakeModelFromMaps(n_tri_obj, &FillMat,
    	                                      name, "-fill",
        	                                  n_fill_vertices, n_fill_faces,
            	                              fill_vertex_map, face_map);
	    model->wire_model = MakeModelFromMaps(n_tri_obj, &WireMat,
    	                                      name, "-wire",
        	                                  n_wire_vertices, n_wire_faces,
            	                              wire_vertex_map, face_map);
	} else if(n_fill_faces) {
	    model->fill_model = MakeModelFromMaps(n_tri_obj, &FillMat,
    	                                      name, "",
        	                                  n_fill_vertices, n_fill_faces,
            	                              fill_vertex_map, face_map);
	} else if(n_wire_faces) {
	    model->wire_model = MakeModelFromMaps(n_tri_obj, &WireMat,
    	                                      name, "",
        	                                  n_wire_vertices, n_wire_faces,
            	                              wire_vertex_map, face_map);
	}

    BrMemFree(wire_vertex_map);
    BrMemFree(fill_vertex_map);
    BrMemFree(face_map);

    if ((n_fill_faces > 0 && model->fill_model == NULL)
          || (n_wire_faces > 0 && model->wire_model == NULL)) {
        if (model->fill_model != NULL) {
            BrModelFree(model->fill_model);
        }
        if (model->wire_model != NULL) {
            BrModelFree(model->wire_model);
        }
        BrMemFree(model);
        return OUT_OF_MEMORY;
    }

    named_obj->data.model = model;

    return OK;
}

/*
 * Generate a transform from a combination of the position,
 * target and bank angle of an object. Used to generate the
 * 'to_world' matrix for cameras and lights.
 */
STATIC void MakeMatrixFromTarget(br_matrix34 *trans, br_vector3 *posn, br_vector3 *target, Float_t bank_angle_deg)
{
    br_vector3 viewvec, v_normed;
    br_angle bank_angle_br, altitude, azimuth;
    br_fraction x, y, z, out_radius;

    BrVector3Sub(&viewvec, target, posn);
    BrVector3Normalise(&v_normed, &viewvec);

    bank_angle_br = BR_ANGLE_DEG((float) bank_angle_deg);

    x = BrScalarToFraction(v_normed.v[0]);
    y = BrScalarToFraction(v_normed.v[1]);
    z = BrScalarToFraction(v_normed.v[2]);

    out_radius = BrScalarToFraction(BR_LENGTH2(v_normed.v[0], v_normed.v[2]));

    altitude = BR_ATAN2(y, out_radius);

    if (x != BR_SCALAR(0) || z != BR_SCALAR(0)) {
        azimuth = BR_ATAN2(x, z);
    } else {
        azimuth = BR_ANGLE_DEG(90.0);
    }

    BrMatrix34Translate(trans, posn->v[0], posn->v[1], posn->v[2]);
    BrMatrix34PreRotateY(trans, azimuth);
    BrMatrix34PreRotateX(trans, (br_angle)BR_CONST_MUL(altitude, -1));
    BrMatrix34PreRotateZ(trans, bank_angle_br);
    BrMatrix34PreRotateY(trans, BR_ANGLE_DEG(180.0));
}

/*
 * Make a 'Camera_t' and put it into a 'NamedObj_t'
 */
STATIC State_t ConvertNCamera(NCamera_t *n_camera, NamedObj_t *named_obj, br_3ds_options *options)
{
    br_angle fov;
    Camera_t *camera;

    MakeMatrixFromTarget(&(named_obj->to_world), &(n_camera->posn),
                         &(n_camera->target), n_camera->bank_angle);

    CreateFromWorldMatrix(named_obj, options);

	/*
	 * If the 'dummy_cameras' flag is set, then set the type of object to
	 * NONE, so that dummy actors will be made for it, otherwise....
	 */

    if (options->flags & BR_3DS_DUMMY_CAMERAS) {
        named_obj->type = NONE;
    } else {

		/*
		 * Set the type of named object to be a camera, and convert
		 * the focal length to a field of view angle
		 */
        named_obj->type = CAMERA;
        named_obj->data.camera = camera = (Camera_t *) BrMemAllocate(sizeof(Camera_t), BR_MEMORY_CAMERA);
		if (camera == NULL) {
		    return OUT_OF_MEMORY;
		}

        fov = (2 * BR_ATAN2(BR_SCALAR(HALF_IMAGE_PLANE_MM), BrFloatToScalar(n_camera->focal_length)));

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "Focal length: %5.1f mm Angle: %5.1f degrees\n",
			n_camera->focal_length, BrScalarToFloat(BrAngleToDegree(fov)));
	}
#endif

        camera->field_of_view = fov;
    }

    return OK;
}

/*
 * Make a 'Light_t and put it into a 'NameObj_t'
 */
STATIC State_t ConvertNDLight(NDLight_t *n_d_light, NamedObj_t *named_obj, br_3ds_options *options)
{
    Light_t *light;

    if (n_d_light->is_spotlight) {
        MakeMatrixFromTarget(&(named_obj->to_world), &(n_d_light->posn),
                             &(n_d_light->dl_spotlight.target), 0.0F);
    } else {
        BrMatrix34Translate(&(named_obj->to_world),
                            n_d_light->posn.v[0],
                            n_d_light->posn.v[1],
                            n_d_light->posn.v[2]);
    }

    CreateFromWorldMatrix(named_obj, options);

	/*
	 * If the 'dummy_lights' flag is set, then set the type of object to
	 * NONE, so that dummy actors will be made for it, otherwise....
	 */

    if (options->flags & BR_3DS_DUMMY_LIGHTS) {
        named_obj->type = NONE;
    } else {

        /*
		 * Set the type of the named object to be a light
		 */
        named_obj->type = LIGHT;

        named_obj->data.light = light = (Light_t *) BrMemAllocate(sizeof(Light_t), BR_MEMORY_LIGHT);
        if (light == NULL) {
            return OUT_OF_MEMORY;
        }

		/*
		 * Copy the on-switch position, convert the 'Color_t' to a 'br_colour'
		 */
        light->is_off = n_d_light->is_off;
        light->colour = ConvertColor(&(n_d_light->color));

        if (n_d_light->is_spotlight) {

			/*
			 * If it's a spotlight, then set the type of light to a be
			 * a BR_LIGHT_SPOT, and convert the cone angles
			 */
            light->type = BR_LIGHT_SPOT;
            light->cone_inner = BrDegreeToAngle(BrFloatToScalar((float)(
                n_d_light->dl_spotlight.cone_inner)));
            light->cone_outer = BrDegreeToAngle(BrFloatToScalar((float)(
                n_d_light->dl_spotlight.cone_inner)));

        } else {

			/*
			 * If it's not a spotlight, just set the type
			 */
            light->type = BR_LIGHT_POINT;

        }

    }

    return OK;
}

/*
 * Initialise all component parts of a 3DS triangle
 * mesh object, ready to read in the enclosed chunks
 */
STATIC void InitialiseNTriObj(NTriObj_t *n_tri_obj)
{
    UASSERT(n_tri_obj != NULL);

    InitialisePointArray(&(n_tri_obj->point_array));
    InitialiseFaceArray(&(n_tri_obj->face_array));
    InitialiseTexVerts(&(n_tri_obj->tex_verts));
}

/*
 * De-allocate all memory used to hold a 3DS triangle mesh object
 */
STATIC void DismantleNTriObj(NTriObj_t *n_tri_obj)
{
    UASSERT(n_tri_obj != NULL);

    DismantlePointArray(&(n_tri_obj->point_array));
    DismantleFaceArray(&(n_tri_obj->face_array));
    DismantleTexVerts(&(n_tri_obj->tex_verts));
}

/*
 * Actor enumeration callback used to count cameras
 */
STATIC br_uint_32 BR_CALLBACK CountCameras(br_actor *actor, void *arg)
{
	br_uint_32 *count = (br_uint_32 *)arg;

	if(actor->type == BR_ACTOR_CAMERA) (*count)++;

	return BrActorEnum(actor, CountCameras, arg);
}

/*
 * Actor enumeration callback used to fill table of cameras
 */
STATIC br_uint_32 BR_CALLBACK FindCameras(br_actor *actor, void *arg)
{
	br_camera ***camera = (br_camera ***)arg;

	if(actor->type == BR_ACTOR_CAMERA) *((*camera)++) = (br_camera *)actor->type_data;

	return BrActorEnum(actor, FindCameras, arg);
}

/*
 * Actor enumeration callback used to detach
 * camera type data resources from parent actors
 */
STATIC br_uint_32 BR_CALLBACK DetachCameras(br_actor *actor, void *arg)
{
	br_fmt_results *results = (br_fmt_results *)arg;

	if(actor->type == BR_ACTOR_CAMERA) BrResAdd(results->cameras, actor->type_data);

	return BrActorEnum(actor, DetachCameras, arg);
}

/*
 * Collect cameras and generate an array of pointers
 */
STATIC void CollectCameras(br_fmt_results *results, br_actor *actor)
{
	br_uint_32 count;
	br_camera **camera;

	UASSERT(actor != NULL);
	UASSERT(results != NULL);

	/*
	 * Count cameras
	 */
	count = 0;
	BrActorEnum(actor, CountCameras, &count);
	if(count == 0) return;

	/*
	 * Allocate space for table of pointers
	 */
	results->cameras = (br_camera **)BrResAllocate(results, count * sizeof(br_camera *), BR_MEMORY_FMT_RESULTS);
	results->ncameras = count;

	/*
	 * Fill table
	 */
	camera = results->cameras;
	BrActorEnum(actor, FindCameras, &camera);
}

/*
 * Actor enumeration callback used to count lights
 */
STATIC br_uint_32 BR_CALLBACK CountLights(br_actor *actor, void *arg)
{
	br_uint_32 *count = (br_uint_32 *)arg;

	if(actor->type == BR_ACTOR_LIGHT) (*count)++;

	return BrActorEnum(actor, CountLights, arg);
}

/*
 * Actor enumeration callback used to fill table of lights
 */
STATIC br_uint_32 BR_CALLBACK FindLights(br_actor *actor, void *arg)
{
	br_light ***light = (br_light ***)arg;

	if(actor->type == BR_ACTOR_LIGHT) *((*light)++) = (br_light *)actor->type_data;

	return BrActorEnum(actor, FindLights, arg);
}

/*
 * Actor enumeration callback used to detach
 * light type data resources from parent actors
 */
STATIC br_uint_32 BR_CALLBACK DetachLights(br_actor *actor, void *arg)
{
	br_fmt_results *results = (br_fmt_results *)arg;

	if(actor->type == BR_ACTOR_LIGHT) BrResAdd(results->lights, actor->type_data);

	return BrActorEnum(actor, DetachLights, arg);
}

/*
 * Collect lights and generate an array of pointers
 */
STATIC void CollectLights(br_fmt_results *results, br_actor *actor)
{
	br_uint_32 count;
	br_light **light;

	UASSERT(actor != NULL);
	UASSERT(results != NULL);

	/*
	 * Count lights
	 */
	count = 0;
	BrActorEnum(actor, CountLights, &count);
	if(count == 0) return;

	/*
	 * Allocate space for table of pointers
	 */
	results->lights = (br_light **)BrResAllocate(results, count * sizeof(br_light *), BR_MEMORY_FMT_RESULTS);
	results->nlights = count;

	/*
	 * Fill table
	 */
	light = results->lights;
	BrActorEnum(actor, FindLights, &light);
}

/*
 * Scale a model
 */
STATIC void ModelScale(br_model *model, br_vector3 *scale)
{
	br_uint_32 v;

	for(v = 0; v<model->nvertices; v++) {
		model->vertices[v].p.v[0] = BR_MUL(model->vertices[v].p.v[0], scale->v[0]);
		model->vertices[v].p.v[1] = BR_MUL(model->vertices[v].p.v[1], scale->v[1]);
		model->vertices[v].p.v[2] = BR_MUL(model->vertices[v].p.v[2], scale->v[2]);
	}
}

/*
 * Apply a scaling to all models
 */
STATIC void ScaleModels(NamedObj_t *list, br_scalar scale)
{
	br_vector3 vscale;
	NamedObj_t *item;

	BrVector3Set(&vscale, scale, scale, scale);

	for(item = list; item != NULL; item = item->next) {
		if(item->type == MODEL) {
			if(item->data.model->fill_model != NULL) ModelScale(item->data.model->fill_model, &vscale);
			if(item->data.model->wire_model != NULL) ModelScale(item->data.model->wire_model, &vscale);
		}
	}
}

/*
 * Collect models and generate an array of pointers
 */
STATIC void CollectModels(br_fmt_results *results, NamedObj_t *list)
{
	Int_t i,count;
	NamedObj_t *item;

	UASSERT(results != NULL);

	/*
	 * Count models
	 */
	count = 0;
	for(item = list; item != NULL; item = item->next) {
		if(item->type == MODEL) {
			if(item->data.model->fill_model != NULL) count++;
			if(item->data.model->wire_model != NULL) count++;
		}
	}
	if(count == 0) return;

	/*
	 * Allocate space for table of pointers
	 */
	results->models = (br_model **)BrResAllocate(results, count * sizeof(br_model *), BR_MEMORY_FMT_RESULTS);
	results->nmodels = count;

	/*
	 * Fill table
	 */
	for(i = 0, item = list; item != NULL; item = item->next) {
		if(item->type == MODEL) {
			if(item->data.model->fill_model != NULL) results->models[i++] = item->data.model->fill_model;
			if(item->data.model->wire_model != NULL) results->models[i++] = item->data.model->wire_model;
		}
	}

	/*
	 * Attach models to table as resources
	 */
	for(i = 0; i<count; i++) BrResAdd(results->models, results->models[i]);
}

/*
 * Collect pixelmaps and generate an array of pointers
 */
STATIC void CollectPixelmaps(br_fmt_results *results, PixmapList_t *list)
{
	Int_t i,count;
	PixmapList_t *item;

	UASSERT(results != NULL);

	/*
	 * Count pixelmaps
	 */
	count = 0;
	for(item = list; item != NULL; item = item->next) count++;
	if(count == 0) return;

	/*
	 * Allocate space for table of pointers
	 */
	results->pixelmaps = (br_pixelmap **)BrResAllocate(results, count * sizeof(br_pixelmap), BR_MEMORY_FMT_RESULTS);
	results->npixelmaps = count;

	/*
	 * Fill table
	 */
	for(i = 0, item = list; item != NULL; item = item->next) results->pixelmaps[i++] = item->pixelmap;

	/*
	 * Attach pixelmaps to table as resources
	 */
	for(i = 0; i<count; i++) BrResAdd(results->pixelmaps, results->pixelmaps[i]);
}

/*
 * Collect materials and generate an array of pointers
 */
STATIC void CollectMaterials(br_fmt_results *results, MaterialList_t *list)
{
	Int_t i,count;
	MaterialList_t *item;

	UASSERT(results != NULL);

	/*
	 * Count materials
	 */
	count = 0;
	for(item = list; item != NULL; item = item->next) count++;

	if(count == 0) return;

	/*
	 * Allocate space for table of pointers
	 */
	results->materials = (br_material **)BrResAllocate(results, count * sizeof(br_material *), BR_MEMORY_FMT_RESULTS);
	results->nmaterials = count;

	/*
	 * Fill table
	 */
	for(i = 0, item = list; item != NULL; item = item->next) results->materials[i++] = item->material;

	/*
	 * Attach materials to table as resources
	 */
	for(i = 0; i<count; i++) BrResAdd(results->materials, results->materials[i]);
}

/*
 * Actor enumeration callback used to attach
 * actors to actor hierarchy table as resources
 */
STATIC br_uint_32 BR_CALLBACK ResAddHierarchy(br_actor *actor, void *arg)
{
	br_fmt_results *results = (br_fmt_results *)arg;

	BrResAdd(results->actors, actor);

	return BrActorEnum(actor, ResAddHierarchy, arg);
}

/*
 * Actor enumeration callback used to clear references to models
 */
STATIC br_uint_32 BR_CALLBACK ClearActorModels(br_actor *actor, void *arg)
{
	actor->model = NULL;

	return BrActorEnum(actor, ClearActorModels, arg);
}

/*
 * Actor enumeration callback used to clear references to materials
 */
STATIC br_uint_32 BR_CALLBACK ClearActorMaterials(br_actor *actor, void *arg)
{
	actor->material = NULL;

	return BrActorEnum(actor, ClearActorMaterials, arg);
}

/*
 * Actor enumeration callback used to scale hierarchy
 */
STATIC br_uint_32 BR_CALLBACK ScaleTransform(br_actor *actor, void *arg)
{
	BrVector3Scale(&actor->t.t.translate.t, &actor->t.t.translate.t, *((br_scalar *)arg));

	return BrActorEnum(actor, ScaleTransform, arg);
}

/*
 * Scale an actor hierarchy
 */
STATIC void ScaleHierarchy(br_actor *actor, br_scalar scale)
{
	BrVector3Scale(&actor->t.t.translate.t, &actor->t.t.translate.t, scale);

	BrActorEnum(actor, ScaleTransform, &scale);
}

/*
 * Read a light from the input stream and
 * leave it pushed onto the stack
 */
STATIC State_t ReadNDLight(void *stream, Stack_t *top, br_3ds_options *options)
{
    NDLight_t *n_d_light;

    UASSERT((stream != NULL) && (top != NULL) && (options != NULL));

    n_d_light = &(top->data.n_d_light);

    n_d_light->is_off       = FALSE;
    n_d_light->is_spotlight = FALSE;

    if (!ReadPoint(stream,top,&(n_d_light->posn),options)) {
        return PARSE_ERROR;
    }

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "Light: '%s'\n", top[-1].data.named_obj->name);
		ReportMessage(options->report, "  position: (%f %f %f)\n",
			BrScalarToFloat(n_d_light->posn.v[0]),
			BrScalarToFloat(n_d_light->posn.v[1]),
			BrScalarToFloat(n_d_light->posn.v[2]));
	}
#endif

    return OK;
}

/*
 * Read a spotlight form the input stream and
 * leave it pushed on the stack
 */
STATIC State_t ReadDlSpotlight(void *stream, Stack_t *top, br_3ds_options *options)
{
    DlSpotlight_t *dl_spotlight;

    UASSERT((stream != NULL) && (top != NULL) && (options != NULL));

    dl_spotlight = &(top->data.dl_spotlight);

    if (!ReadPoint(stream,top,&(dl_spotlight->target),options)
          || !ReadFloat(stream,&(dl_spotlight->cone_inner))
          || !ReadFloat(stream,&(dl_spotlight->cone_outer))
          || dl_spotlight->cone_inner < 1.0
          || dl_spotlight->cone_inner > 160.0
          || dl_spotlight->cone_outer < 1.0
          || dl_spotlight->cone_outer > 160.0) {
        return PARSE_ERROR;
    }

    top->done += 2 * sizeof(Float_t);

    if (!SkipRest(stream,top)) {
        return PARSE_ERROR;
    }

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "  spotlight target: (%f %f %f)\n",
			BrScalarToFloat(dl_spotlight->target.v[0]),
			BrScalarToFloat(dl_spotlight->target.v[1]),
			BrScalarToFloat(dl_spotlight->target.v[2]));
		ReportMessage(options->report, "  cone angles: %f degrees -> %f degrees\n",
			dl_spotlight->cone_inner, dl_spotlight->cone_outer);
	}
#endif

    return OK;
}

/*
 * Read a camera from the input stream, skipping all
 * enclosed chunks, and leave it pushed on the stack
 */
STATIC State_t ReadNCamera(void *stream, Stack_t *top, br_3ds_options *options)
{
    NCamera_t *n_camera;

    UASSERT((stream != NULL) && (top != NULL) && (options != NULL));

    n_camera = &(top->data.n_camera);

    if (!ReadPoint(stream,top,&(n_camera->posn), options)
          || !ReadPoint(stream,top,&(n_camera->target), options)
          || !ReadFloat(stream,&(n_camera->bank_angle))
          || !ReadFloat(stream,&(n_camera->focal_length))) {
        return PARSE_ERROR;
    }

    top->done += 2 * sizeof(Float_t);

    if (!SkipRest(stream,top)) {
        return PARSE_ERROR;
    }

#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "Camera: '%s'\n", top[-1].data.named_obj->name);
		ReportMessage(options->report, "  position: (%f %f %f)\n",
			BrScalarToFloat(n_camera->posn.v[0]),
			BrScalarToFloat(n_camera->posn.v[1]),
			BrScalarToFloat(n_camera->posn.v[2]));
		ReportMessage(options->report, "  target: (%f %f %f)\n",
			BrScalarToFloat(n_camera->target.v[0]),
			BrScalarToFloat(n_camera->target.v[1]),
			BrScalarToFloat(n_camera->target.v[2]));
	}
#endif

    return OK;
}

/*
 * Find a named object
 */
STATIC NamedObj_t * LookupNamedObj(char *name, NamedObj_t *named_objs)
{
	NamedObj_t *obj;

	UASSERT(name != NULL);

	for (obj = named_objs; obj != NULL; obj = obj->next) {
		if (BrStrCmp(name,obj->name) == 0) {
			return obj;
        }
	}

	return NULL;
}

/*
 * Initialise a node header so that it cannot be referenced as
 * a parent, and has it's own parent set to the root of the hierarchy
 */
STATIC void InitialiseNodeHdr(NodeHdr_t *node_hdr)
{
    UASSERT(node_hdr != NULL);

    node_hdr->index     = -2;
    node_hdr->parent    = -1;
    node_hdr->named_obj = NULL;
}

/*
 * Read a node header from the input stream and
 * leave it pushed onto the top of the stack
 */
STATIC State_t ReadNodeHdr(void *stream, Stack_t *top, Int_t index, NamedObj_t *named_objs)
{
    char buffer[12];
    br_int_16 parent;
    br_uint_16 skip;
    NodeHdr_t *node_hdr;

    UASSERT((stream != NULL) && (top != NULL) && (index >= 0));

    if (!ReadString(stream,top,12,buffer)
          || !ReadUInt16(stream,&skip)
          || !ReadUInt16(stream,&skip)
          || !ReadInt16(stream,&parent)
          || parent < -1 || parent >= index) {
        return PARSE_ERROR;
    }

    top->done += (sizeof(br_uint_16) * 2) + sizeof(br_int_16);

    node_hdr = &(top->data.node_hdr);

	/*
	 * If this is not a dummy node, then try to find the
	 * object named in the preamble. Fail if it cannot be found.
	 */
    if (BrStrCmp(buffer,"$$$DUMMY") != 0) {
        node_hdr->named_obj = LookupNamedObj(buffer,named_objs);
        if (node_hdr->named_obj == NULL) {
            return PARSE_ERROR;
        }
    }

    node_hdr->index  = index;
    node_hdr->parent = parent;

#if SHOW_KEYFRAME_DATA
#if REPORT_MESSAGES
	if(options->report != NULL) {
		ReportMessage(options->report, "  NODE_HDR:  %d  '%s'  %d\n", index, buffer, parent);
	}
#endif
#endif

    return OK;
}

/*
 * Allocate a new node tag and initialise it's node header
 */
STATIC NodeTag_t * AllocateNodeTag(void)
{
    NodeTag_t *node_tag;

    node_tag = (NodeTag_t*)BrMemAllocate(sizeof(NodeTag_t), BR_MEMORY_SCRATCH);

    if (node_tag != NULL) {
        InitialiseNodeHdr(&(node_tag->node_hdr));
        node_tag->instance_name = NULL;
        node_tag->has_pivot     = FALSE;
        node_tag->has_boundbox  = FALSE;
        node_tag->next          = NULL;
    }

    return node_tag;
}

/*
 * Compare strings
 */
STATIC Bool_t SameString(char *a, char *b)
{
    return ((a == NULL && b == NULL) || (a != NULL && b != NULL && BrStrCmp(a,b) == 0));
}

/*
 * Insert a new node tag at the end of an existing list,
 * making sure that it cannot be confused with any node
 * tags already in the list
 */
STATIC Bool_t InsertNodeTag(NodeTag_t *new_tag, NodeTag_t **tags)
{
    NodeTag_t  *tag;
    NodeTag_t  **link;
    NamedObj_t *named_obj;

    UASSERT((new_tag != NULL) && (tags != NULL));

    named_obj = new_tag->node_hdr.named_obj;

    link = tags;

    if (named_obj == NULL) {

        for (tag = *tags; tag != NULL; tag = tag->next) {
            if (tag->node_hdr.named_obj == NULL
                  && SameString(tag->instance_name,new_tag->instance_name)) {
                return FALSE;
            }
            link = &(tag->next);
        }

    } else {

        for (tag = *tags; tag != NULL; tag = tag->next) {
            if (tag->node_hdr.named_obj != NULL
                  && SameString(named_obj->name,tag->node_hdr.named_obj->name)
                  && SameString(new_tag->instance_name,tag->instance_name)) {
                return FALSE;
            }
            link = &(tag->next);
        }

    }

    *link = new_tag;

    return TRUE;
}

/*
 * De-allocate all memory used by a list of node tags
 */
STATIC void DeallocateNodeTags(NodeTag_t *node_tags)
{
    NodeTag_t *dud;

    while (node_tags != NULL) {
        dud = node_tags;
        node_tags = node_tags->next;

        if (dud->instance_name != NULL) {
            BrMemFree(dud->instance_name);
        }

        BrMemFree(dud);
   }
}

/*
 * Allocate a new actor with a specified name and type,
 * and with the default rendering style and identity transform
 */
STATIC br_actor * AllocateNamedActor(char *name, br_uint_8 type)
{
    br_actor *actor;

    actor = BrActorAllocate(type,NULL);
    if (actor == NULL) {
        return NULL;
    }

    actor->render_style = BR_RSTYLE_DEFAULT;
    actor->t.type = BR_TRANSFORM_IDENTITY;

    if (name != NULL) {
        actor->identifier = BrResStrDup(actor, name);
        if (actor->identifier == NULL) {
            BrActorFree(actor);
            return NULL;
        }
    }

    return actor;
}

#if REPORT_MESSAGES
/*
 * Report actor information
 */
STATIC void DisplayActor(Int_t depth, br_actor *actor, br_3ds_options *options)
{
    Int_t i, indent_depth;

    UASSERT(depth >= 0);

    if (actor != NULL) {

		ReportMessage(options->report, "%4d  ", depth);

        indent_depth = depth % MAX_DISPLAY_DEPTH;

        if (indent_depth != depth) {
			ReportMessage(options->report, "... ");
        }

        for(i=0; i < indent_depth; i++) {
			ReportMessage(options->report, "|   ");
        }

        switch (actor->type) {
            case BR_ACTOR_NONE :
				ReportMessage(options->report, "BR_ACTOR_NONE");
                break;
            case BR_ACTOR_LIGHT :
				ReportMessage(options->report, "BR_ACTOR_LIGHT");
                break;
            case BR_ACTOR_CAMERA :
				ReportMessage(options->report, "BR_ACTOR_CAMERA");
                break;
            case BR_ACTOR_MODEL :
				ReportMessage(options->report, "BR_ACTOR_MODEL");
                break;
        }

        if (actor->identifier != NULL){
			ReportMessage(options->report, ": '%s'", actor->identifier);
        }

        ReportMessage(options->report, "\n");

    }
}
#endif

/*
 * Translate a model's vertices
 */
STATIC void TranslateModelVertices(br_model *m, br_scalar x, br_scalar y, br_scalar z)
{
 	int i;

	for(i=0; i < m->nvertices; i++) {
		m->vertices[i].p.v[0] =BR_ADD(m->vertices[i].p.v[0], x);
		m->vertices[i].p.v[1] =BR_ADD(m->vertices[i].p.v[1], y);
		m->vertices[i].p.v[2] =BR_ADD(m->vertices[i].p.v[2], z);
	}
}

/*
 * Create a small hierarchy for a named object
 */
STATIC br_actor * MakeEnclosingActor(Int_t depth,
									 char *instance_name,
									 NamedObj_t *named_obj,
									 br_matrix34 *world_to_parent,
									 br_matrix34 *world_to_actor_ret,
									 br_vector3 *pivot,
									 br_3ds_options *options)
{
    char *actor_name;
    br_actor *actor;
    br_actor *fill_actor;
    br_actor *wire_actor;
    br_light *light;
    br_camera *camera;
    Int_t name_len;
    Model_t *model;

    actor_name = NULL;

	/*
	 * Work out name for this actor
	 */
    if (named_obj == NULL) {

        if (instance_name != NULL) {
            actor_name = BrResStrDup(NULL, instance_name);
            if (actor_name == NULL) {
                return NULL;
            }
        }

    } else if (instance_name != NULL) {

        name_len   = BrStrLen(named_obj->name) + BrStrLen(instance_name) + 2;
        actor_name = (char *)BrResAllocate(NULL, name_len * sizeof(char), BR_MEMORY_STRING);
        if (actor_name == NULL) {
            return NULL;
        }

        BrStrCpy(actor_name,named_obj->name);
        BrStrCat(actor_name,".");
        BrStrCat(actor_name,instance_name);

    } else {

        actor_name = BrResStrDup(NULL, named_obj->name);
        if (actor_name == NULL) {
            return NULL;
        }

    }

	/*
	 * Allocate the actor structure
	 */
    if (named_obj != NULL && named_obj->type == CAMERA) {
        actor = AllocateNamedActor(NULL,BR_ACTOR_CAMERA);
    } else if (named_obj != NULL && named_obj->type == LIGHT) {
        actor = AllocateNamedActor(NULL,BR_ACTOR_LIGHT);
    } else {
        actor = AllocateNamedActor(NULL,BR_ACTOR_NONE);
    }

    if (actor == NULL) {
        BrResFree(actor_name);
        return NULL;
    }

	BrResAdd(actor, actor_name);
    actor->identifier = actor_name;

    fill_actor = NULL;
    wire_actor = NULL;

    if (named_obj != NULL) {

		if(pivot) {
			BrMatrix34PreTranslate(&(named_obj->to_world),
				pivot->v[0],pivot->v[1],pivot->v[2]);
			BrMatrix34PostTranslate(&(named_obj->from_world),
				-pivot->v[0],-pivot->v[1],-pivot->v[2]);
		}

		/*
		 * Do this for node tags that reference a named object
		 */
        actor->t.type = BR_TRANSFORM_MATRIX34;
        BrMatrix34Mul(&(actor->t.t.mat), &(named_obj->to_world), world_to_parent);

		/*
		 * Watt-style matrices, remember...
		 */
        BrMatrix34Copy(world_to_actor_ret, &(named_obj->from_world));

    } else {

		/*
		 * Do this for dummy node tags
		 */
        actor->t.type = BR_TRANSFORM_IDENTITY;
        BrMatrix34Copy(world_to_actor_ret, world_to_parent);

    }

    if (named_obj != NULL && named_obj->type == LIGHT) {

        light = (br_light*)(actor->type_data);

		/*
		 * Copy the named object's light into the actors light
		 */
        light->identifier = BrResStrDup(light, named_obj->name);
        if (light->identifier == NULL) {
            BrActorFree(actor);
            return NULL;
        }

        light->colour = named_obj->data.light->colour;
        light->type = named_obj->data.light->type;

        if (light->type == BR_LIGHT_SPOT) {
            light->cone_inner = named_obj->data.light->cone_inner;
            light->cone_outer = named_obj->data.light->cone_outer;
        }

        if (named_obj->data.light->is_off) {
            BrLightDisable(actor);
        } else {
            BrLightEnable(actor);
        }

    } else if (named_obj != NULL && named_obj->type == CAMERA) {

        camera = (br_camera*)(actor->type_data);

        /*
		 * Copy the named object's camera into the actor's camera
		 */
        camera->identifier = BrResStrDup(camera, named_obj->name);
        if (camera->identifier == NULL) {
            BrActorFree(actor);
            return NULL;
        }

        camera->type = BR_CAMERA_PERSPECTIVE;
        camera->field_of_view = named_obj->data.camera->field_of_view;
        camera->aspect = BR_SCALAR(1.6);

        if(options->flags & BR_3DS_SET_HITHER) camera->hither_z = options->hither;
        if(options->flags & BR_3DS_SET_YON) camera->yon_z = options->yon;

    } else if (named_obj != NULL &&
			named_obj->type == MODEL &&
        	named_obj->data.model->fill_model != NULL &&
        	named_obj->data.model->wire_model != NULL) {

        model = named_obj->data.model;

		/*
		 * Allocate an actor for the filled faces mesh, and add it
		 * as a child to the actor allocated at the start of this function
		 */
        fill_actor = AllocateNamedActor(model->fill_model->identifier, BR_ACTOR_MODEL);
        if (fill_actor == NULL) {
            BrActorFree(actor);
            return NULL;
        }

        fill_actor->model        = model->fill_model;
        fill_actor->render_style = BR_RSTYLE_FACES;
        fill_actor->t.type = BR_TRANSFORM_IDENTITY;

		if(pivot)
			if (options->flags & BR_3DS_USE_MODEL_PIVOT)
				BrVector3Copy(&fill_actor->model->pivot, pivot);
			else
				TranslateModelVertices(fill_actor->model,
					-pivot->v[0],-pivot->v[1],-pivot->v[2]);

        if (BrActorAdd(actor,fill_actor) == NULL) {
            BrActorFree(actor);
            BrActorFree(fill_actor);
            return NULL;
        }

		/*
		 * Allocate an actor for the wireframed faces mesh, and add it
		 * as a child to the actor allocated at the start of this function
		 */
        wire_actor = AllocateNamedActor(model->wire_model->identifier, BR_ACTOR_MODEL);
        if (wire_actor == NULL) {
            BrActorFree(actor);
            return NULL;
        }

        wire_actor->model        = model->wire_model;
        wire_actor->render_style = BR_RSTYLE_EDGES;
        wire_actor->t.type = BR_TRANSFORM_IDENTITY;

		if(pivot)
			if (options->flags & BR_3DS_USE_MODEL_PIVOT)
				BrVector3Copy(&wire_actor->model->pivot, pivot);
			else
				TranslateModelVertices(wire_actor->model,
					-pivot->v[0],-pivot->v[1],-pivot->v[2]);

        if (BrActorAdd(actor,wire_actor) == NULL) {
            BrActorFree(actor);
            BrActorFree(wire_actor);
            return NULL;
        }

    } else if (named_obj != NULL &&
			named_obj->type == MODEL &&
        	named_obj->data.model->fill_model != NULL &&
        	named_obj->data.model->wire_model == NULL) {

        actor->model = named_obj->data.model->fill_model;
		actor->render_style = BR_RSTYLE_FACES;
		actor->type = BR_ACTOR_MODEL;

		if(pivot)
			if (options->flags & BR_3DS_USE_MODEL_PIVOT)
				BrVector3Copy(&actor->model->pivot, pivot);
			else
				TranslateModelVertices(actor->model,
					-pivot->v[0],-pivot->v[1],-pivot->v[2]);

    } else if (named_obj != NULL &&
			named_obj->type == MODEL &&
        	named_obj->data.model->fill_model == NULL &&
        	named_obj->data.model->wire_model != NULL) {

        actor->model = named_obj->data.model->wire_model;
		actor->type = BR_ACTOR_MODEL;
		actor->render_style = BR_RSTYLE_EDGES;

		if(pivot)
			if (options->flags & BR_3DS_USE_MODEL_PIVOT)
				BrVector3Copy(&actor->model->pivot, pivot);
			else
				TranslateModelVertices(actor->model,
					-pivot->v[0],-pivot->v[1],-pivot->v[2]);
	}

#if REPORT_MESSAGES
	if(options->report != NULL) {
		DisplayActor(depth, actor, options);
		DisplayActor(depth+1, fill_actor, options);
		DisplayActor(depth+1, wire_actor, options);
	}
#endif

    return actor;
}

/*
 * Find all of the direct descendents of the node tag
 * with the index 'parent', allocate actors for them,
 * and if they can be recursed on, add them as children
 * to the 'actor' parameter
 */
STATIC Bool_t CollectDescendents(Int_t depth,
								 br_actor *actor,
								 Int_t parent,
								 NodeTag_t *node_tags,
								 br_matrix34 *world_to_parent,
								 br_3ds_options *options)
{
    br_actor *child;
    br_matrix34 world_to_child;
    NodeTag_t *node_tag;

    UASSERT(actor != NULL);

    for (node_tag = node_tags; node_tag != NULL; node_tag = node_tag->next) {

		/*
		 * Find a node tag with the indicated parent
		 */
        if (node_tag->node_hdr.parent == parent) {

			/*
			 * Allocate it's actor
			 */
            child = MakeEnclosingActor(depth+1, node_tag->instance_name,
                                       node_tag->node_hdr.named_obj,
                                       world_to_parent,
                                       &world_to_child,
									   (options->flags & BR_3DS_APPLY_PIVOT)?&node_tag->pivot:NULL,
									   options);
            if (child == NULL) {
                return FALSE;
            }

			/*
			 * Collect it's own children, and it's children's children, etc....
			 */
            if (!CollectDescendents(depth+1, child,
                                    node_tag->node_hdr.index,
                                    node_tags, &world_to_child,
									options)
                  || BrActorAdd(actor,child) == NULL) {
                BrActorFree(child);
                return FALSE;
            }

        }

    }

    return TRUE;
}

/*
 * Allocate a root actor and build a hierarchy
 */
STATIC br_actor * BuildComplexHierarchy(NodeTag_t *node_tags, br_3ds_options *options)
{
    br_actor *world;
    br_matrix34 world_to_world;

    world = AllocateNamedActor(NULL,BR_ACTOR_NONE);

    if (world != NULL) {

#if REPORT_MESSAGES
	if(options->report != NULL) {
		DisplayActor(0, world, options);
	}
#endif

        BrMatrix34Identity(&world_to_world);

        if (!CollectDescendents(0, world, -1, node_tags, &world_to_world, options)) {
            BrActorFree(world);
            world = NULL;
        }

    }

    return world;
}

/*
 * Allocate a root actor and add all named
 * object actors to it as children
 */
STATIC br_actor * BuildFlatHierarchy(NamedObj_t *named_objs, br_3ds_options *options)
{
    br_actor *world, *child;
    br_matrix34 world_to_world;
    br_matrix34 dummy;
    NamedObj_t *obj;

    world = AllocateNamedActor(NULL,BR_ACTOR_NONE);

    if (world != NULL) {

#if REPORT_MESSAGES
	if(options->report != NULL) {
		DisplayActor(0, world, options);
	}
#endif

        BrMatrix34Identity(&world_to_world);

        for (obj = named_objs; obj != NULL; obj = obj->next) {

            child = MakeEnclosingActor(1, NULL, obj, &world_to_world, &dummy, NULL, options);
            if (child == NULL) {
                BrActorFree(world);
                return NULL;
            }

            if (BrActorAdd(world,child) == NULL) {
                BrActorFree(world);
                BrActorFree(child);
                return NULL;
            }

        }

    }

    return world;
}

/*
 * Convert a 3D Studio .3DS file
 */
br_fmt_results * BR_PUBLIC_ENTRY BrFmt3DSLoad(char *name, br_fmt_options *fmt_options, br_3ds_options *options)
{
	int mode;
	void *stream;
	br_actor *actor;
	br_fmt_results *results;

	Int_t n_node_tags;
	State_t state;
    Stack_t stack[16], *top, *parent;
	MaterialList_t *material_list;
	PixmapList_t *pixmap_list;
	NamedObj_t *named_objs;
	NodeTag_t *node_tags;

	/*
	 * Use default options if none is supplied
	 */
	if(options == NULL) options = &_BrDefault3DSOptions;

	/*
	 * Open file for reading
	 */
	mode = BR_FS_MODE_BINARY;
	if((stream = BrFileOpenRead(name, 0, NULL, &mode)) == NULL)
		BR_FAILURE1("Could not open file '%s' for reading", name);

	/*
	 * Initialise
	 */
	state = OK;
	top = stack;
	material_list = NULL;
	named_objs = NULL;
	pixmap_list = NULL;
	node_tags = NULL;
	n_node_tags = 0;

	/*
	 * Read the first header onto the top of the stack - don't look
	 * at what it is, since there are several options, depending on
	 * how the file was written out
	 */
	if(!ReadHeader(stream,top) || top->length < 6)
		BR_FAILURE1("Could not read first chunk header from file '%s%'", name);

	/*
	 * Loop while stack is not empty
	 */
    while((top >= stack) && (state == OK)) {

		/*
		 * Push chunk
		 */
        parent = top++;

		/*
		 * Read chunk header
		 */
        if((!ReadHeader(stream,top)) || (top->length < 6) || (top->length > (parent->length - parent->done))) {
			BR_FAILURE1("Could not read chunk header from file '%s'", name);
		} else {

			/*
			 * Switch on chunk ID tag
			 */
			switch (top->id_tag) {

                case M3DMAGIC:
                    break;

                case MLIBMAGIC:
                    break;

                case MDATA:
                    break;

                case COLOR_F :
                    if ( ((parent->id_tag != AMBIENT_LIGHT)
                            && (parent->id_tag != MAT_AMBIENT)
                            && (parent->id_tag != MAT_DIFFUSE)
                            && (parent->id_tag != MAT_SPECULAR)
                            && (parent->id_tag != N_DIRECT_LIGHT) )
                          || (parent->flags & GOT_COLOR_F) != 0
                          || (parent->flags & GOT_COLOR_24) != 0
                          || !ReadColorF(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case COLOR_24 :
                    if ( ((parent->id_tag != AMBIENT_LIGHT)
                            && (parent->id_tag != MAT_AMBIENT)
                            && (parent->id_tag != MAT_DIFFUSE)
                            && (parent->id_tag != MAT_SPECULAR)
                            && (parent->id_tag != N_DIRECT_LIGHT) )
                          || (parent->flags & GOT_COLOR_F) != 0
                          || (parent->flags & GOT_COLOR_24) != 0
                          || !ReadColor24(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case INT_PERCENTAGE :
                    if ( ( (parent->id_tag == MAT_SHININESS)
                            || (parent->id_tag == MAT_TRANSPARENCY)
                            || (parent->id_tag == MAT_TEXMAP)
                            || (parent->id_tag == MAT_REFLMAP) )
                          && (parent->flags & GOT_INT_PERCENTAGE) == 0
                          && (parent->flags & GOT_FLOAT_PERCENTAGE) == 0) {
                        br_int_16 percent;

                        if (ReadInt16(stream,&percent)
                               && percent >= 0
                               && percent <= 100) {
                            /*
                             *  Note: the lower bound needs checking against
                             * -100 for the (unsupported) MAT_XPFALL parent
                             */
                            top->data.percent = ((Float_t)(percent)) / 100.0F;

                            top->done += sizeof(br_int_16);

                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case FLOAT_PERCENTAGE :
                    if ( ( (parent->id_tag == MAT_SHININESS)
                            || (parent->id_tag == MAT_TRANSPARENCY)
                            || (parent->id_tag == MAT_TEXMAP)
                            || (parent->id_tag == MAT_REFLMAP) )
                          && (parent->flags & GOT_INT_PERCENTAGE) == 0
                          && (parent->flags & GOT_FLOAT_PERCENTAGE) == 0
                          && ReadFloat(stream,&(top->data.percent))
                          && top->data.percent >= 0.0
                          && top->data.percent <= 1.0) {
						 /*
						  * Note: the lower bound needs checking against
                          * -1.0 for the (unsupported) MAT_XPFALL parent
						  */
                         top->done += sizeof(Float_t);

                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_MAPNAME :
                    top->data.string = NULL;
                    if ((parent->id_tag == MAT_TEXMAP
                            || parent->id_tag == MAT_REFLMAP)
                          && (parent->flags & GOT_MAT_MAPNAME) == 0) {
                        char buffer[14];

                        if (ReadString(stream,top,14,buffer)) {
                            top->data.string = BrMemStrDup(buffer);
                            if (top->data.string == NULL) {
                                state = OUT_OF_MEMORY;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case M3D_VERSION :
                    if (parent->id_tag == M3DMAGIC
                          && (parent->flags & GOT_M3D_VERSION) == 0) {
                        br_int_32 version;

                        if (ReadInt32(stream,&version)) {
                            top->done += sizeof(br_int_32);

#if REPORT_MESSAGES
							if(options->report != NULL) {
								if(version == 3) {
									ReportMessage(options->report, "File version %d.0\n", version);
								} else {
									ReportMessage(options->report,
										"File version %d.0 (only able to read version 3.0 chunks)\n", version); 
								}
							}
#endif

                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MESH_VERSION :
                    if (parent->id_tag == MDATA
                          && (parent->flags & GOT_MESH_VERSION) == 0) {
                        br_int_32 version;

                        if (ReadInt32(stream,&version)) {
                            top->done += sizeof(br_int_32);

#if REPORT_MESSAGES
							if(options->report != NULL) {
								if((version == 1) || (version == 2)) {
									ReportMessage(options->report, "Mesh version %d.0\n", version);
								} else {
									ReportMessage(options->report,
										"Mesh version %d.0 (only able to read version 1.0 and 2.0 chunks)\n", version);
								}
							}
#endif

                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MASTER_SCALE :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_MASTER_SCALE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case LO_SHADOW_BIAS :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_LO_SHADOW_BIAS) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case HI_SHADOW_BIAS :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_HI_SHADOW_BIAS) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SHADOW_MAP_SIZE :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_SHADOW_MAP_SIZE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SHADOW_SAMPLES :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_SHADOW_SAMPLES) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SHADOW_RANGE :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_SHADOW_RANGE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SHADOW_FILTER :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_SHADOW_FILTER) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case AMBIENT_LIGHT :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_AMBIENT_LIGHT) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case O_CONSTS :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_O_CONSTS) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case BIT_MAP :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_BIT_MAP) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SOLID_BGND :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_SOLID_BGND) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case V_GRADIENT :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_V_GRADIENT) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case USE_BIT_MAP :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_USE_BIT_MAP) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case USE_SOLID_BGND :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_USE_SOLID_BGND) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case USE_V_GRADIENT :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_USE_V_GRADIENT) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case FOG :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_FOG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case FOG_BGND :
                    state = PARSE_ERROR;
                    break;

                case DISTANCE_CUE :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_DISTANCE_CUE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case DCUE_BGND :
                    state = PARSE_ERROR;
                    break;

                case USE_FOG :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_USE_FOG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case USE_DISTANCE_CUE :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_USE_DISTANCE_CUE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case DEFAULT_VIEW :
                    if (parent->id_tag != MDATA
                          || (parent->flags & GOT_DEFAULT_VIEW) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case VIEW_TOP :          /*  fall through .....  */
                case VIEW_BOTTOM :
                case VIEW_LEFT :
                case VIEW_RIGHT :
                case VIEW_FRONT :
                case VIEW_BACK :
                case VIEW_USER :
                case VIEW_CAMERA :       /*  ..... until here  */
                    state = PARSE_ERROR;
                    break;

                case MAT_ENTRY :
                    InitialiseMatEntry(&(top->data.mat_entry));
                    break;

                case MAT_NAME :
                    top->data.string = NULL;
                    if (parent->id_tag == MAT_ENTRY
                          && (parent->flags & GOT_MAT_NAME) == 0) {
                        char buffer[17];

                        if (ReadString(stream,top,17,buffer)
                              && LookupMaterial(buffer,material_list)
                                   == NULL) {
                            top->data.string = BrMemStrDup(buffer);
                            if (top->data.string == NULL) {
                                state = OUT_OF_MEMORY;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_AMBIENT :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_AMBIENT) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_DIFFUSE :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_DIFFUSE) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SPECULAR :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_SPECULAR) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SHININESS :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_SHININESS) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_TRANSPARENCY :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_TRANSPARENCY) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_XPFALL :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_XPFALL) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_USE_XPFALL :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_USE_XPFALL) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_REFBLUR :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_REFBLUR) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_USE_REFBLUR :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_USE_REFBLUR) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SELF_ILLUM :
                    if (parent->id_tag != MAT_ENTRY
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_TWO_SIDE :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_TWO_SIDE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_DECAL :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_DECAL) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_ADDITIVE :
                    if (parent->id_tag != MAT_ENTRY
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SHADING :
                    if (parent->id_tag == MAT_ENTRY
                          && (parent->flags & GOT_MAT_SHADING) == 0) {
                        br_int_16 shading;

                        if (ReadInt16(stream,&shading)) {
#if TEST_WIREFRAME
                            shading -= 1;
#endif
                            top->done += sizeof(br_int_16);
                            switch (shading) {
                                case 0  : top->data.mat_shading = WIREFRAME;
                                          break;
                                case 1  : top->data.mat_shading = FLAT;
                                          break;
                                default : top->data.mat_shading = GOURAUD;
                                          break;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_TEXMAP :
                    InitialisePixmapRef(&(top->data.pixmap_ref));
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_TEXMAP) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SXP_TEXT_DATA :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_SXP_TEXT_DATA) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_OPACMAP :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_OPACMAP) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SXP_OPAC_DATA :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_SXP_OPAC_DATA) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_REFLMAP :
                    InitialisePixmapRef(&(top->data.pixmap_ref));
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_REFLMAP) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_ACUBIC :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_ACUBIC) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_BUMPMAP :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_BUMPMAP) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MAT_SXP_BUMP_DATA :
                    if (parent->id_tag != MAT_ENTRY
                          || (parent->flags & GOT_MAT_SXP_BUMP_DATA) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case NAMED_OBJECT :
                    top->data.named_obj = NULL;
                    if (parent->id_tag == MDATA) {
                        char buffer[12];

                        if (ReadString(stream,top,12,buffer)
                              && LookupNamedObj(buffer,named_objs) == NULL) {
                            top->data.named_obj = AllocateNamedObj(buffer);
                            if (top->data.named_obj == NULL) {
                                state = OUT_OF_MEMORY;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case N_TRI_OBJECT :
                    InitialiseNTriObj(&(top->data.n_tri_obj));
                    if (parent->id_tag == NAMED_OBJECT
                          && (parent->flags & GOT_N_TRI_OBJECT) == 0
                          && (parent->flags & GOT_N_DIRECT_LIGHT) == 0
                          && (parent->flags & GOT_N_CAMERA) == 0) {

#if REPORT_MESSAGES
						if(options->report != NULL) {
							ReportMessage(options->report, "Triangle mesh: '%s'\n", parent->data.named_obj->name);
						}
#endif

                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case POINT_ARRAY :
                    InitialisePointArray(&(top->data.point_array));
                    if (parent->id_tag == N_TRI_OBJECT
                          && (parent->flags & GOT_POINT_ARRAY) == 0) {
                        state = ReadPointArray(stream,top,options);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case POINT_FLAG_ARRAY :
                    if (parent->id_tag != N_TRI_OBJECT
                          || (parent->flags & GOT_POINT_FLAG_ARRAY) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case FACE_ARRAY :
                    InitialiseFaceArray(&(top->data.face_array));
                    if (parent->id_tag == N_TRI_OBJECT
                          && (parent->flags & GOT_FACE_ARRAY) == 0) {
                        state = ReadFaceArray(stream,top);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MSH_MAT_GROUP :
                    top->data.msh_mat_group = NULL;
                    if (parent->id_tag == FACE_ARRAY) {
                        state = ReadMshMatGroup(stream,top,material_list);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case SMOOTH_GROUP :
                    top->data.smooth_group = NULL;
                    if (parent->id_tag == FACE_ARRAY
                          && (parent->flags & GOT_SMOOTH_GROUP) == 0) {
                        state = ReadSmoothGroup(stream,top,
                                          parent->data.face_array.n_faces,
                                          options);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case TEX_VERTS :
                    InitialiseTexVerts(&(top->data.tex_verts));
                    if (parent->id_tag == N_TRI_OBJECT
                          && (parent->flags & GOT_TEX_VERTS) == 0) {
                        state = ReadTexVerts(stream,top);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MESH_MATRIX :
                    if (parent->id_tag == N_TRI_OBJECT
                          && (parent->flags & GOT_MESH_MATRIX) == 0) {
                        state = ReadMeshMatrix(stream,top,options);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MESH_TEXTURE_INFO :
                    if (parent->id_tag != N_TRI_OBJECT
                          || (parent->flags & GOT_MESH_TEXTURE_INFO) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case PROC_NAME :
                    if (parent->id_tag != N_TRI_OBJECT
                          || (parent->flags & GOT_PROC_NAME) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case PROC_DATA :
                    if (parent->id_tag != N_TRI_OBJECT
                          || (parent->flags & GOT_PROC_DATA) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case N_DIRECT_LIGHT :
                    if (parent->id_tag == NAMED_OBJECT
                          && (parent->flags & GOT_N_TRI_OBJECT) == 0
                          && (parent->flags & GOT_N_DIRECT_LIGHT) == 0
                          && (parent->flags & GOT_N_CAMERA) == 0) {
                        state = ReadNDLight(stream,top,options);

                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case DL_OFF :
                    if (parent->id_tag == N_DIRECT_LIGHT
                          && (parent->flags & GOT_DL_OFF) == 0
                          && SkipRest(stream,top)) {

#if REPORT_MESSAGES
						if(options->report != NULL) {
							ReportMessage(options->report, "  light is turned off\n");
						}
#endif

                    } else {
                       state = PARSE_ERROR;
                    }
                    break;

                case DL_SPOTLIGHT :
                    if (parent->id_tag == N_DIRECT_LIGHT
                          && (parent->flags & GOT_DL_SPOTLIGHT) == 0) {
                        state = ReadDlSpotlight(stream,top,options);

                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case DL_SHADOWED :         /*  fall through .....  */
                case DL_LOCAL_SHADOW2 :
                case DL_SEE_CONE :         /*  ..... until here    */
                    state = PARSE_ERROR;
                    break;

                case N_CAMERA :
                    if (parent->id_tag == NAMED_OBJECT
                          && (parent->flags & GOT_N_TRI_OBJECT) == 0
                          && (parent->flags & GOT_N_DIRECT_LIGHT) == 0
                          && (parent->flags & GOT_N_CAMERA) == 0) {
                        state = ReadNCamera(stream,top,options);

                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case CAM_SEE_CONE :
                    state = PARSE_ERROR;
                    break;

                case OBJ_HIDDEN :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_HIDDEN) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_VIS_LOFTER :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_VIS_LOFTER) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_DOESNT_CAST :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_DOESNT_CAST) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_MATTE :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_MATTE) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_FAST :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_FAST) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_PROCEDURAL :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_PROCEDURAL) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJ_FROZEN :
                    if (parent->id_tag != NAMED_OBJECT
                          || (parent->flags & GOT_OBJ_FROZEN) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case KFDATA :
                    if (parent->id_tag != M3DMAGIC
                          || (parent->flags & GOT_KFDATA) != 0) {
                        state = PARSE_ERROR;
                    }
                    break;

                case KFHDR :
                    if (parent->id_tag == KFDATA
                          && (parent->flags & GOT_KFHDR) == 0) {
                        br_int_16 version;

                        if (ReadInt16(stream,&version)) {
                            top->done += sizeof(br_int_16);

#if REPORT_MESSAGES
							if(options->report != NULL) {
								if((version == 1) || (version == 2)) {
									ReportMessage(options->report, "Keyframe data version %d.0\n", version);
								} else {
									ReportMessage(options->report,
										"Keyframe data version %d.0 (only able to read version 1.0 and 2.0 chunks)\n", version); 
								}
							}
#endif

                            if (!SkipRest(stream,top)) {
                                state = PARSE_ERROR;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case KFSEG :
                    if (parent->id_tag != KFDATA
                          || (parent->flags & GOT_KFSEG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case KFCURTIME :
                    if (parent->id_tag != KFDATA
                          || (parent->flags & GOT_KFCURTIME) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case OBJECT_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag == KFDATA) {
                        top->data.node_tag = AllocateNodeTag();
                        if (top->data.node_tag == NULL) {
                            state = OUT_OF_MEMORY;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case CAMERA_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag == KFDATA) {
                        top->data.node_tag = AllocateNodeTag();
                        if (top->data.node_tag == NULL) {
                            state = OUT_OF_MEMORY;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case TARGET_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag != KFDATA
                         || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case LIGHT_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag == KFDATA) {
                        top->data.node_tag = AllocateNodeTag();
                        if (top->data.node_tag == NULL) {
                            state = OUT_OF_MEMORY;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case SPOTLIGHT_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag == KFDATA) {
                        top->data.node_tag = AllocateNodeTag();
                        if (top->data.node_tag == NULL) {
                            state = OUT_OF_MEMORY;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case L_TARGET_NODE_TAG :
                    top->data.node_tag = NULL;
                    if (parent->id_tag != KFDATA
                         || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case NODE_HDR :
                    InitialiseNodeHdr(&(top->data.node_hdr));
                    if ((parent->id_tag == OBJECT_NODE_TAG
                            || parent->id_tag == CAMERA_NODE_TAG
                            || parent->id_tag == LIGHT_NODE_TAG
                            || parent->id_tag == SPOTLIGHT_NODE_TAG)
                          && (parent->flags & GOT_NODE_HDR) == 0) {
                        state = ReadNodeHdr(stream,top,n_node_tags,named_objs);
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case PIVOT :
                    if (parent->id_tag == OBJECT_NODE_TAG
                          && (parent->flags & GOT_PIVOT) == 0) {
                        if (ReadPoint(stream,top,&(top->data.pivot), options)) {

#if SHOW_KEYFRAME_DATA
#if REPORT_MESSAGES
							if(options->report != NULL) {
								ReportMessage(options->report, "  PIVOT: (%f %f %f)\n",
									BrScalarToFloat(top->data.pivot.v[0]),
									BrScalarToFloat(top->data.pivot.v[1]),
									BrScalarToFloat(top->data.pivot.v[2]));
							}
#endif
#endif

                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case INSTANCE_NAME :
                    top->data.string = NULL;
                    if (parent->id_tag == OBJECT_NODE_TAG
                          && (parent->flags & GOT_INSTANCE_NAME) == 0) {
                        char buffer[12];

                        if (ReadString(stream,top,12,buffer)) {

#if SHOW_KEYFRAME_DATA
#if REPORT_MESSAGES
							if(options->report != NULL) {
								ReportMessage(options->report, "  INSTANCE NAME: '%s'\n", buffer);
							}
#endif
#endif

                            if (parent->data.named_obj != NULL) {
                                top->data.string = BrMemStrDup(buffer);
                                if (top->data.string == NULL) {
                                    state = OUT_OF_MEMORY;
                                }
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case MORPH_SMOOTH :
                    if (parent->id_tag != OBJECT_NODE_TAG
                          || (parent->flags & GOT_MORPH_SMOOTH) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case BOUNDBOX :
                    if (parent->id_tag == OBJECT_NODE_TAG
                          && (parent->flags & GOT_BOUNDBOX) == 0) {
                        br_vector3 *min;
                        br_vector3 *max;

                        min = &(top->data.boundbox.min);
                        max = &(top->data.boundbox.max);

                        if (ReadPoint(stream,top,min, options)
                              && ReadPoint(stream,top,max, options)) {

#if SHOW_KEYFRAME_DATA
#if REPORT_MESSAGES
							if(options->report != NULL) {
								ReportMessage(options->report, "  BOUNDBOX:\n"
															   "    min: (%f %f %f)\n"
															   "    max: (%f %f %f)\n",
									BrScalarToFloat(min->v[0]),
									BrScalarToFloat(min->v[1]),
									BrScalarToFloat(min->v[2]),
									BrScalarToFloat(max->v[0]),
									BrScalarToFloat(max->v[1]),
									BrScalarToFloat(max->v[2]));
							}
#endif
#endif

                        } else {
                            state = PARSE_ERROR;
                        }
                    } else {
                        state = PARSE_ERROR;
                    }
                    break;

                case POS_TRACK_TAG :
                    if ((parent->id_tag != OBJECT_NODE_TAG
                            && parent->id_tag == CAMERA_NODE_TAG
                            && parent->id_tag == LIGHT_NODE_TAG
                            && parent->id_tag == SPOTLIGHT_NODE_TAG)
                          || (parent->flags & GOT_POS_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case COL_TRACK_TAG :
                    if ((parent->id_tag != SPOTLIGHT_NODE_TAG
                             && parent->id_tag != LIGHT_NODE_TAG)
                          || (parent->flags & GOT_COL_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case ROT_TRACK_TAG :
                    if (parent->id_tag != OBJECT_NODE_TAG
                          || (parent->flags & GOT_ROT_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case SCL_TRACK_TAG :
                    if (parent->id_tag != OBJECT_NODE_TAG
                          || (parent->flags & GOT_SCL_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case MORPH_TRACK_TAG :
                    if (parent->id_tag != OBJECT_NODE_TAG
                          || (parent->flags & GOT_MORPH_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case FOV_TRACK_TAG :
                    if (parent->id_tag != CAMERA_NODE_TAG
                          || (parent->flags & GOT_FOV_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case ROLL_TRACK_TAG :
                    if ((parent->id_tag != CAMERA_NODE_TAG
                             && parent->id_tag != SPOTLIGHT_NODE_TAG)
                          || (parent->flags & GOT_ROLL_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case HOT_TRACK_TAG :
                    if (parent->id_tag != SPOTLIGHT_NODE_TAG
                          || (parent->flags & GOT_HOT_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                case FALL_TRACK_TAG :
                    if (parent->id_tag != SPOTLIGHT_NODE_TAG
                          || (parent->flags & GOT_FALL_TRACK_TAG) != 0
                          || !SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

                default :
					/*
					 * Skip unidentified chunks
					 */
                    if (!SkipRest(stream,top)) {
                        state = PARSE_ERROR;
                    }
                    break;

			}

		}

		while((top >= stack) && ((state != OK) || (top->done == top->length))) {

			/*
			 * Switch on chunk ID tag
			 */
			switch(top->id_tag) {

                case COLOR_F :
                    if (state == OK) {
                        if (parent->id_tag == N_DIRECT_LIGHT) {
                            BlockCopy(&(parent->data.n_d_light.color),
                                      &(top->data.color),
                                      sizeof(Color_t));
                        } else {
                            BlockCopy(&(parent->data.color),
                                      &(top->data.color),
                                      sizeof(Color_t));
                        }
                        parent->flags |= GOT_COLOR_F;
                    }
                    break;
                case COLOR_24 :
                    if (state == OK) {
                        if (parent->id_tag == N_DIRECT_LIGHT) {
                            BlockCopy(&(parent->data.n_d_light.color),
                                      &(top->data.color),
                                      sizeof(Color_t));
                        } else {
                            BlockCopy(&(parent->data.color),
                                      &(top->data.color),
                                      sizeof(Color_t));
                        }
                        parent->flags |= GOT_COLOR_24;
                    }
                    break;

                case INT_PERCENTAGE :
                    if (state == OK) {
                        if (parent->id_tag == MAT_TEXMAP
                              || parent->id_tag == MAT_REFLMAP) {
                            parent->data.pixmap_ref.strength
                                 = top->data.percent;
                        } else {
                            parent->data.percent = top->data.percent;
                        }
                        parent->flags |= GOT_INT_PERCENTAGE;
                    }
                    break;

                case FLOAT_PERCENTAGE :
                    if (state == OK) {
                        if (parent->id_tag == MAT_TEXMAP
                              || parent->id_tag == MAT_REFLMAP) {
                            parent->data.pixmap_ref.strength
                                 = top->data.percent;
                        } else {
                            parent->data.percent = top->data.percent;
                        }
                        parent->flags |= GOT_FLOAT_PERCENTAGE;
                    }
                    break;

                case MAT_MAPNAME :
                    if (state == OK) {
                        parent->data.pixmap_ref.mat_mapname = top->data.string;
                        parent->flags |= GOT_MAT_MAPNAME;
                    }
                    break;

                case M3DMAGIC :
                    break;

                case M3D_VERSION :
                    if (state == OK) {
                        parent->flags |= GOT_M3D_VERSION;
                    }
                    break;

                case MDATA :
                    if (state == OK) {
                        parent->flags |= GOT_MDATA;
                    }
                    break;

                case MESH_VERSION :
                    if (state == OK) {
                        parent->flags |= GOT_MESH_VERSION;
                    }
                    break;

                case MASTER_SCALE :
                    if (state == OK) {
                        parent->flags |= GOT_MASTER_SCALE;
                    }
                    break;

                case LO_SHADOW_BIAS :
                    if (state == OK) {
                        parent->flags |= GOT_LO_SHADOW_BIAS;
                    }
                    break;

                case HI_SHADOW_BIAS :
                    if (state == OK) {
                        parent->flags |= GOT_HI_SHADOW_BIAS;
                    }
                    break;

                case SHADOW_MAP_SIZE :
                    if (state == OK) {
                        parent->flags |= GOT_SHADOW_MAP_SIZE;
                    }
                    break;

                case SHADOW_SAMPLES :
                    if (state == OK) {
                        parent->flags |= GOT_SHADOW_SAMPLES;
                    }
                    break;

                case SHADOW_RANGE :
                    if (state == OK) {
                        parent->flags |= GOT_SHADOW_RANGE;
                    }
                    break;

                case SHADOW_FILTER :
                    if (state == OK) {
                        parent->flags |= GOT_SHADOW_FILTER;
                    }
                    break;

                case AMBIENT_LIGHT :
                    if (state == OK) {
                        parent->flags |= GOT_AMBIENT_LIGHT;
                    }
                    break;

                case O_CONSTS :
                    if (state == OK) {
                        parent->flags |= GOT_O_CONSTS;
                    }
                    break;

                case BIT_MAP :
                    if (state == OK) {
                        parent->flags |= GOT_BIT_MAP;
                    }
                    break;

                case SOLID_BGND :
                    if (state == OK) {
                        parent->flags |= GOT_SOLID_BGND;
                    }
                    break;

                case V_GRADIENT :
                    if (state == OK) {
                        parent->flags |= GOT_V_GRADIENT;
                    }
                    break;

                case USE_BIT_MAP :
                    if (state == OK) {
                        parent->flags |= GOT_USE_BIT_MAP;
                    }
                    break;

                case USE_SOLID_BGND :
                    if (state == OK) {
                        parent->flags |= GOT_USE_SOLID_BGND;
                    }
                    break;

                case USE_V_GRADIENT :
                    if (state == OK) {
                        parent->flags |= GOT_USE_V_GRADIENT;
                    }
                    break;

                case FOG :
                    if (state == OK) {
                        parent->flags |= GOT_FOG;
                    }
                    break;

                case FOG_BGND :
                    break;

                case DISTANCE_CUE :
                    if (state == OK) {
                        parent->flags |= GOT_DISTANCE_CUE;
                    }
                    break;

                case DCUE_BGND :
                    break;

                case USE_FOG :
                    if (state == OK) {
                        parent->flags |= GOT_USE_FOG;
                    }
                    break;

                case USE_DISTANCE_CUE :
                    if (state == OK) {
                        parent->flags |= GOT_USE_DISTANCE_CUE;
                    }
                    break;

                case DEFAULT_VIEW :
                    if (state == OK) {
                        parent->flags |= GOT_DEFAULT_VIEW;
                    }
                    break;

                case VIEW_TOP :               /*  fall through .....  */
                case VIEW_BOTTOM :
                case VIEW_LEFT :
                case VIEW_RIGHT :
                case VIEW_FRONT :
                case VIEW_BACK :
                case VIEW_USER :
                case VIEW_CAMERA :            /*  ..... until here    */
                    break;

                case MAT_ENTRY :
                    if (state == OK) {
                        if (top->flags & GOT_MAT_NAME) {
                            MaterialList_t *material_link;

                            material_link = ConvertMaterial(
                                                       &(top->data.mat_entry),
                                                       &pixmap_list,options);

                            if (material_link != NULL) {
                                material_link->next = material_list;
                                material_list       = material_link;
                            } else {
                                state = OUT_OF_MEMORY;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    DismantleMatEntry(&(top->data.mat_entry));
                    break;

                case MAT_NAME :
                    if (state == OK) {
                        parent->data.mat_entry.mat_name = top->data.string;
                        parent->flags |= GOT_MAT_NAME;
                    }
                    break;

                case MAT_AMBIENT :
                    if (state == OK) {
                        if ((top->flags & GOT_COLOR_F) != 0
                              || (top->flags & GOT_COLOR_24) != 0) {
                            BlockCopy(&(parent->data.mat_entry.mat_ambient),
                                      &(top->data.color),
                                      sizeof(Color_t));
                            parent->flags |= GOT_MAT_AMBIENT;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case MAT_DIFFUSE :
                    if (state == OK) {
                        if ((top->flags & GOT_COLOR_F) != 0
                              || (top->flags & GOT_COLOR_24) != 0) {
                            BlockCopy(&(parent->data.mat_entry.mat_diffuse),
                                      &(top->data.color),
                                      sizeof(Color_t));
                            parent->flags |= GOT_MAT_DIFFUSE;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case MAT_SPECULAR :
                    if (state == OK) {
                        if ((top->flags & GOT_COLOR_F) != 0
                              || (top->flags & GOT_COLOR_24) != 0) {
                            BlockCopy(&(parent->data.mat_entry.mat_specular),
                                      &(top->data.color),
                                      sizeof(Color_t));
                            parent->flags |= GOT_MAT_SPECULAR;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case MAT_SHININESS :
                    if (state == OK) {
                        if ((top->flags & GOT_FLOAT_PERCENTAGE) != 0
                              || (top->flags & GOT_INT_PERCENTAGE) != 0) {
                            parent->data.mat_entry.mat_shininess =
                                 top->data.percent;
                            parent->flags |= GOT_MAT_SHININESS;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case MAT_TRANSPARENCY :
                    if (state == OK) {
                        if ((top->flags & GOT_FLOAT_PERCENTAGE) != 0
                              || (top->flags & GOT_INT_PERCENTAGE) != 0) {
                            parent->data.mat_entry.mat_transparency
                                 = top->data.percent;
                            parent->flags |= GOT_MAT_TRANSPARENCY;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case MAT_XPFALL :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_XPFALL;
                    }
                    break;

                case MAT_USE_XPFALL :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_USE_XPFALL;
                    }
                    break;

                case MAT_REFBLUR :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_REFBLUR;
                    }
                    break;

                case MAT_USE_REFBLUR :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_USE_REFBLUR;
                    }
                    break;

                case MAT_SELF_ILLUM :
                    break;

                case MAT_TWO_SIDE :
                    if (state == OK) {
                        parent->data.mat_entry.mat_two_side = TRUE;
                        parent->flags |= GOT_MAT_TWO_SIDE;
                    }
                    break;

                case MAT_DECAL :
                    if (state == OK) {
                        parent->data.mat_entry.mat_decal = TRUE;
                        parent->flags |= GOT_MAT_DECAL;
                    }
                    break;

                case MAT_ADDITIVE :
                    break;

                case MAT_SHADING :
                    if (state == OK) {
                        parent->data.mat_entry.mat_shading
                                   = top->data.mat_shading;
                        parent->flags |= GOT_MAT_SHADING;
                    }
                    break;

                case MAT_TEXMAP :
                    if (state == OK) {
                        if ((top->flags & GOT_MAT_MAPNAME) != 0
                             && ((top->flags & GOT_FLOAT_PERCENTAGE) != 0
                                  || (top->flags & GOT_INT_PERCENTAGE) != 0)) {
                            UpdatePixmapRef(
                                     &(parent->data.mat_entry.pixmap_ref),
                                     &(top->data.pixmap_ref));

                            parent->flags |= GOT_MAT_TEXMAP;

                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK) {
                        DismantlePixmapRef(&(top->data.pixmap_ref));
                    }
                    break;

                case MAT_SXP_TEXT_DATA :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_SXP_TEXT_DATA;
                    }
                    break;

                case MAT_OPACMAP :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_OPACMAP;
                    }
                    break;

                case MAT_SXP_OPAC_DATA :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_SXP_OPAC_DATA;
                    }
                    break;

                case MAT_REFLMAP :
                    if (state == OK) {
                        if ((top->flags & GOT_MAT_MAPNAME) != 0
                             && ((top->flags & GOT_FLOAT_PERCENTAGE) != 0
                                  || (top->flags & GOT_INT_PERCENTAGE) != 0)) {
							top->data.pixmap_ref.is_reflection_map = TRUE;
							UpdatePixmapRef(
								&(parent->data.mat_entry.pixmap_ref),
								&(top->data.pixmap_ref));
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK) {
                        DismantlePixmapRef(&(top->data.pixmap_ref));
                    }
                    break;

                case MAT_ACUBIC :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_ACUBIC;
                    }
                    break;

                case MAT_BUMPMAP :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_BUMPMAP;
                    }
                    break;

                case MAT_SXP_BUMP_DATA :
                    if (state == OK) {
                        parent->flags |= GOT_MAT_SXP_BUMP_DATA;
                    }
                    break;

                case NAMED_OBJECT :
                    if (state == OK) {
                        if ((top->flags & GOT_N_CAMERA) != 0
                              || (top->flags & GOT_N_DIRECT_LIGHT) != 0
                              || (top->flags & GOT_N_TRI_OBJECT) != 0) {

                            top->data.named_obj->next = named_objs;
                            named_objs = top->data.named_obj;

                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK) {
                        DeallocateNamedObjList(top->data.named_obj);
                    }
                    break;

                case N_TRI_OBJECT :
                    if (state == OK) {
                        if ((top->flags & GOT_POINT_ARRAY) != 0
                               && (top->flags & GOT_FACE_ARRAY) != 0) {
                            state = ConvertNTriObj(&(top->data.n_tri_obj),
                                                   parent->data.named_obj,
                                                   options);
                            if (state == OK) {
                                parent->flags |= GOT_N_TRI_OBJECT;
                            }
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    DismantleNTriObj(&(top->data.n_tri_obj));
                    break;

                case POINT_ARRAY :
                    if (state == OK) {

#if REPORT_MESSAGES
						if(options->report != NULL) {
							ReportMessage(options->report, "  %d vertices\n", top->data.point_array.n_vertices);
						}
#endif

                        BlockCopy(&(parent->data.n_tri_obj.point_array),
                                  &(top->data.point_array),
                                  sizeof(PointArray_t));
                        parent->flags |= GOT_POINT_ARRAY;
                    } else {
                        DismantlePointArray(&(top->data.point_array));
                    }
                    break;

                case POINT_FLAG_ARRAY :
                    if (state == OK) {
                        parent->flags |= GOT_POINT_FLAG_ARRAY;
                    }
                    break;

                case FACE_ARRAY :
                    if (state == OK) {

#if REPORT_MESSAGES
						if(options->report != NULL) {
							ReportMessage(options->report, "  %d faces\n", top->data.face_array.n_faces);
						}
#endif

                        BlockCopy(&(parent->data.n_tri_obj.face_array),
                                  &(top->data.face_array),
                                  sizeof(FaceArray_t));
                        parent->flags |= GOT_FACE_ARRAY;
                    } else {
                        DismantleFaceArray(&(top->data.face_array));
                    }
                    break;

                case MSH_MAT_GROUP :
                    if (state == OK) {
                        top->data.msh_mat_group->next
                             = parent->data.face_array.msh_mat_groups;
                        parent->data.face_array.msh_mat_groups
                             = top->data.msh_mat_group;
                    } else {
                        DeallocateMshMatGroups(top->data.msh_mat_group);
                    }
                    break;

                case SMOOTH_GROUP :
                    if (state == OK) {
                        parent->data.face_array.smooth_group
                             = top->data.smooth_group;
                        parent->flags |= GOT_SMOOTH_GROUP;
                    } else if (top->data.smooth_group != NULL) {
                        BrMemFree(top->data.smooth_group);
                    }
                    break;

                case TEX_VERTS :
                    if (state == OK) {

#if REPORT_MESSAGES
						if(options->report != NULL) {
							ReportMessage(options->report, "  %d texture vertices\n", top->data.tex_verts.n_texverts);
						}
#endif

                        BlockCopy(&(parent->data.n_tri_obj.tex_verts),
                                  &(top->data.tex_verts),
                                  sizeof(TexVerts_t));
                        parent->flags |= GOT_TEX_VERTS;
                    } else {
                        DismantleTexVerts(&(top->data.tex_verts));
                    }
                    break;

                case MESH_MATRIX :
                    if (state == OK) {
                        BrMatrix34Copy(&(parent->data.n_tri_obj.mesh_matrix),
                                       &(top->data.mesh_matrix));
                        parent->flags |= GOT_MESH_MATRIX;
                    }
                    break;
  
                case MESH_TEXTURE_INFO :
                    if (state == OK) {
                        parent->flags |= GOT_MESH_TEXTURE_INFO;
                    }
                    break;

                case PROC_NAME :
                    if (state == OK) {
                        parent->flags |= GOT_PROC_NAME;
                    }
                    break;

                case PROC_DATA :
                    if (state == OK) {
                        parent->flags |= GOT_PROC_DATA;
                    }
                    break;

                case N_DIRECT_LIGHT :
                    if (state == OK) {
                        if ((top->flags & GOT_COLOR_F) != 0
                               || (top->flags & GOT_COLOR_24) != 0) {
                            ConvertNDLight(&(top->data.n_d_light),
                                           parent->data.named_obj,
                                           options);
                            parent->flags |= GOT_N_DIRECT_LIGHT;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    break;

                case DL_OFF :
                    if (state == OK) {
                        parent->data.n_d_light.is_off = TRUE;

                        parent->flags |= GOT_DL_OFF;
                    }
                    break;

                case DL_SPOTLIGHT :
                    if (state == OK) {
                        parent->data.n_d_light.is_spotlight = TRUE;
                        BlockCopy(&(parent->data.n_d_light.dl_spotlight),
                                  &(top->data.dl_spotlight),
                                  sizeof(DlSpotlight_t));

                        parent->flags |= GOT_DL_SPOTLIGHT;
                    }
                    break;

                case DL_SHADOWED :         /*  fall through .....  */
                case DL_LOCAL_SHADOW2 :
                case DL_SEE_CONE :         /*  ..... until here    */
                    break;

                case N_CAMERA :
                    if (state == OK) {
                        ConvertNCamera(&(top->data.n_camera),
                                       parent->data.named_obj,
                                       options);
                        parent->flags |= GOT_N_CAMERA;
                    }
                    break;

                case CAM_SEE_CONE :
                    state = PARSE_ERROR;
                    break;

                case OBJ_HIDDEN :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_HIDDEN;
                    }
                    break;

                case OBJ_VIS_LOFTER :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_VIS_LOFTER;
                    }
                    break;

                case OBJ_DOESNT_CAST :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_DOESNT_CAST;
                    }
                    break;

                case OBJ_MATTE :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_MATTE;
                    }
                    break;

                case OBJ_FAST :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_FAST;
                    }
                    break;

                case OBJ_PROCEDURAL :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_PROCEDURAL;
                    }
                    break;

                case OBJ_FROZEN :
                    if (state == OK) {
                        parent->flags |= GOT_OBJ_FROZEN;
                    }
                    break;

                case KFDATA :
                    if (state == OK) {
                        if ((top->flags & GOT_KFHDR) != 0) {
                             parent->flags |= GOT_KFDATA;
                        } else {
                             state = PARSE_ERROR;
                        }
                    }
                    break;

                case KFHDR :
                    if (state == OK) {
                        parent->flags |= GOT_KFHDR;
                    }
                    break;

                case KFSEG :
                    if (state == OK) {
                        parent->flags |= GOT_KFSEG;
                    }
                    break;

                case KFCURTIME :
                    if (state == OK) {
                        parent->flags |= GOT_KFCURTIME;
                    }
                    break;

                case OBJECT_NODE_TAG :
                    if (state == OK) {
                        if ((top->flags & GOT_NODE_HDR) != 0
                              && (top->flags & GOT_PIVOT) != 0
                              && (top->flags & GOT_POS_TRACK_TAG) != 0
                              && (top->flags & GOT_ROT_TRACK_TAG) != 0
                              && (top->flags & GOT_SCL_TRACK_TAG) != 0
                              && InsertNodeTag(top->data.node_tag,
                                               &node_tags)) {
                            n_node_tags += 1;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK && top->data.node_tag != NULL) {
                        DeallocateNodeTags(top->data.node_tag);
                    }
                    break;

                case CAMERA_NODE_TAG :
                    if (state == OK) {
                        if ((top->flags & GOT_NODE_HDR) != 0
                              && (top->flags & GOT_POS_TRACK_TAG) != 0
                              && (top->flags & GOT_FOV_TRACK_TAG) != 0
                              && (top->flags & GOT_ROLL_TRACK_TAG) != 0
                              && InsertNodeTag(top->data.node_tag,
                                               &node_tags)) {
                            n_node_tags += 1;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK && top->data.node_tag != NULL) {
                        DeallocateNodeTags(top->data.node_tag);
                    }
                    break;

                case TARGET_NODE_TAG :
                    if (state == OK) {
                        n_node_tags += 1;
                    }
                    break;

                case LIGHT_NODE_TAG :
                    if (state == OK) {
                        if ((top->flags & GOT_NODE_HDR) != 0
                              && (top->flags & GOT_POS_TRACK_TAG) != 0
                              && (top->flags & GOT_COL_TRACK_TAG) != 0
                              && InsertNodeTag(top->data.node_tag,
                                               &node_tags)) {
                            n_node_tags += 1;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK && top->data.node_tag != NULL) {
                        DeallocateNodeTags(top->data.node_tag);
                    }
                    break;

                case SPOTLIGHT_NODE_TAG :
                    if (state == OK) {
                        if ((top->flags & GOT_NODE_HDR) != 0
                              && (top->flags & GOT_POS_TRACK_TAG) != 0
                              && (top->flags & GOT_COL_TRACK_TAG) != 0
                              && (top->flags & GOT_HOT_TRACK_TAG) != 0
                              && (top->flags & GOT_FALL_TRACK_TAG) != 0
                              && InsertNodeTag(top->data.node_tag,
                                               &node_tags)) {
                            n_node_tags += 1;
                        } else {
                            state = PARSE_ERROR;
                        }
                    }
                    if (state != OK && top->data.node_tag != NULL) {
                        DeallocateNodeTags(top->data.node_tag);
                    }
                    break;

                case L_TARGET_NODE_TAG :
                    if (state == OK) {
                        n_node_tags += 1;
                    }
                    break;

                case NODE_HDR :
                    if (state == OK) {
                        BlockCopy(&(parent->data.node_tag->node_hdr),
                                  &(top->data.node_hdr),
                                  sizeof(NodeHdr_t));

                        parent->flags |= GOT_NODE_HDR;
                    }
                    break;

                case PIVOT :
                    if (state == OK) {
                        parent->data.node_tag->has_pivot = TRUE;
                        BrVector3Copy(&(parent->data.node_tag->pivot),
                                      &(top->data.pivot));

                        parent->flags |= GOT_PIVOT;
                    }
                    break;

                case INSTANCE_NAME :
                    if (state == OK) {
                        parent->data.node_tag->instance_name
                                         = top->data.string;

                        parent->flags |= GOT_INSTANCE_NAME;
                    }
                    if (state != OK && top->data.string != NULL) {
                        BrMemFree(top->data.string);
                    }
                    break;

                case MORPH_SMOOTH :
                    if (state == OK) {
                        parent->flags |= GOT_MORPH_SMOOTH;
                    }
                    break;

                case BOUNDBOX :
                    if (state == OK) {
                        parent->data.node_tag->has_boundbox = TRUE;
                        BrVector3Copy(&(parent->data.node_tag->boundbox.min),
                                      &(top->data.boundbox.min));
                        BrVector3Copy(&(parent->data.node_tag->boundbox.max),
                                      &(top->data.boundbox.max));

                        parent->flags |= GOT_BOUNDBOX;
                    }
                    break;

                case POS_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_POS_TRACK_TAG;
                    }
                    break;

                case COL_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_COL_TRACK_TAG;
                    }
                    break;

                case ROT_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_ROT_TRACK_TAG;
                    }
                    break;

                case SCL_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_SCL_TRACK_TAG;
                    }
                    break;

                case MORPH_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_MORPH_TRACK_TAG;
                    }
                    break;

                case FOV_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_FOV_TRACK_TAG;
                    }
                    break;

                case ROLL_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_ROLL_TRACK_TAG;
                    }
                    break;

                case HOT_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_HOT_TRACK_TAG;
                    }
                    break;

                case FALL_TRACK_TAG :
                    if (state == OK) {
                        parent->flags |= GOT_FALL_TRACK_TAG;
                    }
                    break;

                default :
                    break;

			}

			/*
			 * Pop chunk
			 */
			parent->done += top->done;
			top = parent--;

		}

	}

	/*
	 * Close file
	 */
	BrFileClose(stream);

	/*
	 * Check for successful parse
	 */
	if(state == PARSE_ERROR) {

#if REPORT_MESSAGES
		if(options->report != NULL) {
			ReportMessage(options->report, "File '%s' is corrupt\n", name);
		}
#endif

		return NULL;
	}

	/*
	 * Allocate and initialise results
	 */
	results = BrResAllocate(NULL, sizeof(br_fmt_results), BR_MEMORY_FMT_RESULTS);
	results->nactors = 0;
	results->nmodels = 0;
	results->nlights = 0;
	results->ncameras = 0;
	results->nclip_planes = 0;
	results->nbounds = 0;
	results->nbounds_correct = 0;
	results->nmaterials = 0;
	results->npixelmaps = 0;

	/*
	 * Actors, cameras and lights
	 */
	if(fmt_options->convert_flags & (BR_FMT_ACTORS | BR_FMT_CAMERAS | BR_FMT_LIGHTS)) {

		/*
		 * Generate actor hierarchy
		 */
		if((n_node_tags == 0) || (options->flags & BR_3DS_FLAT_HIERARCHY)) {
			actor = BuildFlatHierarchy(named_objs, options);
		} else {
			actor = BuildComplexHierarchy(node_tags, options);
		}

		/*
		 * Collect cameras
		 */
		if(fmt_options->convert_flags & BR_FMT_CAMERAS) CollectCameras(results, actor);

		/*
		 * Collect lights
		 */
		if(fmt_options->convert_flags & BR_FMT_LIGHTS) CollectLights(results, actor);

		/*
		 * Collect actors
		 */
		if(fmt_options->convert_flags & BR_FMT_ACTORS) {
			/*
			 * Attach actors in hierarchy to table as resources
			 */
			results->actors = (br_actor **)BrResAllocate(results, sizeof(br_actor *), BR_MEMORY_FMT_RESULTS);
			BrActorEnum(actor, ResAddHierarchy, results);
			BrResAdd(results->actors, actor);
			results->actors[0] = actor;
			results->nactors = 1;

			/*
			 * Scale hierarchy if required
			 */
			if(options->flags & BR_3DS_APPLY_SCALE) ScaleHierarchy(actor, options->scale);

			/*
			 * Clear references to models and materials if not converted
			 */
			if(!(fmt_options->convert_flags & BR_FMT_MODELS)) BrActorEnum(actor, ClearActorModels, NULL);
			if(!(fmt_options->convert_flags & BR_FMT_MATERIALS)) BrActorEnum(actor, ClearActorMaterials, NULL);

		} else {
			/*
			 * Detach camera and light resources from parent actors
			 */
			if(fmt_options->convert_flags & BR_FMT_CAMERAS) BrActorEnum(actor, DetachCameras, results);
			if(fmt_options->convert_flags & BR_FMT_LIGHTS) BrActorEnum(actor, DetachLights, results);

			/*
			 * Free actor hierarchy
			 */
			if(fmt_options->convert_flags & (BR_FMT_CAMERAS | BR_FMT_LIGHTS)) BrActorFree(actor);
		}

	}

	/*
	 * Models
	 */
	if(fmt_options->convert_flags & BR_FMT_MODELS) {
		/*
		 * Scale models if required
		 */
		if(options->flags & BR_3DS_APPLY_SCALE) ScaleModels(named_objs, options->scale);

		/*
		 * Collect models and de-allocate list nodes
		 */
		CollectModels(results, named_objs);
		DeallocateNamedObjListNodes(named_objs);

		/*
		 * Clear references to materials if not converted
		 */
		if(!(fmt_options->convert_flags & BR_FMT_MATERIALS)) ClearModelMaterials(results);

	} else {
		/*
		 * De-allocate models and list nodes
		 */
		DeallocateNamedObjList(named_objs);
	}

	/*
	 * Materials
	 */
	if(fmt_options->convert_flags & BR_FMT_MATERIALS) {
		/*
		 * Collect materials and de-allocate list nodes
		 */
		CollectMaterials(results, material_list);
		DeallocateMaterialListNodes(material_list);

		/*
		 * Clear references to pixelmaps if not converted
		 */
		if(!(fmt_options->convert_flags & BR_FMT_PIXELMAPS)) ClearMaterialPixelmaps(results);

	} else {
		/*
		 * De-allocate materials and list nodes
		 */
		DeallocateMaterialList(material_list);
	}

	/*
	 * Pixelmaps
	 */
	if(fmt_options->convert_flags & BR_FMT_PIXELMAPS) {
		/*
		 * Collect pixelmaps and de-allocate list nodes
		 */
		CollectPixelmaps(results, pixmap_list);
		DeallocatePixmapListNodes(pixmap_list);

	} else {
		/*
		 * De-allocate pixelmaps and list nodes
		 */
		DeallocatePixmapList(pixmap_list);
	}

	/*
	 * De-allocate remaining workspace
	 */
	DeallocateNodeTags(node_tags);

#if REPORT_MESSAGES
	/*
	 * Report successful parse
	 */
	if(options->report !=NULL) {
		ReportMessage(options->report, "File '%s' successfully parsed\n", name);
	}
#endif

	/*
	 * Return a pointer to the results
	 */
	return results;
}

