/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Rendering hooks
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"


extern br_boolean r_textured;
extern br_boolean r_no_colour;     /* True only for non-modulate textures */
extern br_uint_8 opacity;




#define W_CONVERT(v) (1.0f / ((v)->comp_f[C_W]) )

/* z is wanted in the range 0.0 to 1.0: set in the match ranges */
#define Z_CONVERT(v) ((v)->comp_f[C_SZ])

/*
 * All point primitives
 */
void BR_ASM_CALL PointRenderFlat(brp_block *block, brp_vertex *a)
{
}


void BR_ASM_CALL LineRenderFlat(brp_block *block, brp_vertex *a,brp_vertex *b)
{
    T_msiVertex vtx[3];

    vtx[0].x = a->comp_f[C_SX];
    vtx[0].y = a->comp_f[C_SY];
    vtx[0].z = Z_CONVERT(a);

    vtx[1].x = b->comp_f[C_SX];
    vtx[1].y = b->comp_f[C_SY];
    vtx[1].z = Z_CONVERT(b);

    if (r_textured) {
        vtx[0].u = a->comp_f[C_U];
        vtx[0].v = a->comp_f[C_V];
	    vtx[0].invW = W_CONVERT(a);

        vtx[1].u = b->comp_f[C_U];
        vtx[1].v = b->comp_f[C_V];
	    vtx[1].invW = W_CONVERT(b);
    }

    if (!r_no_colour) {
        float r, g, b;

        r = a->comp_f[C_R];
        g = a->comp_f[C_G];
        b = a->comp_f[C_B];
        if (r_textured) {
            vtx[0].mr = r;
            vtx[0].mg = g;
            vtx[0].mb = b;
            vtx[1].mr = r;
            vtx[1].mg = g;
            vtx[1].mb = b;
        } else {
            vtx[0].r = r;
            vtx[0].g = g;
            vtx[0].b = b;
            vtx[1].r = r;
            vtx[1].g = g;
            vtx[1].b = b;
        }
    }

	vtx[2] = vtx[1];
	if (vtx[2].x > 2.0f)
		vtx[2].x -= 2.0f;
	else
		vtx[2].x += 2.0f;
	if (vtx[2].y > 2.0f)
		vtx[2].y -= 2.0f;
	else
		vtx[2].y += 2.0f;

//    syslog("LineRenderFlat");
    msiRenderTriangle(vtx+0, vtx+1, vtx+2, opacity);
//    syslog(" -> complete");
}



void BR_ASM_CALL LineRenderSmooth(brp_block *block, brp_vertex *a,brp_vertex *b)
{
    T_msiVertex vtx[3];
								 
    vtx[0].x = a->comp_f[C_SX];
    vtx[0].y = a->comp_f[C_SY];
    vtx[0].z = Z_CONVERT(a);

    vtx[1].x = b->comp_f[C_SX];
    vtx[1].y = b->comp_f[C_SY];
    vtx[1].z = Z_CONVERT(b);

    if (r_textured) {
        vtx[0].u = a->comp_f[C_U];
        vtx[0].v = a->comp_f[C_V];
	    vtx[0].invW = W_CONVERT(a);

        vtx[1].u = b->comp_f[C_U];
        vtx[1].v = b->comp_f[C_V];
	    vtx[1].invW = W_CONVERT(b);
    }

    if (!r_no_colour) {
        if (r_textured) {
            vtx[0].mr = a->comp_f[C_R];
            vtx[0].mg = a->comp_f[C_G];
            vtx[0].mb = a->comp_f[C_B];
            vtx[1].mr = b->comp_f[C_R];
            vtx[1].mg = b->comp_f[C_G];
            vtx[1].mb = b->comp_f[C_B];
        } else {
            vtx[0].r =  a->comp_f[C_R];
            vtx[0].g =  a->comp_f[C_G];
            vtx[0].b =  a->comp_f[C_B];
            vtx[1].r =  b->comp_f[C_R];
            vtx[1].g =  b->comp_f[C_G];
            vtx[1].b =  b->comp_f[C_B];
        }
    }

	vtx[2] = vtx[1];
	if (vtx[2].x > 2.0f)
		vtx[2].x -= 2.0f;
	else
		vtx[2].x += 2.0f;
	if (vtx[2].y > 2.0f)
		vtx[2].y -= 2.0f;
	else
		vtx[2].y += 2.0f;

//    syslog("LineRenderSmooth");
    msiRenderTriangle(vtx+0, vtx+1, vtx+2, opacity);
//    syslog(" -> complete");
}





/*
 * All Flat-shaded triangle primitives
 */
void BR_ASM_CALL TriangleRenderFlat(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
    T_msiVertex vtx[3];

    vtx[0].x = a->comp_f[C_SX];
    vtx[0].y = a->comp_f[C_SY];
    vtx[0].z = Z_CONVERT(a);

    vtx[1].x = b->comp_f[C_SX];
    vtx[1].y = b->comp_f[C_SY];
    vtx[1].z = Z_CONVERT(b);

    vtx[2].x = c->comp_f[C_SX];
    vtx[2].y = c->comp_f[C_SY];
    vtx[2].z = Z_CONVERT(c);

    if (r_textured) {
        vtx[0].u = a->comp_f[C_U];
        vtx[0].v = a->comp_f[C_V];
	    vtx[0].invW = W_CONVERT(a);

        vtx[1].u = b->comp_f[C_U];
        vtx[1].v = b->comp_f[C_V];
	    vtx[1].invW = W_CONVERT(b);

        vtx[2].u = c->comp_f[C_U];
        vtx[2].v = c->comp_f[C_V];
	    vtx[2].invW = W_CONVERT(c);
    }

    if (!r_no_colour) {
        float r, g, b;

        r = a->comp_f[C_R];
        g = a->comp_f[C_G];
        b = a->comp_f[C_B];
        if (r_textured) {
            vtx[0].mr = r;
            vtx[0].mg = g;
            vtx[0].mb = b;
            vtx[1].mr = r;
            vtx[1].mg = g;
            vtx[1].mb = b;
            vtx[2].mr = r;
            vtx[2].mg = g;
            vtx[2].mb = b;
        } else {
            vtx[0].r = r;
            vtx[0].g = g;
            vtx[0].b = b;
            vtx[1].r = r;
            vtx[1].g = g;
            vtx[1].b = b;
            vtx[2].r = r;
            vtx[2].g = g;
            vtx[2].b = b;
        }
    }

//    syslog("TriRenderFlat");
    msiRenderTriangle(vtx+0, vtx+1, vtx+2, opacity);
//    syslog(" -> complete");
}

/*
 * Z buffered, smooth shaded, all types
 */
void BR_ASM_CALL TriangleRenderSmooth(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c)
{
    T_msiVertex vtx[3];
								 
    vtx[0].x = a->comp_f[C_SX];
    vtx[0].y = a->comp_f[C_SY];
    vtx[0].z = Z_CONVERT(a);

    vtx[1].x = b->comp_f[C_SX];
    vtx[1].y = b->comp_f[C_SY];
    vtx[1].z = Z_CONVERT(b);

    vtx[2].x = c->comp_f[C_SX];
    vtx[2].y = c->comp_f[C_SY];
    vtx[2].z = Z_CONVERT(c);

    if (r_textured) {
        vtx[0].u = a->comp_f[C_U];
        vtx[0].v = a->comp_f[C_V];
	    vtx[0].invW = W_CONVERT(a);

        vtx[1].u = b->comp_f[C_U];
        vtx[1].v = b->comp_f[C_V];
	    vtx[1].invW = W_CONVERT(b);

        vtx[2].u = c->comp_f[C_U];
        vtx[2].v = c->comp_f[C_V];
	    vtx[2].invW = W_CONVERT(c);
    }

    if (!r_no_colour) {
        if (r_textured) {
            vtx[0].mr = a->comp_f[C_R];
            vtx[0].mg = a->comp_f[C_G];
            vtx[0].mb = a->comp_f[C_B];
            vtx[1].mr = b->comp_f[C_R];
            vtx[1].mg = b->comp_f[C_G];
            vtx[1].mb = b->comp_f[C_B];
            vtx[2].mr = c->comp_f[C_R];
            vtx[2].mg = c->comp_f[C_G];
            vtx[2].mb = c->comp_f[C_B];
        } else {
            vtx[0].r =  a->comp_f[C_R];
            vtx[0].g =  a->comp_f[C_G];
            vtx[0].b =  a->comp_f[C_B];
            vtx[1].r =  b->comp_f[C_R];
            vtx[1].g =  b->comp_f[C_G];
            vtx[1].b =  b->comp_f[C_B];
            vtx[2].r =  c->comp_f[C_R];
            vtx[2].g =  c->comp_f[C_G];
            vtx[2].b =  c->comp_f[C_B];
        }
    }

//    syslog("TriRenderSmooth");
    msiRenderTriangle(vtx+0, vtx+1, vtx+2, opacity);
//    syslog(" -> complete");
}

