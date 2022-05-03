/*
 * rastparm.h
 *
 * The layout of the workspace used for the new improved triangle scan converter
 *
 * The order of fields in these structures is important - the MMX code often relies
 * on pairs of fields being in the same quad word.
 */

/*
 * Standard header for all triangle blocks
 */
struct tsb_header {
	/*
	 * Code to rasterise from this parameter block
	 */
	void *function;

	/*
	 * Size of top and bottom trapezoids
	 */
	union {
	    br_uint_32 l;
		br_int_16 s[2];
	} counts;

	/*
	 * Starting value and gradient for major edge
	 */
	br_int_32 xm;
	br_int_32 d_xm;

	/*
	 * Starting value and gradient for first minor edge
	 */
	br_int_32 x1;
	br_int_32 d_x1;

	/*
	 * Starting value and gradient for second minor edge
	 */
	br_int_32 x2;
	br_int_32 d_x2;

	/*
	 * Pointers to start of screen and depth buffers
	 */
	void *screen_address;
	void *depth_address;

	/*
	 * Stride of buffers in bytes
	 */
	br_int_32 screen_stride;
	br_int_32 depth_stride;

	/*
	 * Starting scanline
	 */
	br_int_32 start_scanline;

	/*
	 * Useful 32 bit value - for things like constant colour
	 */
    unsigned int c;
};


struct texture_info {
	void *texture_address;	void *map_address;

	char width_s;
	char height_s;
	char tile_s;

	char _pad0[5];

};


/*
 * Group of parameter blocks 1
 *
 * Z RGB UV A
 */
struct param_z {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;
};

struct param_zrgb {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;

	unsigned int s_r;		unsigned int d_r_y1;
	unsigned int d_r_x;		unsigned int d_r_y0;

	unsigned int s_g;		unsigned int d_g_y1;
	unsigned int d_g_x;		unsigned int d_g_y0;

	unsigned int s_b;		unsigned int d_b_y1;
	unsigned int d_b_x;		unsigned int d_b_y0;
};

struct param_zrgbuv {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;

	unsigned int s_r;		unsigned int d_r_y1;
	unsigned int d_r_x;		unsigned int d_r_y0;

	unsigned int s_g;		unsigned int d_g_y1;
	unsigned int d_g_x;		unsigned int d_g_y0;

	unsigned int s_b;		unsigned int d_b_y1;
	unsigned int d_b_x;		unsigned int d_b_y0;

	struct texture_info tinfo;

	unsigned int s_u;		unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int s_v;		unsigned int d_v_y1;
	unsigned int d_v_x;		unsigned int d_v_y0;

};

struct param_zrgbuva {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;

	unsigned int s_r;		unsigned int d_r_y1;
	unsigned int d_r_x;		unsigned int d_r_y0;

	unsigned int s_g;		unsigned int d_g_y1;
	unsigned int d_g_x;		unsigned int d_g_y0;

	unsigned int s_b;		unsigned int d_b_y1;
	unsigned int d_b_x;		unsigned int d_b_y0;

	struct texture_info tinfo;

	unsigned int s_u;		unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int s_v;		unsigned int d_v_y1;
	unsigned int d_v_x;		unsigned int d_v_y0;

	unsigned int s_a;		unsigned int d_a_y1;
	unsigned int d_a_x;		unsigned int d_a_y0;
};

/*
 * Group of parameter blocks 2
 *
 * Z UV A
 */
struct param_zuv {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;

	struct texture_info tinfo;

	unsigned int s_u;		unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int s_v;		unsigned int d_v_y1;
	unsigned int d_v_x;		unsigned int d_v_y0;
};

struct param_zuva {
	struct tsb_header		h;

	unsigned int s_z;		unsigned int d_z_y1;
	unsigned int d_z_x;		unsigned int d_z_y0;

	struct texture_info tinfo;

	unsigned int s_u;		unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int s_v;		unsigned int d_v_y1;
	unsigned int d_v_x;		unsigned int d_v_y0;

	unsigned int s_a;		unsigned int d_a_y1;
	unsigned int d_a_x;		unsigned int d_a_y0;
};


/*
 * Work structures for MMX rasterisers
 */
struct work_mmx_zrgb {
	unsigned int alpha01;   unsigned int alpha23;

	unsigned int d_z_x;		unsigned int h_d_z_x;

	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	unsigned int d_r_y1;	unsigned int h_d_r_y1;
	unsigned int d_r_y0;	unsigned int h_d_r_y0;

	unsigned int d_g_y1;	unsigned int h_d_g_y1;
	unsigned int d_g_y0;	unsigned int h_d_g_y0;

	unsigned int d_b_y1;	unsigned int h_d_b_y1;
	unsigned int d_b_y0;	unsigned int h_d_b_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	unsigned int r01;		unsigned int r23;
	unsigned int d_r_x;	    unsigned int h_d_r_x;

	unsigned int g01;		unsigned int g23;
	unsigned int d_g_x;	    unsigned int h_d_g_x;

	unsigned int b01;		unsigned int b23;
	unsigned int d_b_x;	    unsigned int h_d_b_x;
};

struct work_mmx_zrgbuv {
	unsigned int alpha01;   unsigned int alpha23;

	void *screen_end;		void *mask_end;
	unsigned int d_z_x;		unsigned int h_d_z_x;

	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	unsigned int d_r_y1;	unsigned int h_d_r_y1;
	unsigned int d_r_y0;	unsigned int h_d_r_y0;

	unsigned int d_g_y1;	unsigned int h_d_g_y1;
	unsigned int d_g_y0;	unsigned int h_d_g_y0;

	unsigned int d_b_y1;	unsigned int h_d_b_y1;
	unsigned int d_b_y0;	unsigned int h_d_b_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	unsigned int r01;		unsigned int r23;
	unsigned int d_r_x;	    unsigned int h_d_r_x;

	unsigned int g01;		unsigned int g23;
	unsigned int d_g_x;	    unsigned int h_d_g_x;

	unsigned int b01;		unsigned int b23;
	unsigned int d_b_x;	    unsigned int h_d_b_x;

	void *texture_address;	void *map_address;
	unsigned int u_mask;	unsigned int v_mask;

    unsigned int u;			unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int v;			unsigned int d_v_y1;
   	unsigned int d_v_x;		unsigned int d_v_y0;
};

struct work_mmx_zuv {
	unsigned int alpha01;   unsigned int alpha23;

	void *mask_end;			void *screen_end;

	unsigned int d_z_x;		unsigned int h_d_z_x;
	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	void *texture_address;	void *map_address;
	unsigned int u_mask;	unsigned int v_mask;

    unsigned int u;			unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int v;			unsigned int d_v_y1;
   	unsigned int d_v_x;		unsigned int d_v_y0;
};

/*
 * Worspace when IA registers are used for U & V
 */
struct work_mmx_zuvc {
	unsigned int alpha01;   unsigned int alpha23;

	unsigned int cr01;      unsigned int cr23;
	unsigned int cg01;      unsigned int cg23;
	unsigned int cb01;      unsigned int cb23;

	void *mask_end;			void *screen_end;

	unsigned int d_z_x;		unsigned int h_d_z_x;
	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	void *texture_address;	void *map_address;
	unsigned int u_mask;	unsigned int v_mask;

    unsigned int u;			unsigned int d_u_y1;
	unsigned int d_u_x;		unsigned int d_u_y0;

	unsigned int v;			unsigned int d_v_y1;
   	unsigned int d_v_x;		unsigned int d_v_y0;
};

/*
 * Worspace when MMX registers are used for U & V
 */
struct work_mmx_zuv2c {
	unsigned int alpha01;   unsigned int alpha23;

	unsigned int cr01;      unsigned int cr23;
	unsigned int cg01;      unsigned int cg23;
	unsigned int cb01;      unsigned int cb23;

	void *mask_end;			void *screen_end;

	unsigned int v_mask;	unsigned int h_v_mask;

	unsigned int d_u_y1;	unsigned int h_d_u_y1;
	unsigned int d_u_y0;	unsigned int h_d_u_y0;

	unsigned int d_v_y1;	unsigned int h_d_v_y1;
	unsigned int d_v_y0;	unsigned int h_d_v_y0;

	unsigned int d_z_x;		unsigned int h_d_z_x;
	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	void *texture_address;	void *map_address;
	unsigned int u_mask;	unsigned int h_u_mask;

    unsigned int u0;		unsigned int u1;
	unsigned int d_u_x;		unsigned int h_d_u_x;

    unsigned int v0;		unsigned int v1;
	unsigned int d_v_x;		unsigned int h_d_v_x;
};

/*
 * Worspace when MMX registers are used for U & V
 */
struct work_mmx_zuv4c {
	unsigned int alpha01;   unsigned int alpha23;

	unsigned int cr01;      unsigned int cr23;
	unsigned int cg01;      unsigned int cg23;
	unsigned int cb01;      unsigned int cb23;

	unsigned int u_bridge_bits;   unsigned int v_bridge_bits;

	void *mask_end;			void *screen_end;

	unsigned int d_u_x;		unsigned int d_v_x;

	unsigned int d_u_y1;	unsigned int h_d_u_y1;
	unsigned int d_u_y0;	unsigned int h_d_u_y0;

	unsigned int d_v_y1;	unsigned int h_d_v_y1;
	unsigned int d_v_y0;	unsigned int h_d_v_y0;

	unsigned int d_z_x;		unsigned int h_d_z_x;
	unsigned int d_z_y1;	unsigned int h_d_z_y1;
	unsigned int d_z_y0;	unsigned int h_d_z_y0;

	struct tsb_header		h;

	unsigned int z0;		unsigned int z1;
	unsigned int z2;		unsigned int z3;

	void *texture_address;	void *map_address;
	unsigned int u_mask;	unsigned int v_mask;

    unsigned int u0;		unsigned int u2;
	unsigned int u1;		unsigned int u3;

    unsigned int v0;		unsigned int v2;
	unsigned int v1;		unsigned int v3;
};

/*
 * This is the largest amount of scratch space (difference between parm_ and work_) that is used (in bytes)
 */

#define MAX_SCRATCH_SIZE 40

