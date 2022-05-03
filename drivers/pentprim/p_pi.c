/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: p_pi.c 1.1 1997/12/10 16:47:59 jon Exp $
 * $Locker: $
 *
 * Point renderers
 */
#include "drv.h"
#include "shortcut.h"
#include "blockops.h"

static char rscid[] = "$Id: p_pi.c 1.1 1997/12/10 16:47:59 jon Exp $";

#define ScalarsToRGB15(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 14) & 0x3e0 | ((r) >> 9) & 0x7c00)

#define ScalarsToRGB16(r,g,b) \
    (((b) >> 19) & 0x1f | ((g) >> 13) & 0x7c0 | ((r) >> 8) & 0xf800)

#define ScalarsToRGB24(r,g,b) \
    (((b) >> 16) & 0xff | ((g) >> 8) & 0xff00 | ((r) >> 8) & 0xff0000)

/*
 * Fetch and clamp screen coordinates
 */
#define SETUP_POINT \
        x0 = BrFixedToInt(tvp->comp_x[C_SX]); \
        y0 = BrFixedToInt(tvp->comp_x[C_SY]); \
	CLAMP_LP(x0,y0);

/*
 * Fetch details about texture
 */
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

#if PARTS & PART_8
void BR_ASM_CALL PointRenderPI(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;

	SETUP_POINT;

	o = y0*work.colour.stride_b+x0;

        ((char *)work.colour.base)[o] = BrFixedToInt(tvp->comp_x[C_I]);
}
#endif

#if PARTS & PART_8
void BR_ASM_CALL PointRenderPITI(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv;
	char texel;

	SETUP_POINT;
	SETUP_TEXTURE;
	
        pi = tvp->comp_x[C_I];

	SETUP_UV;

	o = y0*work.colour.stride_b+x0;

	texel = ((char *)work.texture.base)[(pv>>16)*stride+(pu>>16)];
	((char *)work.colour.base)[o] = work.shade_table[256*(pi>>16)+texel];
}
#endif

#if PARTS & PART_8
void BR_ASM_CALL PointRenderPIT(struct brp_block *block, union brp_vertex *tvp)
{
	int o;
	int x0,y0;
	br_fixed_ls width,height,stride,pu,pv;
	char texel;

	SETUP_POINT;
	SETUP_TEXTURE;
	SETUP_UV;

	o = y0*work.colour.stride_b+x0;

	texel = ((char *)work.texture.base)[(pv>>16)*width+(pu>>16)];
	if (texel)
	  ((char *)work.colour.base)[o] = texel;
}
#endif

typedef enum {
  LINE_RGB,LINE_TEXTURE
} point_type;


#if PARTS & PART_24
void BR_ASM_CALL PointRenderPI_Generic_RGB_888(point_type mode,union brp_vertex *tvp)
{
	int oc;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	char *texel;

	SETUP_POINT;
	
	if (mode==LINE_RGB) {
          pr = tvp->comp_x[C_R];
          pg = tvp->comp_x[C_G];
          pb = tvp->comp_x[C_B];
	}

	if (mode==LINE_TEXTURE) {

	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	oc = y0*work.colour.stride_b+3*x0;

	if (mode==LINE_TEXTURE) {
	  texel = ((char *)work.texture.base)+(pv>>16)*stride+3*(pu>>16);
	  if (texel[0] || texel[1] || texel[2]) {
	    ((char *)work.colour.base)[oc] = texel[0];
	    ((char *)work.colour.base)[oc+1] = texel[1];
	    ((char *)work.colour.base)[oc+2] = texel[2];
	  }
	} else {
	  ((char *)work.colour.base)[oc] = pb>>16;
	  ((char *)work.colour.base)[oc+1] = pg>>16;
	  ((char *)work.colour.base)[oc+2] = pr>>16;
	}
}
#endif

#if PARTS & PART_24
void BR_ASM_CALL PointRenderPI_RGB_888(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_888(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_24
void BR_ASM_CALL PointRenderPIT_RGB_888(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_888(LINE_TEXTURE,tvp);
}
#endif

#if PARTS & PART_15
void BR_ASM_CALL PointRenderPI_Generic_RGB_555(point_type mode,union brp_vertex *tvp)
{
	int oc;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	unsigned short texel;

	SETUP_POINT;
	
	if (mode==LINE_RGB) {
          pr = tvp->comp_x[C_R];
          pg = tvp->comp_x[C_G];
          pb = tvp->comp_x[C_B];
	}

	if (mode==LINE_TEXTURE) {
	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	oc = y0*work.colour.stride_b+2*x0;

	if (mode==LINE_TEXTURE) {
	  texel = *(unsigned short *)(((char *)work.texture.base)+(pv>>16)*stride+2*(pu>>16));
	  if (texel)
	    *(unsigned short *)(((char *)work.colour.base)+oc) = texel;
	} else {
	  *(unsigned short *)(((char *)work.colour.base)+oc) = ScalarsToRGB15(pr,pg,pb);
	}
}
#endif

#if PARTS & PART_15
void BR_ASM_CALL PointRenderPI_RGB_555(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_555(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_15
void BR_ASM_CALL PointRenderPIT_RGB_555(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_555(LINE_TEXTURE,tvp);
}
#endif

#if PARTS & PART_16
void BR_ASM_CALL PointRenderPI_Generic_RGB_565(point_type mode,union brp_vertex *tvp)
{
	int oc;
	int x0,y0;
	br_fixed_ls width,height,stride,pi,pu,pv,pr,pg,pb;
	unsigned short texel;

	SETUP_POINT;
	
	if (mode==LINE_RGB) {
          pr = tvp->comp_x[C_R];
          pg = tvp->comp_x[C_G];
          pb = tvp->comp_x[C_B];
	}

	if (mode==LINE_TEXTURE) {
	  /* Textured */

	  SETUP_TEXTURE;
	  SETUP_UV;

	}

	oc = y0*work.colour.stride_b+2*x0;

	if (mode==LINE_TEXTURE) {
	  texel = *(unsigned short *)(((char *)work.texture.base)+(pv>>16)*stride+2*(pu>>16));
	  if (texel)
	    *(unsigned short *)(((char *)work.colour.base)+oc) = texel;
	} else {
	  *(unsigned short *)(((char *)work.colour.base)+oc) = ScalarsToRGB16(pr,pg,pb);
	}
}
#endif

#if PARTS & PART_16
void BR_ASM_CALL PointRenderPI_RGB_565(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_565(LINE_RGB,tvp);
}
#endif

#if PARTS & PART_16
void BR_ASM_CALL PointRenderPIT_RGB_565(struct brp_block *block, union brp_vertex *tvp) {
  PointRenderPI_Generic_RGB_565(LINE_TEXTURE,tvp);
}
#endif
