/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: image.c 1.2 1998/06/23 13:46:15 jon Exp $
 * $Locker: $
 *
 * Loadable image management
 */
#include "fw.h"
#include "host.h"
#include "brassert.h"

BR_RCS_ID("$Id: image.c 1.2 1998/06/23 13:46:15 jon Exp $")

#define ARENA_ALIGN 4096

br_boolean BR_RESIDENT_ENTRY BrImageAdd(br_image *img)
{
	BR_ADDHEAD(&fw.images,img);

	return BR_TRUE;
}

br_boolean BR_RESIDENT_ENTRY BrImageRemove(br_image *img)
{
	BR_REMOVE(img);

	return BR_TRUE;
}

/*
 * Looks for an image in the currently loaded set
 */
br_image * BR_RESIDENT_ENTRY BrImageFind(char *pattern)
{
	char *c;
	br_image *img;

	/*
	 * Remove extension
	 */
	if(c = BrStrRChr(pattern, '.')) {
		if(!BrStrICmp(c, ".dll") || !BrStrICmp(c, ".bdd") || !BrStrICmp(c, ".bed"))
			*c = '\0';
	}

	/*
 	 * Look for image in currently loaded set
	 */
	BR_FOR_LIST(&fw.images,img)
		if(BrNamePatternMatch(pattern, img->identifier))
			return img;

	return NULL;
}

/*
 * Try to buil a br_image via the host image loader
 */
static br_image *imageLoadHost(char *name)
{
	br_image *img;
	void *host_image;

    host_image = HostImageLoad(name);

	if(host_image) {
		img = BrResAllocate(NULL,sizeof(*img), BR_MEMORY_IMAGE);
		img->identifier = BrResStrDup(img,name);
		img->type = BR_IMG_HOST;
		img->type_pointer = host_image;

        return img;
	}

    return NULL;
}

/*
 * Lookup a named image
 */
br_image * BR_RESIDENT_ENTRY BrImageReference(char *name)
{
    char *suffix,*scratch = BrScratchString();
	br_image *img;

#if 0
	br_exception ex_type;
	void *ex_value;

	if(ex_type = BrExceptionCatch(&ex_value)) {
		SET_ERROR(ex_type, ex_value);
		return NULL;
	}
#endif

	/*
	 * See if image is already loaded...
	 */
	img = BrImageFind(name);

	if(img) {
		img->ref_count++;
		return img;
	}

    /*
     * Try to load an image from the filesystem
     * The search order depends on the extension
     *
     * No Suffix:
     *      ImageLoad(xxx.BED) (If eval. version)
     *      ImageLoad(xxx.BDD)
     *      ImageLoadHost(xxx)
     *      ImageLoad(xxx.DLL)
     *
     * .BDD or .BED
     *      ImageLoad(xxx)
     *      ImageLoadHost(xxx)
     *
     * Any other suffix:
     *      ImageLoadHost(xxx)
     *      ImageLoad(xxx)
     */

    for(suffix = name; *suffix && *suffix != '.'; suffix++)
        ;

    if(*suffix == '\0') {
#if EVAL
   		BrStrCpy(scratch, name);
		BrStrCat(scratch, ".BED");
        img = ImageLoad(scratch);
#endif

        if(!img) {
       		BrStrCpy(scratch, name);
    		BrStrCat(scratch, ".BDD");
            img = ImageLoad(scratch);
        }

        if(!img)
            img = imageLoadHost(name);

        if(!img) {
       		BrStrCpy(scratch, name);
    		BrStrCat(scratch, ".DLL");
            img = ImageLoad(scratch);
        }

    } else if(!BrStrICmp(suffix,".bdd") || !BrStrICmp(suffix,".bed")) {
        if(!img)
            img = ImageLoad(name);

        if(!img)
            img = imageLoadHost(name);
    } else{
        if(!img)
            img = imageLoadHost(name);

        if(!img)
            img = ImageLoad(name);
    }

	/*
 	 * Add image to framework resources and current list
	 */
	if(img) {

		/* Splat any extension from the identifier to avoid problems when searching for loaded image.  */

	    for(suffix = img->identifier; *suffix && *suffix != '.'; suffix++);
		if(suffix) *suffix='\0';

		BrResAdd(fw.res, img);
		BrImageAdd(img);

		img->ref_count++;
	}

	return img;
}

/*
 * Find a symbol in an image file
 */
static void * imageLookupName(br_image *img, char *name, br_uint_32 hint)
{
	int c,limit,base;

	/*
	 * See if 'hint' matches
	 */
	if(hint < img->n_names && !BrStrCmp(name, img->names[hint]))
		return img->functions[img->name_ordinals[hint]];

	/*
	 * Binary search on name table	
	 *
	 * XXX
	 */
	limit = img->n_names;
	base = 0;

	while(limit) {
		/*
		 * Compare with halfway point
		 */
		c = BrStrCmp(name, img->names[base+limit/2]);
	
		if(c < 0) {
			/*
			 * Lower
			 */
			limit = limit/2;
		} else if(c > 0) {
			/*
			 * Higher
			 */
			base += limit/2+1;
			limit = limit - (limit/2+1);
		} else {
			/*
			 * Hit
			 */
			return img->functions[img->name_ordinals[base+limit/2]];
		}
	}
		
	/*
	 * No match
	 */
	return NULL;
}

/*
 *Public entry point for looking up symbols
 *
 * Tries
 *  <name>
 *  <name> with any leading underbar removed
 *  '_'<name>
 */
void * BR_RESIDENT_ENTRY BrImageLookupName(br_image *img, char *name, br_uint_32 hint)
{
	char *scratch = BrScratchString();
    void *p;

    /*
     * If this is a host library - just pass the request on
     */
	if(img->type == BR_IMG_HOST ) {
		ASSERT(img->type_pointer);

		return HostImageLookupName(img->type_pointer, name, hint);
	}


    /*
     * Try the name as provided
     */
    p = imageLookupName(img, name, hint);

    if(p)
        return p;

    /*
     * Drop any leading underbar
     */
     if(*name == '_') {

        p = imageLookupName(img, name+1, hint);

        if(p)
            return p;
    }

    /*
     * Try adding a leading underbar
     */
     scratch[0] = '_';
     BrStrCpy(scratch+1,name);

     p = imageLookupName(img, scratch, hint);

     return p;
}

void * BR_RESIDENT_ENTRY BrImageLookupOrdinal(br_image *img, br_uint_32 ordinal)
{
	if(img->type == BR_IMG_HOST) {
		ASSERT(img->type_pointer);

		return HostImageLookupOrdinal(img->type_pointer, ordinal);
	}

	ordinal -= img->ordinal_base;

	/*
	 * Check that ordinal exists
	 */
	if(ordinal > img->n_functions)
		return NULL;

	return img->functions[ordinal];
}

void BR_RESIDENT_ENTRY BrImageDereference(br_image *image)
{
	image->ref_count--;

	if(image->ref_count > 0)
		return;

	switch(image->type) {
	case BR_IMG_RESIDENT:
		/*
		 * Resident images are locked in memory
		 * because they contain pointers into the
		 * program
		 */
		return;

	case BR_IMG_HOST:
		HostImageUnload(image->type_pointer);
		break;

	case BR_IMG_FRAMEWORK:
		break;
	}

	/*
	 * Unlink image
	 */
	BR_REMOVE(image);
	
	/*
	 * Free resource
	 */
	BrResFree(image);
}

void BR_RESIDENT_ENTRY BrImageFree(br_image *image)
{
	int i;

	/*
	 * Dereference all referenced DLLs
	 */
	for(i=0; i < image->n_imports; i++)
		BrImageDereference(image->imports[i]);
}

/*
 * Resource destructor
 */
void BR_CALLBACK _BrImageFree(void *res, br_uint_8 res_class, br_size_t size)
{
	BrImageFree(res);
}
