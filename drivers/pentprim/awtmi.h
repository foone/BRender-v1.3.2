/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: awtmi.h 1.1 1997/12/10 16:46:36 jon Exp $
 *
 * Triangle renderer for arbitrary width linear texture mapping
 */

void BR_ASM_CALL
FNAME(struct brp_block *block, union brp_vertex *a,union brp_vertex *b,union brp_vertex *c)
{
    /* v[0] and v[1] contain the (fractional) screen coordinates */
    /* And comp_x[C_W] contain the 3-space z-value */

    /* (v[0]-width/2)*comp_x[C_W] gives the 3-space x-values */
    /* Similarly for y */

    int sxa,sxb,sxc,sya,syb,syc;

    int dv_int;
    int offset;
    int grad;

    br_fixed_ls g_divisor,dp1,dp2;

    int dv_int_nocarry;
    int dv_int_carry;
    int v_int_current;
    int direction;

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

    sxa = sar16(a->comp_x[C_SX]);
    sya = sar16(a->comp_x[C_SY]);
    sxb = sar16(b->comp_x[C_SX]);
    syb = sar16(b->comp_x[C_SY]);
    sxc = sar16(c->comp_x[C_SX]);
    syc = sar16(c->comp_x[C_SY]);

    work.top.x = sxb-sxa;
    work.top.y = syb-sya;
    work.bot.x = sxc-sxb;
    work.bot.y = syc-syb;
    work.main.x = sxc-sxa;
    work.main.y = syc-sya;

    work.main.grad = work.main.x*_reciprocal[work.main.y];
    work.main.d_f = work.main.grad << 16;
    grad = sar16(work.main.grad);
    work.main.d_i = grad+work.colour.stride_p;

    work.top.grad = work.top.y ? work.top.x*_reciprocal[work.top.y] : 0;
    work.top.d_f = work.top.grad << 16;
    work.top.d_i = sar16(work.top.grad)+work.colour.stride_p;

    work.main.f = work.top.f = 0x80000000;

    offset = high16(a->comp_x[C_SX])+high16(a->comp_x[C_SY])*((int)work.colour.stride_p);
    work.awsl.start = work.awsl.end = (char *)work.colour.base+offset*DBPP;
#if ZB
    work.awsl.zstart = (char *)work.depth.base +offset*2;
#endif
    work.top.count = syb-sya;

    work.bot.f = 0x80000000;

    work.bot.grad = work.bot.y ? work.bot.x*_reciprocal[work.bot.y] : 0;
    work.bot.d_f = work.bot.grad << 16;
    work.bot.d_i = sar16(work.bot.grad)+work.colour.stride_p;
    work.bot.count = syc-syb;

    g_divisor = work.top.x*work.main.y-work.main.x*work.top.y;
    if (!g_divisor) return;

    direction = work.top.count && work.top.grad>=work.main.grad ||
	work.bot.count && work.bot.grad<work.main.grad;

#define PARAM_SETUP(param,p) {\
    dp1 = b->p-a->p;\
    dp2 = c->p-a->p;\
    param.grad_x = SafeFixedMac2Div(dp1,work.main.y,-dp2,work.top.y,g_divisor);\
    param.grad_y = SafeFixedMac2Div(dp2,work.top.x,-dp1,work.main.x,g_divisor);\
    param.current = a->p+param.grad_x/2;\
    param.d_nocarry = BrFixedToInt(work.main.grad)*param.grad_x+param.grad_y;\
    param.d_carry = param.d_nocarry+param.grad_x;\
}

#define PARAM_SETUP_UNSIGNED(param,p) {\
    dp1 = b->p-a->p;\
    dp2 = c->p-a->p;\
    param.grad_x = SafeFixedMac2Div(dp1,work.main.y,-dp2,work.top.y,g_divisor);\
    param.grad_y = SafeFixedMac2Div(dp2,work.top.x,-dp1,work.main.x,g_divisor);\
    param.current = (a->p+param.grad_x/2) ^ 0x80000000;\
    param.d_nocarry = BrFixedToInt(work.main.grad)*param.grad_x+param.grad_y;\
    param.d_carry = param.d_nocarry+param.grad_x;\
}

#if LIGHT
    PARAM_SETUP(work.pi,comp_x[C_I]);
#endif
#if ZB
    PARAM_SETUP_UNSIGNED(work.pz,comp_x[C_SZ]);
#endif

    PARAM_SETUP(work.pu,comp_x[C_U]);
    PARAM_SETUP(work.pv,comp_x[C_V]);

    work.awsl.du_nocarry = work.pu.d_nocarry << 16;
    work.awsl.dv_nocarry = work.pv.d_nocarry << 16;
    work.awsl.du_carry = work.pu.d_carry << 16;
    work.awsl.dv_carry = work.pv.d_carry << 16;
    work.awsl.u_current = work.pu.current << 16;
    work.awsl.v_current = work.pv.current << 16;

	/*
	 * Per scan line increments
	 */
    if (powerof2(work.texture.width_p)) {
		work.awsl.du_int_nocarry = (sar16(work.pu.d_nocarry) & work.texture.width_p-1)
		    -work.texture.width_p;
		work.awsl.du_int_carry = (sar16(work.pu.d_carry) & work.texture.width_p-1)
		    -work.texture.width_p;
		work.awsl.u_int_current = sar16(work.pu.current) & work.texture.width_p-1;
    } else {
		work.awsl.du_int_nocarry = sar16(work.pu.d_nocarry) % (int)work.texture.width_p;
		work.awsl.du_int_carry = sar16(work.pu.d_carry) % (int)work.texture.width_p;
		work.awsl.u_int_current = sar16(work.pu.current) % (int)work.texture.width_p;
		if (work.awsl.du_int_nocarry>=0) work.awsl.du_int_nocarry -= work.texture.width_p;
		if (work.awsl.du_int_carry>=0) work.awsl.du_int_carry -= work.texture.width_p;
		if (work.awsl.u_int_current<0) work.awsl.u_int_current += work.texture.width_p;
    }

    if (powerof2(work.texture.height)) {
		dv_int_nocarry = (sar16(work.pv.d_nocarry) & work.texture.height-1)
		    -work.texture.height;
		dv_int_carry = (sar16(work.pv.d_carry) & work.texture.height-1)
		    -work.texture.height;
		v_int_current = sar16(work.pv.current) & work.texture.height-1;
    } else {
		dv_int_nocarry = sar16(work.pv.d_nocarry) % (int)work.texture.height;
		dv_int_carry = sar16(work.pv.d_carry) % (int)work.texture.height;
		v_int_current = sar16(work.pv.current) % (int)work.texture.height;
		if (dv_int_nocarry>=0) dv_int_nocarry -= work.texture.height;
		if (dv_int_carry>=0) dv_int_carry -= work.texture.height;
		if (v_int_current<0) v_int_current += work.texture.height;
    }

    work.awsl.dsource_nocarry = work.awsl.du_int_nocarry*SBPP+work.texture.stride_b*dv_int_nocarry;
    work.awsl.dsource_carry = work.awsl.du_int_carry*SBPP+work.texture.stride_b*dv_int_carry;
    work.awsl.source_current = (char *)work.texture.base + work.awsl.u_int_current*SBPP+work.texture.stride_b*v_int_current;
    work.awsl.u_int_current += 0x8000;

    if (direction) {
		work.awsl.du = work.pu.grad_x << 16;
		work.awsl.dv = work.pv.grad_x << 16;
		work.awsl.du_int = sar16(work.pu.grad_x) % (int)work.texture.width_p;
		if (powerof2(work.texture.height))
		    dv_int = (sar16(work.pv.grad_x) & work.texture.height-1)-work.texture.height;
		 else {
		    dv_int = sar16(work.pv.grad_x) % (int)work.texture.height;
	    	if (dv_int>=0) dv_int -= work.texture.height;
		 }
    } else {
		work.awsl.du = -work.pu.grad_x << 16;
		work.awsl.dv = -work.pv.grad_x << 16;
		work.awsl.du_int = sar16(-work.pu.grad_x) % (int)work.texture.width_p;
		if (powerof2(work.texture.height)) {
		    dv_int = (sar16(-work.pv.grad_x) & work.texture.height-1)-work.texture.height;
		 } else
		    dv_int = sar16(-work.pv.grad_x) % (int)work.texture.height;
	    if (dv_int>=0) dv_int -= work.texture.height;
    }

    if (work.awsl.du_int>=0) work.awsl.du_int -= work.texture.width_p;

    work.awsl.dsource = work.awsl.du_int*SBPP+work.texture.stride_b*dv_int;
    work.awsl.edge = &work.top;

    ASSERT(work.awsl.source_current>=work.texture.base);

    TNAME();

    work.awsl.end = (char *)work.colour.base + high16(b->comp_x[C_SX])*DBPP+high16(b->comp_x[C_SY])*work.colour.stride_b;
    work.awsl.edge = &work.bot;

    ASSERT(work.awsl.source_current>=work.texture.base);

    TNAME();
}
