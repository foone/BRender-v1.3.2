{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)TriangleRenderPIZ2TIPB256_RGB_565, NULL,
#endif

		"RGB 565, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct, RGB blended", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_I|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_I)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRenderPIZ2TIPB256_RGB_565",
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
		(brp_render_fn *)TriangleRenderPIZ2TIPB256_RGB_565, NULL,
#endif

		"RGB 565, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct, RGB blended", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE|BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRenderPIZ2TIPB256_RGB_565",
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
		(brp_render_fn *)TriangleRenderPIZ2TIP256_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_I|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_I)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRenderPIZ2TIP256_RGB_555",
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
		(brp_render_fn *)TriangleRenderPIZ2TIP256_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRenderPIZ2TIP256_RGB_555",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_1024, NULL,
#endif

		"RGB 565, Z Buffered, Textured 1024x1024, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_256, NULL,
#endif

		"RGB 565, Z Buffered, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_256",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_128, NULL,
#endif

		"RGB 565, Z Buffered, Textured 128x128, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_128",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_64, NULL,
#endif

		"RGB 565, Z Buffered, Textured 64x64, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_64",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_32, NULL,
#endif

		"RGB 565, Z Buffered, Textured 32x32, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_32",
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
		(brp_render_fn *)TriangleRenderPIZ2TA15, NULL,
#endif

		"RGB 565, Z Buffered, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TA15",
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
		(brp_render_fn *)TriangleRenderPIZ2TIA_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Constant Intensity, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE|BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPIZ2TIA_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Interpolated Intensity, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TIA_RGB_555",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_1024, NULL,
#endif

		"RGB 565, Z Buffered, Textured 1024x1024", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_256, NULL,
#endif

		"RGB 565, Z Buffered, Textured 256x256", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_256",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_128, NULL,
#endif

		"RGB 565, Z Buffered, Textured 128x128", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_128",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_64, NULL,
#endif

		"RGB 565, Z Buffered, Textured 64x64", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_64",
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
		(brp_render_fn *)TriangleRender_ZT_RGB565_D16_32, NULL,
#endif

		"RGB 565, Z Buffered, Textured 32x32", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V),
		0,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhzf",(void *)"_TriangleRender_ZT_RGB565_D16_32",
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
		(brp_render_fn *)TriangleRenderPIZ2TA15, NULL,
#endif

		"RGB 565, Z Buffered, Textured", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_STRIDE_POSITIVE,
	PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TA15",
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
		(brp_render_fn *)TriangleRenderPIZ2TIA_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Interpolated Intensity, Textured", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPIZ2TIA_RGB_555, NULL,
#endif

		"RGB 565, Z Buffered, Constant Intensity, Textured", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhzf",(void *)"_TriangleRenderPIZ2TIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPIZ2I_RGB_565, NULL,
#endif

		"RGB 565, Z Buffered, Interpolated Colour", NULL,
		BRT_TRIANGLE, 0,

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
	"softhzf",(void *)"_TriangleRenderPIZ2I_RGB_565",
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
		(brp_render_fn *)TriangleRenderPIZ2_RGB_565, NULL,
#endif

		"RGB 565, Z Buffered, Constant Colour", NULL,
		BRT_TRIANGLE, 0,

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
	"softhzf",(void *)"_TriangleRenderPIZ2_RGB_565",
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
		(brp_render_fn *)TriangleRenderPITIPB256_RGB_565, NULL,
#endif

		"RGB 565, Interpolated Intensity, Textured 256x256, Perspective Correct, RGB blended", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_I|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_I)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhf",(void *)"_TriangleRenderPITIPB256_RGB_565",
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
		(brp_render_fn *)TriangleRenderPITIPB256_RGB_565, NULL,
#endif

		"RGB 565, Constant Intensity, Textured 256x256, Perspective Correct, RGB blended", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE|BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_BLEND,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhf",(void *)"_TriangleRenderPITIPB256_RGB_565",
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
		(brp_render_fn *)TriangleRenderPITIP256_RGB_555, NULL,
#endif

		"RGB 565, Interpolated Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_I|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_I)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhf",(void *)"_TriangleRenderPITIP256_RGB_555",
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
		(brp_render_fn *)TriangleRenderPITIP256_RGB_555, NULL,
#endif

		"RGB 565, Constant Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_I)|(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softhf",(void *)"_TriangleRenderPITIP256_RGB_555",
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
		(brp_render_fn *)TriangleRenderPITA15, NULL,
#endif

		"RGB 565, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITA15",
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
		(brp_render_fn *)TriangleRenderPITIA_RGB_555, NULL,
#endif

		"RGB 565, Constant Intensity, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE|BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPITIA_RGB_555, NULL,
#endif

		"RGB 565, Interpolated Intensity, Textured, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_NEED_SUBDIVIDE,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPITA15, NULL,
#endif

		"RGB 565, Textured", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_STRIDE_POSITIVE,
	PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITA15",
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
		(brp_render_fn *)TriangleRenderPITIA_RGB_555, NULL,
#endif

		"RGB 565, Interpolated Intensity, Textured", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPITIA_RGB_555, NULL,
#endif

		"RGB 565, Constant Intensity, Textured", NULL,
		BRT_TRIANGLE, BR_PRIMF_CONST_DUPLICATE,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE,

	/* Texture, depth and shade type
	 */
	PMT_NONE,
	BR_PMT_INDEX_8,
	BR_PMT_RGB_565,
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
	"softhf",(void *)"_TriangleRenderPITIA_RGB_555",
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
		(brp_render_fn *)TriangleRenderPII_RGB_565, NULL,
#endif

		"RGB 565, Interpolated Colour", NULL,
		BRT_TRIANGLE, 0,

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
	"softhf",(void *)"_TriangleRenderPII_RGB_565",
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
		(brp_render_fn *)TriangleRenderPI_RGB_565, NULL,
#endif

		"RGB 565, Constant Colour", NULL,
		BRT_TRIANGLE, 0,

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
	"softhf",(void *)"_TriangleRenderPI_RGB_565",
#else
	NULL,NULL,
#endif
},
