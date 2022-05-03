/*
 * Copyright (c) 1993 Argonaut Software Ltd. All rights reserved.
 *
 * $Id: l_pi.c 1.1 1997/12/10 16:47:11 jon Exp $
 * $Locker: $
 *
 * Mesh rendering to produce edges
 */
#include "drv.h"
#include "shortcut.h"
#include "blockops.h"
#include "brassert.h"

static char rscid[] = "$Id: l_pi.c 1.1 1997/12/10 16:47:11 jon Exp $";

#define ABS(x) (((x)< 0)?-(x):(x))
#define SWAP(type,a,b) { type _t; _t = a; a = b; b = _t; }

#define ScalarsToRGB15(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 14) & 0x3e0 | ((r) >> 9) & 0x7c00)

#define ScalarsToRGB16(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 13) & 0x7c0 | ((r) >> 8) & 0xf800)

#define ScalarsToRGB24(r,g,b) \
    (((b) >> 16) & 0xff | ((g) >> 8) & 0xff00 | ((r) >> 8) & 0xff0000)


#define SETUP_TEXTURE \
        width = work.texture.width_p; \
        height = work.texture.height; \
        stride = work.texture.stride_b;

#define SETUP_LINE \
        x0 = v0->comp_x[C_SX]; \
        y0 = v0->comp_x[C_SY]; \
        x1 = v1->comp_x[C_SX]; \
        y1 = v1->comp_x[C_SY]; \
 \
	X0 = BrFixedToInt(x0); \
	Y0 = BrFixedToInt(y0); \
	X1 = BrFixedToInt(x1); \
	Y1 = BrFixedToInt(y1); \
 \
	CLAMP_LP(X0,Y0); \
	CLAMP_LP(X1,Y1); \
 \
	dx = X1-X0; \
	dy = Y1-Y0;

/*
 * Line drawers - Proper Bresenham algorithm
 */

#if PARTS & PART_8
/*
 * Gouraud shaded line
 */
void BR_ASM_CALL LineRenderPII(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,pi,di,x0,x1,y0,y1;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;

	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 1+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pi = v0->comp_x[C_I];

		if(dx > 0) {
                        di = BrFixedDiv((v1->comp_x[C_I] - v0->comp_x[C_I]),BrIntToFixed(dx));
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *ptr = (br_uint_8)BrFixedToInt(pi);

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr++;
		    }

		    /*
		     * Update parameters
		     */
		    pi += di;
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 1 : -1);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pi = v0->comp_x[C_I];

		if(dy > 0) {
                        di = BrFixedDiv((v1->comp_x[C_I] - v0->comp_x[C_I]),BrIntToFixed(dy));
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *ptr = (br_uint_8)BrFixedToInt(pi);

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pi += di;
		}

	}
}
#endif

#if PARTS & PART_8
/*
 * Arbitrary size lit textured lines
 */

void BR_ASM_CALL LineRenderPITI(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,pi,di,x0,x1,y0,y1;
	br_fixed_ls pu,pv,du,dv;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;
	int width,height,stride;

	SETUP_LINE;
	SETUP_TEXTURE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 1+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pi = v0->comp_x[C_I];
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dx > 0) {
                        di = BrFixedDiv((v1->comp_x[C_I] - v0->comp_x[C_I]),BrIntToFixed(dx));
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dx));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dx));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			char texel;
			texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
			*ptr = work.shade_table[((pi>>8) & 0xff00)+texel];
		    }

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr++;
		    }

		    /*
		     * Update parameters
		     */
		    pi += di;
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 1 : -1);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pi = v0->comp_x[C_I];
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dy > 0) {
                        di = BrFixedDiv((v1->comp_x[C_I] - v0->comp_x[C_I]),BrIntToFixed(dy));
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dy));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dy));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			char texel;
			texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
			*ptr = work.shade_table[((pi>>8) & 0xff00)+texel];
		    }

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pi += di;
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	}
}
#endif

#if PARTS & PART_8
/*
 * Arbitrary size textured lines
 */

void BR_ASM_CALL LineRenderPIT(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pu,pv,du,dv;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;
	int width,height,stride;
	char texel;

	SETUP_TEXTURE;
	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 1+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dx > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dx));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dx));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
		    *ptr = texel;

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr++;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 1 : -1);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dy > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dy));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dy));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
		    *ptr = texel;

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	}
}
#endif

#if PARTS & PART_24
/*
 * 24 bit textured lines
 */
void BR_ASM_CALL LineRenderPIT_RGB_888(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pu,pv,du,dv;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;
	int width,height,stride;

	SETUP_TEXTURE;
	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 3+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dx > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dx));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dx));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+3*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			char *texel;
			texel = (char *)work.texture.base+(pv>>16)*stride+3*(pu>>16);
			if (texel[0] || texel[1] || texel[2]) {
			  ptr[0] = texel[0];
			  ptr[1] = texel[1];
			  ptr[2] = texel[2];
			}
		    }

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 3;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 3 : -3);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dy > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dy));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dy));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0*3+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			char *texel;
			texel = (char *)work.texture.base+(pv>>16)*stride+3*(pu>>16);
			if (texel[0] || texel[1] || texel[2]) {
			  ptr[0] = texel[0];
			  ptr[1] = texel[1];
			  ptr[2] = texel[2];
			}
		    }

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	}
}
#endif

#if PARTS & PART_24
/*
 * Gouraud shaded 24-bit lines
 */
void BR_ASM_CALL LineRenderPII_RGB_888(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pr,pg,pb,dr,dg,db;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;

	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 3+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dx > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dx));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dx));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dx));

		}

		ptr = (char *)work.colour.base+3*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    ptr[0] = pb >> 16;
		    ptr[1] = pg >> 16;
		    ptr[2] = pr >> 16;

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 3;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 3 : -3);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dy > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dy));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dy));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dy));
		}

		ptr = (char *)work.colour.base+X0*3+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    ptr[0] = pb >> 16;
		    ptr[1] = pg >> 16;
		    ptr[2] = pr >> 16;

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	}
}
#endif

#if PARTS & PART_15
/*
 * 15 bit textured lines
 */
void BR_ASM_CALL LineRenderPIT_RGB_555(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pu,pv,du,dv;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;
	int width,height,stride;

	SETUP_TEXTURE;
	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 2+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dx > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dx));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dx));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+2*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			unsigned short texel;
			texel = *(unsigned short *)((char *)work.texture.base+(pv>>16)*stride+2*(pu>>16));
			if (texel)
			  *(short *)ptr = texel;
		    }

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 2;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 2 : -2);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dy > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dy));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dy));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0*2+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			unsigned short texel;
			texel = *(unsigned short *)((char *)work.texture.base+(pv>>16)*stride+2*(pu>>16));
			if (texel)
			  *(unsigned short *)ptr = texel;
		    }

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	}
}
#endif

#if PARTS & PART_15
/*
 * Gouraud shaded 15-bit lines
 */
void BR_ASM_CALL LineRenderPII_RGB_555(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pr,pg,pb,dr,dg,db;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;

	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 2+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dx > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dx));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dx));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dx));

		}

		ptr = (char *)work.colour.base+2*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *(unsigned short *)ptr = ScalarsToRGB15(pr,pg,pb);

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 2;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 2 : -2);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dy > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dy));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dy));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dy));
		}

		ptr = (char *)work.colour.base+X0*2+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *(unsigned short *)ptr = ScalarsToRGB15(pr,pg,pb);

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	}
}
#endif

#if PARTS & PART_16
/*
 * 16 bit textured lines
 */
void BR_ASM_CALL LineRenderPIT_RGB_565(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pu,pv,du,dv;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;
	int width,height,stride;

	SETUP_TEXTURE;
	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 2+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dx > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dx));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dx));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+2*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			unsigned short texel;
			texel = *(unsigned short *)((char *)work.texture.base+(pv>>16)*stride+2*(pu>>16));
			if (texel)
			  *(short *)ptr = texel;
		    }

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 2;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 2 : -2);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pu = v0->comp_x[C_U] % BrIntToFixed(width);
		if (pu<0)
		  pu += BrIntToFixed(width);
                pv = v0->comp_x[C_V] % BrIntToFixed(height);
		if (pv<0)
		  pv += BrIntToFixed(height);

		if(dy > 0) {
                        du = BrFixedDiv((v1->comp_x[C_U] - v0->comp_x[C_U]),BrIntToFixed(dy));
                        dv = BrFixedDiv((v1->comp_x[C_V] - v0->comp_x[C_V]),BrIntToFixed(dy));

			du = du % BrIntToFixed(width);
			if (du>0)
			  du -= BrIntToFixed(width);
			dv = dv % BrIntToFixed(height);
			if (dv>0)
			  dv -= BrIntToFixed(height);
		}

		ptr = (char *)work.colour.base+X0*2+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    {
			unsigned short texel;
			texel = *(unsigned short *)((char *)work.texture.base+(pv>>16)*stride+2*(pu>>16));
			if (texel)
			  *(unsigned short *)ptr = texel;
		    }

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pu += du;
		    if (pu<0)
		      pu += BrIntToFixed(width);
		    pv += dv;
		    if (pv<0)
		      pv += BrIntToFixed(height);
		}

	}
}
#endif

#if PARTS & PART_16
/*
 * Gouraud shaded 16-bit lines
 */
void BR_ASM_CALL LineRenderPII_RGB_565(struct brp_block *block, union brp_vertex *v0,union brp_vertex *v1)
{
	br_fixed_ls dx,dy;
	br_fixed_ls pm,dm,x0,x1,y0,y1;
	br_fixed_ls pr,pg,pb,dr,dg,db;
	int error;
	int count,offset,base;
	int X0,Y0,X1,Y1;
	char *ptr;
	int dptr;

	SETUP_LINE;

	error = 0;

	if(ABS(dx) > ABS(dy)) {
		/*
		 * Major axis is X axis - ensure dx is +ve
		 */


		if(dx < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = 2+(dy>0 ? work.colour.stride_b : -work.colour.stride_b);
		dy = ABS(dy);

		if (dx<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dx > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dx));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dx));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dx));

		}

		ptr = (char *)work.colour.base+2*X0+Y0*work.colour.stride_b;

		count = dx;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *(unsigned short *)ptr = ScalarsToRGB16(pr,pg,pb);

		    error += dy;
		    if (error>0) {
		      error -= dx;
		      ptr += dptr;
		    } else {
		      ptr += 2;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	} else {
		/*
		 * Major axis is Y axis - ensure dy is +ve
		 */


		if(dy < 0) {
			SWAP(union brp_vertex *,v0,v1);
			SWAP(int,X0,X1);
			SWAP(int,Y0,Y1);
			dx = -dx;
			dy = -dy;
		}

		dptr = work.colour.stride_b+(dx>0 ? 2 : -2);
		dx = ABS(dx);
		
		if (dy<0)
			return;
			
                pr = v0->comp_x[C_R];
                pg = v0->comp_x[C_G];
                pb = v0->comp_x[C_B];

		if(dy > 0) {
                        dr = BrFixedDiv((v1->comp_x[C_R] - v0->comp_x[C_R]),BrIntToFixed(dy));
                        dg = BrFixedDiv((v1->comp_x[C_G] - v0->comp_x[C_G]),BrIntToFixed(dy));
                        db = BrFixedDiv((v1->comp_x[C_B] - v0->comp_x[C_B]),BrIntToFixed(dy));
		}

		ptr = (char *)work.colour.base+X0*2+Y0*work.colour.stride_b;

		count = dy;

		while(count-- >= 0) {
		    /*
		     * plot pixel
		     */
	     
		    *(unsigned short *)ptr = ScalarsToRGB16(pr,pg,pb);

		    error += dx;
		    if (error>0) {
		      error -= dy;
		      ptr += dptr;
		    } else {
		      ptr += work.colour.stride_b;
		    }

		    /*
		     * Update parameters
		     */
		    pr += dr;
		    pg += dg;
		    pb += db;
		}

	}
}
#endif
