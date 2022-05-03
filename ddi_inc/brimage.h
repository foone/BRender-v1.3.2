/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brimage.h 1.1 1997/12/10 16:41:04 jon Exp $
 * $Locker: $
 *
 */
#ifndef _BRIMAGE_H_
#define _BRIMAGE_H_

/*
 * In-memory structure describing a loaded image
 */
typedef struct br_image_section {
	char *name;
	void *base;
	br_size_t mem_offset;
	br_size_t mem_size;
	br_uint_32 data_offset;
	br_uint_32 data_size;
} br_image_section;

typedef struct br_image {
	/*
	 * Anchor block for list of images
	 */
	br_node node;

	/*
	 * DLL name
	 */
	char *identifier;

	/*
	 * Type of DLL - Resident, Host, Framework
	 */
	br_int_32 type;

	/*
	 * Number of references to this DLL
	 */
	br_int_32 ref_count;

	/*
	 * Table of exported functions
	 */
	br_uint_32 ordinal_base;
	br_uint_32 n_functions;
	void ** functions;

	/*
	 * Name -> ordinal lookup
	 */
	br_uint_32 n_names;
	char		**names;
	br_uint_16 *name_ordinals;

	/*
	 * Table of imported image pointers
	 */
	br_uint_16 n_imports;
	struct br_image ** imports;

	/*
	 * Image sections
	 */
	br_uint_16 n_sections;
	br_image_section *sections;

	/*
	 * Type specific pointer
	 */
	void	*type_pointer;

} br_image;

/*
 * Possible types of DLL
 */
enum {
	BR_IMG_FRAMEWORK = 1,
	BR_IMG_RESIDENT,
	BR_IMG_HOST,
};

/*
 * Bogus type used to import functions
 */
typedef void BR_PUBLIC_ENTRY br_resident_fn();

#endif

