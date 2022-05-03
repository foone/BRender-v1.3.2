/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: faceops.c 1.4 1998/07/27 14:55:50 jon Exp $
 * $Locker: $
 *
 * Face rendering functions that handle per-face operations, before actual rendering
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "vecifns.h"

BR_RCS_ID("$Id: faceops.c 1.4 1998/07/27 14:55:50 jon Exp $");

/*
 * Clip a triangle
 */
void BR_ASM_CALL OpTriangleClip(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	brp_vertex clip_in[3];
	int nclipped;
	brp_vertex *clipped;

	clip_in[0] = *v0; clip_in[1] = *v1; clip_in[2] = *v2;

	if((clipped = FaceClip(rend.renderer, clip_in, rend.renderer->state.cache.clip_slots, tfp->codes, 3, &nclipped)))
		ClippedRenderTriangles(rend.renderer, block->chain, clipped, nclipped, fp_vertices, fp_edges);
}

/*
 * Clip a triangle and generate per primitive components
 */
void BR_ASM_CALL OpTriangleClipConstantSurf(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	brp_vertex clip_in[3];
	int nclipped,i;
	brp_vertex *clipped;
	br_vector3 *vp_p;
	br_vector2 *vp_map;
	br_colour colour = scache.colour;
	br_vector3 rev_normal;

	/*
     * Face needs to be clipped
     */
	clip_in[0] = *v0; clip_in[1] = *v1; clip_in[2] = *v2;

	if((clipped = FaceClip(rend.renderer, clip_in, rend.renderer->state.cache.clip_slots, tfp->codes, 3, &nclipped))) {

		vp_p = rend.vertex_p + (*fp_vertices)[0];
		vp_map = rend.vertex_map + (*fp_vertices)[0];

		if(rend.renderer->state.surface.colour_source == BRT_GEOMETRY)
			colour = rend.face_colours[rend.current_index];


		if(tfp->flag & TFF_REVERSED) {

			BrVector3Negate(&rev_normal, (br_vector3 *)fp_eqn);

			for(i=0; i < rend.renderer->state.cache.nconstant_fns; i++)
				rend.renderer->state.cache.constant_fns[i](rend.renderer, vp_p, vp_map, &rev_normal, colour, clipped->comp);

		} else {
			for(i=0; i < rend.renderer->state.cache.nconstant_fns; i++)
				rend.renderer->state.cache.constant_fns[i](rend.renderer, vp_p, vp_map, (br_vector3 *)fp_eqn, colour, clipped->comp);
		}

		ClippedRenderTriangles(rend.renderer, block->chain, clipped, nclipped, fp_vertices, fp_edges);
	}
}

/*
 * Generate per primitive components
 */
void BR_ASM_CALL OpTriangleConstantSurf(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	br_vector3 *vp_p;
	br_vector2 *vp_map;
	br_colour colour = scache.colour;
	int i;

	vp_p = rend.vertex_p + (*fp_vertices)[0];
	vp_map = rend.vertex_map + (*fp_vertices)[0];

	if(rend.renderer->state.surface.colour_source == BRT_GEOMETRY)
		colour = rend.face_colours[rend.current_index];

	for(i=0; i < rend.renderer->state.cache.nconstant_fns; i++)
		rend.renderer->state.cache.constant_fns[i](rend.renderer, vp_p, vp_map, (br_vector3 *)fp_eqn, colour, v0->comp);

	block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

void BR_ASM_CALL OpTriangleTwoSidedConstantSurf(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	br_vector3 *vp_p;
	br_vector2 *vp_map;
	br_colour colour = scache.colour;
	br_vector3 rev_normal;
	int i;

	vp_p = rend.vertex_p + (*fp_vertices)[0];
	vp_map = rend.vertex_map + (*fp_vertices)[0];

	if(rend.renderer->state.surface.colour_source == BRT_GEOMETRY)
		colour = rend.face_colours[rend.current_index];

	if(tfp->flag & TFF_REVERSED) {

		BrVector3Negate(&rev_normal, (br_vector3 *)fp_eqn);

		for(i=0; i < rend.renderer->state.cache.nconstant_fns; i++)
			rend.renderer->state.cache.constant_fns[i](rend.renderer, vp_p, vp_map, &rev_normal, colour, v0->comp);

	} else {
		for(i=0; i < rend.renderer->state.cache.nconstant_fns; i++)
			rend.renderer->state.cache.constant_fns[i](rend.renderer, vp_p, vp_map, (br_vector3 *)fp_eqn, colour, v0->comp);
	}

	block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

/*
 * Fix up ugly seams in environment mapping
 */
void BR_ASM_CALL OpTriangleMappingWrapFix(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	br_scalar scale = BR_ABS(rend.renderer->state.cache.comp_scales[C_U]);
	br_scalar half = BR_CONST_DIV(scale,2);
	br_scalar d0,d1,d2;
	brp_vertex fixed[3];
	
	/*
	 * If change in U along any edge is > 0.5, then adjust one of the vertices to bring it
	 * into range (by adding 1)
	 */
	d0 = v1->comp[C_U] - v0->comp[C_U];
	d1 = v2->comp[C_U] - v1->comp[C_U];
	d2 = v0->comp[C_U] - v2->comp[C_U];


	if((d0 > half) || (d0 < -half) ||
	   (d1 > half) || (d1 < -half) ||
	   (d2 > half) || (d2 < -half)) {

		if(d0 > half || d2 <-half) {
			fixed[0] = *v0;
			fixed[0].comp[C_U] += scale;
			v0 = fixed+0;
		}
	
		if(d0 < -half || d1 > half) {
			fixed[1] = *v1;
			fixed[1].comp[C_U] += scale;
			v1 = fixed+1;
		}

		if(d1 < -half || d2 > half) {
			fixed[2] = *v2;
			fixed[2].comp[C_U] += scale;
			v2 = fixed+2;
		}
	}

	block->chain->render(block->chain, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
}

/*
 * handle relighting of vertices if a triangle is two-sided
 */
void BR_ASM_CALL OpTriangleRelightTwoSided(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	int i,v;
	br_vector3 *vp_p;
	br_vector2 *vp_map;
	br_vector3 *vp_n;
	brp_vertex tv[3];
   	br_vector3 rev_normal;
	br_colour colour = scache.colour;

	if(tfp->flag & TFF_REVERSED) {

		/*
		 * Face is visible from the 'other' side
		 * See if any vertices need relighting
		 */
		tv[0] = *v0;
		tv[1] = *v1;
		tv[2] = *v2;

		for(v=0; v<3; v++) {

			if((tv[v].flags & (TVDIR_FRONT | TVDIR_BACK)) == (TVDIR_FRONT | TVDIR_BACK)) {

				vp_p = rend.vertex_p + (*fp_vertices)[v];
				vp_map = rend.vertex_map + (*fp_vertices)[v];
				vp_n = rend.vertex_n + (*fp_vertices)[v];

				if(rend.renderer->state.surface.colour_source == BRT_GEOMETRY)
					colour = rend.vertex_colours[(*fp_vertices)[v]];

				BrVector3Negate(&rev_normal, vp_n);

				for(i=0; i < rend.renderer->state.cache.nvertex_fns; i++)
					rend.renderer->state.cache.vertex_fns[i](rend.renderer, vp_p, vp_map, &rev_normal, colour, tv[v].comp);
			}
		}
		block->chain->render(block->chain, tv+0, tv+1, tv+2, fp_vertices, fp_edges, fp_eqn, tfp);
	} else {
		block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges, fp_eqn, tfp);
	}
}

/*
 * Convert a triangle to lines
 */
void BR_ASM_CALL OpTriangleToLines(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3])
{
	/*
	 * Generate a line for each unrendered edge
	 */
	if(!rend.edge_flags[(*fp_edges)[0]]) {
		block->chain->render(block->chain, v0, v1);
		rend.edge_flags[(*fp_edges)[0]] = 1;
	}

	if(!rend.edge_flags[(*fp_edges)[1]]) {
		block->chain->render(block->chain, v1, v2);
		rend.edge_flags[(*fp_edges)[1]] = 1;
	}

	if(!rend.edge_flags[(*fp_edges)[2]]) {
		block->chain->render(block->chain, v2, v0);
		rend.edge_flags[(*fp_edges)[2]] = 1;
	}
}

/*
 * Copy constant components from vertex 0 to all other vertices (used prior to breaking
 * point into lines and points, or to use interpolated renderers for constant settings)
 */
void BR_ASM_CALL OpTriangleReplicateConstant(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3])
{
	br_uint_32 m;
	int c;

	m = rend.block->constant_mask;

	for(c = 0; m; c++, m >>=1) {
		if(m & 1)
			v1->comp[c] = v2->comp[c] = v0->comp[c];
	}

	block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

/*
 * Special case for just I
 */
void BR_ASM_CALL OpTriangleReplicateConstantI(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3])
{
	v1->comp[C_I] = v2->comp[C_I] = v0->comp[C_I];

	block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

/*
 * Special case for just RGB
 */
void BR_ASM_CALL OpTriangleReplicateConstantRGB(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3])
{
	v1->comp[C_R] = v2->comp[C_R] = v0->comp[C_R];
	v1->comp[C_G] = v2->comp[C_G] = v0->comp[C_G];
	v1->comp[C_B] = v2->comp[C_B] = v0->comp[C_B];

	block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges);
}

void BR_ASM_CALL OpTriangleToPoints(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3])
{
	/*
	 * Generate a point for each unrendered vertex
	 */
	if(!rend.vertex_flags[(*fp_vertices)[0]] && !(v0->flags & OUTCODES_ALL)) {
		block->chain->render(block->chain, v0);
		rend.vertex_flags[(*fp_vertices)[0]] = 1;
	}

	if(!rend.vertex_flags[(*fp_vertices)[1]] && !(v1->flags & OUTCODES_ALL)) {
		block->chain->render(block->chain, v1);
		rend.vertex_flags[(*fp_vertices)[1]] = 1;
	}

	if(!rend.vertex_flags[(*fp_vertices)[2]] && !(v2->flags & OUTCODES_ALL)) {
		block->chain->render(block->chain, v2);
		rend.vertex_flags[(*fp_vertices)[2]] = 1;
	}
}

void BR_ASM_CALL OpTriangleToPoints_OS(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3])
{
	/*
	 * Generate a point for each unrendered vertex
	 */
	if(!rend.vertex_flags[(*fp_vertices)[0]]) {
		block->chain->render(block->chain, v0);
		rend.vertex_flags[(*fp_vertices)[0]] = 1;
	}

	if(!rend.vertex_flags[(*fp_vertices)[1]]) {
		block->chain->render(block->chain, v1);
		rend.vertex_flags[(*fp_vertices)[1]] = 1;
	}

	if(!rend.vertex_flags[(*fp_vertices)[2]]) {
		block->chain->render(block->chain, v2);
		rend.vertex_flags[(*fp_vertices)[2]] = 1;
	}
}

void BR_ASM_CALL OpLineClip(struct brp_block *block, brp_vertex *v0, brp_vertex *v1)
{
	brp_vertex clipped[2];

	if(ClipLine(rend.renderer, clipped, v0, v1, rend.renderer->state.cache.clip_slots, v0->flags | v1->flags))
		ClippedRenderLine(rend.renderer, block->chain, clipped);
}

/**
 ** Triangle subdivision
 **/
#if BASED_FIXED
void BR_ASM_CALL averageVertices(struct br_renderer *renderer, brp_vertex *dest, brp_vertex *src1, brp_vertex *src2);
void BR_ASM_CALL averageVerticesOnScreen(struct br_renderer *renderer, brp_vertex *dest, brp_vertex *src1, brp_vertex *src2);
#else
void BR_ASM_CALL averageVerticesOnScreen(struct br_renderer *renderer, brp_vertex *dest1, brp_vertex *dest2, brp_vertex *dest3, brp_vertex *src1, brp_vertex *src2, brp_vertex *src3);
void BR_ASM_CALL averageVertices(struct br_renderer *renderer, brp_vertex *dest1, brp_vertex *dest2, brp_vertex *dest3, brp_vertex *src1, brp_vertex *src2, brp_vertex *src3);
#endif
br_boolean BR_ASM_CALL subdivideCheck(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2);
/*
 * Find and project midpoint of two vertices
 */

#if BASED_FIXED
void BR_ASM_CALL averageVertices(struct br_renderer *renderer, brp_vertex *dest, brp_vertex *src1, brp_vertex *src2)
{
    dest->comp[C_X] = BR_CONST_DIV(BR_ADD(src1->comp[C_X], src2->comp[C_X]),2);
    dest->comp[C_Y] = BR_CONST_DIV(BR_ADD(src1->comp[C_Y], src2->comp[C_Y]),2);

    dest->comp[C_Z] = BR_CONST_DIV(BR_ADD(src1->comp[C_Z], src2->comp[C_Z]),2);
    dest->comp[C_W] = BR_CONST_DIV(BR_ADD(src1->comp[C_W], src2->comp[C_W]),2);

    dest->comp[C_U] = BR_CONST_DIV(BR_ADD(src1->comp[C_U], src2->comp[C_U]),2);
    dest->comp[C_V] = BR_CONST_DIV(BR_ADD(src1->comp[C_V], src2->comp[C_V]),2);

    dest->comp[C_I] = BR_CONST_DIV(BR_ADD(src1->comp[C_I], src2->comp[C_I]),2);
    dest->comp[C_R] = BR_CONST_DIV(BR_ADD(src1->comp[C_R], src2->comp[C_R]),2);

    dest->comp[C_G] = BR_CONST_DIV(BR_ADD(src1->comp[C_G], src2->comp[C_G]),2);
    dest->comp[C_B] = BR_CONST_DIV(BR_ADD(src1->comp[C_B], src2->comp[C_B]),2);

    dest->flags = OUTCODES_NOT;

   	OUTCODE_POINT(dest->flags, (br_vector4 *)(dest->comp+C_X));

	if(!(dest->flags & OUTCODES_ALL)) {
        PROJECT_VERTEX_WRITE_Q(dest,dest->comp[C_X],dest->comp[C_Y],dest->comp[C_Z],dest->comp[C_W]);
    }
}
#endif

#if BASED_FIXED
/*
 * Find and project midpoint of two vertices - on screen case
 */
STATIC void BR_ASM_CALL averageVerticesOnScreen(struct br_renderer *renderer, brp_vertex *dest, brp_vertex *src1, brp_vertex *src2)
{
    dest->comp[C_X] = BR_CONST_DIV(BR_ADD(src1->comp[C_X], src2->comp[C_X]),2);
    dest->comp[C_Y] = BR_CONST_DIV(BR_ADD(src1->comp[C_Y], src2->comp[C_Y]),2);

    dest->comp[C_Z] = BR_CONST_DIV(BR_ADD(src1->comp[C_Z], src2->comp[C_Z]),2);
    dest->comp[C_W] = BR_CONST_DIV(BR_ADD(src1->comp[C_W], src2->comp[C_W]),2);

    dest->comp[C_U] = BR_CONST_DIV(BR_ADD(src1->comp[C_U], src2->comp[C_U]),2);
    dest->comp[C_V] = BR_CONST_DIV(BR_ADD(src1->comp[C_V], src2->comp[C_V]),2);

    dest->comp[C_I] = BR_CONST_DIV(BR_ADD(src1->comp[C_I], src2->comp[C_I]),2);
    dest->comp[C_R] = BR_CONST_DIV(BR_ADD(src1->comp[C_R], src2->comp[C_R]),2);

    dest->comp[C_G] = BR_CONST_DIV(BR_ADD(src1->comp[C_G], src2->comp[C_G]),2);
    dest->comp[C_B] = BR_CONST_DIV(BR_ADD(src1->comp[C_B], src2->comp[C_B]),2);

    PROJECT_VERTEX_WRITE_Q(dest,dest->comp[C_X],dest->comp[C_Y],dest->comp[C_Z],dest->comp[C_W]);
}
#endif

/*
 * Test for whether a face should be subdivided
 */
#define SUBDIVIDE_THRESHOLD BR_SCALAR(0.9)


#if 0
STATIC br_boolean subdivideCheck(brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
#if 0
    br_scalar lu,hu,lv,hv, q0,q1,q2;

    /*
     * Work out linear and hyperbolic interpolations of U and V at centre of triangle
     */
    lu = BR_CONST_DIV(v0->comp[C_U] + v1->comp[C_U] + v2->comp[C_U],3);
    lv = BR_CONST_DIV(v0->comp[C_V] + v1->comp[C_V] + v2->comp[C_V],3);

    q0 = BR_RCP(v0->comp[C_W]);
    q1 = BR_RCP(v1->comp[C_W]);
    q2 = BR_RCP(v2->comp[C_W]);

    hu = BR_MAC3DIV(v0->comp[C_U],q0,v1->comp[C_U],q1,v2->comp[C_U],q2, q0+q1+q2) ;
    hv = BR_MAC3DIV(v0->comp[C_V],q0,v1->comp[C_V],q1,v2->comp[C_V],q2, q0+q1+q2) ;

    /*
     * If error is greater than threshold, then don't subdivide
     */
    if(BR_ABS(lu-hu) < BR_SCALAR(2.0) && BR_ABS(lv-hv) < BR_SCALAR(2.0)) {
        return BR_FALSE;
    }
#endif

#if 1
    br_scalar z0,z1,z2,zt;

    /*
     * If the ratio of the smallest and largest Z is greater
     * than a threshold, then subdivide
     */
    z0 = BR_ABS(v0->comp[C_Z]);
    z1 = BR_ABS(v1->comp[C_Z]);
    z2 = BR_ABS(v2->comp[C_Z]);

    if(z0 > z1) {
        zt = z0; z0 = z1; z1 = zt;
    }

    if(z0 > z2) {
        zt = z0; z0 = z2; z2 = zt;
    }

    if(z1 > z2) {
        zt = z1; z1 = z2; z2 = zt;
    }

    if(z2 == BR_SCALAR(0.0))
        return BR_FALSE;

    if(z0 > BR_MUL(rend.subdivide_threshold,z2))
        return BR_FALSE;
#endif

    return BR_TRUE;
}
#endif


/*
 * Convert a generic tolerance spec. into a threshold for the above test
 */
void SubdivideSetThreshold(br_int_32 subdivide_tolerance)
{
	/*
	 * Range from 1/(1.05) to 1/(1.45) 0.95 -> 0.68
	 */
	rend.subdivide_threshold = BR_RCP(BR_MUL(BrIntToScalar(subdivide_tolerance),BR_SCALAR(-0.002)) + BR_SCALAR(1.25));
}


/*
 * Recursive function to subdivied a triangle - looks at outcode to see whether triangle should
 * be processed, and whether it is completely on screen
 */
STATIC void triangleSubdivideCheck(int depth, struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);
STATIC void triangleSubdivideOnScreen(int depth, struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
    br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp);


STATIC void triangleSubdivide(int depth, struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
	br_uint_32 combined_codes;

    /*
     * If triangle is not visible - then ignore it
     */
	combined_codes = v0->flags | v1->flags | v2->flags;

    if((combined_codes & OUTCODES_NOT) != OUTCODES_NOT)
        return;

	if(combined_codes & OUTCODES_ALL) {
        triangleSubdivideCheck(depth, block, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
    } else{
        triangleSubdivideOnScreen(depth, block, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
    }
}

/*
 * Special case of subdivision for on screen triangles
 */
STATIC void triangleSubdivideOnScreen(int depth, struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
    br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
    brp_vertex mid0,mid1,mid2;

    if(depth > 0 && subdivideCheck(v0,v1,v2)) {
        /*
         * Subdivide edges and reproject
         */
#if BASED_FIXED
		averageVerticesOnScreen(rend.renderer, &mid0, v0, v1);
        averageVerticesOnScreen(rend.renderer, &mid1, v1, v2);
        averageVerticesOnScreen(rend.renderer, &mid2, v2, v0);
#else
		averageVerticesOnScreen(rend.renderer,&mid1,&mid2,&mid0,v0,v1,v2);
#endif
        triangleSubdivideOnScreen(depth-1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivideOnScreen(depth-1, block, v0, &mid0, &mid2, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivideOnScreen(depth-1, block, v1, &mid1, &mid0, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivideOnScreen(depth-1, block, v2, &mid2, &mid1, fp_vertices, fp_edges, fp_eqn, tfp);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges, fp_eqn, tfp);
    }
}

/*
 * Subdivide a triangle that might be off screen
 */

STATIC void triangleSubdivideCheck(int depth, struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
    brp_vertex mid0,mid1,mid2;

    if(depth > 0 && subdivideCheck(v0,v1,v2)) {
        /*
         * Subdivide edges and reproject
         */
#if BASED_FIXED
		averageVertices(rend.renderer, &mid0, v0, v1);
        averageVertices(rend.renderer, &mid1, v1, v2);
        averageVertices(rend.renderer, &mid2, v2, v0);
#else
		averageVertices(rend.renderer,&mid1,&mid2,&mid0,v0,v1,v2);
#endif

        triangleSubdivide(depth-1, block, &mid0, &mid1, &mid2, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivide(depth-1, block, v0, &mid0, &mid2, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivide(depth-1, block, v1, &mid1, &mid0, fp_vertices, fp_edges, fp_eqn, tfp);
        triangleSubdivide(depth-1, block, v2, &mid2, &mid1, fp_vertices, fp_edges, fp_eqn, tfp);
    } else {
        block->chain->render(block->chain, v0, v1, v2, fp_vertices, fp_edges, fp_eqn, tfp);
    }
}

/*
 * Faces ops for subdivision
 */
void BR_ASM_CALL OpTriangleSubdivide(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
    triangleSubdivide(6, block, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
}

void BR_ASM_CALL OpTriangleSubdivideOnScreen(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3], br_vector4 *fp_eqn, struct temp_face *tfp)
{
    triangleSubdivideOnScreen(6, block, v0,v1,v2, fp_vertices, fp_edges, fp_eqn, tfp);
}

