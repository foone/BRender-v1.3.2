/*
 * We are more likely to use 16 bit renderers than 8 bit, so place them first
 * so that they will be the first blocks tested - less overhead on average
 */

{
	{
		/* Setup and render functions
		 */
		TriRender1624Smooth,NULL,

		"Triangle 16 or 24 bit smooth output", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_W)|(1<<C_U)|(1<<C_V)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_SMOOTH | PRIMF_MODULATE,
	PRIMF_SMOOTH | PRIMF_MODULATE,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		TriRender1624Flat,NULL,

		"Triangle 16 or 24 bit Flat shaded output", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_W)|(1<<C_U)|(1<<C_V)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_MODULATE,
	PRIMF_MODULATE,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		TriRender1624Unlit,NULL,

		"Unlit 16/24 bit triangle renderer", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		CM_UR|CM_UG|CM_UB,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_W)|(1<<C_U)|(1<<C_V)|(1<<C_UR)|(1<<C_UG)|(1<<C_UB),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_MODULATE,
	0,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},



{
	{
		/* Setup and render functions
		 */
		TriRender8Unlit,NULL,

		"Unlit 8 bit triangle renderer", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		CM_UI,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_UI),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_MODULATE,
	0,

	/* Colour, texture, depth and shade type
	 */
	BR_PMT_INDEX_8,
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		TriRender8Smooth,NULL,

		"Gouraud Triangle 8 bit output", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_I,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_I),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_SMOOTH | PRIMF_MODULATE,
	PRIMF_SMOOTH | PRIMF_MODULATE,

	/* Colour, texture, depth and shade type
	 */
	BR_PMT_INDEX_8,
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},



{
	{
		/* Setup and render functions
		 */
		TriRender8Flat,NULL,

		"Flat shaded Triangle 8 bit output", &PrimitiveLibraryHardS3,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_I),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_MODULATE,
	PRIMF_MODULATE,

	/* Colour, texture, depth and shade type
	 */
	BR_PMT_INDEX_8,
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},





/* Edge style rendering functions */

{
	{
		/* Setup and render functions
		 */
		EdgeRender8,NULL,

		"Edge rendered 8 bit output", &PrimitiveLibraryHardS3,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W|CM_I,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_I),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	0,
	0,

	/* Colour, texture, depth and shade type
	 */
	BR_PMT_INDEX_8,
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		EdgeRender1624Smooth,NULL,

		"Edge rendered 16 or 24 bit smooth output", &PrimitiveLibraryHardS3,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		EdgeRender1624Flat,NULL,

		"Edge rendered 16 or 24 bit Flat shaded output", &PrimitiveLibraryHardS3,
		BRT_LINE, 0,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	0,
	0,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},










/* Point style rendering functions */

{
	{
		/* Setup and render functions
		 */
		PointRender8,NULL,

		"Point rendered 8 bit output", &PrimitiveLibraryHardS3,
		BRT_POINT, 0,

		/* components - constant and per vertex
		 */
		CM_I,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_I),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	0,
	0,

	/* Colour, texture, depth and shade type
	 */
	BR_PMT_INDEX_8,
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},


{
	{
		/* Setup and render functions
		 */
		PointRender1624,NULL,

		"Point rendered 16 or 24 bit", &PrimitiveLibraryHardS3,
		BRT_POINT, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_W,

		/* Components as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_U)|(1<<C_V)|(1<<C_W)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		0,
	},

	/* Masks
	 */
	PRIMF_SMOOTH,
	PRIMF_SMOOTH,

	/* Colour, texture, depth and shade type
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

	/* Work buffer
	 */
	0,

	/* Autoload info
	 */
	NULL,NULL,
},




