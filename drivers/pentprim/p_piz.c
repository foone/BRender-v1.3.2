/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: p_piz.c 1.1 1997/12/10 16:48:01 jon Exp $
 * $Locker: $
 *
 * Mesh rendering to produce points
 */
#include "drv.h"
#include "shortcut.h"
#include "blockops.h"

static char rscid[] = "$Id: p_piz.c 1.1 1997/12/10 16:48:01 jon Exp $";

#define ScalarsToRGB15(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 14) & 0x3e0 | ((r) >> 9) & 0x7c00)

#define ScalarsToRGB16(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 13) & 0x7c0 | ((r) >> 8) & 0xf800)

#define ScalarsToRGB24(r,g,b) \
    (((b) >> 16) & 0xff | ((g) >> 8) & 0xff00 | ((r) >> 8) & 0xff0000)

#define SETUP_POINT \
	x0 = BrFixedToInt(tvp->comp_x[C_SX]); \
	y0 = BrFixedToInt(tvp->comp_x[C_SY]); \
	CLAMP_LP(x0,y0);

#define SETUP_TEXTURE \
	width = work.texture.width_p; \
	height = work.texture.height; \
	stride = work.texture.stride_b;

/*
 * Setup arbitrary width u and v
 */
#define SETUP_UV \
	pu = tvp->comp_x[C_U] % BrIntToFixed(width); \
	if (pu<0) \
	  pu += BrIntToFixed(width); \
	pv = tvp->comp_x[C_V] % BrIntToFixed(height); \
	if (pv<0) \
	  pv += BrIntToFixed(height);

/*
 * Set up RGB
 */
#define SETUP_RGB \
	  pr = tvp->comp_x[C_R]; \
	  pg = tvp->comp_x[C_G]; \
	  pb = tvp->comp_x[C_B];

#if PARTS & PART_8Z
/*
 * 8 bit z-buffered points
 */
void BR_ASM_CALL PointRenderPIZ2(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;
	unsigned short pz;

	SETUP_POINT;
	
	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	o = y0*work.colour.stride_b+x0;

	if(((unsigned short *)work.depth.base)[o] > pz) {
		((unsigned short *)work.depth.base)[o] = pz;
		((char *)work.colour.base)[o] = BrFixedToInt(tvp->comp_x[C_I]);
	}
}
#endif

#if PARTS & PART_8Z
void BR_ASM_CALL PointRenderPIZ2TI(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv;
	unsigned short pz;
	char texel;

	SETUP_POINT;
	SETUP_TEXTURE;
	SETUP_UV;

	pi = tvp->comp_x[C_I];
	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	o = y0*work.colour.stride_b+x0;

	if(((unsigned short *)work.depth.base)[o] > pz) {
		texel = ((char *)work.texture.base)[(pv>>16)*width+(pu>>16)];
		if (texel) {
			((unsigned short *)work.depth.base)[o] = pz;
			((char *)work.colour.base)[o] = work.shade_table[256*(pi>>16)+texel];
		}
	}
}
#endif

#if PARTS & PART_8Z
/*
 * 8 bit textured points
 */
void BR_ASM_CALL PointRenderPIZ2T(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;
	br_fixed_ls width,height,stride,pu,pv;
	unsigned short pz;
	char texel;

	SETUP_POINT;
	SETUP_TEXTURE;
	SETUP_UV;
	
	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	o = y0*work.colour.stride_b+x0;

	if(((unsigned short *)work.depth.base)[o] > pz) {
		texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
		if (texel) {
			((unsigned short *)work.depth.base)[o] = pz;
			((char *)work.colour.base)[o] = texel;
		}
	}
}
#endif

typedef enum {
  LINE_RGB,LINE_TEXTURE
} point_type;

#if PARTS & PART_24Z
/*
 * 24 bit point renderer
 */
void BR_ASM_CALL PointRenderPIZ2_Generic_RGB_888(point_type mode,union brp_vertex *tvp)
{
	int oc,oz;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	unsigned short pz;
	char *texel;

	SETUP_POINT;

	if (mode==LINE_RGB) {
	  SETUP_RGB;
	}

	if (mode==LINE_TEXTURE) {

	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	oc = y0*work.colour.stride_b+3*x0;
	oz = y0*work.depth.stride_b+x0+x0;

	if(*(unsigned short *)((char *)work.depth.base+oz) > pz) {
		if (mode==LINE_TEXTURE) {
			texel = ((char *)work.texture.base)+(pv>>16)*stride+3*(pu>>16);
			if (texel[0] || texel[1] || texel[2]) {
				*(unsigned short *)((char *)work.depth.base+oz) = pz;
				((char *)work.colour.base)[oc] = texel[0];
				((char *)work.colour.base)[oc+1] = texel[1];
				((char *)work.colour.base)[oc+2] = texel[2];
			}
		} else {
			*(unsigned short *)((char *)work.depth.base+oz) = pz;
			((char *)work.colour.base)[oc] = pb>>16;
			((char *)work.colour.base)[oc+1] = pg>>16;
			((char *)work.colour.base)[oc+2] = pr>>16;
		}
	}
}
#endif

#if PARTS & PART_24Z
/*
 * 24 bit coloured points
 */
void BR_ASM_CALL PointRenderPIZ2_RGB_888(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_888(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_24Z
/*
 * 24 bit textured points
 */
void BR_ASM_CALL PointRenderPIZ2T_RGB_888(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_888(LINE_TEXTURE,tvp);
}
#endif

#if PARTS & PART_15Z
/*
 * 24 bit points
 */
void BR_ASM_CALL PointRenderPIZ2_Generic_RGB_555(point_type mode,union brp_vertex *tvp)
{
	int oc,oz;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	unsigned short pz;
	unsigned short texel;

	SETUP_POINT;
	
	if (mode==LINE_RGB) {
	  SETUP_RGB;
	}

	if (mode==LINE_TEXTURE) {

	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	oc = y0*work.colour.stride_b+2*x0;
	oz = y0*work.depth.stride_b+x0+x0;

	if(*(unsigned short *)((char *)work.depth.base+oz) > pz) {
		if (mode==LINE_TEXTURE) {

			texel = *(unsigned short *)(((char *)work.texture.base)+(pv>>16)*stride+2*(pu>>16));

			if (texel) {
				*(unsigned short *)((char *)work.depth.base+oz) = pz;
				*(unsigned short *)(((char *)work.colour.base)+oc) = texel;
			}
		} else {
			*(unsigned short *)((char *)work.depth.base+oz) = pz;
			*(unsigned short *)(((char *)work.colour.base)+oc) = ScalarsToRGB15(pr,pg,pb);
		}
	}
}
#endif

#if PARTS & PART_15Z
/*
 * 24 bit coloured points
 */
void BR_ASM_CALL PointRenderPIZ2_RGB_555(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_555(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_15Z
/*
 * 24 bit textured points
 */
void BR_ASM_CALL PointRenderPIZ2T_RGB_555(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_555(LINE_TEXTURE,tvp);
}
#endif

#if PARTS & PART_16Z
/*
 * 24 bit points
 */
void BR_ASM_CALL PointRenderPIZ2_Generic_RGB_565(point_type mode,union brp_vertex *tvp)
{
	int oc,oz;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	unsigned short pz;
	unsigned short texel;

	SETUP_POINT;
	
	if (mode==LINE_RGB) {
	  SETUP_RGB;
	}

	if (mode==LINE_TEXTURE) {

	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	pz = (unsigned short)(tvp->comp_x[C_SZ] >> 16) ^ 0x8000;

	oc = y0*work.colour.stride_b+2*x0;
	oz = y0*work.depth.stride_b+x0+x0;

	if(*(unsigned short *)((char *)work.depth.base+oz) > pz) {
		if (mode==LINE_TEXTURE) {

			texel = *(unsigned short *)(((char *)work.texture.base)+(pv>>16)*stride+2*(pu>>16));

			if (texel) {
				*(unsigned short *)((char *)work.depth.base+oz) = pz;
				*(unsigned short *)(((char *)work.colour.base)+oc) = texel;
			}
		} else {
			*(unsigned short *)((char *)work.depth.base+oz) = pz;
			*(unsigned short *)(((char *)work.colour.base)+oc) = ScalarsToRGB16(pr,pg,pb);
		}
	}
}
#endif

#if PARTS & PART_16Z
/*
 * 24 bit coloured points
 */
void BR_ASM_CALL PointRenderPIZ2_RGB_565(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_565(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_16Z
/*
 * 24 bit textured points
 */
void BR_ASM_CALL PointRenderPIZ2T_RGB_565(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPIZ2_Generic_RGB_565(LINE_TEXTURE,tvp);
}
#endif
