{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)LineRenderPIZ2T_RGB_888, NULL,
#endif

		"RGB 888, Z Buffered, Textured", NULL,
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
	BR_PMT_RGB_888,
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
	"softtz",(void *)"_LineRenderPIZ2T_RGB_888",
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
		(brp_render_fn *)LineRenderPIZ2I_RGB_888, NULL,
#endif

		"RGB 888, Z Buffered, Interpolated Colour", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(0)},
	{BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(0)},

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
	"softtz",(void *)"_LineRenderPIZ2I_RGB_888",
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
		(brp_render_fn *)LineRenderPIZ2I_RGB_888, NULL,
#endif

		"RGB 888, Z Buffered, Constant Colour", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_SX)|(1<<C_SY)|(1<<C_SZ),
		0,

		/* Constant slots
	 	 */
		(1<<C_R)|(1<<C_G)|(1<<C_B),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(0)},
	{BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(0)},

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
	"softtz",(void *)"_LineRenderPIZ2I_RGB_888",
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
		(brp_render_fn *)LineRenderPIT_RGB_888, NULL,
#endif

		"RGB 888, Textured", NULL,
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
	BR_PMT_RGB_888,
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
	"softt",(void *)"_LineRenderPIT_RGB_888",
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
		(brp_render_fn *)LineRenderPII_RGB_888, NULL,
#endif

		"RGB 888, Interpolated Colour", NULL,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_R|CM_G|CM_B,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(0)},
	{BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(0)},

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
	"softt",(void *)"_LineRenderPII_RGB_888",
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
		(brp_render_fn *)LineRenderPII_RGB_888, NULL,
#endif

		"RGB 888, Constant Colour", NULL,
		BRT_LINE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_SX)|(1<<C_SY),
		0,

		/* Constant slots
	 	 */
		(1<<C_R)|(1<<C_G)|(1<<C_B),
	},

	/* Offset and scale for R,G,B,A
	 */
	{BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(1),BR_SCALAR(0)},
	{BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(254),BR_SCALAR(0)},

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
	"softt",(void *)"_LineRenderPII_RGB_888",
#else
	NULL,NULL,
#endif
},
