/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.1 1997/12/10 16:46:02 jon Exp $
 * $Locker: $
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
br_error DeviceMystInitialise(br_device * dev);
extern br_device DriverDeviceMyst;
char * BR_CMETHOD_DECL(br_object_mystique, identifier)( br_object *self);
br_device * BR_CMETHOD_DECL(br_object_mystique, device)( br_object *self);

/*
 * outfcty.c
 */
br_error OutputFacilityMystInitialise(br_output_facility *self, br_device *dev,
	br_primitive_library *prim_lib);

/*
 * devpixmp.c
 */
br_error DevicePixelmapMystAllocateMode(br_device *dev, br_output_facility *type,
										br_device_pixelmap **newpm, br_token_value *tv);
void MystiqueBuffersStartFrame(void);

/*
 * plib.c
 */
extern br_token PrimPartsTokensMyst[];
br_error PrimitiveLibraryMystInitialise(struct br_primitive_library * self, struct br_device *dev);

/*
 * pstate.c
 */
struct br_primitive_state * PrimitiveStateMystAllocate(struct br_primitive_library *plib);

/*
 * sbuffer.c
 */
void ClearHeap(void);
struct br_buffer_stored * BufferStoredSoftAllocateMyst(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv);


/*
 * match.c
 */
void MystiqueInitConstantRanges(struct br_primitive_state *self);

br_error BR_CMETHOD_DECL(br_primitive_state_mystique, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *block_changed,
		br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type);

br_error BR_CMETHOD_DECL(br_primitive_state_mystique, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *pb);

br_error BR_CMETHOD_DECL(br_primitive_state_mystique, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp);

br_error BR_CMETHOD_DECL(br_primitive_state_mystique, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp);

/*
 * render.c
 */
void BR_ASM_CALL PointRenderSmooth(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL PointRenderFlat(brp_block *block, brp_vertex *v0);
void BR_ASM_CALL LineRenderSmooth(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL LineRenderFlat(brp_block *block, brp_vertex *v0,brp_vertex *v1);
void BR_ASM_CALL TriangleRenderFlat(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);
void BR_ASM_CALL TriangleRenderSmooth(brp_block *block, brp_vertex *a,brp_vertex *b,brp_vertex *c);

/*
 * devclut.c
 */
struct br_device_clut * DeviceClutMystiqueAllocate(br_device *dev, char *identifier);

#ifdef __cplusplus
};

#endif

#endif
#endif

