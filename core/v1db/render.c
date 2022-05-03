/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: render.c 1.9 1998/11/12 13:16:45 johng Exp $
 * $Locker: $
 *
 * Traversal of hierachy for rendering
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"
#include "datafile.h"

#include "math_ip.h"

BR_RCS_ID("$Id: render.c 1.9 1998/11/12 13:16:45 johng Exp $")

static void actorRenderOnScreen(br_actor *ap,
								br_model *model,
								br_material *material,
								void *render_data,
								br_uint_8 style,
								br_uint_16 t);

void BR_PUBLIC_ENTRY BrDbModelRender(br_actor *actor,
				  br_model *model,
				  br_material *material,
				  void *render_data,
				  br_uint_8 style,
				  int on_screen,
				  int use_custom)
{
	br_int_32 count;
	br_token_value tv[] = {
		{BRT_V1INSERT_FUNCTION_P,	0},
		{BRT_V1INSERT_ARG1_P,	0},
		{BRT_V1INSERT_ARG2_P,	0},
		{BRT_V1INSERT_ARG3_P,	0},
		{0},
	};

	UASSERT(v1db.rendering != 0);
	UASSERT(model != NULL);
	UASSERT(material != NULL);
	UASSERT(actor != NULL);

	/*
	 * Mark local copy of model_to_screen as invalid
	 */
	v1db.model_to_screen_valid = BR_FALSE;

	/*
	 * If model has custom callback, keep following it until
	 * a model is reached
	 */
	if(use_custom && (model->flags & BR_MODF_CUSTOM)) {
		/*
		 * XXX Fetch current transforms from renderer
		 */
		model->custom(actor, model, material, render_data, style, on_screen);
		return;

	}

	if(model->prepared == NULL && model->stored == NULL)
		BR_ERROR1("Tried to render un-prepared model %s",model->identifier?model->identifier:"<NULL>");

	/*
	 * Optional preparation for Z-Sort
	 */
	if(render_data != NULL) {
		br_order_table *ot = render_data;

		SetOrderTableBounds(&model->bounds, ot);

		if(ot->visits == 0) {
			BrZsOrderTableClear(ot);
			InsertOrderTableList(ot);
		}
	
		ot->visits++;

		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0,
			BRT_V1ORDER_TABLE_P, (br_uint_32) ot);

		/*
		 * See if a 'primitive insertion' function needs to be added
		 */
		if(v1db.primitive_call) {

			tv[0].v.p = v1db.primitive_call;
			tv[1].v.p = actor;
			tv[2].v.p = model;
			tv[3].v.p = material;

			RendererPartSetMany(v1db.renderer, BRT_HIDDEN_SURFACE, 0, tv, &count);
		}
	}

	if(v1db.bounds_call) {
		br_int_32 c;
		char buffer[sizeof(br_vector2)*2];
		br_token_value tv[] = {
			{BRT_AS_VECTOR2_SCALAR(MIN), 0},
			{BRT_AS_VECTOR2_SCALAR(MAX), 0},
			{0,},
		};

		br_int_32 int_bounds[4];

		/*
		 * Clear bounds
		 */
		RendererStateDefault(v1db.renderer, BR_STATE_BOUNDS);

		RenderStyleCalls[style](actor, model, material, render_data, style, on_screen);

		/*
	 	 * Fetch bounds and call user op.
		 */
		RendererPartQueryMany(v1db.renderer, BRT_BOUNDS, 0, tv, buffer, sizeof(buffer), &c);

		int_bounds[0] = BrScalarToInt(((br_vector3 *)tv[0].v.p)->v[0]);
		int_bounds[1] = BrScalarToInt(((br_vector3 *)tv[0].v.p)->v[1]);
		int_bounds[2] = BrScalarToInt(((br_vector3 *)tv[1].v.p)->v[0]);
		int_bounds[3] = BrScalarToInt(((br_vector3 *)tv[1].v.p)->v[1]);

		/*
		 * Clamp to screen boundary
		 */
		if(int_bounds[0] < 0)
			int_bounds[0] = 0;
		if(int_bounds[1] < 0)
			int_bounds[1] = 0;
		if(int_bounds[2] < 0)
			int_bounds[2] = 0;
		if(int_bounds[3] < 0)
			int_bounds[3] = 0;

		if(int_bounds[0] >= v1db.colour_buffer->width)
			int_bounds[0] = v1db.colour_buffer->width-1;

		if(int_bounds[1] >= v1db.colour_buffer->height)
			int_bounds[1] = v1db.colour_buffer->height-1;

		if(int_bounds[2] >= v1db.colour_buffer->width)
			int_bounds[2] = v1db.colour_buffer->width-1;

		if(int_bounds[3] >= v1db.colour_buffer->height)
			int_bounds[3] = v1db.colour_buffer->height-1;

		/*
		 * Make relative to screen origon
		 */
		int_bounds[0] -= v1db.colour_buffer->origin_x;
		int_bounds[1] -= v1db.colour_buffer->origin_y;
		int_bounds[2] -= v1db.colour_buffer->origin_x;
		int_bounds[3] -= v1db.colour_buffer->origin_y;


		if((int_bounds[0] <= int_bounds[2]) &&
		   (int_bounds[1] <= int_bounds[3]) ) {
			/*
			 * XXX Must make sure model_to_screen is up to date
			 */
			v1db.bounds_call(actor, model, material, render_data, style, &v1db.model_to_screen,	int_bounds);
		}

	} else {
		RenderStyleCalls[style](actor, model, material, render_data, style, on_screen);
	}
}

br_uint_32 BR_PUBLIC_ENTRY BrOnScreenCheck(br_bounds3 *bounds)
{
	br_token r;
    UASSERT_MESSAGE("Invalid BrOnScreenCheck pointer", bounds != NULL);
	RendererBoundsTest(v1db.renderer, &r, (void *)bounds);

	return r;
}

/*
 * Prepend an actor's transform onto the model->view transform and return the new combined transform
 * type
 */
static br_uint_16 prependActorTransform(br_actor *ap, br_uint_16 t)
{
	br_matrix34 mt;
    ASSERT_MESSAGE("Invalid prependActorTransform pointer", ap != NULL);

#if 0
	/*
	 * See if this actor is on the camera path - if so, generate a new transform
	 */
	if(ap == v1db.camera_path[ap->depth].a) {

		RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
			BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW), (br_uint_32)&v1db.camera_path[ap->depth].m);
		t = v1db.camera_path[ap->depth].transform_type;

		RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
			BRT_MODEL_TO_VIEW_HINT_T, BrTransformTypeIsLP(t)?BRT_LENGTH_PRESERVING:BRT_NONE);

		RendererModelInvert(v1db.renderer);
	} else {
		if(BrTransformTypeIsMatrix34(ap->t.type))
			RendererModelMul(v1db.renderer, (void *)&ap->t.t.mat);
		else {
			BrTransformToMatrix34(&mt, &ap->t);
			RendererModelMul(v1db.renderer, (void *)&mt);
		}
		t = BrTransformCombineTypes(t, ap->t.type);

		RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
			BRT_MODEL_TO_VIEW_HINT_T, BrTransformTypeIsLP(t)?BRT_LENGTH_PRESERVING:BRT_NONE);
	}
#else
	if(BrTransformTypeIsMatrix34(ap->t.type)) {
		RendererModelMul(v1db.renderer, (void *)&ap->t.t.mat);
	} else {
		BrTransformToMatrix34(&mt, &ap->t);
		RendererModelMul(v1db.renderer, (void *)&mt);
	}
	t = BrTransformCombineTypes(t, ap->t.type);

	RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
		BRT_MODEL_TO_VIEW_HINT_T, BrTransformTypeIsLP(t)?BRT_LENGTH_PRESERVING:BRT_NONE);
#endif

	return t;
}

static br_uint_16 prependMatrix(br_matrix34 *mat, br_uint_16 mat_t, br_uint_16 t)
{
    ASSERT_MESSAGE("Invalid prependMatrix pointer", mat != NULL);

	RendererModelMul(v1db.renderer, (void *)mat);

	t = BrTransformCombineTypes(t, mat_t);

	RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
		BRT_MODEL_TO_VIEW_HINT_T,
		BrTransformTypeIsLP(t)?BRT_LENGTH_PRESERVING:BRT_NONE);

	return t;
}

/*
 * Rendering traversal for the given actor
 */
static void actorRender(br_actor *ap,
						br_model *model,
						br_material *material,
						void *render_data,
						br_uint_8 style,
						br_uint_16 t)
{
	/*
	 * Saved state
	 */
	br_material *this_material;
	br_model *this_model;
	void *this_render_data;
	br_actor *a;
	br_token s;

    ASSERT_MESSAGE("Invalid actorRender pointer", model != NULL);
    ASSERT_MESSAGE("Invalid actorRender pointer", material != NULL);

	/*
	 * See if this actor overrides default material, model, render_data or style
	 */
	if(ap->render_style != BR_RSTYLE_DEFAULT)
		style = ap->render_style;

	if(style == BR_RSTYLE_NONE)
		return;

	/*
	 * Ignore actors with no children that are not models, and actors with renderstyle = NONE
	 */

	if(ap->children == NULL && ap->type != BR_ACTOR_MODEL)
		return;

	this_material = ap->material?ap->material:material;
	this_model = ap->model?ap->model:model;
	this_render_data=ap->render_data?ap->render_data:render_data;

	/*
	 * Catch special case of identity transforms
	 */
	if(ap->t.type == BR_TRANSFORM_IDENTITY) {
		/**
		 ** Actor has no transform
		 **/
		switch(ap->type) {

		case BR_ACTOR_MODEL:
			/*
			 * This is a model -  see if model's bounding box is on screen
			 */
			if((s = BrOnScreenCheck(&this_model->bounds)) != OSC_REJECT) {
				BrLightCullReset();
				BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
			}
			break;

		case BR_ACTOR_BOUNDS:
			/*
			 * A bounding box - truncate whole tree if rejected
			 */
			if(BrOnScreenCheck(ap->type_data) == OSC_REJECT)
	   			/* DONT PROCESS CHILDREN */
				return;
			break;

		case BR_ACTOR_BOUNDS_CORRECT:
			/*
			 * A garuanteed bounding box - test to see if it is on screen
			 */
			switch(BrOnScreenCheck(ap->type_data)) {

			case OSC_ACCEPT:
				/*
				 * Bounding box is completely on screen - process children with special loop
				 */
				BR_FOR_SIMPLELIST(&ap->children, a)
					actorRenderOnScreen(a,this_model,this_material,this_render_data, style, t);
				/* FALL THROUGH */

			case OSC_REJECT:
	   			/* DONT PROCESS CHILDREN */
				return;
			}

		}

		/*
		 * Recurse for children
		 */
		BR_FOR_SIMPLELIST(&ap->children, a)
			actorRender(a, this_model, this_material, this_render_data, style, t);

		return;
	}

	/**
	 ** Actor has a transform
	 **/

	/*
	 * Save the current transforms
	 */
	RendererStatePush(v1db.renderer, BR_STATE_MATRIX);

	t = prependActorTransform(ap, t);

	switch(ap->type) {

	case BR_ACTOR_MODEL:
		/*
		 * This is a model -  see if model's bounding box is on screen
		 */
		if((s = BrOnScreenCheck(&this_model->bounds)) != OSC_REJECT) {
			BrLightCullReset();
			BrDbModelRender(ap, this_model, this_material, this_render_data, style, s, 1);
		}
		break;

	case BR_ACTOR_BOUNDS:
		/*
		 * A bounding box - truncate whole tree if rejected
		 */
		if(BrOnScreenCheck(ap->type_data) == OSC_REJECT) {
			RendererStatePop(v1db.renderer, BR_STATE_MATRIX);
			return;
		}
		break;

	case BR_ACTOR_BOUNDS_CORRECT:
		/*
		 * A garuanteed bounding box - test to see if it is on screen
		 */
		switch(BrOnScreenCheck(ap->type_data)) {

		case OSC_ACCEPT:
			/*
			 * Bounding box is completely on screen - process children with special loop
			 */
			BR_FOR_SIMPLELIST(&ap->children, a)
				actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);
 			/* FALL THROUGH */

		case OSC_REJECT:
   			/*
			 * Don't process children
			 */
			RendererStatePop(v1db.renderer, BR_STATE_MATRIX);
			return;
		}
	}

	/*
	 * Recurse for children
	 */
	BR_FOR_SIMPLELIST(&ap->children, a)
		actorRender(a, this_model, this_material, this_render_data, style, t);

	/*
	 * Restore transforms
	 */
	RendererStatePop(v1db.renderer, BR_STATE_MATRIX);
}

/*
 * Rendering traversal for an actor that is completely on screen, along
 * with any children
 */
static void actorRenderOnScreen(br_actor *ap,
								br_model *model,
								br_material *material,
								void *render_data,
								br_uint_8 style,
								br_uint_16 t)
{
	/*
	 * Saved state
	 */
	br_material *this_material;
	br_model *this_model;
	void *this_render_data;
	br_actor *a;

    ASSERT_MESSAGE("Invalid actorRenderOnScreen pointer", model != NULL);
    ASSERT_MESSAGE("Invalid actorRenderOnScreen pointer", material != NULL);

	/*
	 * See if this actor overrides default material, model or style
	 */
	if(ap->render_style != BR_RSTYLE_DEFAULT)
		style = ap->render_style;

	if(style == BR_RSTYLE_NONE)
		return;

	this_material = ap->material?ap->material:material;
	this_model = ap->model?ap->model:model;
	this_render_data=ap->render_data?ap->render_data:render_data;

	/*
	 * Catch special case of identity transforms
	 */
	if(ap->t.type == BR_TRANSFORM_IDENTITY) {
		/*
		 * This actor has no transform
		 */
		if(ap->type == BR_ACTOR_MODEL) {
			BrLightCullReset();
			BrDbModelRender(ap, this_model, this_material, this_render_data, style, OSC_ACCEPT, 1);
		}

		BR_FOR_SIMPLELIST(&ap->children, a)
			actorRenderOnScreen(a,this_model,this_material, this_render_data, style, t);

		return;
	}

	/**
	 ** Actor has a transform
	 **/

	/*
	 * Save the current transforms
	 */
	RendererStatePush(v1db.renderer, BR_STATE_MATRIX);

	t = prependActorTransform(ap, t);


	if(ap->type == BR_ACTOR_MODEL) {
		BrLightCullReset();
		BrDbModelRender(ap, this_model, this_material, this_render_data, style, OSC_ACCEPT, 1);
	}

	BR_FOR_SIMPLELIST(&ap->children, a)
		actorRenderOnScreen(a, this_model, this_material, this_render_data, style, t);

	/*
	 * Restore transforms
	 */
	RendererStatePop(v1db.renderer, BR_STATE_MATRIX);
}

/*
 * Render all the children of root
 */
static void sceneRenderWorld(br_actor *world)
{
	br_model *model = v1db.default_model;
	br_material *material = v1db.default_material;
	void *render_data = v1db.default_render_data;
	br_uint_8 style = BR_RSTYLE_DEFAULT;
	br_actor *a;

	if(world->model)
		model = world->model;

	if(world->material)
		material = world->material;

	if(world->render_data)
		render_data = world->render_data;

	if(world->render_style != BR_RSTYLE_DEFAULT)
		style = world->render_style;

	BR_FOR_SIMPLELIST(&world->children, a)
		actorRender(a, model, material, render_data, style, (br_uint_16)v1db.ttype);
}

/*
 * Add a sub-tree to the current rendering pass - 
 *
 * Walks up tree from provided actor to find the material, model and
 * style to inherit
 */
static void sceneRenderAdd(br_actor *tree)
{
	br_material *material = NULL;
	br_model *model = NULL;
	void *render_data = NULL;
	br_uint_8 style = BR_RSTYLE_DEFAULT;
	br_actor *a;
	br_int_32 t;
	br_matrix34 m;
	
	if(tree->parent == NULL) {
		/*
		 * Simple case for when added tree is unconnected
		 */
		actorRender(tree,
			v1db.default_model,
			v1db.default_material,
			v1db.default_render_data, BR_RSTYLE_DEFAULT,
			(br_uint_16)v1db.ttype);
		return;
	}

	t = BR_TRANSFORM_IDENTITY;
	BrMatrix34Identity(&m);

	/*
	 * Walk back to current rendering root
	 */
	for(a = tree->parent; a ; a = a->parent) {
		/*
		 * Closest material, model and render_data
		 */
		if(material == NULL && a->material)
			material = a->material;

		if(model == NULL && a->model)
			model = a->model;

		if(render_data == NULL && a->render_data)
			render_data = a->render_data;
		/*
		 * Furthest style
		 */
		if(a->render_style != BR_RSTYLE_DEFAULT)
			style = a->render_style;

		/*
		 * Quit if we have go the the root that is
		 * being used for the current rendering pass
		 * (before accumulating transform s.t. we do not
		 * include root's transform
		 */
		if(a == v1db.render_root)
			break;

		/*
		 * Accumulate transform 
		 */	
		if(a->t.type != BR_TRANSFORM_IDENTITY) {
			BrMatrix34PostTransform(&m,&a->t);
			t = BrTransformCombineTypes(t,a->t.type);
		}
	}

	if(material == NULL)
		material = v1db.default_material;

	if(model == NULL)
		model = v1db.default_model;

	if(render_data == NULL)
		render_data = v1db.default_render_data;

	if(t == BR_TRANSFORM_IDENTITY) {
		actorRender(tree, model, material, render_data, style,
			(br_uint_16)v1db.ttype);
	} else {
		RendererStatePush(v1db.renderer, BR_STATE_MATRIX);
	
		t = prependMatrix(&m, (br_uint_16)t, (br_uint_16)v1db.ttype);

		actorRender(tree, model, material, render_data, style,
			(br_uint_16)t);

		RendererStatePop(v1db.renderer, BR_STATE_MATRIX);
	}
}

/* Signal start of frame */
void BR_PUBLIC_ENTRY BrRendererFrameBegin(){
    RendererFrameBegin(v1db.renderer);
}
/* Signal end of frame */
void BR_PUBLIC_ENTRY BrRendererFrameEnd(){
    RendererFrameEnd(v1db.renderer);
}

/* Hack to tell d3d driver when it is about to loose surfaces */

void BR_PUBLIC_ENTRY BrRendererFocusLossBegin(){
    if(v1db.renderer)
        RendererFocusLossBegin(v1db.renderer);
}
/* Hack to tell d3d driver when it will have to restore surfaces */

void BR_PUBLIC_ENTRY BrRendererFocusLossEnd(){
    if(v1db.renderer)
        RendererFocusLossEnd(v1db.renderer);
}

/*
 * BrDbSceneRenderBegin()
 *
 * Setup a new scene to be rendered - processes the camera, lights
 * and environment
 */
void BR_PUBLIC_ENTRY BrDbSceneRenderBegin(br_actor *world,
					br_actor *camera)
{
	br_matrix34 tfm;
	br_matrix4 vtos;
	br_actor *a;
	int i;
	br_token vtos_type;
	br_uint_32 dummy;

	UASSERT_MESSAGE("No renderer present", v1db.renderer != NULL );
   UASSERT_MESSAGE("Invalid BrDbSceneRenderBegin pointer", world != NULL);
   UASSERT_MESSAGE("Invalid BrDbSceneRenderBegin pointer", camera != NULL);

	/*
	 * Work out View Transform from info. in camera actor
	 */
	vtos_type = CameraToScreenMatrix4(&vtos, camera);
	RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_AS_MATRIX4_SCALAR(VIEW_TO_SCREEN), (br_uint_32)&vtos);
	RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_VIEW_TO_SCREEN_HINT_T, (br_uint_32)vtos_type);

	RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_AS_SCALAR(HITHER_Z), *(br_uint_32 *)&((br_camera *)camera->type_data)->hither_z);
	RendererPartSet(v1db.renderer, BRT_MATRIX, 0, BRT_AS_SCALAR(YON_Z), *(br_uint_32 *)&((br_camera *)camera->type_data)->yon_z);

	/*
	 * Collect transforms from camera to root
	 *
 	 * Make a stack of cumulative transforms for each level between
	 * the camera and the root - this is so that model->view
	 * transforms can use the shortest route, rather than via the root
	 */
	for(i=0; i< MAX_CAMERA_DEPTH; i++)
		v1db.camera_path[i].a = NULL;

	i = camera->depth;
	a = camera;
	BrMatrix34Identity(&v1db.camera_path[i].m);
	v1db.camera_path[i].transform_type = BR_TRANSFORM_IDENTITY;

	for( ; (i > 0) && (a != world) ; a = a->parent, i--) {
		ASSERT(a != NULL);
		BrMatrix34Transform(&tfm,&a->t);
		BrMatrix34Mul(&v1db.camera_path[i-1].m,&v1db.camera_path[i].m,&tfm);

		v1db.camera_path[i-1].transform_type = BrTransformCombineTypes(v1db.camera_path[i].transform_type, a->t.type);

		v1db.camera_path[i].a = a;
	}
	
	if(world != a)
		BR_ERROR0("camera is not in world hierachy");

	/*
	 * Make world->view as initial model->view
	 */
	RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
		BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW), (br_uint_32)&v1db.camera_path[i].m);

	v1db.ttype = v1db.camera_path[i].transform_type;

	RendererPartSet(v1db.renderer, BRT_MATRIX, 0,
		BRT_MODEL_TO_VIEW_HINT_T, BrTransformTypeIsLP(v1db.ttype)?BRT_LENGTH_PRESERVING:BRT_NONE);

	RendererModelInvert(v1db.renderer);

	/*
	 * Setup active lights, clip planes, horizon and environment
	 */
	RendererPartQueryBuffer(v1db.renderer, BRT_MATRIX, 0, &dummy, (br_uint_32 *)&tfm, sizeof(tfm),
		BRT_AS_MATRIX34_SCALAR(MODEL_TO_VIEW));

	BrSetupLights(world, &tfm, v1db.ttype);
	BrSetupClipPlanes(world, &tfm, v1db.ttype, &vtos);
	BrSetupEnvironment(world, &tfm, v1db.ttype);
	BrSetupHorizons(world, &tfm, v1db.ttype);
}

/*
 * Set a callback function for bounding rectangles
 */
br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrDbSetRenderBoundsCallback(br_renderbounds_cbfn *new_cbfn)
{
	br_renderbounds_cbfn *old_cbfn = v1db.bounds_call;
    UASSERT_MESSAGE("Invalid BrDbSetRenderBoundsCallback pointer", new_cbfn != NULL);
	v1db.bounds_call = new_cbfn;

	/*
	 * Enable or disable bounds in renderer
	 */
	if(v1db.renderer)
		RendererPartSet(v1db.renderer, BRT_ENABLE, 0, BRT_BOUNDS_B, (v1db.bounds_call != NULL));

	return old_cbfn;
}

/*
 * Draw a sparse lattice of zero pixels into the output buffer
 */
#if EVAL
static void DrawLattice(br_pixelmap *pmap)
{
	int sx,sy;

	for(sy = -pmap->origin_y + 32; sy < pmap->height; sy += 64)
		for(sx = -pmap->origin_x + 32; sx < pmap->width; sx += 64)
			BrPixelmapPixelSet(pmap, sx, sy, 0);
}
#endif

/*
 * Compatibility functions
 */
static void SetOrigin(br_pixelmap *buffer)
{
    ASSERT( buffer != NULL);
    ASSERT_MESSAGE("SetOrigin divide by zero error", BrIntToScalar(buffer->width/2) != 0);
    ASSERT_MESSAGE("SetOrigin divide by zero error", BrIntToScalar(buffer->height/2) != 0);

	v1db.origin.v[0] = BR_DIV(BrIntToScalar(buffer->origin_x - buffer->width/2),
					 	BrIntToScalar(buffer->width/2));

	v1db.origin.v[1] = -BR_DIV(BrIntToScalar(buffer->origin_y - buffer->height/2),
					 	BrIntToScalar(buffer->height/2));
}

static void SetViewport(br_pixelmap *buffer)
{
    ASSERT( buffer != NULL);
	v1db.vp_ox = BR_SCALAR(buffer->base_x+buffer->width/2)+BR_SCALAR(0.5);
	v1db.vp_width = BR_SCALAR(buffer->width/2);

	v1db.vp_oy = BR_SCALAR(buffer->height/2)+BR_SCALAR(0.5);
	v1db.vp_height = -BR_SCALAR(buffer->height/2);
};

void BR_PUBLIC_ENTRY BrZbSceneRenderBegin(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer)
{
	br_camera *camera_data;

	UASSERT_MESSAGE("No renderer present", v1db.renderer != NULL );
	UASSERT(v1db.rendering == RENDERING_NONE);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin actor pointer", world != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin actor pointer", camera != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin pixelmap pointer", colour_buffer != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin pixelmap pointer", depth_buffer != NULL);

	v1db.rendering = RENDERING_ZB;
	v1db.render_root = world;
	v1db.colour_buffer = colour_buffer;

	/*
	 * Setup output state for renderer
	 */
	SetOrigin(colour_buffer);
	SetViewport(colour_buffer);

	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) colour_buffer);
	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (br_uint_32) depth_buffer);

	/*
	 * Setup primitives heap and order table for deferred primitives
	 */
	if(v1db.format_buckets != NULL) {

		v1db.heap.current = v1db.heap.base;

		camera_data=(br_camera *)camera->type_data;

		v1db.default_order_table->max_z=camera_data->yon_z;
		v1db.default_order_table->min_z=camera_data->hither_z;
		v1db.default_order_table->visits = 0;

		v1db.order_table_list = NULL;

		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (br_uint_32) v1db.default_order_table);
		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (br_uint_32) &v1db.heap);
		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_SORT);
		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_DIVERT_T, BRT_BLENDED);

		v1db.default_render_data = v1db.default_order_table;

	} else {

		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_NONE);
		RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_DIVERT_T, BRT_NONE);
		
		v1db.default_render_data = NULL;
	}

	BrDbSceneRenderBegin(world, camera);
}

void BR_PUBLIC_ENTRY BrZbSceneRenderContinue(br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer)
{
	br_camera *camera_data;

	UASSERT(v1db.rendering == RENDERING_ZB);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin actor pointer", world != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin actor pointer", camera != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin pixelmap pointer", colour_buffer != NULL);
   UASSERT_MESSAGE("Invalid BrZbSceneRenderBegin pixelmap pointer", depth_buffer != NULL);

	v1db.render_root = world;
	v1db.colour_buffer = colour_buffer;

	/*
	 * Setup output state for renderer
	 */
	SetOrigin(colour_buffer);
	SetViewport(colour_buffer);

	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) colour_buffer);
	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, (br_uint_32) depth_buffer);

	BrDbSceneRenderBegin(world, camera);
}

void BR_PUBLIC_ENTRY BrZbSceneRenderAdd(br_actor *tree)
{
	UASSERT(v1db.rendering == RENDERING_ZB);
    UASSERT_MESSAGE("Invalid BrZbSceneRenderAdd pointer", tree != NULL);
	sceneRenderAdd(tree);
}

void BR_PUBLIC_ENTRY BrZbSceneRenderEnd(void)
{
	UASSERT(v1db.rendering == RENDERING_ZB);

	/*
	 * Draw any deferred primitives
	 */
	if(v1db.format_buckets != NULL)

		if(v1db.primary_order_table) {

			/*
			 * Render primitives in the primary order table
			 * and the list of order tables
			 */
			RenderPrimaryOrderTable();
		} else {
			/*
			 * Render primitives in the list of order table
			 */
			RenderOrderTableList();
		}

	/*
	 * Overlay lattice if evaluation version
	 */
#if EVAL
	DrawLattice(v1db.colour_buffer);
#endif

	/*
	 * Tell the renderer to flush
	 */
	RendererFlush(v1db.renderer, BR_FALSE);

	v1db.rendering = RENDERING_NONE;
	v1db.render_root = NULL;
}


void BR_PUBLIC_ENTRY BrZbSceneRender(
					br_actor *world,
					br_actor *camera,
					br_pixelmap *colour_buffer,
					br_pixelmap *depth_buffer)
{
	UASSERT_MESSAGE("No renderer present", v1db.renderer != NULL );
	UASSERT_MESSAGE("Invalid BrZbSceneRender actor pointer", world != NULL);
	UASSERT_MESSAGE("Invalid BrZbSceneRender actor pointer", camera != NULL);
	UASSERT_MESSAGE("Invalid BrZbSceneRender pixelmap pointer", colour_buffer != NULL);
	UASSERT_MESSAGE("Invalid BrZbSceneRender pixelmap pointer", depth_buffer != NULL);

	// Stop BRender dying horribly when no renderer is loaded
 
	if (v1db.renderer) {

		BrZbSceneRenderBegin(world, camera, colour_buffer, depth_buffer);
		sceneRenderWorld(world);
		BrZbSceneRenderEnd();
	}
}

void BR_PUBLIC_ENTRY BrZsSceneRenderBegin(br_actor *world,
										  br_actor *camera,
										  br_pixelmap *colour_buffer)
{
	br_camera *camera_data;

	UASSERT(v1db.renderer != NULL);
	UASSERT(v1db.rendering == RENDERING_NONE);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin actor pointer", world != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin actor pointer", camera != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin pixelmap pointer", colour_buffer != NULL);

	v1db.rendering = RENDERING_ZS;
	v1db.render_root = world;
	v1db.colour_buffer = colour_buffer;

	SetOrigin(colour_buffer);
	SetViewport(colour_buffer);

	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) colour_buffer);
	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, 0);

	/*
	 * Setup primitives heap and order table
	 */
	v1db.heap.current = v1db.heap.base;

	camera_data=(br_camera *)camera->type_data;

	v1db.default_order_table->max_z=camera_data->yon_z;
	v1db.default_order_table->min_z=camera_data->hither_z;
	v1db.default_order_table->visits = 0;

	v1db.order_table_list = NULL;

	RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1ORDER_TABLE_P, (br_uint_32) v1db.default_order_table);
	RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_V1PRIMITIVE_HEAP_P, (br_uint_32) &v1db.heap);
	RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_TYPE_T, BRT_BUCKET_SORT);
	RendererPartSet(v1db.renderer, BRT_HIDDEN_SURFACE, 0, BRT_DIVERT_T, BRT_ALL);

	v1db.default_render_data = v1db.default_order_table;

	BrDbSceneRenderBegin(world, camera);
}

void BR_PUBLIC_ENTRY BrZsSceneRenderContinue(br_actor *world,
										  br_actor *camera,
										  br_pixelmap *colour_buffer)
{
	br_camera *camera_data;

	UASSERT(v1db.rendering == RENDERING_ZS);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin actor pointer", world != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin actor pointer", camera != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderBegin pixelmap pointer", colour_buffer != NULL);

	v1db.render_root = world;
	v1db.colour_buffer = colour_buffer;

	SetOrigin(colour_buffer);
	SetViewport(colour_buffer);

	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) colour_buffer);
	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_DEPTH_BUFFER_O, 0);

	BrDbSceneRenderBegin(world, camera);
}

void BR_PUBLIC_ENTRY BrZsSceneRenderAdd(br_actor *tree)
{
	UASSERT(v1db.rendering == RENDERING_ZS);
	UASSERT_MESSAGE("Invalid BrZsSceneRenderAdd pointer", tree != NULL);
	sceneRenderAdd(tree);
}

void BR_PUBLIC_ENTRY BrZsSceneRenderEnd(void)
{
	UASSERT(v1db.rendering == RENDERING_ZS);

	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) v1db.colour_buffer);

	if(v1db.format_buckets == NULL)
		BR_ERROR0("Renderer does not support buckets");

	if(v1db.primary_order_table) {

		/*
		 * Render primitives in the primary order table
		 * and the list of order tables
		 */
		RenderPrimaryOrderTable();
	} else {
		/*
		 * Render primitives in the list of order table
		 */
		RenderOrderTableList();
	}

	/*
	 * Overlay lattice if evaluation version
	 */
#if EVAL
	DrawLattice(v1db.colour_buffer);
#endif

	/*
	 * Tell the renderer to flush
	 */
	RendererFlush(v1db.renderer, BR_FALSE);

	v1db.rendering = RENDERING_NONE;
	v1db.render_root = NULL;
}

/*
 * Wrapper that invokes above three calls in order
 */
void BR_PUBLIC_ENTRY BrZsSceneRender(br_actor *world,
									 br_actor *camera,
									 br_pixelmap *colour_buffer)
{
	br_actor *a;

	UASSERT_MESSAGE("No renderer present", v1db.renderer != NULL );
	UASSERT_MESSAGE("Invalid BrZsSceneRender actor pointer", world != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRender actor pointer", camera != NULL);
	UASSERT_MESSAGE("Invalid BrZsSceneRender pixelmap pointer", colour_buffer != NULL);

	// Stop BRender dying horribly when no renderer present.

	if (v1db.renderer) {
		BrZsSceneRenderBegin(world, camera, colour_buffer);
		sceneRenderWorld(world);
		BrZsSceneRenderEnd();
	}
}


br_primitive_cbfn * BR_PUBLIC_ENTRY BrZsPrimitiveCallbackSet(br_primitive_cbfn *new_cbfn)
{
	br_primitive_cbfn *old_cbfn = v1db.primitive_call;
	v1db.primitive_call = new_cbfn;

	return old_cbfn;
}

void BR_PUBLIC_ENTRY BrZbModelRender(br_actor *actor,
				  br_model *model,
				  br_material *material,
				  br_uint_8 style,
				  int on_screen,
				  int use_custom)
{

	UASSERT_MESSAGE("Invalid BrZbModelRender model actor pointer", actor != NULL);
	UASSERT_MESSAGE("Invalid BrZbModelRender model pointer", model != NULL);
	UASSERT_MESSAGE("Invalid BrZbModelRender material pointer", material != NULL);

	BrDbModelRender(actor, model, material, NULL, style, on_screen, use_custom);
}

void BR_PUBLIC_ENTRY BrZsModelRender(br_actor *actor,
									 br_model *model,
									 br_material *material,
									 br_order_table *order_table,
									 br_uint_8 style,
									 int on_screen,
									 int use_custom)
{
	UASSERT_MESSAGE("Invalid BrZsModelRender model actor pointer", actor != NULL);
	UASSERT_MESSAGE("Invalid BrZsModelRender model pointer", model != NULL);
	UASSERT_MESSAGE("Invalid BrZsModelRender material pointer", material != NULL);
	UASSERT_MESSAGE("Invalid BrZsModelRender order table pointer", order_table != NULL);

	BrDbModelRender(actor, model, material, order_table, style, on_screen, use_custom);
}

br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrZbRenderBoundsCallbackSet(br_renderbounds_cbfn *new_cbfn)
{
	ASSERT(new_cbfn != NULL);

	if (!v1db.zb_active) {

		BR_ERROR0("BrZbSetRenderBoundsCallback called before BrZbBegin");
		return NULL ;

	} else
		return BrDbSetRenderBoundsCallback(new_cbfn);
}

br_renderbounds_cbfn * BR_PUBLIC_ENTRY BrZsRenderBoundsCallbackSet(br_renderbounds_cbfn *new_cbfn)
{
	ASSERT(new_cbfn != NULL);
	
	if (!v1db.zs_active) {

		BR_ERROR0("BrZsSetRenderBoundsCallback called before BrZsBegin");
		return NULL ;

	} else
		return BrDbSetRenderBoundsCallback(new_cbfn);
}
