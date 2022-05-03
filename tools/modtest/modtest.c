/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: modtest.c 1.1 1997/12/10 16:58:34 jon Exp $
 * $Locker: $
 *
 * Various tests
 */
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "brender.h"
#include "formats.h"
#include "dosio.h"

#define COUNTERS 0

#if COUNTERS
extern int CounterCache;
extern int CounterPerScene;
extern int CounterPerModel;
extern int CounterViewToModel;
extern int CounterModelToScreen;
#endif

#define MAX_DIRTY_RECTANGLES 16

/*
 * A view into the 3D world
 */
struct dirty_rect {
	int min_x;
	int min_y;
	int max_x;
	int max_y;
} DirtyRectangles[MAX_DIRTY_RECTANGLES], RenderBounds, ClearBounds;

int LastDirtyRectangle = 0;

struct br_device * BR_EXPORT BrDrv1GenMMXBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftPrimBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftRendBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1MCGABegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1VESABegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1S3325Begin(char *arguments);
struct br_device * BR_EXPORT BrDrv13DfxBegin(char *arguments);

void dprintf(int x, int y, char *fmt,...);
void PrintMatrix34(int x,int y,br_matrix34 *t);
void DebugCounters(void);

void ViewDrawDirtyRectangles(br_pixelmap *pm);
void CopyDirtyRectangles(br_pixelmap *dst, br_pixelmap *src);

void BuildTestWorld(void);

void BuildTestWorld1(void);

static void BR_CALLBACK ViewBoundsCallback(
	br_actor *actor,
	br_model *model,
	br_material *material,
	void *render_data,
	br_uint_8	style,
	br_matrix4 *model_to_screen,
	br_int_32 *bounds);

void ControlObject(br_actor *camera_actor, br_actor *translate_actor, br_actor *rotate_actor,
	br_int_32 mouse_x, br_int_32 mouse_y);

#if DEBUG
extern int MatchCount;
#endif

struct br_renderer_state_stored *test_state;

#define BOUNDS 0
#define ZB 1
#define ZS 0

#define PRIMSIZE 1000000

br_pixelmap *screen, *render_pm, *depth, *palette, *offscreen;

static br_int_32 mouse_x,mouse_y;
br_uint_32 mouse_buttons;

br_model *model,*md;
br_actor *world, *observer, *model_t_actor, *model_actor,  *a, *observer_r_actor;
br_actor *light1_t_actor, *light1_actor;
br_actor *light2_t_actor, *light2_actor;
br_camera *camera;
br_light *light1,*light2;

int main(int argc, char **argv)
{
	br_error r;
	int m = 0, nmats,i;
	br_material *mats[128], *mat;
	char *model_name, *material_name, *palette_name;
	void *primitives;
  	br_pixelmap *pm, *st;

	BrBegin();

#if 0
	printf("sizeof(fmt_vertex) = %d\n",sizeof(struct fmt_vertex));
	exit(0);
#endif

#if 0
//	r = BrDevAddStatic(NULL, BrDrv1MCGABegin, NULL);
//	r = BrDevAddStatic(NULL, BrDrv1GenMMXBegin, NULL);
//	r = BrDevAddStatic(NULL, BrDrv1SoftPrimBegin, NULL);
	r = BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
#endif

#if 1
//	r = BrDevBegin(&screen, "VESA,w:640,h:480,b:16");
	r = BrDevBegin(&screen, NULL);
	if(r != BRE_OK)
		BR_ERROR0("Could not get screen pixelmap");
#else
	screen=BrPixelmapAllocate(BR_PMT_INDEX_8,256,256,NULL,0);
#endif

	screen->origin_x = screen->width/2;
	screen->origin_y = screen->height/2;

	if(argc >= 4)
		palette_name = argv[3];
	else
		palette_name = "std.pal";

	palette = BrPixelmapLoad(palette_name);
	
	if(palette)
		BrPixelmapPaletteSet(screen,palette);

	offscreen = BrPixelmapMatch(screen, BR_PMMATCH_OFFSCREEN);

#if 0
	render_pm = BrPixelmapAllocateSub(offscreen,
		-(offscreen->width/2-32),
		-(offscreen->height/2-32),
		offscreen->width-64, offscreen->height-64);
#else
	render_pm = offscreen;
#endif

	render_pm->origin_x = render_pm->width/2;
	render_pm->origin_y = render_pm->height/2;

	depth = BrPixelmapMatch(offscreen, BR_PMMATCH_DEPTH_16);

	BrPixelmapFill(offscreen, 184);

	/*
	 * Start up renderer
	 */
#if ZB
	BrZbBegin(render_pm->type, depth->type);
#endif

#if ZS
	primitives = BrMemAllocate(PRIMSIZE,BR_MEMORY_APPLICATION);
	BrZsBegin(render_pm->type, primitives,PRIMSIZE);
#endif

	/*
	 * Load some materials
	 */
	BrTableFindHook(BrTableFindFailedLoad);
	BrMapFindHook(BrMapFindFailedLoad);

	nmats = BrFmtScriptMaterialLoadMany("test.mat",mats,BR_ASIZE(mats));
	BrMaterialAddMany(mats,nmats);

	/*
	 * Set up material
	 */
	if(argc >= 3)
		material_name = argv[2];
	else
		material_name = NULL;

	/*
	 * Load and add a model
	 */
	model_name = argv[1]?argv[1]:"cube.dat";
	model = BrModelLoad(model_name);

	if(model == NULL)
		BR_ERROR1("could not load '%s'",model_name);


	model = BrModelLoad(model_name);

	if(material_name) {
		if(!stricmp(material_name,"default"))
			mat = NULL;
		else
			mat = BrMaterialFind(material_name);

		for(i=0; i < model->nfaces; i++)
			model->faces[i].material = mat;
	}

#if 0
	/*
	 * Generate some random prelit colors
	 */
	for(i=0; i < model->nvertices; i++) {
		model->vertices[i].index = rand() & 0xff;
		model->vertices[i].red = rand() & 0xff;
		model->vertices[i].grn = rand() & 0xff;
		model->vertices[i].blu = rand() & 0xff;
	}
#endif

	for(i=0; i < model->nvertices; i++) {
		model->vertices[i].index = 0xff;
		model->vertices[i].red = 0xff;
		model->vertices[i].grn = 0xff;
	}

  	model->flags |= BR_MODF_UPDATEABLE;
  	model->flags |= BR_MODF_CREASE;
	model->crease_angle = BR_ANGLE_DEG(45);

	if(model)
		BrModelAdd(model);
	else
		BR_ERROR1("Could not load model '%s'",model_name);

	/*
	 * Build world
	 */
	BuildTestWorld();

#if BOUNDS
#if ZB
	BrZbSetRenderBoundsCallback(ViewBoundsCallback);
#endif

#if ZS
	BrZsSetRenderBoundsCallback(ViewBoundsCallback);
#endif
#endif

	DOSMouseBegin();
	DOSKeyBegin();

	/*
	 * Tumble the actor around
	 */
	for(;;) {
		/*
		 * Clear the buffers
		 */
		BrPixelmapFill(render_pm, 10);

#if BOUNDS
		/*
		 * Render world
		 */
		LastDirtyRectangle = 0;
		RenderBounds.min_x = INT_MAX;
		RenderBounds.min_y = INT_MAX;
		RenderBounds.max_x = INT_MIN;
		RenderBounds.max_y = INT_MIN;
#endif

#if ZB
		BrPixelmapFill(depth, 0xFFFFFFFF);
		BrZbSceneRender(world, observer, render_pm, depth);
#endif

#if ZS
		BrZsSceneRender(world, observer, render_pm);
#endif

#if 0
		PrintMatrix34(0,0,&model_actor->t.t.mat);
#endif

#if COUNTERS
		DebugCounters();
#endif

#if BOUNDS
//		ViewDrawDirtyRectangles(render_pm);
#endif


#if BOUNDS
		CopyDirtyRectangles(screen, render_pm);
#else
		BrPixelmapDoubleBuffer(screen, offscreen);
#endif
		/*
	 	 * Controls
		 */
		DOSMouseRead(&mouse_x,&mouse_y,&mouse_buttons);
		
		if(mouse_x || mouse_y) {
			if(DOSKeyTest(SC_L,0,0))
				ControlObject(observer, light1_t_actor, light1_actor, mouse_x, mouse_y);
			else if(DOSKeyTest(SC_M,0,0))
				ControlObject(observer, light2_t_actor, light2_actor, mouse_x, mouse_y);
			else if(DOSKeyTest(SC_O,0,0))
				ControlObject(observer, observer, observer_r_actor, -mouse_x, -mouse_y);
			else
				ControlObject(observer, model_t_actor, model_actor, mouse_x, mouse_y);

			mouse_x = 0;
			mouse_y = 0;
		}

		if(DOSKeyTest(SC_Q,0,REPT_FIRST_DOWN))
			break;


		if(DOSKeyTest(SC_F1,QUAL_NONE,0)) {
			light1->type = (light1->type & BR_LIGHT_VIEW) | BR_LIGHT_DIRECT;
		}

		if(DOSKeyTest(SC_F2,QUAL_NONE,0)) {
			light1->type = (light1->type & BR_LIGHT_VIEW) | BR_LIGHT_POINT;
		}

		if(DOSKeyTest(SC_F3,QUAL_NONE,0)) {
			light1->type = (light1->type & BR_LIGHT_VIEW) | BR_LIGHT_SPOT;
		}

		if(DOSKeyTest(SC_F4,0,REPT_FIRST_DOWN)) {
			light1->type ^= BR_LIGHT_VIEW;
		}

		if(DOSKeyTest(SC_F5,QUAL_NONE,0)) {
			light1->cone_inner += BR_ANGLE_DEG(1.0);
			light1->cone_outer = light1->cone_inner + BR_ANGLE_DEG(5.0);
		}

		if(DOSKeyTest(SC_F6,QUAL_NONE,0)) {
			light1->cone_inner -= BR_ANGLE_DEG(1.0);
			light1->cone_outer = light1->cone_inner + BR_ANGLE_DEG(5.0);
		}

		if(DOSKeyTest(SC_F1,QUAL_SHIFT,0)) {
			light2->type = (light2->type & BR_LIGHT_VIEW) | BR_LIGHT_DIRECT;
		}

		if(DOSKeyTest(SC_F2,QUAL_SHIFT,0)) {
			light2->type = (light2->type & BR_LIGHT_VIEW) | BR_LIGHT_POINT;
		}

		if(DOSKeyTest(SC_F3,QUAL_SHIFT,0)) {
			light2->type = (light2->type & BR_LIGHT_VIEW) | BR_LIGHT_SPOT;
		}

		if(DOSKeyTest(SC_F4,0,REPT_FIRST_DOWN)) {
			light2->type ^= BR_LIGHT_VIEW;
		}

		if(DOSKeyTest(SC_F5,QUAL_SHIFT,0)) {
			light2->cone_inner += BR_ANGLE_DEG(1.0);
			light2->cone_outer = light2->cone_inner + BR_ANGLE_DEG(5.0);
		}

		if(DOSKeyTest(SC_F6,QUAL_SHIFT,0)) {
			light2->cone_inner -= BR_ANGLE_DEG(1.0);
			light2->cone_outer = light2->cone_inner + BR_ANGLE_DEG(5.0);
		}

		if(DOSKeyTest(SC_D,0,REPT_FIRST_DOWN)) {
			
			a = BrActorAdd(world,BrActorAllocate(BR_ACTOR_MODEL,NULL));
			a->model = model_actor->model;
			a->material = model_actor->material;
			a->t.type = BR_TRANSFORM_MATRIX34_LP;
			BrActorToActorMatrix34(&a->t.t.mat, model_actor, world);
		}

		if(DOSKeyTest(SC_P,0,REPT_FIRST_DOWN)) {
			br_material *m;

			m = BrMaterialFind("test_texture");
			m->flags ^= BR_MATF_PERSPECTIVE;
			BrMaterialUpdate(m, BR_MATU_ALL);
		}

		if(DOSKeyTest(SC_W,0,REPT_FIRST_DOWN)) {
			model_actor->render_style = BR_RSTYLE_EDGES;
		}

		if(DOSKeyTest(SC_F,0,REPT_FIRST_DOWN)) {
			model_actor->render_style = BR_RSTYLE_FACES;
		}

		if(DOSKeyTest(SC_V,0,REPT_FIRST_DOWN)) {
			model_actor->render_style = BR_RSTYLE_POINTS;
		}

		if(DOSKeyTest(SC_Z,0,REPT_FIRST_DOWN)) {
			br_material *m;

			m = BrMaterialAllocate("test");
			m->flags |= BR_MATF_SMOOTH;
			m->index_base = 96;
			m->index_range = 31;
			BrMaterialAdd(m);

			model_actor->material = m;

		}

		if(DOSKeyTest(SC_C,0,REPT_FIRST_DOWN)) {
			model->flags ^= BR_MODF_CREASE;
			BrModelUpdate(model, BR_MODU_ALL);
		}

	}

	DOSKeyEnd();
	DOSMouseEnd();

#if ZB
	BrZbEnd();
#endif

#if ZS
	BrZsEnd();
#endif

	/*
	 * Free up
	 */
	BrPixelmapFree(depth);
	BrPixelmapFree(offscreen);

	BrPixelmapFree(screen);

	BrEnd();

	return 0;
}

#define MSCALE BR_SCALAR(0.006)

void ControlObject(br_actor *camera_actor, br_actor *translate_actor, br_actor *rotate_actor,
	br_int_32 mouse_x, br_int_32 mouse_y)
{
	br_matrix34 mat_roll;
	br_scalar tx,ty,tz;

	if(mouse_buttons & BR_MSM_BUTTONL) {
		tx =  BR_MUL(BR_SCALAR(mouse_x),MSCALE);
		ty = -BR_MUL(BR_SCALAR(mouse_y),MSCALE);

		BrMatrix34PostTranslate(&translate_actor->t.t.mat,tx,ty,BR_SCALAR(0.0));

	} else if(mouse_buttons & BR_MSM_BUTTONR) {

		tx = BR_MUL(BR_SCALAR(mouse_x),MSCALE);
		tz = BR_MUL(BR_SCALAR(mouse_y),MSCALE);

		BrMatrix34PostTranslate(&translate_actor->t.t.mat,tx,BR_SCALAR(0.0),tz);

	} else {
		BrMatrix34RollingBall(&mat_roll,mouse_x,-mouse_y,1000);
	
		BrMatrix34Post(&rotate_actor->t.t.mat, &mat_roll);
	}
}

void PrintMatrix34(int x,int y,br_matrix34 *t)
{
	int i,j;

	for(i=0; i<3; i++)
		for(j=0; j<4; j++)
			dprintf(x+i*12,y+j*2,"%11.5f",BrScalarToFloat(t->m[j][i]));
}

void dprintf(int x, int y, char *fmt,...)
{
	char temp[256];
 	va_list args;
	int o = 0;
	/*
	 * Build output string
	 */
	va_start(args,fmt);
	vsprintf(temp,fmt,args);
	va_end(args);
	
	BrPixelmapText(offscreen, x * 4, y * 6, 255, BrFontFixed3x5,  temp);
}

#if BOUNDS
/*
 * Callback function used by renderer to pass back dirty rectangles
 */
static void BR_CALLBACK ViewBoundsCallback(
	br_actor *actor,
	br_model *model,
	br_material *material,
	void *render_data,
	br_uint_8	style,
	br_matrix4 *model_to_screen,
	br_int_32 *bounds)
{
	struct dirty_rect *dp;

	/*
	 * Accumulate total bounding rectangle
	 */
	if(bounds[BR_BOUNDS_MIN_X] < RenderBounds.min_x)
		RenderBounds.min_x = bounds[BR_BOUNDS_MIN_X];

	if(bounds[BR_BOUNDS_MIN_Y] < RenderBounds.min_y)
		RenderBounds.min_y = bounds[BR_BOUNDS_MIN_Y];

	if(bounds[BR_BOUNDS_MAX_X] > RenderBounds.max_x)
		RenderBounds.max_x = bounds[BR_BOUNDS_MAX_X];

	if(bounds[BR_BOUNDS_MAX_Y] > RenderBounds.max_y)
		RenderBounds.max_y = bounds[BR_BOUNDS_MAX_Y];

	/*
	 * If list of rectangles is full, merge current into last
	 */
	if(LastDirtyRectangle >= MAX_DIRTY_RECTANGLES) {
		dp = DirtyRectangles + LastDirtyRectangle-1;

		if(bounds[BR_BOUNDS_MIN_X] < dp->min_x)
			dp->min_x = bounds[BR_BOUNDS_MIN_X];

		if(bounds[BR_BOUNDS_MIN_Y] < dp->min_y)
			dp->min_y = bounds[BR_BOUNDS_MIN_Y];

		if(bounds[BR_BOUNDS_MAX_X] > dp->max_x)
			dp->max_x = bounds[BR_BOUNDS_MAX_X];

		if(bounds[BR_BOUNDS_MAX_Y] > dp->max_y)
			dp->max_y = bounds[BR_BOUNDS_MAX_Y];

		return;
	}

	/*
	 * Add this rectangle to list
	 */
	dp = DirtyRectangles + LastDirtyRectangle;

	dp->min_x = bounds[BR_BOUNDS_MIN_X];
	dp->min_y = bounds[BR_BOUNDS_MIN_Y];
	dp->max_x = bounds[BR_BOUNDS_MAX_X];
	dp->max_y = bounds[BR_BOUNDS_MAX_Y];

	LastDirtyRectangle++;
}


/*
 * ViewDrawDirtyRectangles()
 *
 * Draw a rectangle around each dirty rectangle in current list
 */
void ViewDrawDirtyRectangles(br_pixelmap *pm)
{
	int i;
	struct dirty_rect *dp;
	
	/*
	 * Individual rectangles
	 */
	for(i=0, dp = DirtyRectangles ; i< LastDirtyRectangle; i++, dp++)
		BrPixelmapRectangle(pm, dp->min_x, dp->min_y, dp->max_x-dp->min_x+1, dp->max_y-dp->min_y+1, 63);

	/*
	 * Total rectangle
	 */
	BrPixelmapRectangle(pm,
			RenderBounds.min_x, RenderBounds.min_y,
			RenderBounds.max_x-RenderBounds.min_x+1, RenderBounds.max_y-RenderBounds.min_y+1, 184);


	for(i=0, dp = DirtyRectangles ; i< LastDirtyRectangle; i++, dp++) {
		dprintf(0,i+2,"%2d: %4d %4d %4d %4d",i,
			dp->min_x,
			dp->min_y,
			dp->max_x,
			dp->max_y);
	}

	dprintf(0,0,"    %4d %4d %4d %4d",
		RenderBounds.min_x,
		RenderBounds.min_y,
		RenderBounds.max_x,
		RenderBounds.max_y);
	
}

/*
 * CopyDirtyRectangles()
 *
 * Copy dirty rectangles onto screen
 */
void CopyDirtyRectangles(br_pixelmap *dst, br_pixelmap *src)
{
	int i;
	struct dirty_rect *dp;
	
	/*
	 * Individual rectangles
	 */
	for(i=0, dp = DirtyRectangles ; i< LastDirtyRectangle; i++, dp++)
		BrPixelmapDirtyRectangleCopy(dst,
			src,
			dp->min_x, dp->min_y, dp->max_x-dp->min_x+1, dp->max_y-dp->min_y+1);
#if 0
	/*
	 * Total rectangle
	 */
	BrPixelmapRectangle(pm,
			RenderBounds.min_x, RenderBounds.min_y,
			RenderBounds.max_x-RenderBounds.min_x+1, RenderBounds.max_y-RenderBounds.min_y+1, 184);
#endif
}
#endif


void BuildTestWorld(void)
{
	/*
	 * Build the world
	 */
	world = BrActorAllocate(BR_ACTOR_NONE,NULL);
	world->t.type = BR_TRANSFORM_IDENTITY;

	observer_r_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	observer_r_actor->t.type = BR_TRANSFORM_MATRIX34_LP;

	observer = BrActorAdd(observer_r_actor,BrActorAllocate(BR_ACTOR_CAMERA,NULL));
	observer->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&observer->t.t.euler.t,BR_SCALAR(0.0),BR_SCALAR(0.0),BR_SCALAR(5.0));

#if 1
	camera = (br_camera *)observer->type_data;
	camera->type = BR_CAMERA_PERSPECTIVE;
	camera->hither_z = BR_SCALAR(0.1);
	camera->yon_z = BR_SCALAR(10.0);
	camera->aspect = BR_SCALAR(1.46);

//	camera->type = BR_CAMERA_PARALLEL;

	camera->width = BR_SCALAR(2.0);
	camera->height = BR_SCALAR(2.0);
#endif

	model_t_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	model_t_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	model_actor = BrActorAdd(model_t_actor,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	model_actor->model = model;
	model_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
//	model_actor->material = BrMaterialFind("test_mat");
//	model_actor->render_style = BR_RSTYLE_EDGES;

//	model_actor->type = BR_ACTOR_NONE;

#if 0
	a = BrActorAdd(model_actor,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	a->model = NULL;
	a->material = BrMaterialFind("solid_red");
	a->render_style = BR_RSTYLE_BOUNDING_EDGES;
#endif

#if 0
	a = BrActorAdd(world,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	a->model = model;
	a->material = BrMaterialFind("blue");
	a->render_style = BR_RSTYLE_EDGES;
#endif

#if 0
	a = BrActorAdd(model_actor, BrActorAllocate(BR_ACTOR_MODEL,NULL));
	a->model = model;
	a->material = BrMaterialFind("blue_flat");
//	a->render_style = BR_RSTYLE_EDGES;
	a->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&a->t.t.euler.t,BR_SCALAR(1.5),BR_SCALAR(1.5),BR_SCALAR(0.0));


	a = BrActorAdd(model_actor, BrActorAllocate(BR_ACTOR_MODEL,NULL));
	a->model = model;
	a->material = BrMaterialFind("green_flat");
//	a->render_style = BR_RSTYLE_EDGES;
	a->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&a->t.t.euler.t,BR_SCALAR(0.0),BR_SCALAR(1.5),BR_SCALAR(0.0));

	a = BrActorAdd(model_actor, BrActorAllocate(BR_ACTOR_MODEL,NULL));
	a->model = model;
	a->material = BrMaterialFind("red_flat");
//	a->render_style = BR_RSTYLE_EDGES;
	a->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&a->t.t.euler.t,BR_SCALAR(-1.5),BR_SCALAR(1.5),BR_SCALAR(0.0));

#endif

	light1_t_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	light1_t_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&light1_t_actor->t.t.translate.t,BR_SCALAR(0.0),BR_SCALAR(0.0),BR_SCALAR(2.0));

	light1_actor = BrActorAdd(light1_t_actor,BrActorAllocate(BR_ACTOR_LIGHT,NULL));
	light1_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	light1 = (br_light *)light1_actor->type_data;
//	light1->attenuation_l = BR_SCALAR(1.4);
	light1->attenuation_c = BR_SCALAR(1.4);
	light1->colour = BR_COLOUR_RGB(255,255,255);
	BrLightEnable(light1_actor);


#if 0
	a = BrActorAdd(light1_actor,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	md = BrModelLoad("spot.dat");
	if(md)
		BrModelAdd(md);
	a->model = md;
	a->material = BrMaterialFind("red");

	BrMatrix34Scale(&a->t.t.mat,BR_SCALAR(0.6),BR_SCALAR(0.6),BR_SCALAR(0.6));
#endif

#if 1
	light2_t_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	light2_t_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&light2_t_actor->t.t.translate.t,BR_SCALAR(0.0),BR_SCALAR(0.0),BR_SCALAR(2.0));

	light2_actor = BrActorAdd(light2_t_actor,BrActorAllocate(BR_ACTOR_LIGHT,NULL));
	light2_actor->t.type = BR_TRANSFORM_MATRIX34_LP;

	light2 = (br_light *)light2_actor->type_data;
	light2->attenuation_c = BR_SCALAR(1.4);
//	light2->attenuation_l = BR_SCALAR(1.4);
	light2->colour = BR_COLOUR_RGB(255,255,255);
	BrLightEnable(light2_actor);

#if 0
	a = BrActorAdd(light2_actor,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	md = BrModelLoad("spot.dat");
	if(md)
		BrModelAdd(md);
	a->model = md;
	a->material = BrMaterialFind("blue");

	BrMatrix34Scale(&a->t.t.mat,BR_SCALAR(0.6),BR_SCALAR(0.6),BR_SCALAR(0.6));
#endif
#endif


#if 0
	a = BrActorAdd(model_actor,BrActorAllocate(BR_ACTOR_CLIP_PLANE,NULL));
	BrClipPlaneEnable(a);
#endif

	BrEnvironmentSet(light1_actor);

#if 0
	a = BrActorLoad("example.act");
	BrActorAdd(model_actor,a);
#endif
}

#if COUNTERS
void DebugCounters(void)
{
	dprintf(0,4,"CounterCache = %d",CounterCache);
	dprintf(0,5,"CounterPerScene = %d",CounterPerScene);
	dprintf(0,6,"CounterPerModel = %d",CounterPerModel);
	dprintf(0,7,"CounterViewToModel = %d",CounterViewToModel);
	dprintf(0,8,"CounterModelToScreen = %d",CounterModelToScreen);

	CounterPerScene = 0;
	CounterPerModel = 0;
	CounterViewToModel = 0;
	CounterModelToScreen = 0;
}
#endif

