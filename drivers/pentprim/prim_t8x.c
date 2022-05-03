{
	{
		/* Render function
		 */
#if AUTOLOAD
		(brp_render_fn *)RenderAutoloadThunk, NULL,
#else
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Perspective Correct, Blend, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTFB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTFB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct, Blend, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTFB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTFB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Perspective Correct, Blend, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTFB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTFB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Perspective Correct, Blend, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTFB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTFB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Perspective Correct, Blend, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZPTIFB_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTIFB_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTFB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZPTFB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTIFB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTIFB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIFB_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTIFB_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTFB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Perspective Correct, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTFB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIFB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTIFB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIFB_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTIFB_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTFB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Blend, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"sftizfbx",(void *)"_TriangleRender_ZTFB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Perspective Correct, Blend", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct, Blend", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Perspective Correct, Blend", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Perspective Correct, Blend", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Perspective Correct, Blend", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZPTIB_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTIB_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZPTB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Perspective Correct, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE|BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 16x16, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 16x16, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_16",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_16, NULL,
#endif

		"Indexed, Z Buffered, Textured 16x16, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_16",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_8_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 8x8, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_8_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_8, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 8x8, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_8",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16_8, NULL,
#endif

		"Indexed, Z Buffered, Textured 8x8, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,

	/* Colour & Depth  row size
	 */
	0,0,

	/* Texture size
	 */
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16_8",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIB_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTIB_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTB_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Blend", NULL,
		BRT_TRIANGLE, BR_PRIMF_BLENDED,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	BR_PMT_INDEX_8,
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
	"softizbx",(void *)"_TriangleRender_ZTB_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Perspective Correct, Fog", NULL,
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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTF_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTF_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct, Fog", NULL,
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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTF_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTF_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Perspective Correct, Fog", NULL,
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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTF_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTF_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Perspective Correct, Fog", NULL,
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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTF_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTF_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Perspective Correct, Fog", NULL,
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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZPTIF_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTIF_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPTF_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZPTF_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Fog", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Perspective Correct, Fog", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTF_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Perspective Correct, Fog", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTF_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Fog", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZTF_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Fog", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTF_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Fog", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTIF_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTIF_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTF_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Fog", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	BR_PMT_INDEX_8,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	PMT_NONE,
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZTF_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZIF_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZIF_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZIF_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Fog", NULL,
		BRT_TRIANGLE, 0,

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
	BR_PMT_INDEX_8,

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
	"softizfx",(void *)"_TriangleRender_ZIF_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Decal", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,

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
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTID_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,

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
	"softizx",(void *)"_TriangleRender_ZTID_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD1024",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD256",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD128",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD64",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024, Perspective Correct", NULL,
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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPT_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPT_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256, Perspective Correct", NULL,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPT_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPT_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128, Perspective Correct", NULL,
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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPT_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPT_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64, Perspective Correct", NULL,
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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPT_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPT_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32, Perspective Correct", NULL,
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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZPTI_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPTI_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZPT_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZPT_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softizx",(void *)"_TriangleRender_ZT_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_1024_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 1024x1024", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 1024x1024", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_1024, NULL,
#endif

		"Indexed, Z Buffered, Textured 1024x1024", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_1024",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_256_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 256x256", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_256_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 256x256", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_256, NULL,
#endif

		"Indexed, Z Buffered, Textured 256x256", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_256",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_128_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 128x128", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_128_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 128x128", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_128, NULL,
#endif

		"Indexed, Z Buffered, Textured 128x128", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_128",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_64_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 64x64", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_64_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 64x64", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_64, NULL,
#endif

		"Indexed, Z Buffered, Textured 64x64", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_64",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_32_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 32x32", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_32_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 32x32", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_32, NULL,
#endif

		"Indexed, Z Buffered, Textured 32x32", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_32",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 16x16", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 16x16", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_16",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_16, NULL,
#endif

		"Indexed, Z Buffered, Textured 16x16", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	16,16,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_16",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_8_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured 8x8", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_8_FLAT",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_8, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured 8x8", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_8",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16_8, NULL,
#endif

		"Indexed, Z Buffered, Textured 8x8", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	8,8,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softizx",(void *)"_TriangleRender_ZT_I8_D16_8",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity, Textured, Unscaled_texture_coords", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZTI_I8_D16_FLAT, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity, Textured, Unscaled_texture_coords", NULL,
		BRT_TRIANGLE, 0,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softizx",(void *)"_TriangleRender_ZTI_I8_D16_FLAT",
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
		(brp_render_fn *)TriangleRender_ZT_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Textured, Unscaled_texture_coords", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softizx",(void *)"_TriangleRender_ZT_I8_D16",
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
		(brp_render_fn *)TriangleRender_ZI_I8_D16_ShadeTable, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_SMOOTH|PRIMF_RANGE_ZERO,
	PRIMF_SMOOTH|PRIMF_RANGE_ZERO,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	PMT_NONE,
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
	"softizx",(void *)"_TriangleRender_ZI_I8_D16_ShadeTable",
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
		(brp_render_fn *)TriangleRender_ZI_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Interpolated Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	"softizx",(void *)"_TriangleRender_ZI_I8_D16",
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
		(brp_render_fn *)TriangleRender_Z_I8_D16_ShadeTable, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_RANGE_ZERO|PRIMF_SMOOTH,
	PRIMF_RANGE_ZERO,

	/* Texture, depth and shade type
	 */
	BR_PMT_DEPTH_16,
	PMT_NONE,
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
	"softizx",(void *)"_TriangleRender_Z_I8_D16_ShadeTable",
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
		(brp_render_fn *)TriangleRender_Z_I8_D16, NULL,
#endif

		"Indexed, Z Buffered, Constant Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	"softizx",(void *)"_TriangleRender_Z_I8_D16",
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
		(brp_render_fn *)TriangleRender_Z_I8_D16, NULL,
#endif

		"Indexed, Z Buffered", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ),
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
	"softizx",(void *)"_TriangleRender_Z_I8_D16",
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
		(brp_render_fn *)TriangleRender_TID_I8_256, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 256x256, Decal", NULL,
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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_TID_I8_256",
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
		(brp_render_fn *)TriangleRender_TID_I8_256_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 256x256, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_DECAL,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_TID_I8_256_FLAT",
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
		(brp_render_fn *)TriangleRender_TID_I8, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured, Unscaled_texture_coords, Decal", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,

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
	"softix",(void *)"_TriangleRender_TID_I8",
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
		(brp_render_fn *)TriangleRender_TID_I8_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured, Unscaled_texture_coords, Decal", NULL,
		BRT_TRIANGLE, 0,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_DECAL,

	/* Work buffer
	 */
	&work,

	/* Masks
	 */
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_DECAL,

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
	"softix",(void *)"_TriangleRender_TID_I8_FLAT",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD1024, NULL,
#endif

		"Indexed, Textured 1024x1024, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD1024",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD256, NULL,
#endif

		"Indexed, Textured 256x256, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD256",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD128, NULL,
#endif

		"Indexed, Textured 128x128, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD128",
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
		(brp_render_fn *)TriangleRenderPIZ2TPD64, NULL,
#endif

		"Indexed, Textured 64x64, Perspective Correct, Dithered Map", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE|PRIMF_DITHER_MAP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softd",(void *)"_TriangleRenderPIZ2TPD64",
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
		(brp_render_fn *)TriangleRender_PTI_I8_1024, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 1024x1024, Perspective Correct", NULL,
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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_1024",
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
		(brp_render_fn *)TriangleRender_PTI_I8_1024_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 1024x1024, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_1024_FLAT",
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
		(brp_render_fn *)TriangleRender_PT_I8_1024, NULL,
#endif

		"Indexed, Textured 1024x1024, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PT_I8_1024",
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
		(brp_render_fn *)TriangleRender_PTI_I8_256, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 256x256, Perspective Correct", NULL,
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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_256",
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
		(brp_render_fn *)TriangleRender_PTI_I8_256_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_256_FLAT",
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
		(brp_render_fn *)TriangleRender_PT_I8_256, NULL,
#endif

		"Indexed, Textured 256x256, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PT_I8_256",
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
		(brp_render_fn *)TriangleRender_PTI_I8_128, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 128x128, Perspective Correct", NULL,
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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_128",
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
		(brp_render_fn *)TriangleRender_PTI_I8_128_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 128x128, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_128_FLAT",
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
		(brp_render_fn *)TriangleRender_PT_I8_128, NULL,
#endif

		"Indexed, Textured 128x128, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PT_I8_128",
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
		(brp_render_fn *)TriangleRender_PTI_I8_64, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 64x64, Perspective Correct", NULL,
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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_64",
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
		(brp_render_fn *)TriangleRender_PTI_I8_64_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 64x64, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PTI_I8_64_FLAT",
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
		(brp_render_fn *)TriangleRender_PT_I8_64, NULL,
#endif

		"Indexed, Textured 64x64, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PT_I8_64",
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
		(brp_render_fn *)TriangleRender_PT_I8_32, NULL,
#endif

		"Indexed, Textured 32x32, Perspective Correct", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W),
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_PERSPECTIVE,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_PT_I8_32",
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
		(brp_render_fn *)TriangleRender_TI_I8_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
		BRT_TRIANGLE, BR_PRIMF_SUBDIVIDE,

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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softix",(void *)"_TriangleRender_TI_I8_FLAT",
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
		(brp_render_fn *)TriangleRender_TI_I8, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softix",(void *)"_TriangleRender_TI_I8",
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
		(brp_render_fn *)TriangleRender_T_I8, NULL,
#endif

		"Indexed, Textured, Unscaled_texture_coords, Perspective Correct", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS|RF_NEED_SUBDIVIDE,

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
	"softix",(void *)"_TriangleRender_T_I8",
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
		(brp_render_fn *)TriangleRender_T_I8_1024, NULL,
#endif

		"Indexed, Textured 1024x1024", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	1024,1024,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_T_I8_1024",
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
		(brp_render_fn *)TriangleRender_TI_I8_256, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured 256x256", NULL,
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
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_SMOOTH|PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_TI_I8_256",
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
		(brp_render_fn *)TriangleRender_TI_I8_256_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured 256x256", NULL,
		BRT_TRIANGLE, 0,

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
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP|PRIMF_SMOOTH,
	PRIMF_MODULATE|PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_TI_I8_256_FLAT",
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
		(brp_render_fn *)TriangleRender_T_I8_256, NULL,
#endif

		"Indexed, Textured 256x256", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	256,256,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_T_I8_256",
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
		(brp_render_fn *)TriangleRender_T_I8_128, NULL,
#endif

		"Indexed, Textured 128x128", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	128,128,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_T_I8_128",
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
		(brp_render_fn *)TriangleRender_T_I8_64, NULL,
#endif

		"Indexed, Textured 64x64", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	64,64,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_T_I8_64",
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
		(brp_render_fn *)TriangleRender_T_I8_32, NULL,
#endif

		"Indexed, Textured 32x32", NULL,
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
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,
	PRIMF_STRIDE_POSITIVE|PRIMF_NO_SKIP,

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
	32,32,

	/* Input colour type
	 */
	0,

	/* Autoload info
	 */
#if AUTOLOAD
	"softix",(void *)"_TriangleRender_T_I8_32",
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
		(brp_render_fn *)TriangleRender_TI_I8, NULL,
#endif

		"Indexed, Interpolated Intensity, Textured, Unscaled_texture_coords", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softix",(void *)"_TriangleRender_TI_I8",
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
		(brp_render_fn *)TriangleRender_TI_I8_FLAT, NULL,
#endif

		"Indexed, Constant Intensity, Textured, Unscaled_texture_coords", NULL,
		BRT_TRIANGLE, 0,

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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softix",(void *)"_TriangleRender_TI_I8_FLAT",
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
		(brp_render_fn *)TriangleRender_T_I8, NULL,
#endif

		"Indexed, Textured, Unscaled_texture_coords", NULL,
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
	RF_UNSCALED_TEXTURE_COORDS,

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
	"softix",(void *)"_TriangleRender_T_I8",
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
		(brp_render_fn *)TriangleRender_I_I8, NULL,
#endif

		"Indexed, Interpolated Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	"softix",(void *)"_TriangleRender_I_I8",
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
		(brp_render_fn *)TriangleRender_I8, NULL,
#endif

		"Indexed, Constant Intensity", NULL,
		BRT_TRIANGLE, 0,

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
	"softix",(void *)"_TriangleRender_I8",
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
		(brp_render_fn *)TriangleRender_I8, NULL,
#endif

		"Indexed", NULL,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY,

		/* Component slots as - float, fixed or integer
		 */
		0,
		(1<<C_SX)|(1<<C_SY),
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
	"softix",(void *)"_TriangleRender_I8",
#else
	NULL,NULL,
#endif
},
