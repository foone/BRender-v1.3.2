/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: perspzi.h 1.1 1997/12/10 16:47:34 jon Exp $
 *
 * Perspective texture mapped triangle renderer
 */

#define SHIFT 1

/*
 * Macros for moving left, right, up and down in texture whose size is
 * SIZE*SIZE.
 */

#if SIZE==256 && defined __WATCOMC__

#define incv(a) inch(a)
#define incu(a) incl(a)
#define decv(a) dech(a)
#define decu(a) decl(a)

#else

#define incv(a) (a+SIZE & SIZE*SIZE-1);
#define incu(a) ((a+1 & SIZE-1) | (a & ~(SIZE-1)));
#define decv(a) (a-SIZE & SIZE*SIZE-1);
#define decu(a) ((a-1 & SIZE-1) | (a & ~(SIZE-1)));

#endif

/*
 * Setup and outer loops for perspective texture mappers
 */

void BR_ASM_CALL
FNAME(struct brp_block *block, union brp_vertex *a,union brp_vertex *b,union brp_vertex *c)
{
    /*
     * In linear texture mapping we interpolate u and v which we then use
     * as the texture coordinates.
     *
     * In perspective texture mapping we linearly interpolate uq, wq and q
     * and use uq/q and wq/q as texture coordinates.
     *
     * q is simply a constant times comp_x[C_W] chose to scale things for
     * maximal accuracy
     *
     * comp_x[C_SX] and comp_x[C_SY] contain the (fractional) screen coordinates
     */

    /*
     * Screen coordinates
     */
    int sxa,sxb,sxc,sya,syb,syc;

    int direction;

    /*
     * Values of uq,vq and q at vertices
     */
    float au,av,bu,bv,cu,cv;

    br_fixed_ls dp1,dp2,g_divisor;

    int du_numerator_nocarry;
    int dv_numerator_nocarry;
    int u,v;
    int offset,zoffset;
    int u_base,v_base;

    int carry,grad;

    float aw,bw,cw,maxuv,d1,d2,g_inverse,zmx,zmy,ztx,zty;

    /* Optimal sort of 3 elements? */

    if (a->comp_x[C_SY]>b->comp_x[C_SY]) {
        if (b->comp_x[C_SY]>c->comp_x[C_SY]) {
	    swap(union brp_vertex *,a,c);
	} else {
            if (a->comp_x[C_SY]>c->comp_x[C_SY]) {
		swap(union brp_vertex *,a,b);
		swap(union brp_vertex *,b,c);
	    } else {
		swap(union brp_vertex *,a,b);
	    }
	}
    } else {
        if (b->comp_x[C_SY]>c->comp_x[C_SY]) {
            if (a->comp_x[C_SY]>c->comp_x[C_SY]) {
		swap(union brp_vertex *,b,c);
		swap(union brp_vertex *,a,b);
	    } else {
		swap(union brp_vertex *,b,c);
	    }
	}
    }

#define RANGE(min,max,a,b,c) { \
  int middle; \
  min = a; \
  middle = b; \
  max = c; \
  if (min>middle) swap(int,min,middle); \
  if (middle>max) swap(int,middle,max); \
  if (min>middle) min = middle; \
}

#if CHEAT
    {
        br_fixed_ls min,max,range;

        RANGE(min,max,a->comp_x[C_SX],b->comp_x[C_SX],c->comp_x[C_SX]);

	range = max-min;

        if (c->comp_x[C_SY]-a->comp_x[C_SY]>range)
          range = c->comp_x[C_SY]-a->comp_x[C_SY];

	range /= 0x10000;

        RANGE(min,max,a->comp_x[C_W],b->comp_x[C_W],c->comp_x[C_W]);

	while ((min | max) & 0xffff0000) {
	    min /= 4;
	    max /= 4;
	}

	if ((max-min)*range<cutoff*min) {
	  LINEAR(block,a,b,c);
	  return;
	}
    }
#endif

    /*
     * Set up general triangle parameters
     */
    sxa = sar16(a->comp_x[C_SX]);
    sxb = sar16(b->comp_x[C_SX]);
    sxc = sar16(c->comp_x[C_SX]);
    sya = sar16(a->comp_x[C_SY]);
    syb = sar16(b->comp_x[C_SY]);
    syc = sar16(c->comp_x[C_SY]);

    if (sya==syc) return;

    work.top.x = sxb-sxa;
    work.top.y = syb-sya;
    work.bot.x = sxc-sxb;
    work.bot.y = syc-syb;
    work.main.x = sxc-sxa;
    work.main.y = syc-sya;

    g_divisor = work.top.x*work.main.y-work.main.x*work.top.y;
    if (!g_divisor) return;

    direction = g_divisor>=0;

    if (direction && g_divisor<100)
      g_inverse = frcp[g_divisor];
    else if (!g_divisor && g_divisor>-100)
      g_inverse = -frcp[-g_divisor];
    else
      g_inverse = 1.0f/g_divisor;

    /*
     * Set up texture specific parameters
     */

    /*
     * Eliminate rounding errors by shifting to new base point
     */
    u_base = a->comp_x[C_U] & 0xffff0000;
    v_base = a->comp_x[C_V] & 0xffff0000;

    u = (a->comp_x[C_U]-u_base)/0x10000;
    v = (a->comp_x[C_V]-v_base)/0x10000;

    au = (float)(a->comp_x[C_U]-u_base)/0x10000;
    bu = (float)(b->comp_x[C_U]-u_base)/0x10000;
    cu = (float)(c->comp_x[C_U]-u_base)/0x10000;
    av = (float)(a->comp_x[C_V]-v_base)/0x10000;
    bv = (float)(b->comp_x[C_V]-v_base)/0x10000;
    cv = (float)(c->comp_x[C_V]-v_base)/0x10000;

    u_base /= 0x10000;
    v_base /= 0x10000;

    /*
     * Convert texture coordinates u,v to projective coordinates u,v,1/w
     */
    aw = (float)b->comp_x[C_W]*c->comp_x[C_W];
    bw = (float)c->comp_x[C_W]*a->comp_x[C_W];
    cw = (float)b->comp_x[C_W]*a->comp_x[C_W];

    /*
     * Normalise u,v,q to maximise accuracy
     */

    maxuv = aw>0 ? aw : -aw;
    maxuv += au>0 ? au : -au;
    maxuv += av>0 ? av : -av;

    au *= aw;
    maxuv += au>0 ? au : -au;
    av *= aw;
    maxuv += av>0 ? av : -av;
    bu *= bw;
    maxuv += bu>0 ? bu : -bu;
    bv *= bw;
    maxuv += bv>0 ? bv : -bv;
    cu *= cw;
    maxuv += cu>0 ? cu : -cu;
    cv *= cw;
    maxuv += cv>0 ? cv : -cv;

    maxuv = (1<<28)/maxuv;

    au *= maxuv;
    av *= maxuv;
    aw *= maxuv;
    bu *= maxuv;
    bv *= maxuv;
    bw *= maxuv;
    cu *= maxuv;
    cv *= maxuv;
    cw *= maxuv;

    d1 = bu-au;
    d2 = cu-au;

    /*
     * Convert deltas along edges to deltas along x and y
     */
    zmx = work.main.x*g_inverse;
    zmy = work.main.y*g_inverse;
    ztx = work.top.x*g_inverse;
    zty = work.top.y*g_inverse;

    work.pu.grad_x = (br_int_32)(d1*zmy-d2*zty);
    work.pu.grad_y = (br_int_32)(d2*ztx-d1*zmx);

    d1 = bv-av;
    d2 = cv-av;

    work.pv.grad_x = (br_int_32)(d1*zmy-d2*zty);
    work.pv.grad_y = (br_int_32)(d2*ztx-d1*zmx);

    d1 = bw-aw;
    d2 = cw-aw;

    work.pq.grad_x = (br_int_32)(d1*zmy-d2*zty);
    work.pq.grad_y = (br_int_32)(d2*ztx-d1*zmx);

    work.pu.current = (br_int_32)au;
    work.pv.current = (br_int_32)av;
    work.pq.current = (br_int_32)aw;

    if (!work.pq.current)
      return;

    /*
     * Calculate deltas along triangle edges
     */

    work.main.grad = work.main.x*_reciprocal[work.main.y];
    work.main.d_f = work.main.grad << 16;
    grad = sar16(work.main.grad);
    work.main.d_i = grad;

    work.pu.d_nocarry = grad*work.pu.grad_x+work.pu.grad_y;
    work.pv.d_nocarry = grad*work.pv.grad_x+work.pv.grad_y;
    work.pq.d_nocarry = grad*work.pq.grad_x+work.pq.grad_y;

    work.top.grad = work.top.y ? work.top.x*_reciprocal[work.top.y] : 0;
    work.top.d_f = work.top.grad << 16;
    work.top.d_i = sar16(work.top.grad);
    work.top.count = syb-sya;

    work.bot.grad = work.bot.y ? work.bot.x*_reciprocal[work.bot.y] : 0;
    work.bot.d_f = work.bot.grad << 16;
    work.bot.d_i = sar16(work.bot.grad);
    work.bot.count = syc-syb;

#define PARAM_SETUP(param,p) {\
    dp1 = b->p-a->p;\
    dp2 = c->p-a->p;\
    param.grad_x = SafeFixedMac2Div(dp1,work.main.y,-dp2,work.top.y,g_divisor);\
    param.grad_y = SafeFixedMac2Div(dp2,work.top.x,-dp1,work.main.x,g_divisor);\
    param.current = a->p+(g_divisor >=0 ? param.grad_x/2 : -param.grad_x/2);\
    param.d_nocarry = BrFixedToInt(work.main.grad)*param.grad_x+param.grad_y;\
    param.d_carry = param.d_nocarry+param.grad_x;\
}

#define PARAM_SETUP_UNSIGNED(param,p) {\
    dp1 = b->p-a->p;\
    dp2 = c->p-a->p;\
    param.grad_x = SafeFixedMac2Div(dp1,work.main.y,-dp2,work.top.y,g_divisor);\
    param.grad_y = SafeFixedMac2Div(dp2,work.top.x,-dp1,work.main.x,g_divisor);\
    param.current = (a->p+(g_divisor >=0 ? param.grad_x/2 : -param.grad_x/2)) ^ 0x80000000;\
    param.d_nocarry = BrFixedToInt(work.main.grad)*param.grad_x+param.grad_y;\
    param.d_carry = param.d_nocarry+param.grad_x;\
}

#if LIGHT
    PARAM_SETUP(work.pi,comp_x[C_I]);
#endif

    /* Shift right by half a pixel */

    work.main.f = work.top.f = work.bot.f = 0x80000000;

    du_numerator_nocarry = u*work.pq.d_nocarry-work.pu.d_nocarry;
    work.pu.grad_x = u*work.pq.grad_x-work.pu.grad_x;

    dv_numerator_nocarry = v*work.pq.d_nocarry-work.pv.d_nocarry;
    work.pv.grad_x = v*work.pq.grad_x-work.pv.grad_x;

    /* u_numerator and v_numerator are the 'Bresenham style' error terms */

    work.pu.current = work.pu.current-u*work.pq.current;
    work.pv.current = work.pv.current-v*work.pq.current;

    /*
     * Set up pointer into texture taking into account the fact that
     * its size may be an arbitrary power of two
     */

    work.tsl.source = (v+v_base & SIZE-1) * SIZE | u+u_base & SIZE-1;

    offset = sar16(a->comp_x[C_SX])*BPP+sar16(a->comp_x[C_SY])*work.colour.stride_b-BPP*(g_divisor<0);
    zoffset = sar16(a->comp_x[C_SX])*2+sar16(a->comp_x[C_SY])*work.depth.stride_b-2*(g_divisor<0);
    work.tsl.start = work.tsl.end = ((char *)work.colour.base)+offset;
    work.tsl.zstart = (char *)work.depth.base+zoffset;

    PARAM_SETUP_UNSIGNED(work.pz,comp_x[C_SZ]);

#if SHIFT

    /* shift by 1/2 pixel */

    if (g_divisor>=0) {
	work.pq.current += work.pq.grad_x/2;
	work.pu.current += -work.pu.grad_x/2;
	work.pv.current += -work.pv.grad_x/2;
    } else {
	work.pq.current -= work.pq.grad_x/2;
	work.pu.current -= -work.pu.grad_x/2;
	work.pv.current -= -work.pv.grad_x/2;
    }

#define PDIVIDE \
    while (work.pu.current>=(signed)work.pq.current) { \
	work.pu.current -= work.pq.current; \
	work.tsl.source = incu(work.tsl.source); \
	work.pu.grad_x += work.pq.grad_x; \
	du_numerator_nocarry += work.pq.d_nocarry; \
    } \
    while (work.pu.current<0) { \
	work.pu.current += work.pq.current; \
	work.tsl.source = decu(work.tsl.source); \
	work.pu.grad_x -= work.pq.grad_x; \
	du_numerator_nocarry -= work.pq.d_nocarry; \
    } \
    while (work.pv.current>=(signed)work.pq.current) { \
	work.pv.current -= work.pq.current; \
	work.tsl.source = incv(work.tsl.source); \
	work.pv.grad_x += work.pq.grad_x; \
	dv_numerator_nocarry += work.pq.d_nocarry; \
    } \
    while (work.pv.current<0) { \
	work.pv.current += work.pq.current; \
	work.tsl.source = decv(work.tsl.source); \
	work.pv.grad_x -= work.pq.grad_x; \
	dv_numerator_nocarry -= work.pq.d_nocarry; \
    } 
#endif

    work.tsl.y = sya;

    while (work.top.count--) {
	/* Start of second innermost loop */

	/*
	 * Call scan line renderer
	 */
	if (direction && work.tsl.start<work.tsl.end || !direction && work.tsl.start>work.tsl.end) {
	  PDIVIDE;
	  SNAME();
	}

	work.tsl.y++;
	
	work.top.f += work.top.d_f;
	carry = (unsigned)work.top.f < (unsigned)work.top.d_f;
	work.tsl.end += work.top.d_i*BPP+work.colour.stride_b+carry*BPP;
	work.main.f += work.main.d_f;
	carry = (unsigned)work.main.f < (unsigned)work.main.d_f;
	work.pq.current += work.pq.d_nocarry;

	work.pu.current += -du_numerator_nocarry;
	work.pv.current += -dv_numerator_nocarry;
	work.tsl.start += work.main.d_i*BPP+work.colour.stride_b+carry*BPP;
	work.tsl.zstart += work.main.d_i*2+work.depth.stride_b+carry*2;
#if LIGHT
	work.pi.current += work.pi.d_nocarry;
#endif
	work.pz.current += work.pz.d_nocarry;
	if (carry) {
	    work.pq.current +=work.pq.grad_x;

	    work.pu.current += -work.pu.grad_x;
	    work.pv.current += -work.pv.grad_x;
#if LIGHT
	    work.pi.current += work.pi.grad_x;
#endif
#if ZB
	    work.pz.current += work.pz.grad_x;
#endif
	}

	/*
	 * The core perspective calculations along the main edge 
	 */

	/* End of second innermost loop */
    }

    work.tsl.end = ((char *)work.colour.base)+BPP*(sar16(b->comp_x[C_SX]))+sar16(b->comp_x[C_SY])*work.colour.stride_b
        -(!direction)*BPP;

while (work.bot.count--) {
	/* Start of second innermost loop */

	/*
	 * Call scan line renderer
	 */
	if (direction && work.tsl.start<work.tsl.end || !direction && work.tsl.start>work.tsl.end) {
	  PDIVIDE;
	  SNAME();
	}

	work.tsl.y++;

	work.bot.f += work.bot.d_f;
	carry = (unsigned)work.bot.f < (unsigned)work.bot.d_f;
	work.tsl.end += work.bot.d_i*BPP+work.colour.stride_b+carry*BPP;
	work.main.f += work.main.d_f;
	carry = (unsigned)work.main.f < (unsigned)work.main.d_f;
	work.pq.current += work.pq.d_nocarry;

	work.pu.current += -du_numerator_nocarry;
	work.pv.current += -dv_numerator_nocarry;
	work.tsl.start += work.main.d_i*BPP+work.colour.stride_b+carry*BPP;
	work.tsl.zstart += work.main.d_i*2+work.depth.stride_b+carry*2;
#if LIGHT
	work.pi.current += work.pi.d_nocarry;
#endif
#if ZB
	work.pz.current += work.pz.d_nocarry;
#endif
	if (carry) {
	    work.pq.current +=work.pq.grad_x;

	    work.pu.current += -work.pu.grad_x;
	    work.pv.current += -work.pv.grad_x;
#if LIGHT
	    work.pi.current += work.pi.grad_x;
#endif
#if ZB
	    work.pz.current += work.pz.grad_x;
#endif
	}
	/* End of second innermost loop */
    }
}

#undef decu
#undef decv
#undef incu
#undef incv
