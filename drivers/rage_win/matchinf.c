{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LSPT_TriRender,NULL,

		"Lit, Smooth, Perspective Textured triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_R|CM_G|CM_B,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_PERSPECTIVE,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_PERSPECTIVE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LST_TriRender,NULL,

		"Lit, Smooth, Linear Textured triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_R|CM_G|CM_B,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_MODULATE,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_MODULATE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LFPT_TriRender,NULL,

		"Lit, Flat, Perspective Textured Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_MODULATE|PRIMF_PERSPECTIVE|PRIMF_SMOOTH,
	PRIMF_TEXTURE_BUFFER|PRIMF_MODULATE|PRIMF_PERSPECTIVE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LFT_TriRender,NULL,

		"Lit, Flat, Linear textured Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_MODULATE|PRIMF_SMOOTH,
	PRIMF_TEXTURE_BUFFER|PRIMF_MODULATE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_ULPT_TriRender,NULL,

		"Unlit,Perspective textured Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_W)|(1<<C_U)|(1<<C_V),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_PERSPECTIVE,
	PRIMF_TEXTURE_BUFFER|PRIMF_PERSPECTIVE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_ULT_TriRender,NULL,

		"Unlit, Linear textured Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_W)|(1<<C_U)|(1<<C_V),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER,
	PRIMF_TEXTURE_BUFFER,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LS_TriRender,NULL,

		"Lit, Smooth Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_W|CM_R|CM_G|CM_B,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_LF_TriRender,NULL,

		"Lit, Flat Triangle renderer",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
    PRIMF_MODULATE,
    PRIMF_MODULATE,

	/* Work buffer */
    0
},

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATI_UL_TriRender,NULL,

		"Unlit Triangle renderer (untextured special case)",
		&PrimitiveLibraryHardATI,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_UR|CM_UG|CM_UB,
		CM_SX|CM_SY|CM_SZ|CM_W,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_UR)|(1<<C_UG)|(1<<C_UB)|(1<<C_W),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_MODULATE,
	0,

	/* Work buffer */
    0
},





/* LINE RENDER STYLES */

{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATIEdgeRender,NULL,

		"Generic Edge renderer", &PrimitiveLibraryHardATI,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_R|CM_G|CM_B,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	0,
	0,

	/* Work buffer */
    0
},

/* Lit Points */
{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATIPointRender,NULL,

		"Generic Point renderer", &PrimitiveLibraryHardATI,
		BRT_POINT, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_R|CM_G|CM_B,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_MODULATE,
	PRIMF_MODULATE,

	/* Work buffer */
    0
},

/* Unlit Points */
{
	{
		/* Setup and render functions */
		(brp_render_fn*)ATIPointRender,NULL,

		"Generic Point renderer", &PrimitiveLibraryHardATI,
		BRT_POINT, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_UR|CM_UG|CM_UB,

		/* Components as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_UR)|(1<<C_UG)|(1<<C_UB),
		0,
		0,

		/* Constant slots */
		0,

		// Reserved stuff
		0,0,0,0,
	},

	/* Masks */
	PRIMF_MODULATE,
	0,

	/* Work buffer */
    0
},
