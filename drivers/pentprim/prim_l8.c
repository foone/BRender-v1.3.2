{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2TI, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_I|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_I)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE,
	PRIMF_SMOOTH|PRIMF_MODULATE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softiz",(void *)"_LineRenderPIZ2TI",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2TI, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		(1<<C_I),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_SMOOTH,
	PRIMF_MODULATE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softiz",(void *)"_LineRenderPIZ2TI",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2T, NULL,
#endif

		"Indexed, Z Buffered, Textured", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	0,
	0,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softiz",(void *)"_LineRenderPIZ2T",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2I, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_I,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_I),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softiz",(void *)"_LineRenderPIZ2I",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2I, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_SZ),
		0,

		/* Constant slots
	 	 */
		(1<<C_I),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH,
	0,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softiz",(void *)"_LineRenderPIZ2I",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPITI, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_I|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_I)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE,
	PRIMF_SMOOTH|PRIMF_MODULATE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softi",(void *)"_LineRenderPITI",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPITI, NULL,
#endif

		"Indexed, Constant Intensity, Textured", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		(1<<C_I),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_SMOOTH,
	PRIMF_MODULATE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softi",(void *)"_LineRenderPITI",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIT, NULL,
#endif

		"Indexed, Textured", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	0,
	0,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softi",(void *)"_LineRenderPIT",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPII, NULL,
#endif

		"Indexed, Interpolated Intensity", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_I,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_I),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softi",(void *)"_LineRenderPII",
#else
	NULL,NULL,
#endif
},
{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPII, NULL,
#endif

		"Indexed, Constant Intensity", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY),
		0,

		/* Constant slots
	 	 */
		(1<<C_I),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},
	{BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0),BR_SCALAR(0)},

	/* range flags
	 */
	0,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH,
	0,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	0,0,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softi",(void *)"_LineRenderPII",
#else
	NULL,NULL,
#endif
},
