{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog_BlendSmooth, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog_Blend, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_BlendSmooth, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Blend, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth, NULL,

		"Line, Textured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog_BlendSmooth, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog_Blend, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Fog, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_BlendSmooth, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth_Blend, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_SMOOTH|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Smooth, NULL,

		"Line, Untextured, Smooth", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

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
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_SMOOTH,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog_BlendSmooth, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog_Blend, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER|PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_BlendSmooth, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Blend, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_TEXTURE_BUFFER|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat, NULL,

		"Line, Textured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_TEXTURE_BUFFER,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog_BlendSmooth, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog_Blend, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	PRIMF_FOG|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Fog, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	PRIMF_FOG,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_BlendSmooth, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q|CM_A,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	0|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat_Blend, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B|CM_A,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q)|(1<<C_A),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND|PRIMF_SMOOTH_ALPHA,
	0|PRIMF_BLEND,

	/* Block specific begin/end */
	NULL,
	NULL,
},
{
	{
		/* Render function */
        (brp_render_fn *) LineRender_Flat, NULL,

		"Line, Untextured, Flat", &PrimitiveLibrary3Dfx,
		BRT_LINE, 0,

		/* components - constant and per vertex */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_Q,

		/* Component slots as - float, fixed or integer */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_Q),
		0,
		0,

		/* Constant slots */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks */
	PRIMF_TEXTURE_BUFFER|PRIMF_SMOOTH|PRIMF_FOG|PRIMF_BLEND,
	0,

	/* Block specific begin/end */
	NULL,
	NULL,
}
