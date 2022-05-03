/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: fw.h 1.1 1997/12/10 16:41:07 jon Exp $
 * $Locker: $
 *
 * Internal types and structures for framework
 */
#ifndef _FW_H_
#define _FW_H_

/*
 * Pull in all the public definitions/declarations
 */
#ifndef _BRDDI_H_
#include "brddi.h"
#endif


/*
 * Private state of framework
 */
typedef struct br_framework_state {
	/*
	 * Flag to indicate that framework is set up
	 */
	br_boolean active;

	/*
	 * Various lists of registered items
	 */
	br_registry reg_resource_classes;

	/*
	 * An index of registered resources by class
	 */
	br_resource_class *resource_class_index[BR_MAX_RESOURCE_CLASSES];

	/*
	 * Collection of tokens
	 */
	br_list tokens;
	br_token next_free_token;

	/*
	 * Current filesystem, memory, and error handlers
	 */
	br_filesystem *fsys;
	br_allocator *mem;
	br_diaghandler *diag;

	/*
	 * File write mode
	 */
	int open_mode;

	/*
	 * Base resource of which everything else is a descendant
	 */
	void *res;

	/*
	 * Global scratch space
	 */
	void *scratch_ptr;
	br_size_t scratch_size;
	br_size_t scratch_last;
	int scratch_inuse;

	/*
	 * error value
	 */
	br_error last_error_type;
	void ** last_error_value;

	/*
	 * List of loaded images
	 */
	br_list images;

	/*
	 * Pointers to loaded devices and the images whence they came
	 */
	struct br_open_device *dev_slots;
	br_int_32 ndev_slots;

    br_associative_array *sys_config ;
	br_boolean bAlreadyLoadedDrivers ;

} br_framework_state;

/*
 * Device pointer and image from where it came (or NULL if device is static).
 */
typedef struct br_open_device {
    struct br_device *dev;
    struct br_image *image;
} br_open_device;

/*
 * Global renderer state
 */
#ifdef __cplusplus
extern "C" {
#endif
extern br_framework_state BR_ASM_DATA fw;
#ifdef __cplusplus
};
#endif

/*
 * Minimum scratch space to allocate for render temps.
 */
#define MIN_WORKSPACE	8192

#if DEBUG
/*
 * Controls whether the source file/line of each
 * resource allocation is tracked
 *
 * XXX Unimplemented
 */
#define BR_RES_TRACKING 1

/*
 * True if resources are tagged with a magic number and pointer - allows validation
 * and debug dumping
 */
#define BR_RES_TAGGING 1
#else
#define BR_RES_TRACKING 1
#define BR_RES_TAGGING 1
#endif

/*
 * Initial number of device slots
 */
#define NDEV_SLOTS 16

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _FW_IP_H_
#include "fw_ip.h"
#endif

#ifndef _FW_IX_H_
#include "fw_ix.h"
#endif

#endif
#endif

