/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Prototypes for functions internal to driver
 */
#ifndef _DRV_IP_H_
#define _DRV_IP_H_

#ifndef NO_PROTOTYPES

#ifdef __cplusplus
extern "C" {
#endif

/*
 * device.c
 */
br_error Device3DfxInitialise(br_device * dev);
extern br_device DriverDevice3Dfx;
char * BR_CMETHOD_DECL(br_object_3dfx, identifier)( br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_3dfx, device)( br_object *self);

/*
 * outfcty.c
 */
br_uint_32 OutputFacility3DfxInitialise(br_device *dev, br_primitive_library *prim_lib);

/*
 * devpixmp.c
 */
br_error DevicePixelmap3DfxAllocateMode(br_device *dev, br_output_facility *type,
										br_device_pixelmap **newpm, br_token_value *tv);

/*
 * plib.c
 */
extern br_token PrimParts3DFXTokens[];
br_error PrimitiveLibrary3DfxInitialise(struct br_primitive_library * self, struct br_device *dev);

/*
 * pstate.c
 */
struct br_primitive_state * PrimitiveState3DfxAllocate(struct br_primitive_library *plib);

/*
 * sbuffer.c
 */
void SetupRenderBuffer(struct render_buffer *rb, br_device_pixelmap *pm);

struct br_buffer_stored * BufferStored3DfxAllocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv);


/*
 * match.c
 */
br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type);

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb);

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp);

br_error BR_CMETHOD_DECL(br_primitive_state_3dfx, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp);

void Clear3DfxStateCache(void);

void initConstantRanges(struct br_primitive_state *self);

/*
 * rendfunc.c
 */
void BR_ASM_CALL PointRender(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL LineRender_Smooth(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRender_Flat(brp_block *block, brp_vertex *v0,brp_vertex *v1);

void BR_ASM_CALL TriangleRender_Smooth(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_Flat(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_UnlitTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_FlatTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRender_SmoothTextured(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);



/*
 * devclut.c
 */
struct br_device_clut * DeviceClut3DfxAllocate(br_device *dev, char *identifier);

/*
 * memloops.asm
 */
void BR_ASM_CALL _MemCopyBits3DFX_A(
	char *dest, br_uint_32 dest_qual, br_int_32 d_stride,
	br_uint_8 *src,br_uint_32 s_stride,
	br_uint_32 start_bit,br_uint_32 end_bit,
	br_uint_32 nrows, br_uint_32 colour);

#ifdef __cplusplus
};

#endif

#endif
#endif

