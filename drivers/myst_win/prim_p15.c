{
	{
		/* Render function
		 */
        (brp_render_fn *) PointRenderFlat, NULL,

		"Flat", &PrimitiveLibraryMyst,
		BRT_POINT, 0,

		/* components - constant and per vertex
		 */
		CM_R|CM_G|CM_B,
		CM_SX|CM_SY|CM_SZ,

		/* Component slots as - float, fixed or integer
		 */
		(1<<C_SX)|(1<<C_SY)|(1<<C_SZ)|(1<<C_R)|(1<<C_G)|(1<<C_B),
		0,
		0,

		/* Constant slots
	 	 */
		(1<<C_R)|(1<<C_G)|(1<<C_B),

		/* Reserved */
		0, 0, 0, 0
	},

	/* Masks
	 */
	0,
	0,
}
