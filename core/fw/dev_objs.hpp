// Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
//
// $Id: dev_objs.hpp 1.7 1998/11/17 15:58:54 jon Exp $
// $Locker: $
//
// C++ Classes for BRender device interface
// Various structures and dispatch tables used to represent device objects
//
// The formatting of this file is important - it is parsed by classgen.pl to
// produce C headers.



#ifdef __CLASSGEN__
// Macros for declaring overloaded methods
#define CG_F(x)	x##F
#define CG_X(x)	x##X
#else
#define CG_PUBLIC
#define CG_F(x)	x
#define CG_X(x)	x
#endif

class br_object;
class br_object_container;
class br_device;
class br_facility;
class br_output_facility;
class br_device_pixelmap;
class br_device_clut;
class br_renderer_facility;
class br_renderer;

class br_geometry;
class br_geometry_format;

class br_geometry_stored;
class br_renderer_state_stored;
class br_map_stored;
class br_table_stored;

struct br_tv_template;

// The root abstract base class for all device objects
//
// Supports queries about the object, both for a few specific
// things (identifer, type & devce), and a more general set of
// token/value queries

class br_object {
	public:
		// Four slots reserved for the device implementor
		//
		virtual	void BR_METHOD _reserved0(void);
		virtual	void BR_METHOD _reserved1(void);
		virtual	void BR_METHOD _reserved2(void);
		virtual	void BR_METHOD _reserved3(void);

		virtual	void BR_METHOD free(void);
		virtual	char * BR_METHOD identifier(void);
		virtual	br_token BR_METHOD type(void);
		virtual	br_boolean BR_METHOD isType(br_token t);
		virtual	class br_device * BR_METHOD device(void);
		virtual	br_int_32 BR_METHOD space(void);

	protected:
		virtual	struct br_tv_template * BR_METHOD templateQuery(void) = 0;

	public:
		virtual	br_error BR_METHOD query(br_uint_32 *pvalue, br_token t);
		virtual br_error BR_METHOD queryBuffer(br_uint_32 *pvalue, br_uint_32 *buffer, br_size_t buffer_size, br_token t);
		virtual	br_error BR_METHOD queryMany(br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount);
		virtual	br_error BR_METHOD queryManySize(br_size_t *pextra_size, br_token_value *tv);
		virtual	br_error BR_METHOD queryAll(br_token_value *buffer, br_size_t buffer_size);
		virtual	br_error BR_METHOD queryAllSize(br_size_t *psize);
};

// Abstract base class for an obect that can contain other objects
// Allows objects to be added, removed and enumerated.

class br_object_container : public br_object {
	protected:
		virtual	void * BR_METHOD listQuery(void) = 0;
		virtual	void * BR_METHOD tokensMatchBegin(br_token t, br_token_value *tv);
		virtual	br_boolean BR_METHOD tokensMatch(class br_object *h, void *arg);
		virtual	void BR_METHOD tokensMatchEnd(void *arg);

	public:
		virtual	br_error BR_METHOD addFront(class br_object *h);
		virtual	br_error BR_METHOD removeFront(class br_object **h);
		virtual	br_error BR_METHOD remove(class br_object *h);
		virtual br_error BR_METHOD find(class br_object **ph, br_token type, char *pattern, br_token_value *tv);
		virtual br_error BR_METHOD findMany(class br_object **objects, br_int_32 max_objects,
			br_int_32 *pnum_objects, br_token type, char *pattern, br_token_value *tv);
		virtual br_error BR_METHOD count(br_uint_32 *pcount, br_token type, char *pattern, br_token_value *tv);
};

// The device object - contains the various type objects
// for services the device has on offer

class br_device : public br_object_container {
	public:
};

// Some facility that a device supports 

class br_facility : public br_object_container {
	public:
};

// A type of output that a device supports. (typically this would
// corespond to a screen mode).
//
// device_pixelmaps (and, if supported, CLUTs) can be instantiated from this

class br_output_facility : public br_facility {
	public:
		virtual br_error BR_METHOD validSource(br_boolean *bp, class br_object *h);
		virtual br_error BR_METHOD pixelmapNew(class br_device_pixelmap **ppmap, br_token_value *tv);
		virtual br_error BR_METHOD clutNew(class br_device_clut **pclut, br_token_value *tv);
		virtual br_error BR_METHOD queryCapability(br_token_value *buffer_in, br_token_value *buffer_out,
			br_size_t size_buffer_out);
};

// A drawable surface - compatibile with br_pixelmap. May represent
// the screen, the off-screen buffer a depth buffer or others.

class br_device_pixelmap : public br_object {

	public:

	// Public fields for compatibility with br_pixelmap

		CG_PUBLIC char *pm_identifier ;
		CG_PUBLIC BR_PIXELMAP_MEMBERS

		virtual br_error BR_METHOD validSource(br_boolean *bp, class br_object *h);

		virtual br_error BR_METHOD resize(br_int_32 width, br_int_32 height);
		virtual br_error BR_METHOD match(class br_device_pixelmap **newpm, br_token_value *tv);
		virtual br_error BR_METHOD allocateSub(class br_device_pixelmap **newpm, br_rectangle *rect);


	// Operations on whole pixemap (with versions that include a hint dirty rectangle)

		virtual br_error BR_METHOD copy(class br_device_pixelmap *src);
		virtual br_error BR_METHOD copyTo(class br_device_pixelmap *src);
		virtual br_error BR_METHOD copyFrom(class br_device_pixelmap *src);
		virtual br_error BR_METHOD fill(br_uint_32 colour);
		virtual br_error BR_METHOD doubleBuffer(class br_device_pixelmap *src);

		virtual br_error BR_METHOD copyDirty(class br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects);
		virtual br_error BR_METHOD copyToDirty(class br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects);
		virtual br_error BR_METHOD copyFromDirty(class br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects);
		virtual br_error BR_METHOD fillDirty(br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects);
		virtual br_error BR_METHOD doubleBufferDirty(class br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects);


	// Direct 2D rendering operations

		virtual br_error BR_METHOD rectangle(br_rectangle *rect, br_uint_32 colour);
		virtual br_error BR_METHOD rectangle2(br_rectangle *rect, br_uint_32 colour_tl, br_uint_32 colour_br);
		virtual br_error BR_METHOD rectangleCopy(br_point *p, class br_device_pixelmap *src, br_rectangle *src_rect);
		virtual br_error BR_METHOD rectangleCopyTo(br_point *p, class br_device_pixelmap *src, br_rectangle *src_rect);
		virtual br_error BR_METHOD rectangleCopyFrom(br_point *p, class br_device_pixelmap *src, br_rectangle *src_rect);

		virtual br_error BR_METHOD rectangleStretchCopy(br_rectangle *r,class br_device_pixelmap *s,br_rectangle *q);
		virtual br_error BR_METHOD rectangleStretchCopyTo(br_rectangle *r,class br_device_pixelmap *s,br_rectangle *q);
		virtual br_error BR_METHOD rectangleStretchCopyFrom(br_rectangle *r,class br_device_pixelmap *s,br_rectangle *q);
	
		virtual br_error BR_METHOD rectangleFill(br_rectangle *rect, br_uint_32 colour);

		virtual br_error BR_METHOD pixelSet(br_point *point, br_uint_32 colour);

		virtual br_error BR_METHOD line(br_point *start, br_point *end, br_uint_32 colour);

		virtual br_error BR_METHOD copyBits(br_point *point,
			br_uint_8 *src,br_uint_16 s_stride,
			br_rectangle *bit_rect,
			br_uint_32 colour);


	// Text

		virtual br_error BR_METHOD text(br_point *point, br_font *font, char *text, br_uint_32 colour);
		virtual br_error BR_METHOD textBounds(br_rectangle *rect, struct br_font *font, char *text);


	// Row read/write

		virtual br_error BR_METHOD rowSize(br_size_t *size);
		virtual br_error BR_METHOD rowSet(void *buffer, br_size_t buffer_size, br_uint_32 row);
		virtual br_error BR_METHOD rowQuery(void *buffer, br_size_t buffer_size, br_uint_32 row);


	// Reading back

		virtual br_error BR_METHOD pixelQuery(br_uint_32 *pcolour, br_point *point);
		virtual br_error BR_METHOD pixelAddressQuery(void **pptr, br_uint_32 *pqual, br_point *point);


	// Updating the pixelmap

		virtual br_error BR_METHOD pixelAddressSet(void *ptr, br_uint_32 *qual);
		virtual br_error BR_METHOD originSet(br_point *p);


	// Flush any cached output

		virtual br_error BR_METHOD flush(void);

	// Syncronisation

		virtual br_error BR_METHOD synchronise(br_token sync_type, br_boolean block);

	// Locking/unlocking for direct pixel access
	//
		virtual br_error BR_METHOD directLock(br_boolean block);
		virtual br_error BR_METHOD directUnlock(void);

	// Locking/unlocking for direct pixel access
	//
		virtual br_error BR_METHOD getControls(br_display_controls *controls);
		virtual br_error BR_METHOD setControls(br_display_controls *controls);
};

// A device Colour LookUp Table
//
// There may be multiple instances of this, if the device supports
// it. (eg: CLUT per wndows)

class br_device_clut : public br_object {
	public:
		virtual br_error BR_METHOD entrySet(br_int_32 index, br_colour entry);
		virtual br_error BR_METHOD entryQuery(br_colour *entry, br_int_32 index);
		virtual br_error BR_METHOD entrySetMany(br_int_32 index, br_int_32 count, br_colour *entries);
		virtual br_error BR_METHOD entryQueryMany(br_colour *entries, br_int_32 index, br_int_32 count);
};

// A type of renderer - this class contains all instaniated renderers
// of this type, and the stored maps, tables and states associated
// with this type of renderer

class br_renderer_facility : public br_facility {
	public:
		virtual br_error BR_METHOD validDestination(br_boolean *bp, class br_object *h);
		virtual br_error BR_METHOD rendererNew(class br_renderer **prenderer, br_token_value *tv);
};

// An active renderer

class br_renderer : public br_object_container {
	public:

		virtual br_error BR_METHOD validDestination(br_boolean *bp, class br_object *h);

	// Creating new stored states

		virtual br_error BR_METHOD stateStoredNew(class br_renderer_state_stored **pstate,
				br_uint_32 mask, br_token_value *tv);
		virtual br_error BR_METHOD stateStoredAvail(br_int_32 *psize, br_uint_32 mask, br_token_value *tv);

	// Creating new stored input buffers

		virtual br_error BR_METHOD bufferStoredNew(class br_buffer_stored **rtp,
			br_token use, class br_device_pixelmap *pm, br_token_value *tv);

		virtual br_error BR_METHOD bufferStoredAvail(br_int_32 *space,
			br_token use, br_token_value *tv);

	// Writing current state
		virtual br_error BR_METHOD partSet(br_token part, br_int_32 index, br_token t, br_uint_32 pvalue);
		virtual br_error BR_METHOD partSetMany(br_token part, br_int_32 index, br_token_value * tv, br_int_32 *pcount);

	// Reading current state

		virtual br_error BR_METHOD partQuery(br_token part, br_int_32 index, br_uint_32 *pvalue, br_token t);
		virtual br_error BR_METHOD partQueryBuffer(br_token part, br_int_32 index, br_uint_32 *pvalue,
			br_uint_32 *buffer, br_size_t buffer_size, br_token t);
		virtual br_error BR_METHOD partQueryMany(br_token part, br_int_32 index,
			br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount);
		virtual br_error BR_METHOD partQueryManySize(br_token part, br_int_32 index, br_size_t *pextra_size, br_token_value *tv);
		virtual br_error BR_METHOD partQueryAll(br_token part, br_int_32 index, br_token_value *buffer, br_size_t buffer_size);
		virtual br_error BR_METHOD partQueryAllSize(br_token part, br_int_32 index, br_size_t *psize);

	// Find out the range of indices for a part
		virtual br_error BR_METHOD partIndexQuery(br_token part, br_int_32 *pnindex);

	// Special case state manipulation for ease of use

		virtual br_error BR_METHOD CG_F(modelMul)(br_matrix34_f *m);
		virtual br_error BR_METHOD CG_X(modelMul)(br_matrix34_x *m);

		virtual br_error BR_METHOD CG_F(modelPopPushMul)(br_matrix34_f *m);
		virtual br_error BR_METHOD CG_X(modelPopPushMul)(br_matrix34_x *m);

		virtual br_error BR_METHOD modelInvert(void);

	// State stacking and saving

		virtual br_error BR_METHOD statePush(br_uint_32 mask);
		virtual br_error BR_METHOD statePop(br_uint_32 mask);

		virtual br_error BR_METHOD stateSave(class br_renderer_state_stored *h, br_uint_32 mask);
		virtual br_error BR_METHOD stateRestore(class br_renderer_state_stored *h, br_uint_32 mask);

	// Given a list of parts, produce a value for the push/pop/default/save/restore masks

		virtual br_error BR_METHOD stateMask(br_uint_32 *mask, br_token *parts, int n_parts);

	// Reset parts of state to the default

		virtual br_error BR_METHOD stateDefault(br_uint_32 mask);

	// Various queries based on current transforms

		virtual br_error BR_METHOD CG_F(boundsTest)(br_token *r, br_bounds3_f *bounds);
		virtual br_error BR_METHOD CG_X(boundsTest)(br_token *r, br_bounds3_x *bounds);

		virtual br_error BR_METHOD CG_F(coverageTest)(br_float *r, br_bounds3_f *bounds);
		virtual br_error BR_METHOD CG_X(coverageTest)(br_fixed_ls *r, br_bounds3_x *bounds);

		virtual br_error BR_METHOD CG_F(viewDistance)(br_float *r);
		virtual br_error BR_METHOD CG_X(viewDistance)(br_fixed_ls *r);

	// Control current command mode

		virtual br_error BR_METHOD commandModeSet(br_token mode);
		virtual br_error BR_METHOD commandModeQuery(br_token *mode);
		virtual br_error BR_METHOD commandModeDefault(void);
		virtual br_error BR_METHOD commandModePush(void);
		virtual br_error BR_METHOD commandModePop(void);

	// Flush any cached output

		virtual br_error BR_METHOD flush(br_boolean wait);


	// Sync. waiting

		virtual br_error BR_METHOD synchronise(br_token sync_type, br_boolean block);

#if 0
	// Possible additions
	//
		virtual br_error modelToView
		virtual br_error modelToScreen
		virtual br_error viewToModel
		virtual br_error viewToScreen
		virtual br_error screenToModel
		virtual br_error screenToView
#endif

#if 0
	// Test hook
	//
		virtual br_error BR_METHOD testRender(br_token type,
			union brp_vertex *v0,
			union brp_vertex *v1,
			union brp_vertex *v2);
#endif

	// Query state performance
		virtual br_error BR_METHOD partQueryCapability(br_token part, br_int_32 index, br_token_value *buffer,
			br_size_t buffer_size);
		virtual br_error BR_METHOD stateQueryPerformance(br_fixed_lu *speed);
		virtual br_error BR_METHOD frameBegin(void);
		virtual br_error BR_METHOD frameEnd(void);
		virtual br_error BR_METHOD focusLossBegin(void);
		virtual br_error BR_METHOD focusLossEnd(void);
};

// Stored renderer state

class br_renderer_state_stored : public br_object {
	public:
};

// Abstract base class for some sort of geometry that can be stuffed into a renderer

class br_geometry : public br_object {
	public:
}

// Stored geometry

class br_geometry_stored : public br_geometry {
	public:
		virtual br_error BR_METHOD render(class br_renderer *r);
		virtual br_error BR_METHOD renderOnScreen(class br_renderer *r);
}

// Predefined geometry formats

class br_geometry_primitives : public br_geometry {
	public:
		virtual br_error BR_METHOD CG_F(render)(class br_renderer *r,
				struct fmt_vertex *vertices, int nvertices, br_token type);
		virtual br_error BR_METHOD CG_X(render)(class br_renderer *r,
				struct fmt_vertex *vertices, int nvertices, br_token type);
		virtual br_error BR_METHOD CG_F(renderOnScreen)(class br_renderer *r,
				struct fmt_vertex *vertices, int nvertices, br_token type);
		virtual br_error BR_METHOD CG_X(renderOnScreen)(class br_renderer *r,
				struct fmt_vertex *vertices, int nvertices, br_token type);
		virtual br_error BR_METHOD CG_F(storedNew)(class br_renderer *r,
				class br_geometry_stored **psg,
				struct fmt_vertex *vertices, int nvertices, br_token type, br_token_value *tv);
		virtual br_error BR_METHOD CG_X(storedNew)(class br_renderer *r,
				class br_geometry_stored **psg,
				struct fmt_vertex *vertices, int nvertices, br_token type, br_token_value *tv);
		virtual br_error BR_METHOD storedAvail(br_int_32 *psize, br_token_value *tv);
}

class br_geometry_v1_model : public br_geometry {
	public:
		virtual br_error BR_METHOD CG_F(render)(class br_renderer *r, struct v1model_f *model,
			class br_renderer_state_stored *default_state,
			br_token type);
		virtual br_error BR_METHOD CG_X(render)(class br_renderer *r, struct v1model_x *model,
			class br_renderer_state_stored *default_state,
			br_token type);
		virtual br_error BR_METHOD CG_F(renderOnScreen)(class br_renderer *r, struct v1model_f *model,
			class br_renderer_state_stored *default_state,
			br_token type);
		virtual br_error BR_METHOD CG_X(renderOnScreen)(class br_renderer *r, struct v1model_x *model,
			class br_renderer_state_stored *default_state,
			br_token type);
		virtual br_error BR_METHOD CG_F(storedNew)(class br_renderer *r, class br_geometry_stored **psg,
				struct v1model_f *model, br_token type, br_token_value *tv);
		virtual br_error BR_METHOD CG_X(storedNew)(class br_renderer *r, class br_geometry_stored **psg,
				struct v1model_x *model, br_token type, br_token_value *tv);

		virtual br_error BR_METHOD storedAvail(br_int_32 *psize, br_token_value *tv);
}

class br_geometry_v1_buckets : public br_geometry {
	public:
		virtual br_error BR_METHOD render(class br_renderer *r,
				class br_primitive **buckets, br_int_32 nbuckets);
		virtual br_error BR_METHOD renderOnScreen(class br_renderer *r,
				class br_primitive **buckets, br_int_32 nbuckets);
}

class br_geometry_lighting : public br_geometry {
	public:
		virtual br_error BR_METHOD CG_F(render)(class br_renderer *r,
			br_vector3_f *points, br_vector3_f *normals, br_colour *colour_in,
			br_colour *colour_out, br_uint_16 *redirect, int pstride, int nstride,
			int cinstride, int coutstride, int nvertices);
		virtual br_error BR_METHOD CG_X(render)(class br_renderer *r,
			br_vector3_x *points, br_vector3_x *normals, br_colour *colour_in,
			br_colour *colour_out, br_uint_16 *redirect, int pstride, int nstride,
			int cinstride, int coutstride, int nvertices);
}

// Stored texture map/table etc.

class br_buffer_stored : public br_object {
	public:
		virtual br_error BR_METHOD update(class br_device_pixelmap *pm, br_token_value *tv);
};

// Primitives library

class br_primitive_library : public br_object_container {
	public:
		virtual br_error BR_METHOD stateNew(class br_primitive_state **rps);
		virtual br_error BR_METHOD bufferStoredNew(class br_buffer_stored **rtp,
			br_token use, class br_device_pixelmap *pm, br_token_value *tv);
		virtual br_error BR_METHOD bufferStoredAvail(br_int_32 *space,
			br_token use, br_token_value *tv);

		virtual br_error BR_METHOD flush(br_boolean wait);
		virtual br_error BR_METHOD synchronise(br_token sync_type, br_boolean block);

	// Given a list of parts, produce a bitmask
		virtual br_error BR_METHOD mask(br_uint_32 *mask, br_token *parts, int n_parts);

};


// Primitives state

class br_primitive_state : public br_object {
	public:

	// Writing current state
		virtual br_error BR_METHOD partSet(br_token part, br_int_32 index, br_token t, br_uint_32 value);
		virtual br_error BR_METHOD partSetMany(br_token part, br_int_32 index, br_token_value * tv, br_int_32 *pcount);

	// Reading current state
		virtual br_error BR_METHOD partQuery(br_token part, br_int_32 index, br_uint_32 *pvalue, br_token t);
		virtual br_error BR_METHOD partQueryBuffer(br_token part, br_int_32 index, br_uint_32 *pvalue,
			br_uint_32 *buffer, br_size_t buffer_size, br_token t);
		virtual br_error BR_METHOD partQueryMany(br_token part, br_int_32 index,
			br_token_value *tv, void *extra, br_size_t extra_size, br_int_32 *pcount);
		virtual br_error BR_METHOD partQueryManySize(br_token part, br_int_32 index, br_size_t *pextra_size, br_token_value *tv);
		virtual br_error BR_METHOD partQueryAll(br_token state, br_int_32 index, br_token_value *buffer, br_size_t buffer_size);
		virtual br_error BR_METHOD partQueryAllSize(br_token part, br_int_32 index, br_size_t *psize);

	// Find out the range of indices for a part
		virtual br_error BR_METHOD partIndexQuery(br_token part, br_int_32 *pnindex);

	// Reset parts of state to the default
		virtual br_error BR_METHOD stateDefault(br_uint_32 mask);

	// Copy parts of state from one to another
		virtual br_error BR_METHOD stateCopy(class br_primitive_state *source, br_uint_32 mask);
	
	// Get a pointer to a primitive block representing the current state
	//
		virtual br_error BR_METHOD renderBegin(class brp_block **rpb,
				br_boolean *block_changed, br_boolean *ranges_changed, br_boolean no_render,
				br_int_32 prim_type);

		virtual br_error BR_METHOD renderEnd(class brp_block *rpb);

	// Find the current component transformations
	//
		virtual br_error BR_METHOD CG_F(rangesQuery)(br_float *offset, br_float *scale, br_int_32 max_comp);
		virtual br_error BR_METHOD CG_X(rangesQuery)(br_fixed_ls *offset, br_fixed_ls *scale, br_int_32 max_comp);

	// Query state performance
		virtual br_error BR_METHOD partQueryCapability(br_token part, br_int_32 index, br_token_value *buffer,
			br_size_t buffer_size);
		virtual br_error BR_METHOD stateQueryPerformance(br_fixed_lu *speed);
};

