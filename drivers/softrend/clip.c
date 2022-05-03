/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: clip.c 1.2 1998/07/20 14:45:10 jon Exp $
 * $Locker: $
 *
 * Mesh rendering to produce faces
 */
#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

BR_RCS_ID("$Id: clip.c 1.2 1998/07/20 14:45:10 jon Exp $");

/*
 * Core of 3D homogenous clipper - loosly based on Paul Heckbert's
 * implemetationin Graphics Gems I.
 *
 * Clip a polygon to an arbitary plane eqn.
 */
STATIC int ClipFaceToPlane(
		union brp_vertex *vp,
		union brp_vertex *verts_out,
		int num_in,
		br_vector4 *plane,
		int cmask)
{
	union brp_vertex *wp = verts_out;
	int num_out = 0;

	union brp_vertex *up;
	br_scalar t,tu,tv;
	int m;
	br_scalar *usp,*vsp,*wsp;
	/*
	 * Go round face, an edge at a time
	 */
	up = vp+num_in-1;

	tu =-BR_MAC4(
			plane->v[0],up->comp[C_X],
			plane->v[1],up->comp[C_Y],
			plane->v[2],up->comp[C_Z],
			plane->v[3],up->comp[C_W]);

	for( ; num_in-- ; up = vp, tu = tv, vp++) {

		tv = -BR_MAC4(
				plane->v[0],vp->comp[C_X],
				plane->v[1],vp->comp[C_Y],
				plane->v[2],vp->comp[C_Z],
				plane->v[3],vp->comp[C_W]);

		if(tv <= S0) {

			/*
			 * This vertex is inside clip space
			 */
			if(tu <= S0) {

				/*
				 * last vertex was as well - add this vertex
				 */
				*wp++ = *vp;
				num_out++;
				continue;
			}

			if (tv < S0) {

				/*
				 * Edge crosses out to in, add intersection	and this vertex
				 */
				t = BR_DIVR(tv,(tv-tu));

				usp = up->comp;
				vsp = vp->comp;
				wsp = wp->comp;

				for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
					if(m & 1)
						*wsp = *vsp + BR_MUL(t,(*usp-*vsp));

				wp->flags = TV_CLIPPED;
				wp++;
				num_out++;
			}

			/*
			 * Copy next vertex
			 */
			*wp++ = *vp;
			num_out++;

		} else {

			/*
			 * This vertex is outside clip space
			 */
			if(tu >= S0)

				/*
				 * last vertex was as well - don't do anything
				 */
				continue;

			/*
			 * Edge crosses in to out, add intersection 
			 */

			t = BR_DIVR(tu,(tu-tv));

			usp = up->comp;
			vsp = vp->comp;
			wsp = wp->comp;

			for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
				if(m & 1)
					*wsp = *usp + BR_MUL(t,(*vsp-*usp));

			wp->flags = TV_CLIPPED;
			wp++;
			num_out++;
		}
	}

	return num_out;
}

/*
 * Special case of sign = 1, k = 1.0
 */
STATIC int ClipFaceToPlus1(
		union brp_vertex *vp,
		union brp_vertex *verts_out,
		int num_in,
		int axis,
		int cmask)
{
	union brp_vertex *wp = verts_out;
	int num_out = 0;

	union brp_vertex *up;
	br_scalar t,tu,tv;
	int m;
	br_scalar *usp,*vsp,*wsp;

	/*
	 * Go round face, an edge at a time
	 */
	up = vp+num_in-1;
	tu = up->comp[axis] - up->comp[C_W];

	for( ; num_in-- ; up = vp, tu = tv, vp++) {

		tv = vp->comp[axis] - vp->comp[C_W];

		if(tv <= S0) {

			/*
			 * This vertex is inside clip space
			 */
			if(tu <= S0) {

				/*
				 * last vertex was as well - add this vertex
				 */
				*wp++ = *vp;
				num_out++;
				continue;
			}

			if (tv < S0) {

				/*
				 * Edge crosses out to in, add intersection	and this vertex
				 */
				t = BR_DIVR(tv,(tv-tu));

				usp = up->comp;
				vsp = vp->comp;
				wsp = wp->comp;

				for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
					if(m & 1)
						*wsp = *vsp + BR_MUL(t,(*usp-*vsp));

				wp->comp[axis] = wp->comp[C_W];
				wp->flags = TV_CLIPPED;
				wp++;
				num_out++;
			}

			/*
			 * Copy next vertex
			 */
			*wp++ = *vp;
			num_out++;

		} else {

			/*
			 * This vertex is outside clip space
			 */
			if(tu >= S0)

				/*
				 * last vertex was as well - don't do anything
				 */
				continue;

			/*
			 * Edge crosses in to out, add intersection 
			 */

			t = BR_DIVR(tu,(tu-tv));

			usp = up->comp;
			vsp = vp->comp;
			wsp = wp->comp;

			for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
				if(m & 1)
					*wsp = *usp + BR_MUL(t,(*vsp-*usp));

			wp->comp[axis] = wp->comp[C_W];
			wp->flags = TV_CLIPPED;
			wp++;
			num_out++;
		}
	}

	return num_out;
}

/*
 * Special case of sign = -1, k = 1.0
 */
STATIC int ClipFaceToMinus1(
		union brp_vertex *vp,
		union brp_vertex *verts_out,
		int num_in,
		int axis,
		int cmask)
{
	union brp_vertex *wp = verts_out;
	int num_out = 0;

	union brp_vertex *up;
	br_scalar t,tu,tv;
	int m;
	br_scalar *usp,*vsp,*wsp;

	/*
	 * Go round face, an edge at a time
	 */
	up = vp+num_in-1;
	tu = - up->comp[axis] - up->comp[C_W];

	for( ; num_in-- ; up = vp, tu = tv, vp++) {

		tv = - vp->comp[axis] - vp->comp[C_W];

		if(tv <= S0) {

			/*
			 * This vertex is inside clip space
			 */
			if(tu <= S0) {

				/*
				 * last vertex was as well - add this vertex
				 */
				*wp++ = *vp;
				num_out++;
				continue;
			}

			if (tv < S0) {

				/*
				 * Edge crosses out to in, add intersection	and this vertex
				 */
				t = BR_DIVR(tv,(tv-tu));

				usp = up->comp;
				vsp = vp->comp;
				wsp = wp->comp;

				for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
					if(m & 1)
						*wsp = *vsp + BR_MUL(t,(*usp-*vsp));

				wp->comp[axis] = -wp->comp[C_W] + 2*BR_SCALAR_EPSILON;
				wp->flags = TV_CLIPPED;
				wp++;
				num_out++;
			}

			/*
			 * Copy next vertex
			 */
			*wp++ = *vp;
			num_out++;

		} else {

			/*
			 * This vertex is outside clip space
			 */
			if(tu >= S0)

				/*
				 * last vertex was as well - don't do anything
				 */
				continue;

			/*
			 * Edge crosses in to out, add intersection 
			 */

			t = BR_DIVR(tu,(tu-tv));

			usp = up->comp;
			vsp = vp->comp;
			wsp = wp->comp;

			for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
				if(m & 1)
					*wsp = *usp + BR_MUL(t,(*vsp-*usp));

			wp->comp[axis] = -wp->comp[C_W] + 2*BR_SCALAR_EPSILON;
			wp->flags = TV_CLIPPED;
			wp++;
			num_out++;
		}
	}

	return num_out;
}

#define CLIP_TOGGLE 			\
	if(toggle = !toggle) {		\
		cp_in = clip_poly_2;	\
		cp_out = clip_poly_1;	\
	} else {					\
		cp_in = clip_poly_1;	\
		cp_out = clip_poly_2;	\
	}							\

/*
 * Clip a face to the view volume
 */
union brp_vertex *FaceClip(br_renderer *self, union brp_vertex *clip_in, br_uint_32 mask, br_uint_32 codes, int n, int *n_out)
{
	static union brp_vertex clip_poly_1[16];
	static union brp_vertex clip_poly_2[16];
	union brp_vertex *cp_in = clip_in,*cp_out = clip_poly_1;
	int c;
	br_boolean toggle = BR_TRUE;

	/*
	 * Clip against each plane - if necessary
	 * After each plane, swap polygon buffers, and quit if
	 * polygon is completely clipped away
	 */
	if(codes & OUTCODE_HITHER) {
		n = ClipFaceToPlus1(cp_in,cp_out,n,C_Z,mask & ~(1<<C_Z));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	if(codes & OUTCODE_YON) {
		n = ClipFaceToMinus1(cp_in,cp_out,n,C_Z,mask & ~(1<<C_Z));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	if(codes & OUTCODE_RIGHT) {
		n = ClipFaceToPlus1(cp_in,cp_out,n,C_X,mask & ~(1<<C_X));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	if(codes & OUTCODE_LEFT) {
		n = ClipFaceToMinus1(cp_in,cp_out,n,C_X,mask & ~(1<<C_X));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	if(codes & OUTCODE_TOP) {
		n = ClipFaceToPlus1(cp_in,cp_out,n,C_Y,mask & ~(1<<C_Y));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	if(codes & OUTCODE_BOTTOM) {
		n = ClipFaceToMinus1(cp_in,cp_out,n,C_Y,mask & ~(1<<C_Y));
		if(n < 3) return NULL;
		CLIP_TOGGLE;
	}

	/*
	 * User-defined clip plane
	 */
	if(scache.user_clip_active) {
			for(c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

			if(self->state.clip[c].type != BRT_PLANE)
				continue;

		 	if(!(codes & (OUTCODE_USER << c)))
				continue;

			n = ClipFaceToPlane(cp_in,cp_out,n,
							&self->state.clip[c].plane,
							mask);
			if(n < 3)
				return NULL;
			CLIP_TOGGLE;
		}
	}

	*n_out = n;
	return cp_in;
}

/*
 * Render a clipped face using the current block
 */
void ClippedRenderTriangles(struct br_renderer *renderer, brp_block *block, union brp_vertex *cp_in, int n,
	br_uint_16 (*fp_vertices)[3], br_uint_16 (*fp_edges)[3])
{
	int i;
	union brp_vertex *tvp;

	/*
	 * Re-project all the vertices
	 */
	if (rend.block->vertex_components & CM_Q)
		for(i=0, tvp = cp_in; i < n; i++, tvp++) {
			if(tvp->flags & (TV_CLIPPED | OUTCODES_ALL)) {
				PROJECT_VERTEX_WRITE_Q(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
				UPDATE_BOUNDS(tvp);
			}
		}
	else
		for(i=0, tvp = cp_in; i < n; i++, tvp++) {
			if(tvp->flags & (TV_CLIPPED | OUTCODES_ALL)) {
				PROJECT_VERTEX(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
				UPDATE_BOUNDS(tvp);
			}
		}

	/*
	 * Triangulate polygon
	 */
	for(i = 2; i < n; i++)
		block->render(block,&cp_in[0],&cp_in[i-1],&cp_in[i], fp_vertices, fp_edges);
}

/*
 * Clip a line to an arbitary plane eqn. Return true if any part
 * of the line remains
 */
br_boolean ClipLineToPlane(
		union brp_vertex *in,
		union brp_vertex *out,
		br_vector4 *plane,
		int cmask)
{
	br_scalar t,tu,tv;
	int m;
	br_scalar *usp,*vsp,*wsp;

	tu =-BR_MAC4(
			plane->v[0],in[0].comp[C_X],
			plane->v[1],in[0].comp[C_Y],
			plane->v[2],in[0].comp[C_Z],
			plane->v[3],in[0].comp[C_W]);

	tv =-BR_MAC4(
			plane->v[0],in[1].comp[C_X],
			plane->v[1],in[1].comp[C_Y],
			plane->v[2],in[1].comp[C_Z],
			plane->v[3],in[1].comp[C_W]);

	out[0] = in[0];
	out[1] = in[1];

	if(tu <= S0) {
		/*
		 * First vertex is inside clip space
		 */

		if(tv <= S0) {
			/*
			 * last vertex was as well - return whole line
			 */
			out[1] = in[1];
			return BR_TRUE;
		}

		/*
		 * Line crosses in to out, truncate to intersection
		 */
		t = BR_DIVR(tu,(tu-tv));

		usp = in[0].comp;
		vsp = in[1].comp;
		wsp = out[1].comp;

		for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
			if(m & 1)
				*wsp = *usp + BR_MUL(t,(*vsp-*usp));
	} else {
		/*
		 * First vertex is outside clip space
		 */
		if(tv > S0)
			/*
			 * last vertex was as well - return false
			 */
			return BR_FALSE;

		/*
		 * Line crosses out to in, truncate to intersection
		 */

		t = BR_DIVR(tv,(tv-tu));

		usp = in[0].comp;
		vsp = in[1].comp;
		wsp = out[0].comp;

		for(m = cmask ; m ; m >>=1, usp++,vsp++,wsp++)
			if(m & 1)
				*wsp = *vsp + BR_MUL(t,(*usp-*vsp));
	}

	return BR_TRUE;
}

br_boolean ClipLine(br_renderer *self, union brp_vertex *out, union brp_vertex *v0, union brp_vertex *v1, br_uint_32 mask, br_uint_32 codes)
{
	static br_vector4 plane_px = BR_VECTOR4(-1, 0, 0,1);
	static br_vector4 plane_nx = BR_VECTOR4( 1, 0, 0,1);
	static br_vector4 plane_py = BR_VECTOR4( 0,-1, 0,1);
	static br_vector4 plane_ny = BR_VECTOR4( 0, 1, 0,1);
	static br_vector4 plane_pz = BR_VECTOR4( 0, 0,-1,1);
	static br_vector4 plane_nz = BR_VECTOR4( 0, 0, 1,1);

	union brp_vertex cv0[2],cv1[2];
	union brp_vertex  *cp_in = cv0,*cp_out = cv1,*cp_tmp;
	int c;

	cp_in[0] = *v0;
	cp_in[1] = *v1;

	if(codes & OUTCODE_LEFT) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_nx,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	if(codes & OUTCODE_RIGHT) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_px,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	if(codes & OUTCODE_TOP) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_py,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	if(codes & OUTCODE_BOTTOM) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_ny,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	if(codes & OUTCODE_HITHER) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_pz,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	if(codes & OUTCODE_YON) {
		if(!ClipLineToPlane(cp_in,cp_out,&plane_nz,mask))
			return BR_FALSE;

		cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
	}

	/*
	 * User-defined clip plane
	 */

	if(scache.user_clip_active) {
			for(c = 0; c < MAX_STATE_CLIP_PLANES; c++) {

			if(self->state.clip[c].type != BRT_PLANE)
				continue;

		 	if(!(codes & (OUTCODE_USER << c)))
				continue;

			if(ClipLineToPlane(cp_in, cp_out, &self->state.clip[c].plane,mask) == 0)
				return BR_FALSE;

			cp_tmp = cp_in; cp_in = cp_out; cp_out = cp_tmp;
		}
	}

	out[0] = cp_in[0];
	out[1] = cp_in[1];

	return BR_TRUE;
}
void ClippedRenderLine(struct br_renderer *renderer, brp_block *block, union brp_vertex *cp_in)
{
	int i;
	union brp_vertex *tvp;

	/*
	 * Re-project all the vertices
	 */
	if (rend.block->vertex_components & CM_Q)
		for(i=0, tvp = cp_in; i < 2; i++, tvp++) {
			PROJECT_VERTEX_WRITE_Q(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
			UPDATE_BOUNDS(tvp);
		}
	else
		for(i=0, tvp = cp_in; i < 2; i++, tvp++) {
			PROJECT_VERTEX(tvp,tvp->comp[C_X],tvp->comp[C_Y],tvp->comp[C_Z],tvp->comp[C_W]);
			UPDATE_BOUNDS(tvp);
		}

	/*
	 * Render the line
	 */
	block->render(block,&cp_in[0],&cp_in[1]);
}

