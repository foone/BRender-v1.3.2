/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: renderer.h 1.1 1997/12/10 16:52:30 jon Exp $
 * $Locker: $
 *
 * Private renderer structure
 */
#ifndef _RENDERER_H_
#define _RENDERER_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Renderer - must be compatible with br_renderer_state
 */
typedef struct br_renderer {
	/*
	 * Dispatch table
	 */
	struct br_renderer_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

    /*
     * Pointer to owning device
     */
    struct br_device *device;

	/*
	 * List of objects associated with this renderer
	 */
	void *object_list;

	/*
	 * State of renderer
	 */
	struct state_all state;

	/*
	 * Default state of renderer
	 */
	struct state_all *default_state;

	/*
	 * last restored state
	 */
	struct br_renderer_state_stored *last_restored;

	/*
	 * Output primitives
	 */
	struct br_primitive_library *plib;

	/*
	 * Stack of saved states (XXX should allocate on the fly)
	 */
	struct state_all state_stack[MAX_STATE_STACK];
	br_uint_32 stack_top;

	/*
	 * Type of renderer
	 */
	br_renderer_facility *renderer_facility;

} br_renderer;

#ifdef __cplusplus
};
#endif
#endif


