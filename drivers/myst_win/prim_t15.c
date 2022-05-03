{
	{
		/* Render function
		 */
        (brp_render_fn *) TriangleRenderSmooth, NULL,

        "Smooth", &PrimitiveLibraryMyst,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		0,
		CM_SX|CM_SY|CM_SZ|CM_R|CM_G|CM_B|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
        (1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_W),
        0,
		0,

		/* Constant slots
	 	 */
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks
	 */
    PRIMF_SMOOTH,
    PRIMF_SMOOTH,
},
{
	{
		/* Render function
		 */
        (brp_render_fn *) TriangleRenderFlat, NULL,

        "Flat", &PrimitiveLibraryMyst,
		BRT_TRIANGLE, 0,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ|CM_U|CM_V|CM_W,

		/* Component slots as - float, fixed or integer
		 */
        (1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B)|(1<<C_U)|(1<<C_V)|(1<<C_W),
        0,
		0,

		/* Constant slots
	 	 */
//		(1<<C_R)|(1<<C_G)|(1<<C_B),		// May be buggy and slow, removed
		0,

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks
	 */
    PRIMF_SMOOTH,
    0,
},
