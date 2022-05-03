/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fwsetup.c 1.2 1998/07/20 21:52:55 jon Exp $
 * $Locker: $
 *
 */
#include "fw.h"
#include "brassert.h"
#include "datafile.h"
#include "syscn_p.h"
#include "brmath.h"


BR_RCS_ID("$Id: fwsetup.c 1.2 1998/07/20 21:52:55 jon Exp $")

/*
 * Global renderer state
 */
br_framework_state BR_ASM_DATA fw;

/*
 * Various bits of default data
 */
extern br_diaghandler * BR_ASM_DATA _BrDefaultDiagHandler;
extern br_filesystem * BR_ASM_DATA _BrDefaultFilesystem;
extern br_allocator * BR_ASM_DATA  _BrDefaultAllocator;
extern br_file_primitives _BrFilePrimsNull;

/*
 * System resource classes
 */
static br_resource_class resourceClasses[] = {
	/*
	 * These two classes have to be initialised by hand
	 */
	{0,	"REGISTRY",			BR_MEMORY_REGISTRY, 		NULL 				},
	{0,	"ANCHOR",			BR_MEMORY_ANCHOR,			NULL 				},

	{0,	"RESOURCE_CLASS",	BR_MEMORY_RESOURCE_CLASS,	NULL 				},
	{0,	"SCRATCH,",			BR_MEMORY_SCRATCH, 			NULL, 			16	},
	{0,	"STRING",			BR_MEMORY_STRING, 			NULL,			1	},
	{0,	"FILE",				BR_MEMORY_FILE, 			_BrFileFree 		},
	{0,	"POOL",				BR_MEMORY_POOL, 			NULL,			16	},
	{0,	"EXCEPTION_HANDLER",BR_MEMORY_EXCEPTION_HANDLER,NULL 				},
	{0,	"TOKEN",			BR_MEMORY_TOKEN,			NULL 				},
	{0,	"TOKEN_MAP",		BR_MEMORY_TOKEN_MAP,		NULL,           8	},
	{0,	"DATAFILE",			BR_MEMORY_DATAFILE,			NULL 				},
	{0,	"LEXER",			BR_MEMORY_LEXER,			NULL 				},
	{0,	"OBJECT_LIST",		BR_MEMORY_OBJECT_LIST,		NULL 				},
	{0,	"OBJECT_LIST_ENTRY",BR_MEMORY_OBJECT_LIST_ENTRY,NULL 				},
	{0,	"APPLICATION",		BR_MEMORY_APPLICATION,		NULL 				},

	{0,	"IMAGE",			BR_MEMORY_IMAGE,			_BrImageFree 		},
	{0,	"IMAGE_ARENA",		BR_MEMORY_IMAGE_ARENA,		NULL 				},
	{0,	"IMAGE_SECTIONS",	BR_MEMORY_IMAGE_SECTIONS,	NULL 				},
	{0,	"IMAGE_NAMES",		BR_MEMORY_IMAGE_NAMES,		NULL				},
	{0,	"OBJECT",			BR_MEMORY_OBJECT,			_BrObjectFree,	16	},
	{0,	"OBJECT_DATA",		BR_MEMORY_OBJECT_DATA,		NULL,			16	},
	{0,	"DRIVER",			BR_MEMORY_DRIVER,			NULL				},
	{0,	"FMT_RESULTS",		BR_MEMORY_FMT_RESULTS,		NULL				},
	{0,	"TOKEN_VALUE",		BR_MEMORY_TOKEN_VALUE,		NULL				},
	{0,	"TOKEN_TEMPLATE",	BR_MEMORY_TOKEN_TEMPLATE,	NULL				},
};

/*
 * Resident image that is used to expose resident framework 
 * functions to loaded devices.
 */
#include "fwimg.h"

br_error BR_PUBLIC_ENTRY BrFwBegin(void)
{
	int i;

	if(fw.active)
		return BRE_ALLREADY_ACTIVE;

	/*
	 * Set handlers to use defaults
	 */
	if(fw.diag == NULL)
		fw.diag = _BrDefaultDiagHandler;

	if(fw.fsys == NULL)
		fw.fsys = _BrDefaultFilesystem;

	if(fw.mem == NULL)
		fw.mem = _BrDefaultAllocator;

	/*
	 * Set up initial state of file writing
	 */
	fw.open_mode = BR_FS_MODE_BINARY;

	/*
	 * Initialise all registries
	 */
	BrRegistryNew(&fw.reg_resource_classes);

	/*
	 * Fake the resource classes that are required to
	 * support the registry
	 */
	fw.resource_class_index[BR_MEMORY_REGISTRY] = 
		resourceClasses+0;

	fw.resource_class_index[BR_MEMORY_ANCHOR] = 
		resourceClasses+1;

	/*
	 * Allocate the zero sized base resource instance for
	 * the framework
	 */
	fw.res = BrResAllocate(NULL, 0, BR_MEMORY_ANCHOR);

	/*
	 * Register all the system resource classes
	 */
	for(i=0; i < BR_ASIZE(resourceClasses); i++)
		BrResClassAdd(resourceClasses+i);

	/*
	 * Setup list of loaded images
	 */
	BrNewList(&fw.images);

	/*
	 * Initialise table of devices
	 */
	fw.dev_slots = BrResAllocate(fw.res, NDEV_SLOTS * sizeof(*fw.dev_slots), BR_MEMORY_DRIVER );
	fw.ndev_slots = NDEV_SLOTS;

	/*
	 * Mark as active
	 */
	fw.active = BR_TRUE;

	/*
	 * Set up tokens
	 */
	BrTokenBegin();

   /*
    *  Set up default system configuration
    */ 

    BrSystemConfigBegin();

	// Mark default drivers as still to be loaded.
	
	fw.bAlreadyLoadedDrivers = BR_FALSE ;
	
	/*
	 * Set up resident framework image
	 */
	BrImageAdd(&Image_BRCORE1);

	return BRE_OK;
}

br_error BR_PUBLIC_ENTRY BrFwEnd(void)
{
	if(!fw.active)
		return BRE_NOT_ACTIVE;

	BrImageRemove(&Image_BRCORE1);

	/*
	 * Free all resources ...
	 */
	BrResFree(fw.res);

	/*
	 * Clear out fw structure
	 */
	BrMemSet(&fw, 0, sizeof(fw));

	return BRE_OK;
}

/*
 * User functions for setting new errorhandler, filesystem, or allocator
 */
br_diaghandler * BR_PUBLIC_ENTRY BrDiagHandlerSet(br_diaghandler *newdh)
{
	br_diaghandler *old = fw.diag;

	if(newdh == NULL)
		fw.diag = _BrDefaultDiagHandler;
	else
		fw.diag = newdh;

	return old;
}

br_filesystem * BR_PUBLIC_ENTRY BrFilesystemSet(br_filesystem *newfs)
{
	br_filesystem *old = fw.fsys;

	if(newfs == NULL)
		fw.fsys = _BrDefaultFilesystem;
	else
		fw.fsys = newfs;

	return old;
}

br_allocator * BR_PUBLIC_ENTRY BrAllocatorSet(br_allocator *newal)
{
	br_allocator *old = fw.mem;

	if(newal == NULL)
		fw.mem = _BrDefaultAllocator;
	else
		fw.mem = newal;

	return old;
}

