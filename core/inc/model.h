/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: model.h 1.4 1998/07/21 17:33:17 jon Exp $
 * $Locker: $
 *
 * In-memory structures for models, both public and private areas
	Last change:  TN    9 Apr 97    4:40 pm
 */
#ifndef _MODEL_H_
#define _MODEL_H_

typedef struct br_vertex {
	br_vector3 p;	/* Point in model space		*/
	br_vector2 map;	/* Mapping coordinates		*/

	/*
	 * Colour for prelit models
	 */
	br_uint_8 index;
	br_uint_8 red;
	br_uint_8 grn;
	br_uint_8 blu;

	/*
	 * Private fields
	 */
	br_uint_16 _pad0;
	br_fvector3 n;	/* Surface normal at vertex	*/

} br_vertex;

typedef struct br_face {
	br_uint_16 vertices[3];		/* Vertices around face 				*/
	br_uint_16 smoothing;		/* Controls if shared edges are smooth	*/
	br_material *material;		/* Face material (or NULL) 				*/

	/*
	 * Colour for prelit models
	 */
	br_uint_8 index;
	br_uint_8 red;
	br_uint_8 grn;
	br_uint_8 blu;

	br_uint_8 flags;			/* Bits 0,1 and 2 denote internal edges	*/
	br_uint_8 _pad0;
	br_uint_32 _pad1;

	br_fvector3 n;				/* Plane equation of face				*/
	br_scalar d;
} br_face;

/*
 * Bits for face flags
 */
enum {
	BR_FACEF_COPLANAR_0 = 0x01,	/* The face adjoining edge 0 is coplanar with this face */
	BR_FACEF_COPLANAR_1 = 0x02,	/*         ""              1          ""                */
	BR_FACEF_COPLANAR_2 = 0x04,	/*         ""              2          ""                */
    BR_FACEF_COPLANAR_3 = 0x08,	/*         ""              3          ""                */

	BR_FACEF_QUAD_MASK = 0x70,	/* For quad based texture mapping, which half of a quad this triangle represents */
	BR_FACEF_QUAD_012 = 0x00,	/* Vertices 0, 1 and 2 of this face are vertices 0, 1 and 2 of a quad */
	BR_FACEF_QUAD_123 = 0x10,	/* Vertices 0, 1 and 2 of this face are vertices 1, 2 and 3 of a quad */
	BR_FACEF_QUAD_230 = 0x20,	/* Vertices 0, 1 and 2 of this face are vertices 2, 3 and 0 of a quad */
	BR_FACEF_QUAD_301 = 0x30,	/* Vertices 0, 1 and 2 of this face are vertices 3, 0 and 1 of a quad */
	BR_FACEF_QUAD_032 = 0x40,	/* Vertices 0, 1 and 2 of this face are vertices 0, 3 and 2 of a quad */
	BR_FACEF_QUAD_103 = 0x50,	/* Vertices 0, 1 and 2 of this face are vertices 1, 0 and 3 of a quad */
	BR_FACEF_QUAD_210 = 0x60,	/* Vertices 0, 1 and 2 of this face are vertices 2, 1 and 0 of a quad */
	BR_FACEF_QUAD_321 = 0x70,	/* Vertices 0, 1 and 2 of this face are vertices 3, 2 and 1 of a quad */
	BR_FACEF_QUAD_SHIFT = 4
};

/*
 * Primitive list added for future expansion
 */

typedef struct br_primitive_list {
    struct br_primitive_list *next;         /* Ptr to next primitive list. */
    br_uint_32 prim_type;                   /* Primitive type (see below). */
    br_uint_16 nprims;                      /* Number of primitives. */
    br_uint_16 nspares;                     /* Number of 'spare' structures. */
    void      *prim;                        /* Ptr to array of primitives. */
    void      *spare;                       /* Ptr to array of spares. */
} br_primitive_list;

/*
 * Bits for prim_type field above.
 */

enum {
    BR_PRIM_VERTEX_SGL  = 0x01,             /* prim points to br_vertex_single structures. */
    BR_PRIM_VERTEX_DBL  = 0x02,             /* prim points to br_vertex_double structures. */
    BR_PRIM_POINT       = 0x03,             /* prim points to br_point_prim structures. */
    BR_PRIM_LINE        = 0x04,             /* prim points to br_line structures. */
    BR_PRIM_TRIANGLE    = 0x05,             /* prim points to br_triangle structures. */
    BR_PRIM_QUAD        = 0x06,             /* prim points to br_quad structures. */
    BR_PRIM_TRI_STRIP   = 0x07,             /* prim points to br_tri_strip structures. */
    BR_PRIM_TRI_FAN     = 0x08,             /* prim points to br_tri_fan structures. */
    BR_PRIM_QUAD_STRIP  = 0x09              /* prim points to br_quad_strip structures. */
};

/*
 * New primitive types
 */

typedef struct br_vertex_single {           /* Vertex for single-textured primitives. */
	br_vector3 p;	/* Point in model space		*/
	br_vector2 map;	/* Mapping coordinates		*/

	/*
	 * Colour for prelit models
	 */
	br_uint_8 alpha;
	br_uint_8 red;
	br_uint_8 grn;
	br_uint_8 blu;

	/*
	 * Private fields
	 */
	br_uint_16 _pad0;
	br_fvector3 n;	/* Surface normal at vertex	*/
} br_vertex_single;


typedef struct br_vertex_double {           /* Vertex for double-textured primitives. */
    br_vector3 p;       /* Point in model space. */
    br_vector2 map0;    /* Primary U/V */

    br_uint_8 alpha0;   /* Primary ARGB */
    br_uint_8 red0;
    br_uint_8 grn0;
    br_uint_8 blu0;

    br_vector2 map1;    /* Secondary U/V */
    br_uint_8 alpha1;   /* Secondary ARGB */
    br_uint_8 red1;
    br_uint_8 grn1;
    br_uint_8 blu1;

	/*
	 * Private fields
	 */
	br_fvector3 n;	/* Surface normal at vertex	*/

} br_vertex_double;


typedef struct br_point_prim {              /* Point primitive. */
    br_uint_16   vertices[1];   /* Vertex index. */
    br_material *material;      /* Point material (or NULL). */
} br_point_prim;


typedef struct br_line {                    /* Line primitive. */
    br_uint_16   vertices[2];   /* Start & end vertices. */
    br_material *material;      /* Line material (or NULL). */

    /*
     * Colours for pre-lit models
     */
    br_uint_8 alpha0;
    br_uint_8 red0;
    br_uint_8 grn0;
    br_uint_8 blu0;

    br_uint_8 alpha1;
    br_uint_8 red1;
    br_uint_8 grn1;
    br_uint_8 blu1;
} br_line;


typedef struct br_triangle {                /* Triangle primitive. */
	br_uint_16 vertices[3];		/* Vertices around triangle	*/
	br_uint_16 smoothing;		/* Controls if shared edges are smooth */
	br_material *material;		/* Triangle material (or NULL) */

	/*
	 * Colours for prelit models
	 */
	br_uint_8 alpha0;
	br_uint_8 red0;
	br_uint_8 grn0;
	br_uint_8 blu0;

    br_uint_8 alpha1;
    br_uint_8 red1;
    br_uint_8 grn1;
    br_uint_8 blu1;

	br_uint_8 flags;			/* Bits 0,1 and 2 denote internal edges	*/

	br_fvector3 n;				/* Plane equation of face				*/
	br_scalar d;
} br_triangle;


typedef struct br_quad {                    /* Quadrilateral primitive. */
    br_uint_16   vertices[4];   /* Vertices around quad. */
    br_uint_16   smoothing;     /* Controls if shared edges are smooth. */
    br_material *material;      /* Quad material (or NULL). */

    /*
     * Colour for pre-lit models
     */
    br_uint_8 alpha0;
    br_uint_8 red0;
    br_uint_8 grn0;
    br_uint_8 blu0;

    br_uint_8 alpha1;
    br_uint_8 red1;
    br_uint_8 grn1;
    br_uint_8 blu1;

    br_uint_8 flags;            /* Bits 0, 1, 2 and 3 denote internal edges. */

    br_fvector3 n;              /* Plane equation of quad. */
    br_scalar d;
} br_quad;


typedef struct br_strip_face_data {
    br_uint_16 smoothing;      /* Smoothing group. */
    /*
     * Colours for pre-lit models
     */
    br_uint_8 alpha0;
    br_uint_8 red0;
    br_uint_8 grn0;
    br_uint_8 blu0;

    br_uint_8 alpha1;
    br_uint_8 red1;
    br_uint_8 grn1;
    br_uint_8 blu1;

    br_uint_8 flags;            /* Shared edge flags. */

    br_fvector3 n;              /* Plane equation of quad. */
    br_scalar d;
} br_strip_face_data;

typedef struct br_tri_strip {               /* Tri-strip primitive. */
    br_uint_16   nvertices;     /* Number of vertices. */
    br_uint_16  *vertices;      /* Array of indexes to vertices in strip. */
    br_material *material;      /* Strip material (or NULL). */

    /*
     * Array of data for each face in the strip.
     */

    br_strip_face_data *face_data;
} br_tri_strip;


typedef br_tri_strip br_tri_fan;            /* Tri-fan primitive. */


typedef br_tri_strip br_quad_strip;         /* Quad-strip primitive. */


/*
 * Callback function type for custom models
 */
struct br_actor;
struct br_model;
struct br_material;

typedef void BR_CALLBACK br_model_custom_cbfn(
				struct br_actor *actor,
				struct br_model *model,
				struct br_material *material,
				void *render_data,
				br_uint_8 style,
				int on_screen);

/*
 * NB: To get at the model & screen matrices during a model
 *     callback, use BrModelToViewQuery() and BrModelToScreenQuery()
 */

typedef struct br_model {
	br_uint_32 _reserved;

	char *identifier;

	br_vertex *vertices;
	br_face *faces;

	br_uint_16 nvertices;
	br_uint_16 nfaces;

	/*
	 * Offset of model's pivot point (where it attaches to parent)
	 */
	br_vector3 pivot;

	/*
	 * Flags describing what is allowed in ModelPrepare()
	 */
	br_uint_16 flags;

	/*
	 * Application call
	 */
	br_model_custom_cbfn *custom;

	/*
	 * Application defined data - untouched by system
	 */
	void *user;

	/*
	 * Crease angle (used if MODF_CREASE is set)
	 */
	br_angle crease_angle;

	/*
	 * Bounding radius of model from origin
	 */
	br_scalar radius;

	/*
	 * Axis-aligned box that bounds model in model coords
	 */
	br_bounds bounds;

	/*
	 * Private fields
	 */
	void *prepared;
	void *stored;

    /*
     * New primitive extensions
     */

    br_uint_16 nprimitive_lists;
    br_primitive_list *primitive_list;

} br_model;

/*
 * Bits for br_model->flags
 */
enum {
	BR_MODF_DONT_WELD		= 0x0001,	/* Vertices with same x,y,z cannot be merged	*/
	BR_MODF_KEEP_ORIGINAL	= 0x0002,	/* Obselete */
	BR_MODF_GENERATE_TAGS	= 0x0004,	/* Obselete */
	BR_MODF_QUICK_UPDATE	= 0x0010,	/* ModelUpdate is fast - but may produce slower models */

	BR_MODF_CUSTOM			= 0x0020,   /* Invoke custom callback for this model */
	BR_MODF_PREPREPARED		= 0x0040,	/* Model structure is pre-prepared - update performs no work */

	BR_MODF_UPDATEABLE		= 0x0080,	/* ModelUpdate can be used */

	BR_MODF_CREASE			= 0x0100,	/* Create creases in smoothing along edges if face<->face angle is g.t model->crease */
	BR_MODF_CUSTOM_NORMALS	= 0x0200,	/* Uses vertex normals from br_vertex structure */
	BR_MODF_CUSTOM_BOUNDS	= 0x0400,	/* Bounding box is already set up				*/
	BR_MODF_FACES_ONLY		= 0x0800,	/* Model will only be used to render faces (not edges or points) */

	BR_MODF_USED_PREPARED_USER = 0x1000,/* User fields in prepared data used */

	BR_MODF_CUSTOM_EQUATIONS= 0x2000,	/* Uses face equations from br_face structure */

	_BR_MODF_RESERVED		= 0x8000
};

/*
 * Flags to BrModelUpdate()
 */
enum {
	BR_MODU_VERTEX_POSITIONS	= 0x0001,
	BR_MODU_VERTEX_COLOURS		= 0x0002,
	BR_MODU_VERTEX_MAPPING		= 0x0004,
	BR_MODU_VERTEX_NORMALS		= 0x0008,
	BR_MODU_PRIMITIVE_MATERIALS	= 0x0010,
	BR_MODU_PRIMITIVE_COLOURS	= 0x0020,
	BR_MODU_VERTICES			= 0x0040,
	BR_MODU_FACES				= 0x0080,
	BR_MODU_PIVOT				= 0x0100,
	BR_MODU_PREPARED			= 0x0200,
	BR_MODU_PRIMITIVE_EQUATIONS	= 0x0400,
	BR_MODU_ALL					= 0x7FFF,

	_BR_MODU_RESERVED			= 0x8000
};

/*
 * Backwards compatibility
 */

#define BR_MODU_FACE_MATERIALS  BR_MODU_PRIMITIVE_MATERIALS
#define BR_MODU_FACE_COLOURS	BR_MODU_PRIMITIVE_COLOURS
#define BR_MODU_FACE_EQUATIONS	BR_MODU_PRIMITIVE_EQUATIONS
#define BR_MODU_NORMALS       	BR_MODU_VERTEX_POSITIONS
#define BR_MODU_EDGES		 	BR_MODU_FACES
#define BR_MODU_RADIUS		 	BR_MODU_VERTEX_POSITIONS
#define BR_MODU_GROUPS		 	BR_MODU_FACES
#define BR_MODU_BOUNDING_BOX	BR_MODU_VERTEX_POSITIONS
#define BR_MODU_MATERIALS      	BR_MODU_FACE_MATERIALS

/*
 * Values for BrModelApplyMap()
 */
enum br_apply_map_types {
	BR_APPLYMAP_PLANE,
	BR_APPLYMAP_SPHERE,
	BR_APPLYMAP_CYLINDER,
	BR_APPLYMAP_DISC,
	BR_APPLYMAP_NONE
};

/*
 * Axis values for BrModelFitMap()
 */
enum br_fitmap_axis {
	BR_FITMAP_PLUS_X,
	BR_FITMAP_PLUS_Y,
	BR_FITMAP_PLUS_Z,
	BR_FITMAP_MINUS_X,
	BR_FITMAP_MINUS_Y,
	BR_FITMAP_MINUS_Z
};

#endif

/* 
 * Local Variables:
 * tab-width: 4
 * End:
 */

