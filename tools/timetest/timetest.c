/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: timetest.c 1.1 1997/12/10 16:59:18 jon Exp $
 * $Locker: $
 *
 * Time some standard scenes
 */
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "brender.h"
#include "dosio.h"
#include "blockops.h"

#include "timing.h"

#define TIMING_FRAMES 61
#define USE_SYSTEM_CLOCK 0

/*
 * Either use system clock, or our own
 */
#if USE_SYSTEM_CLOCK
#include <time.h>

#define CLOCKREAD() clock()
#define CLOCKINIT() 
#undef CLOCK_RATE
#define CLOCK_RATE CLOCKS_PER_SEC

#else

#define CLOCK_RATE BR_DOS_CLOCK_RATE

#define CLOCKREAD() DOSClockRead()
#define CLOCKINIT() DOSClockBegin()
#define CLOCKTERM() DOSClockEnd()

#endif

#define MAX_DIRTY_RECTANGLES 16

struct dirty_rect {
	int min_x;
	int min_y;
	int max_x;
	int max_y;
} DirtyRectangles[MAX_DIRTY_RECTANGLES], RenderBounds, ClearBounds;

int LastDirtyRectangle = 0;

struct br_device * BR_EXPORT BrDrv1SoftPrimBegin(char *arguments);
struct br_device * BR_EXPORT BrDrv1SoftRendBegin(char *arguments);

void dprintf(int x, int y, char *fmt,...);

void ViewDrawDirtyRectangles(br_pixelmap *pm);

void BuildTestWorld(void);

#if 0
/*
 * timing info
 */
br_timer TS_SceneRender;
br_timer TS_ModelRender;
br_timer TS_Cull;
br_timer TS_Transform;
br_timer TS_Render;

void DebugTimestamp(void);
#endif

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

#define BOUNDS 0
#define ZB 1
#define ZS 0

#define PRIMSIZE 1000000

br_pixelmap *screen, *render_pm, *depth, *palette, *offscreen, *timscreen;

static br_int_32 mouse_x,mouse_y;
br_uint_32 mouse_buttons;

br_model *model,*md;
br_actor *world, *observer, *model_t_actor, *model_actor,  *a, *observer_r_actor;
br_actor *light1_t_actor, *light1_actor;
br_actor *light2_t_actor, *light2_actor;
br_camera *camera;
br_light *light1,*light2;
br_material *material;

#define PixelmapMemAddress(pm,x,y,bpp) \
		((char *)(((br_pixelmap *)(pm))->pixels)+\
		(((br_pixelmap *)(pm))->base_y+(y))*((br_pixelmap *)(pm))->row_bytes+\
		(((br_pixelmap *)(pm))->base_x+(x)) * (bpp))

int main(int argc, char **argv)
{
	br_error r;
	int m = 0, nmats,i;
	br_material *mats[128], *mat1, *mat2;
	char *model_name,*device_string = NULL, *material_name = NULL;
	void *primitives;
  	br_pixelmap *pm, *st;
	long start_time=0,end_time=100000;
	int total_faces = 0;
	int time_count = 0;
	int scene_faces;
	void *cbuffer, *dbuffer;
	br_size_t cbytes,dbytes;
	BrBegin();


#if 0
	r = BrDevAddStatic(NULL, BrDrv1SoftPrimBegin, NULL);
	r = BrDevAddStatic(NULL, BrDrv1SoftRendBegin, NULL);
#endif

#if 1
	r = BrDevBegin(&screen, device_string);
	if(r != BRE_OK)
		BR_ERROR0("Could not get screen pixelmap");
#else
	screen=BrPixelmapAllocate(BR_PMT_INDEX_8,256,256,NULL,0);
#endif

	palette = BrPixelmapLoad("std.pal");
	
	if(palette)
		BrPixelmapPaletteSet(screen,palette);

	offscreen = BrPixelmapMatch(screen, BR_PMMATCH_OFFSCREEN);
	timscreen = offscreen;

	render_pm = BrPixelmapAllocateSub(offscreen, 0,10, offscreen->width, offscreen->height-10);
//	render_pm = offscreen;

	depth = BrPixelmapMatch(render_pm,BR_PMMATCH_DEPTH_16);

	render_pm->origin_x = render_pm->width/2;
	render_pm->origin_y = render_pm->height/2;

	cbuffer = PixelmapMemAddress(render_pm,0,0,1);
	dbuffer = PixelmapMemAddress(depth,0,0,2);
	cbytes = render_pm->row_bytes * render_pm->height;
	dbytes = depth->row_bytes * depth->height;

	BrPixelmapFill(render_pm, 10);
#if ZB
	BrPixelmapFill(depth, 0xFFFFFFFF);
#endif

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

#if 1
	/*
	 * Load some materials
	 */
	BrTableFindHook(BrTableFindFailedLoad);
	BrMapFindHook(BrMapFindFailedLoad);

	nmats = BrFmtScriptMaterialLoadMany("test.mat",mats,BR_ASIZE(mats));
	BrMaterialAddMany(mats,nmats);
#endif

	/*
	 * Load and add a model
	 */
	model_name = (argc >=2)?argv[1]:"teapot4k.dat";
	model = BrModelLoad(model_name);

    material_name = (argc >=3)?argv[2]:"red";
	material = BrMaterialFind(material_name);

	if(model == NULL)
		BR_ERROR1("could not load '%s'",model_name);

	scene_faces = model->nfaces;

	for(i=0; i < model->nfaces; i++)
		model->faces[i].material = NULL;

	for(i=0; i < model->nvertices; i++) {
		model->vertices[i].index = rand() & 0xff;
		model->vertices[i].red = rand() & 0xff;
		model->vertices[i].grn = rand() & 0xff;
		model->vertices[i].blu = rand() & 0xff;
	}

// 	model->flags |= BR_MODF_UPDATEABLE;
// 	model->flags |= BR_MODF_CREASE;
//	model->crease_angle = BR_ANGLE_DEG(45);

	BrModelAdd(model);

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
	CLOCKINIT();

	/*
	 * Timing loop
	 */
	for(;;) {
		/*
		 * Clear the buffers
		 */
#if 1
		BrPixelmapFill(render_pm, 10);
#if ZB
		BrPixelmapFill(depth, 0xFFFFFFFF);
#endif
#endif

#if 0
		memset(cbuffer,0,cbytes);
		memset(dbuffer,0xFF,dbytes);
#endif

#if 0
		BrBlockFill(cbuffer,0,cbytes/4);
		BrBlockFill(dbuffer,0xFFFFFFFF,dbytes/4);
#endif

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

//		BR_TIMER_START(TS_SceneRender);
#if ZB
		BrZbSceneRender(world, observer, render_pm, depth);
#endif
#if ZS
		BrZsSceneRender(world, observer, render_pm);
#endif
//		BR_TIMER_END(TS_SceneRender);


		if(time_count-- == 0) {
			int i;

			end_time = CLOCKREAD();

			BrPixelmapRectangleFill(timscreen,0,0,timscreen->width,10,24);

			BrPixelmapTextF(timscreen,0,0,255,BrFontFixed3x5,
				"Frames/Sec = %16g Polys/Sec = %16g",
				TIMING_FRAMES / ((end_time-start_time)/(double)CLOCK_RATE),
				total_faces / ((end_time-start_time)/(double)CLOCK_RATE) );

			time_count = TIMING_FRAMES;
			total_faces = 0;
			start_time = CLOCKREAD();
	  				
		} else
			total_faces += scene_faces;

#if 0
		/*
		 * Display timestamp info
		 */
		DebugTimestamp();
#endif

		BrPixelmapDoubleBuffer(screen, offscreen);

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

#if 0
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

			m = BrMaterialFind("red");
			m->flags ^= BR_MATF_PERSPECTIVE;
			BrMaterialUpdate(m, BR_MATU_ALL);
		}

		if(DOSKeyTest(SC_Z,0,REPT_FIRST_DOWN)) {
			br_material *m;

			m = BrMaterialFind("blue");

			BrMaterialUpdate(m,BR_MATU_ALL);
		}

		if(DOSKeyTest(SC_C,0,REPT_FIRST_DOWN)) {
			model->flags ^= BR_MODF_CREASE;
			BrModelUpdate(model, BR_MODU_ALL);
		}
#endif

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
		BrPixelmapRectangle(pm, dp->min_x, dp->min_y, dp->max_x-dp->min_x, dp->max_y-dp->min_y, 63);

	/*
	 * Total rectangle
	 */
	BrPixelmapRectangle(pm,
			RenderBounds.min_x, RenderBounds.min_y,
			RenderBounds.max_x-RenderBounds.min_x, RenderBounds.max_y-RenderBounds.min_y, 184);
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
	BrVector3Set(&observer->t.t.euler.t,BR_SCALAR(0.0),BR_SCALAR(0.0),BR_SCALAR(10.0));

#if 1
	camera = (br_camera *)observer->type_data;
	camera->type = BR_CAMERA_PERSPECTIVE;
	camera->hither_z = BR_SCALAR(0.1);
	camera->yon_z = BR_SCALAR(50.0);
	camera->aspect = BR_SCALAR(1.46);

//	camera->type = BR_CAMERA_PARALLEL;

	camera->width = BR_SCALAR(2.0);
	camera->height = BR_SCALAR(2.0);
#endif

#if 0
	model_t_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	model_t_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
#else
	model_t_actor = BrActorAllocate(BR_ACTOR_NONE,NULL);
#endif

	model_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_MODEL,NULL));
	model_actor->model = model;
	model_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	model_actor->material = material;
  	BrMatrix34RotateX(&model_actor->t.t.mat,BR_ANGLE_DEG(90));

#if 0
	light1_t_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_NONE,NULL));
	light1_t_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	BrVector3Set(&light1_t_actor->t.t.translate.t,BR_SCALAR(0.0),BR_SCALAR(0.0),BR_SCALAR(2.0));
#else
	light1_t_actor = BrActorAllocate(BR_ACTOR_NONE,NULL);
#endif

	light1_actor = BrActorAdd(world,BrActorAllocate(BR_ACTOR_LIGHT,NULL));
	light1_actor->t.type = BR_TRANSFORM_MATRIX34_LP;
	light1 = (br_light *)light1_actor->type_data;
	light1->colour = BR_COLOUR_RGB(255,255,255);
	BrLightEnable(light1_actor);
}

#if 0
void DebugTimestamp(void)
{
	dprintf(0, 4,"SceneRender = %08x:%08x",TS_SceneRender.time_h, TS_SceneRender.time_l);
	dprintf(0, 5,"ModelRender = %08x:%08x",TS_ModelRender.time_h, TS_ModelRender.time_l);
	dprintf(0, 6,"Cull        = %08x:%08x",TS_Cull.time_h, TS_Cull.time_l);
	dprintf(0, 7,"Transform   = %08x:%08x",TS_Transform.time_h, TS_Transform.time_l);
	dprintf(0, 8,"Render      = %08x:%08x",TS_Render.time_h, TS_Render.time_l);

	dprintf(0,10,"S - M       = %08x",TS_SceneRender.time_l - TS_ModelRender.time_l);
	dprintf(0,11,"M - (R+C+T) = %08x",TS_ModelRender.time_l -
			(TS_Cull.time_l+TS_Transform.time_l+TS_Render.time_l));
}
#endif

