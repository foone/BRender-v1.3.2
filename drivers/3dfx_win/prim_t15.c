{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured_Fog_BlendSmooth, NULL,

		"Triangle, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured_Fog_Blend, NULL,

		"Triangle, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured_Fog, NULL,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured_BlendSmooth, NULL,

		"Triangle, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_SmoothTextured_Blend, NULL,

		"Triangle, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured_Fog_BlendSmooth, NULL,

		"Triangle, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured_Fog_Blend, NULL,

		"Triangle, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured_Fog, NULL,

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
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured_BlendSmooth, NULL,

		"Triangle, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_FlatTextured_Blend, NULL,

		"Triangle, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_BLEND,

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
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_MODULATE|PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured_Fog_BlendSmooth, NULL,

		"Triangle, Textured, Unlit", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured_Fog_Blend, NULL,

		"Triangle, Textured, Unlit", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured_Fog, NULL,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured_BlendSmooth, NULL,

		"Triangle, Textured, Unlit", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_UnlitTextured_Blend, NULL,

		"Triangle, Textured, Unlit", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_BLEND,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth_Fog_BlendSmooth, NULL,

		"Triangle, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth_Fog_Blend, NULL,

		"Triangle, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth_Fog, NULL,

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
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth_BlendSmooth, NULL,

		"Triangle, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Smooth_Blend, NULL,

		"Triangle, Smooth", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_A,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_BLEND,

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
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat_Fog_BlendSmooth, NULL,

		"Triangle, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat_Fog_Blend, NULL,

		"Triangle, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat_Fog, NULL,

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
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat_BlendSmooth, NULL,

		"Triangle, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	0|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) TriangleRender_Flat_Blend, NULL,

		"Triangle, Flat", &PrimitiveLibrary3Dfx,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	0|PRIMF_BLEND,

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
		0,//		(1<<C_R)|(1<<C_G)|(1<<C_B),	// This was buggy (and v. slow) last time we checked

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	0,

	/* Block specific begin/end */
	NULL,
	NULL,
}
