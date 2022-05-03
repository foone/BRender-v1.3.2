/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: actsupt.c 1.4 1998/11/17 16:01:01 nikh Exp $
 * $Locker: $
 *
 * Actor support routines 
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"
#include "datafile.h"
#include "math_ip.h"

BR_RCS_ID("$Id: actsupt.c 1.4 1998/11/17 16:01:01 nikh Exp $")


/*
 * Invoke a callback for each one of an actors children
 */
br_uint_32 BR_PUBLIC_ENTRY BrActorEnum(br_actor *parent,
	br_actor_enum_cbfn *callback, void *arg)
{
	br_actor *a,*next;
	br_uint_32 r;

	UASSERT_MESSAGE("NULL pointer to the actor whose children are to be enumerated", parent != NULL);
	UASSERT_MESSAGE("NULL pointer to the callback function to be called for each child actor", callback != NULL);

	a = parent->children;

	while(a) {
		next = a->next;

		if((r = callback(a,arg)))
			return r;

		a = next;
	}

	return 0;
}

/*
 * Find all the named actors matching a pattern in the given hierachy
 *
 */
br_uint_32 BR_PUBLIC_ENTRY BrActorSearchMany(br_actor *root, char *pattern, br_actor **actors, int max)
{
	br_actor *a;
	char *sub = NULL;
	int n,remaining = max;

	UASSERT_MESSAGE("NULL pointer to an actor", root != NULL);
	UASSERT_MESSAGE("NULL pointer to a series of actor pointers", actors != NULL);

	/*
	 * Take first component of path, and match it against all children
	 * of the root actor
	 */
    if(pattern) {
        while(*pattern =='/')
	    	pattern++;

    	for(sub=pattern; (*sub != '\0') && (*sub != '/'); sub++)
    		;

    	while(*sub == '/')
	    	sub++;
    }

	BR_FOR_SIMPLELIST(&root->children,a) {

		/*
		 * Abort if no more space for results
		 */
		if(remaining <= 0)
			break;

		if(BrNamePatternMatch(pattern,a->identifier)) {
			/*
			 * If there is more path
			 * 		recurse with remainder of path
			 * else
			 * 		add actor to accumulated list
			 */

			if(sub && *sub != '\0') {
				n = BrActorSearchMany(a,sub,actors,remaining);
				actors += n;
				remaining -= n;
			} else {
				*actors++ = a;
				remaining--;
			}
		}
	}

	return max - remaining;
}

/*
 * Find the first named actor matching a pattern in the given hierachy
 */
br_actor * BR_PUBLIC_ENTRY BrActorSearch(br_actor *root, char *pattern)
{
	br_actor *a;


    UASSERT_MESSAGE("NULL pointer to an actor", root != NULL);

	if(BrActorSearchMany(root,pattern,&a,1) == 1)
		return a;
	else
		return NULL;
}

/*
 * Recursive function to propgate depth number down a hierachy
 */
STATIC void RenumberActor(br_actor *a,int d)
{
	br_actor *ac;

        ASSERT_MESSAGE("NULL pointer to an actor ", a != NULL);

	a->depth = d;

        BR_FOR_SIMPLELIST(&a->children,ac)

        	RenumberActor(ac,d+1);
}


/*
 * Add an actor to the children of the given parent
 */

#if !(defined(_MSC_VER) && !defined(_USE_INTEL_COMPILER))
br_actor * BR_PUBLIC_ENTRY BrActorAddNoRenumber(br_actor *parent, br_actor *a)
{
	/*
	 * Link this actor into sibling list of parent
	 */
	BR_SIMPLEADDHEAD(&parent->children,a);
	a->parent = parent;

	return a;
}
#endif


br_actor * BR_PUBLIC_ENTRY BrActorAdd(br_actor *parent, br_actor *a)
{
	br_actor *ac;

	UASSERT_MESSAGE("NULL pointer to the parent's new child", a != NULL);
	UASSERT_MESSAGE("NULL pointer to the parental actor", parent != NULL);
	UASSERT(a->prev == NULL);

	BrActorAddNoRenumber(parent,a);

	/*
	 * Update depth for added hierachy
	 */
	a->depth = parent->depth+1;

	BR_FOR_SIMPLELIST(&a->children,ac)
		RenumberActor(ac,a->depth+1);
	
	return a;
}

#if !(defined(_MSC_VER) && !defined(_USE_INTEL_COMPILER))
br_actor * BR_PUBLIC_ENTRY BrActorRemoveNoRenumber(br_actor *a)
{
	BR_SIMPLEREMOVE(a);

	a->parent = NULL;

	return a;
}
#endif

br_actor * BR_PUBLIC_ENTRY BrActorRemove(br_actor *a)
{
	br_actor *ac;

	UASSERT_MESSAGE("NULL pointer to the hierrachy to remove", a != NULL);
	UASSERT(a->prev != NULL);
	
	BrActorRemoveNoRenumber(a);
	/*
	 * Renumber the removed hierachy
	 */
	a->depth = 0;

	BR_FOR_SIMPLELIST(&a->children,ac)
		RenumberActor(ac,1);

	return a;
}
/*
 * Move an actor in the hierachy, but preserve it's apparent world
 * transform by manipulating the transform
 */
void BR_PUBLIC_ENTRY BrActorRelink(br_actor *parent,br_actor *a)
{
	br_matrix34 mat;

	UASSERT_MESSAGE("NULL pointer to the new parent", parent != NULL);
	UASSERT_MESSAGE("NULL pointer to the actor to move", a != NULL);

	/*
	 * Catch simple case
	 */
	if(a->parent == parent)
		return;

	/*
	 * Find the transform we need
	 */
	BrActorToActorMatrix34(&mat,a,parent);

	/*
	 * Convert matrix into actors transform
	 */
	BrMatrix34ToTransform(&a->t,&mat);

	/*
	 * Re-attach into the hierachy
	 */
	BrActorAdd(parent,BrActorRemove(a));
}

/*
 * Allocate a new actor structure and return a pointer to it
 *
 * Model actors are allocated from a pool associated with the renderer -
 * this is a very common case and is worth the speedup
 *
 * All other actor types come from the normal heap - and include any
 * type specific data bocks
 *
 */
br_actor * BR_PUBLIC_ENTRY BrActorAllocate(br_uint_8 type, void *type_data)
{
	br_actor *a = NULL;
	br_light *light;
	br_camera *camera;
	br_bounds *bounds;
	br_vector4 *clip_plane;

	UASSERT(type < BR_ACTOR_MAX);

	a = BrResAllocate(v1db.res,sizeof(*a),BR_MEMORY_ACTOR);

	/*
	 * Initialise common fields
	 */
	BrSimpleNewList((struct br_simple_list *) &a->children);
	a->type = type;
	a->depth = 0;
	a->t.type = BR_TRANSFORM_MATRIX34;
	BrMatrix34Identity(&a->t.t.mat);

	if(type_data) {

		/*
		 * Fill in the type_data pointer
		 */
		a->type_data = type_data;

	} else {
		/*
		 * No type data - allocate a default
		 */
		switch(type) {

		case BR_ACTOR_LIGHT:
			/*
		 	 * Setup to be a directional light source
			 */
			light = BrResAllocate(a, sizeof(*light), BR_MEMORY_LIGHT);
			a->type_data = light;

			light->type = BR_LIGHT_DIRECT;
			light->colour = BR_COLOUR_RGB(255,255,255);
			light->attenuation_c = BR_SCALAR(1.0);
			light->cone_outer = BR_ANGLE_DEG(15.0);
			light->cone_inner = BR_ANGLE_DEG(10.0);
			break;

		case BR_ACTOR_CAMERA:
			/*
	  		 * Perspective camera, 45 degree FOV
			 */
			camera = BrResAllocate(a, sizeof(*camera), BR_MEMORY_CAMERA);
			a->type_data = camera;

			camera->type = BR_CAMERA_PERSPECTIVE;
			camera->field_of_view = BR_ANGLE_DEG(45);
			camera->hither_z = BR_SCALAR(1.0);
			camera->yon_z = BR_SCALAR(10.0);
			camera->aspect = BR_SCALAR(1.0);
			break;

		case BR_ACTOR_BOUNDS:
		case BR_ACTOR_BOUNDS_CORRECT:
			/*
			 * Allocate a bounds structure
			 */
			bounds = BrResAllocate(a, sizeof(*bounds), BR_MEMORY_BOUNDS);
			a->type_data = bounds;
			break;

		case BR_ACTOR_CLIP_PLANE:
			/*
			 * Allocate a 4-vector
			 */
			clip_plane = BrResAllocate(a, sizeof(*clip_plane), BR_MEMORY_BOUNDS);
			clip_plane->v[0] = BR_SCALAR(0.0);
			clip_plane->v[1] = BR_SCALAR(0.0);
			clip_plane->v[2] = BR_SCALAR(1.0);
			clip_plane->v[3] = BR_SCALAR(0.0);
			a->type_data = clip_plane;
			break;
		}
	}

	return a;
}

/*
 * Free an actor
 */
STATIC void InternalActorFree(br_actor *a)
{

    UASSERT_MESSAGE("NULL actor pointer", a !=NULL);
    UASSERT(a->prev == NULL);

	/*
	 * Release any children
	 */
	while(BR_SIMPLEHEAD(&a->children))
		InternalActorFree(BR_SIMPLEREMOVE(a->children));

	/*
	 * See if this is an 'enabled' actor
	 */
	BrActorEnableCheck(a);

	/*
	 * Release this actor
	 */
	BrResFree(a);
}

void BR_PUBLIC_ENTRY BrActorFree(br_actor *a)
{

   UASSERT_MESSAGE("NULL actor pointer", a !=NULL);
   UASSERT_MESSAGE("Actor still in hierarchy, call BrActorRemove.", a->prev == NULL );

	InternalActorFree(a);
}

/*
 * Accumulate the transform between an actor and the root
 *
 * Returns a flag to indicate if the actor did have world
 * as an ancestor
 */
br_boolean ActorToRoot(br_actor *a, br_actor *world, br_matrix34 *m)
{
	UASSERT(a != NULL);
	UASSERT(world != NULL);
	UASSERT(m != NULL);

	/*
	 * Catch stupid case
	 */
	if(a == world) {
		BrMatrix34Identity(m);
		return BR_TRUE;
	}

	/*
	 * Start with actor's transform
	 */
	BrMatrix34Transform(m,&a->t);
	a = a->parent;

	/*
	 * Accumulate all the transforms up to the root (ignoring any
	 * identity transforms)
	 */
	for( ; a && a != world; a = a->parent )
		if(a->t.type != BR_TRANSFORM_IDENTITY)
			BrMatrix34PostTransform(m,&a->t);

	return (a == world);
}

br_boolean ActorToRootTyped(br_actor *a, br_actor *world, br_matrix34 *m, br_int_32 *type)
{
	br_int_32 t;

	UASSERT(a != NULL);
	UASSERT(world != NULL);
	UASSERT(m != NULL);

	/*
	 * Catch stupid case
	 */
	if(a == world) {
		BrMatrix34Identity(m);
		*type = BR_TRANSFORM_IDENTITY;
		return BR_TRUE;
	}

	/*
	 * Start with actor's transform
	 */
	BrMatrix34Transform(m,&a->t);
	t = a->t.type;
	a = a->parent;

	/*
	 * Accumulate all the transforms up to the root (ignoring any
	 * identity transforms)
	 */
	for( ; a && a != world; a = a->parent ) {
		if(a->t.type != BR_TRANSFORM_IDENTITY) {
			BrMatrix34PostTransform(m,&a->t);
			t = BrTransformCombineTypes(t,a->t.type);
		}
	}

	*type = t;

	return (a == world);
}

void Matrix4PerspectiveNew(br_matrix4 *mat, 
		br_angle field_of_view,
		br_scalar aspect,
		br_scalar hither,
		br_scalar yon,
		br_scalar origin_x,
		br_scalar origin_y)
{
   br_scalar scale ;

   UASSERT_MESSAGE("NULL pointer to destination matrix to receive the perspective transform", mat != NULL);
   UASSERT_MESSAGE("field_of view is zero", field_of_view != 0 );
   UASSERT(hither <= BR_SCALAR(0.0)); /* hither and yon are in view space - view volume is along -ve Z */
   UASSERT_MESSAGE("Divide by zero error", BR_SIN((br_angle)(field_of_view/2)) != 0);

   scale = BR_DIV( BR_COS((br_angle)(field_of_view/2)),
		   BR_SIN((br_angle)(field_of_view/2)));

    UASSERT_MESSAGE("Divide by zero error, aspect=0", aspect != 0);
	M(0,0) = BR_DIV(scale,aspect);
    M(1,1) = scale;
    UASSERT_MESSAGE("Divide by zero error, yon-hither=0", yon-hither != 0);
	M(2,2) = BR_DIV((yon+hither),(yon-hither));
    M(3,2) = BR_CONST_MUL(BR_MULDIV(yon,hither,(yon-hither)),-2);

    M(0,1) = S0; M(0,2) = S0; M(0,3) = S0;
	M(1,0) = S0; M(1,2) = S0; M(1,3) = S0;
    M(2,0) = -origin_x; M(2,1) = -origin_y; M(2,3) = BR_SCALAR(-1);
	M(3,0) = S0; M(3,1) = S0; M(3,3) = S0;
}

/*
 * Work out a camera 4x4 matrix
 *
 * Return type of transform, one of BR_VTOS_*
 */
br_token CameraToScreenMatrix4(br_matrix4 *mat, br_actor *camera)
{
	br_camera *camera_type;
	br_matrix34 mat34;

    UASSERT_MESSAGE("NULL pointer to destination matrix to receive the perspective transform", mat != NULL);
	UASSERT_MESSAGE("NULL pointer to the camera", camera != NULL);
	UASSERT_MESSAGE("Invalid camera", camera->type == BR_ACTOR_CAMERA);
	UASSERT_MESSAGE("Invalid camera data", camera->type_data != NULL);
	
	camera_type = camera->type_data;

	UASSERT_MESSAGE("Invalid camera data range", camera_type->hither_z > S0);
	UASSERT_MESSAGE("Invalid camera data condition1", camera_type->hither_z < camera_type->yon_z);

	switch(camera_type->type) {

	case BR_CAMERA_PERSPECTIVE_FOV:
		Matrix4PerspectiveNew(mat,
			camera_type->field_of_view,camera_type->aspect,
			-camera_type->hither_z,-camera_type->yon_z,
			v1db.origin.v[0], v1db.origin.v[1]);

		return BRT_PERSPECTIVE;

	case BR_CAMERA_PERSPECTIVE_FOV_OLD:

		BrMatrix4Perspective(mat,
			camera_type->field_of_view,camera_type->aspect,
			-camera_type->hither_z,-camera_type->yon_z);

		return BRT_PERSPECTIVE;

	case BR_CAMERA_PERSPECTIVE_WHD:

		/* */

		return BRT_PERSPECTIVE;

	case BR_CAMERA_PARALLEL:

		if(camera_type->width == BR_SCALAR(0.0))
			BR_ERROR0("Parallel camera has zero width");

		if(camera_type->height == BR_SCALAR(0.0))
			BR_ERROR0("Parallel camera has zero height");

		if(camera_type->yon_z <= camera_type->hither_z)
			BR_ERROR0("Parallel camera has invalid yon and hither");

		BrMatrix34Scale(&mat34,
			BR_CONST_MUL(BR_RCP(BR_MUL(camera_type->width,camera_type->aspect)),2),
			BR_CONST_MUL(BR_RCP(camera_type->height),2),
			BR_CONST_MUL(BR_RCP(BR_SUB(camera_type->yon_z,camera_type->hither_z)),2)	);
		BrMatrix34PreTranslate(&mat34,S0,S0,BR_CONST_DIV(BR_ADD(camera_type->hither_z,camera_type->yon_z),2)	);

		BrMatrix34PostTranslate(&mat34,v1db.origin.v[0],v1db.origin.v[1],S0);

		BrMatrix4Copy34(mat,&mat34);

		return BRT_PARALLEL;

	case BR_CAMERA_PARALLEL_OLD:

		if(camera_type->width == BR_SCALAR(0.0))
			BR_ERROR0("Parallel camera has zero width");

		if(camera_type->height == BR_SCALAR(0.0))
			BR_ERROR0("Parallel camera has zero height");

		if(camera_type->yon_z <= camera_type->hither_z)
			BR_ERROR0("Parallel camera has invalid yon and hither");

		BrMatrix34Scale(&mat34,
			BR_RCP(camera_type->width),
			BR_RCP(camera_type->height),
			BR_RCP(camera_type->yon_z - camera_type->hither_z));
		BrMatrix34PreTranslate(&mat34,S0,S0,camera_type->hither_z);

		BrMatrix4Copy34(mat,&mat34);

		return BRT_PARALLEL;

	}

	return 0;
}


/*
 * Accumulate the transform between one actor an another
 *
 * Returns a transform type that matches the combined result according
 * to the combination rules in transfrm.c
 *
 */
br_uint_16 BR_PUBLIC_ENTRY BrActorToActorMatrix34(br_matrix34 *m, br_actor *a, br_actor *b)
{
	br_matrix34 mata,matb,matc;
	br_uint_8 at,bt;

        UASSERT_MESSAGE("NULL pointer to the destination matrix for the transform", m != NULL);
        UASSERT_MESSAGE("NULL pointer to actor from whose co-ordinate space, co-ordinates are to be transformed", a != NULL);
        UASSERT_MESSAGE("NULL pointer to actor into whose co-ordinate space, co-ordinates are to be transformed", b != NULL);


	/*
	 * Spot trivial cases
	 */
	 /*
	  * Same actor
	  */
	if(a == b) {
		BrMatrix34Identity(m);
		return BR_TRANSFORM_IDENTITY;		
	}

	 /*
	  * A is child of B
	  */
	if(a->parent == b) {
		BrMatrix34Transform(m,&a->t);
		return a->t.type;
	}

	 /*
	  * B is child of A
	  */
	if(b->parent == a) {
		BrMatrix34Transform(&matb,&b->t);

		if(BrTransformTypeIsLP(b->t.type))
			BrMatrix34LPInverse(m,&matb);
		else
			BrMatrix34Inverse(m,&matb);

		return b->t.type;
	}

	/*
	 * Starting at the lowest actor, move up, accumulating transforms
	 * until we get to a common ancestor
	 */

	at = BR_TRANSFORM_IDENTITY;
	BrMatrix34Identity(&mata);

	bt = BR_TRANSFORM_IDENTITY;
	BrMatrix34Identity(&matb);

	while( a && b && a != b) {

		if(a->depth > b->depth) {

			if(a->t.type != BR_TRANSFORM_IDENTITY) {
				BrMatrix34PostTransform(&mata,&a->t);
				at = BrTransformCombineTypes(at,a->t.type);
			}
			a=a->parent;

		} else	if(b->depth > a->depth) {

			if(b->t.type != BR_TRANSFORM_IDENTITY) {
				BrMatrix34PostTransform(&matb,&b->t);
				bt = BrTransformCombineTypes(bt,b->t.type);
			}
			b=b->parent;

		} else {

			if(a->t.type != BR_TRANSFORM_IDENTITY) {
				BrMatrix34PostTransform(&mata,&a->t);
				at = BrTransformCombineTypes(at,a->t.type);
			}
			if(b->t.type != BR_TRANSFORM_IDENTITY) {
				BrMatrix34PostTransform(&matb,&b->t);
				bt = BrTransformCombineTypes(bt,b->t.type);
			}
			b=b->parent;
			a=a->parent;

		}
	}

	/*
	 * We now have the tranforms from the actors up to a common parent
	 * compose A with inverse(B) to get A->B
	 */
	if(bt == BR_TRANSFORM_IDENTITY) {
		BrMatrix34Copy(m,&mata);
		return at;

	}

	if(at == BR_TRANSFORM_IDENTITY) {
		if(BrTransformTypeIsLP(bt))  {
			BrMatrix34LPInverse(m,&matb);
		} else
			BrMatrix34Inverse(m,&matb);

		return bt;
	}

	if(BrTransformTypeIsLP(bt)) {
		BrMatrix34LPInverse(&matc,&matb);
	} else
		BrMatrix34Inverse(&matc,&matb);

	BrMatrix34Mul(m,&mata,&matc);

	return BrTransformCombineTypes(at,bt);
}

/*
 * Accumulate the transform between an actor and the screen
 *
 */
void BR_PUBLIC_ENTRY BrActorToScreenMatrix4(br_matrix4 *m, br_actor *a, br_actor *camera)
{
	br_matrix34 a2c;

	UASSERT_MESSAGE("NULL pointer to the destination matrix", m != NULL);
	UASSERT_MESSAGE("NULL pointer to an actor", a != NULL);
	UASSERT_MESSAGE("NULL pointer to a camera actor", camera != NULL);


	/*
	 * Build camera to screen
	 */
	CameraToScreenMatrix4(m,camera);

	/*
	 * Only do more work if the actor is not the camera itself
	 */
	if(a != camera) {
			BrActorToActorMatrix34(&a2c,a,camera);
			BrMatrix4Pre34(m,&a2c);
	}
}

/*
 * Find the bounding box of an actor and all it's children
 *
 * 
 */
STATIC void ActorToBounds(br_bounds *dest, br_actor *ap, br_model *model)
{
	br_actor *a;
	br_bounds new;
	br_matrix34 m2v;
	int i;

        ASSERT_MESSAGE("NULL pointer to the resulting bounding box", dest != NULL);
        ASSERT_MESSAGE("NULL pointer to actor", ap != NULL);
        ASSERT(model != NULL);

	/*
	 * Propogate default model
	 */
	if(ap->model != NULL)
		model = ap->model;

	/*
	 * Prepend this transform
	 */
	m2v = v1db.model_to_view;
	BrMatrix34PreTransform(&v1db.model_to_view,&ap->t);

	/*
	 * If actor has bounds, transform and compare
	 */
	if(ap->type == BR_ACTOR_MODEL) {
		BrMatrix34ApplyBounds(&new,&model->bounds,&v1db.model_to_view);

		for(i=0; i < 3; i++) {
			if(new.min.v[i] < dest->min.v[i])
				dest->min.v[i] = new.min.v[i];

			if(new.max.v[i] > dest->max.v[i])
				dest->max.v[i] = new.max.v[i];

		}
	}

	/*
	 * Recurse into children
	 */
	BR_FOR_SIMPLELIST(&ap->children,a)
		ActorToBounds(dest,a,model);

	v1db.model_to_view = m2v;
}

br_bounds * BR_PUBLIC_ENTRY BrActorToBounds( br_bounds *b, br_actor *ap)
{
	br_matrix34 m2v = v1db.model_to_view;
	br_model  *model;
	br_actor *a;

        UASSERT_MESSAGE("NULL pointer to the resulting bounding box", b != NULL);
        UASSERT_MESSAGE("NULL pointer to actor whose bounding box is required", ap != NULL);

	if(ap->model == NULL)
		model = v1db.default_model;
	else
		model = ap->model;

	BrMatrix34Identity(&v1db.model_to_view);

	/*
	 * If actor has bounds, transform and compare
	 */
	if(ap->type == BR_ACTOR_MODEL) {
		*b = model->bounds;
	} else {
		/*
		 * Start with empty bounds
		 */
		b->min.v[0] = b->min.v[1] = b->min.v[2] = BR_SCALAR_MAX;
		b->max.v[0] = b->max.v[1] = b->max.v[2] = BR_SCALAR_MIN;
	}

	/*
	 * Recurse into children
	 */
	BR_FOR_SIMPLELIST(&ap->children,a)
		ActorToBounds(b,a,model);

	v1db.model_to_view = m2v;
	return b;
}

