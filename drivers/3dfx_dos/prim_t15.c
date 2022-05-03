{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured, NULL,

		"Triangle, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured, NULL,

		"Triangle, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		0,
//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured, NULL,

		"Triangle, Textured, Unlit", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER,
	PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth, NULL,

		"Triangle, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat, NULL,

		"Triangle, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		0,
//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH,
	0,

	/* Block specific begin/end */
	NULL,
	NULL,
}
