/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.2 1998/10/21 15:18:31 jon Exp $
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
 * driver.c
 */
extern br_device DriverDeviceS3;

/*
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_s3, identifier)(br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_s3, device)(br_object *self);

/*
 * device.c
 */
br_error DeviceS3Initialise(br_device *dev);

/*
 * outfcty.c
 */
br_error OutputFacilityS3Initialise(br_device *dev);

/*
 * dpmfront.c
 */
br_device_pixelmap * DevicePixelmapS3Allocate(br_device *dev, br_output_facility *type, br_token_value *tv);
static br_token BufferMethodInit(br_device *dev, br_token method_hint);

br_token BR_CMETHOD_DECL(br_device_pixelmap_s3, type)(br_device_pixelmap *self);
br_boolean BR_CMETHOD_DECL(br_device_pixelmap_s3, isType)(br_device_pixelmap *self, br_token t);
br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_s3, space)(br_device_pixelmap *self);
struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_s3, queryTemplate)(br_device_pixelmap *self);

/*
 * dpmback.c
 */
br_error BufferMethodDirectDrawInit(br_device *dev);
br_error BufferDirectDrawAllocate(br_device_pixelmap *self);
br_error BufferDirectDrawAllocateOff(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);
br_error BufferDirectDrawFree(br_device_pixelmap *self);
br_error DoubleBufferDirectDraw(br_device_pixelmap *self, br_device_pixelmap *src);
br_error DoubleBufferDirtyDirectDraw(br_device_pixelmap *self, br_device_pixelmap *src, br_rectangle *dirty, br_int_32 num_rects);
static br_error LockSurface(br_device_pixelmap *self);
static br_error UnlockSurface(br_device_pixelmap *self);

br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, copyBits)\
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, line)\
	(br_device_pixelmap *self, br_point *s, br_point *e, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *r, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleCopy)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleCopyTo)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleCopyFrom)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, directLock)(br_device_pixelmap *self, br_boolean block);
br_error BR_CMETHOD_DECL(br_device_pixelmap_s3_dd, directUnlock)(br_device_pixelmap *self);

/*
 * devclut.c
 */
br_device_clut * DeviceClutS3Allocate(br_device *dev, br_device_pixelmap *dpm, char *identifier);


/*
 * plib.c
 */
extern br_token PrimPartsTokensS3[];
br_error PrimitiveLibraryHardS3Initialise(struct br_primitive_library * self, br_device *dev);

/*
 * pstate.c
 */
struct br_primitive_state * PrimitiveStateHardS3Allocate(struct br_primitive_library *plib);

/*
 * sbuffer.c
 */
br_error S3SetupRenderBuffer(struct render_buffer *rb,
						br_device_pixelmap *pm,
						br_buffer_stored *self,
						br_boolean texture);

struct br_buffer_stored * BufferStoredHardS3Allocate(struct br_primitive_library *plib,
	br_token use, struct br_device_pixelmap *pm, br_token_value *tv);

/*
 * match.c
 */
br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, renderBegin)(
		struct br_primitive_state *self,
		struct brp_block **rpb,
		br_boolean *pchanged,
      br_boolean *ranges_changed,
		br_boolean no_render,
		br_token prim_type);

br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, renderEnd)(
		struct br_primitive_state *self,
		struct brp_block *rpb);

br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, rangesQueryF)(
		struct br_primitive_state *self,
		br_float *offset,
		br_float *scale,
		br_int_32 max_comp);

br_error BR_CMETHOD_DECL(br_primitive_state_hardS3, rangesQueryX)(
		struct br_primitive_state *self,
		br_fixed_ls *offset,
		br_fixed_ls *scale,
		br_int_32 max_comp);



/*
 * ocfree.c
 */
br_error ObjectContainerFree(struct br_object_container *self, br_token type, char *pattern, br_token_value *tv);

#ifdef __cplusplus
};
#endif

#endif
#endif


