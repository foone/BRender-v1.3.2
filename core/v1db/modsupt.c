/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: modsupt.c 1.1 1997/12/10 16:41:31 jon Exp $
 * $Locker: $
 *
 * Model  support routines 
	Last change:  TN    4 Apr 97   11:15 am
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: modsupt.c 1.1 1997/12/10 16:41:31 jon Exp $")

/*
 * Generate U and V values for a model's vertices.
 *
 * The vertices are optionally transformed by a 3x4 matrix
 * 
 * The u,v values are then assigned according to one of the
 * following schemes -
 *
 * Planar mapping:
 * 	u = (x+1)/2
 *  v = (y+1)/2
 *
 * Spherical mapping:
 * 	u = atan2(-z,x)/2*pi
 *  v = 1-atan2(sqrt(x*x+z*z),y)/pi
 *
 * Cylindrical mapping:
 * 	u = atan2(-z,x)/2*pi
 *  v = (y+1)/2
 *
 * Disc mapping:
 * 	u = atan2(-z,x)/2*pi
 *  v = sqrt(x*x+z*z)
 */
void BR_PUBLIC_ENTRY BrModelApplyMap(br_model *model,int map_type, br_matrix34 *xform)
{
	int v;
	br_vertex *vp;
	br_vector3 mv;
	br_matrix34 default_xform;
	br_scalar d;

	UASSERT_MESSAGE("BrModelApplyMap NULL pointer to a model", model != NULL);
	UASSERT(model->vertices != NULL);
	UASSERT(map_type >= 0 && map_type <= BR_APPLYMAP_NONE);
	
	/*
	 * If no default provided, use identity
	 */
	if(xform == NULL) {
		BrMatrix34Identity(&default_xform);
		xform = &default_xform;
	}

	/*
	 * Set mapping for each vertex
	 */
	for(v=0, vp=model->vertices; v < model->nvertices; v++, vp++) {
		BrMatrix34ApplyP(&mv,&vp->p,xform);

		switch(map_type) {
		case BR_APPLYMAP_NONE:
			/*
			 * No mapping:
			 * 	u = 0
			 * 	v = 0
			 */
			vp->map.v[0] = BR_SCALAR(0.0);
			vp->map.v[1] = BR_SCALAR(0.0);
			break;

		case BR_APPLYMAP_PLANE:
			/*
			 * Planar mapping:
			 * 	u = (x+1)/2
			 *  v = (y+1)/2
			 */
			vp->map.v[0] = BR_MUL(mv.v[0]+BR_SCALAR(1.0),
				BrFractionToScalar(BR_FRACTION(0.5)));

			vp->map.v[1] = BR_MUL(mv.v[1]+BR_SCALAR(1.0),
				BrFractionToScalar(BR_FRACTION(0.5)));
			break;

		case BR_APPLYMAP_SPHERE:
			/*
			 * Spherical mapping:
			 * 	u = atan2(-z,x)/2*pi
			 *  v = 1-atan2(sqrt(x*x+z*z),y)/pi
			 */
			vp->map.v[0] = BR_DIV(
				BrAngleToDegree(BR_ATAN2(-mv.v[2],mv.v[0])),
				BR_SCALAR(360.0));
			
			d = BR_LENGTH2(mv.v[0],mv.v[2]);

			vp->map.v[1] = BR_SCALAR(1.0) - BR_DIV(
				BrAngleToDegree(BR_ATAN2(d,mv.v[1])),
				BR_SCALAR(180.0));

			break;

		case BR_APPLYMAP_CYLINDER:
			/*
			 * Cylindrical mapping:
			 * 	u = atan2(-z,x)/2*pi
			 *  v = (y+1)/2
			 */
			vp->map.v[0] = BR_DIV(
				BrAngleToDegree(BR_ATAN2(-mv.v[2],mv.v[0])),
				BR_SCALAR(360.0));
			
			vp->map.v[1] = BR_MUL(mv.v[1]+BR_SCALAR(1.0),
				BrFractionToScalar(BR_FRACTION(0.5)));
			break;


		case BR_APPLYMAP_DISC:
			/*
			 * Disc mapping:
			 * 	u = atan2(-y,x)/2*pi
			 *  v = sqrt(x*x+y*y)
			 */
			vp->map.v[0] = BR_DIV(
				BrAngleToDegree(BR_ATAN2(-mv.v[1],mv.v[0])),
				BR_SCALAR(360.0));
			
			vp->map.v[1] = BR_LENGTH2(mv.v[0],mv.v[1]);
			break;
		}
	}
}

/*
 * Work out a transform such that it can be fed to BrModelApplyMap
 * and have the mapping type fit the bounds of the model exactly
 *
 * The two axis along which the mapping can be applied are provided -
 *
 *	BR_FITMAP_PLUS_X,
 *	BR_FITMAP_PLUS_Y,
 *	BR_FITMAP_PLUS_Z,
 *	BR_FITMAP_MINUS_X,
 *	BR_FITMAP_MINUS_Y,
 *	BR_FITMAP_MINUS_Z,
 *
 * Returns the supplied pointer to 'transform'
 */

br_matrix34 * BR_PUBLIC_ENTRY BrModelFitMap(br_model *model, int axis_0, int axis_1, br_matrix34 *transform)
{
	br_vector3 axis_3;
	br_vector3 tr;
	br_vector3 sc;
	int i;

	static br_vector3 axis_vectors[] = {
		BR_VECTOR3( 1, 0, 0),				/* +X */
		BR_VECTOR3( 0, 1, 0),				/* +Y */
		BR_VECTOR3( 0, 0, 1),				/* +Z */
		BR_VECTOR3(-1, 0, 0),				/* -X */
		BR_VECTOR3( 0,-1, 0),				/* -Y */
		BR_VECTOR3( 0, 0,-1),				/* -Z */
	};

	UASSERT(model != NULL);
	UASSERT(transform != NULL);
	UASSERT(axis_0 >= BR_FITMAP_PLUS_X && axis_0 <= BR_FITMAP_MINUS_Z);
	UASSERT(axis_1 >= BR_FITMAP_PLUS_X && axis_1 <= BR_FITMAP_MINUS_Z);

	/*
	 * Start the mapping transform by filling in the two user
	 * supplied axes (as columns), and then using a cross product to generate
	 * the third column.
	 */
	BrMatrix34Identity(transform);

	BrVector3Cross(&axis_3,
		axis_vectors + axis_0,
		axis_vectors + axis_1);

	for(i=0; i< 3; i++) {
		transform->m[i][0] = axis_vectors[axis_0].v[i];
		transform->m[i][1] = axis_vectors[axis_1].v[i];
		transform->m[i][2] = axis_3.v[i];
	}

	/*
	 * Find the translation to align the object with the mapping
	 */
	for(i=0; i< 3; i++)
		tr.v[i] = -(BR_CONST_DIV(model->bounds.max.v[i],2) +
				    BR_CONST_DIV(model->bounds.min.v[i],2));

	/*
	 * Find the scale to fit object to the mapping
	 */
	for(i=0; i< 3; i++)
		if(model->bounds.max.v[i] != model->bounds.min.v[i])
			sc.v[i] = BR_RCP(BR_CONST_DIV(model->bounds.max.v[i],2) -
					  		 BR_CONST_DIV(model->bounds.min.v[i],2));
		else
			sc.v[i] = BR_SCALAR(1.0);

	BrMatrix34PreScale(transform,sc.v[0],sc.v[1],sc.v[2]);
	BrMatrix34PreTranslate(transform,tr.v[0],tr.v[1],tr.v[2]);

	return transform;
}

/*
 * Free a model
 */
void BR_PUBLIC_ENTRY BrModelFree(br_model *m)
{
	UASSERT(m != NULL);

	BrResFree(m);
}

/*
 * Allocate a model of a given size
 */
br_model * BR_PUBLIC_ENTRY BrModelAllocate(char *name, int nvertices, int nfaces)
{
	br_model *m;

	UASSERT(nvertices >= 0);
	UASSERT(nfaces >= 0);

	m = BrResAllocate(v1db.res,sizeof(*m),BR_MEMORY_MODEL);

	m->nvertices = nvertices;
	m->nfaces = nfaces;

	if(name)
		m->identifier = BrResStrDup(m, name);

	if(nvertices)
		m->vertices = BrResAllocate(m, nvertices*sizeof(*m->vertices),BR_MEMORY_VERTICES);

	if(nfaces)
		m->faces = BrResAllocate(m, nfaces * sizeof(*m->faces),BR_MEMORY_FACES);

	return m;
}

/*
 * Allocate a primitive list of the given type and size.
 */
br_primitive_list * BR_PUBLIC_ENTRY BrPrimitiveListAllocate(br_uint_32 prim_type, br_uint_16 num_prims)
{
    return(NULL);
}


/*
 * Add the primitive list passed to the model passed.
 */
br_uint_32 BR_PUBLIC_ENTRY BrModelAddPrimitiveList(br_model *model, br_primitive_list *primitive_list)
{
    return(0);
}

