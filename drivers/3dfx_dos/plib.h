/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Private primitive library structure
 */
#ifndef _PRIMLIB_H_
#define _PRIMLIB_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct br_primitive_library {
	/*
	 * Dispatch table
	 */
	struct br_primitive_library_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	/*
	 * List of objects associated with this renderer
	 */
	void *object_list;

} br_primitive_library;

#ifdef __cplusplus
};
#endif
#endif


