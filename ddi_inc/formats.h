/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: formats.h 1.4 1998/09/21 14:50:54 johng Exp $
 * $Locker: $
 *
 * All the geometry formats understood by the renderer
 * This internal format is being generally release to remove the need for
 * for duplicate copies of model data when accessing is required.
 */
#ifndef _FORMATS_H_
#define _FORMATS_H_

// This was for restricting our evaluation version.
#if EVAL
#define PAD_SIZE (2048-32)
#endif

/**
 ** Generic vertex structure used for geometry formats
 **/
#if 0 
struct fmt_vertex {
	br_vector3 p;			/* Point in model space				*/
	br_vector2 map;			/* Mapping coordinates				*/
	br_vector3 n;			/* Surface normal at vertex			*/
#if EVAL
	br_int_8 _pad[PAD_SIZE];
#endif
};

struct fmt_vertex_x {
	br_vector3_x p;			/* Point in model space				*/
	br_vector2_x map;		/* Mapping coordinates				*/
	br_vector3_x n;		    /* Surface normal at vertex			*/
#if EVAL
	br_int_8 _pad[PAD_SIZE];
#endif
};
		  
struct fmt_vertex_f {
	br_vector3_f p;			/* Point in model space				*/
	br_vector2_f map;		/* Mapping coordinates				*/
	br_vector3_f n;		    /* Surface normal at vertex			*/
#if EVAL
	br_int_8 _pad[PAD_SIZE];
#endif
};
/**
 ** Version 1.1 prepared model format
 **/
struct v11face {
	br_uint_16 vertices[3];		/* Vertices around face 		            */
	br_uint_16 edges[3];		/* Edges around face			            */
    br_vector4 eqn;             /* Plane equation as a 4 vector (Nx,Ny,Nz,D)*/
};

struct v11face_x {
	br_uint_16 vertices[3];		/* Vertices around face 		            */
	br_uint_16 edges[3];		/* Edges around face			            */
    br_vector4_x eqn;           /* Plane equation as a 4 vector (Nx,Ny,Nz,D)*/
};

struct v11face_f {
	br_uint_16 vertices[3];		/* Vertices around face 		            */
	br_uint_16 edges[3];		/* Edges around face			            */
    br_vector4_f eqn;           /* Plane equation as a 4 vector (Nx,Ny,Nz,D)*/
};
#endif 

// This groups together all the faces with the same material.
struct v11group {
	void *stored;					/* Group material (or NULL) 			*/

//	struct v11face *faces;  	 	/* faces in group						*/
	br_vector3_u16 *vertex_numbers;
	br_vector3_u16 *edges;
	br_vector4 * eqn;
    br_colour *face_colours;   		/* Colour for geometry				    */
    br_uint_16 *face_user;          /* Per face user data                   */
	br_uint_8 *face_flags;			/* Per face flags                       */

//	struct fmt_vertex *vertices;	/* vertices in group					*/
	br_vector3 * position;
	br_vector2 * map;
	br_vector3 * normal;
    br_colour *vertex_colours;   	/* Colour for geometry				    */
    br_uint_16 *vertex_user;        /* Per vertex user data                 */

	br_uint_16 nfaces;				/* Number of faces in this group		*/
	br_uint_16 nvertices;			/* Number of vertices in this group		*/
	br_uint_16 nedges;				/* Number of edges in this group		*/
};

#if 0
struct v11group_x {
	void *stored;					/* Group material (or NULL) 			*/

	struct v11face_x *faces;	 	/* faces in group						*/
    br_colour *face_colours;   		/* Colour for geometry				    */
    br_uint_16 *face_user;   		/* Per face user data                   */
	br_uint_8 *face_flags;			/* Per face flags                       */

	struct fmt_vertex_x *vertices;	/* vertices in group					*/
    br_colour *vertex_colours;   	/* Colour for geometry				    */
    br_uint_16 *vertex_user;       	/* Per vertex user data                 */

	br_uint_16 nfaces;				/* Number of faces in this group		*/
	br_uint_16 nvertices;			/* Number of vertices in this group		*/
	br_uint_16 nedges;				/* Number of edges in this group		*/
};

struct v11group_f {
	void *stored;					/* Group material (or NULL) 			*/

	struct v11face_f *faces;	 	/* faces in group						*/
    br_colour *face_colours;   		/* Colour for geometry				    */
    br_uint_16 *face_user;   		/* Per face user data                   */
	br_uint_8 *face_flags;			/* Per face flags                       */

	struct fmt_vertex_f *vertices;	/* vertices in group					*/
    br_colour *vertex_colours;   	/* Colour for geometry				    */
    br_uint_16 *vertex_user;        /* Per vertex user data                 */

	br_uint_16 nfaces;				/* Number of faces in this group		*/
	br_uint_16 nvertices;			/* Number of vertices in this group		*/
	br_uint_16 nedges;				/* Number of edges in this group		*/
};
#endif 
struct v11model {
	br_size_t size;
	br_uint_32 flags;
	br_uint_16 ngroups;
	br_vector3 pivot;

	struct v11group *groups;

	br_bounds bounds;
	br_scalar radius;
        br_vector3 centre;
        br_scalar centred_radius;
};

#if 0 
struct v11model_x {
	br_size_t size;
	br_uint_32 flags;
	br_uint_16 ngroups;
	br_vector3_x pivot;

	struct v11group_x *groups;

	br_bounds3_x bounds;
	br_fixed_ls radius;
        br_vector3_x centre;
        br_fixed_ls centred_radius;
};

struct v11model_f {
	br_size_t size;
	br_uint_32 flags;
	br_uint_16 ngroups;
	br_vector3_f pivot;

	struct v11group_f *groups;

	br_bounds3_f bounds;
	float radius;
        br_vector3_f centre;
        float centred_radius;
};
#endif 
#endif
