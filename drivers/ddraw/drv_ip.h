/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv_ip.h 1.1 1997/12/10 16:45:17 jon Exp $
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
 * object.c
 */
char * BR_CMETHOD_DECL(br_object_dd, identifier)(br_object *self);
br_device *	BR_CMETHOD_DECL(br_object_dd, device)(br_object *self);

/*
 * device.c
 */
br_device *DeviceDirectDrawAllocate(char *id, char *args);

/*
 * outfcty.c
 */
br_error OutputFacilityDirectDrawInitialise(br_device *dev);

/*
 * devpixmp.c
 */
br_device_pixelmap * DevicePixelmapDirectDrawAllocate(br_device *dev, br_output_facility *type, br_token_value *tv);

br_token BR_CMETHOD_DECL(br_device_pixelmap_dd, type)(br_device_pixelmap *self);
br_boolean BR_CMETHOD_DECL(br_device_pixelmap_dd, isType)(br_device_pixelmap *self, br_token t);
br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_dd, space)(br_device_pixelmap *self);
struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_dd, queryTemplate)(br_device_pixelmap *self);

/*
 * devpmddo.c
 */
br_error BufferDirectDrawAllocateOff(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h, int type );
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, directLock)(br_device_pixelmap *self, br_boolean block);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, directUnlock)(br_device_pixelmap *self);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, flush)(struct br_device_pixelmap *self);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, line)\
	(br_device_pixelmap *self, br_point *s, br_point *e, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, copyBits)\
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour);

br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopy)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopyTo)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, pixelSet)\
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, fill)\
	(br_device_pixelmap *self, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleFill)\
	(br_device_pixelmap *self, br_rectangle *r, br_uint_32 colour);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, fillDirty)\
	(br_device_pixelmap *self, br_uint_32 colour, br_rectangle *dirty, br_int_32 num_rects);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, copy)\
	(br_device_pixelmap *self, br_device_pixelmap *src);
br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, rectangleCopyFrom)\
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r);

br_error BR_CMETHOD_DECL(br_device_pixelmap_ddo, pixelQuery)\
	(br_device_pixelmap *self, br_uint_32 *pcolour, br_point *p);


/*
 * devclut.c
 */
br_device_clut * DeviceClutDirectDrawAllocate(br_device *dev, br_device_pixelmap *dpm, char *identifier);

/*
 * ocfree.c
 */
br_error ObjectContainerFree(struct br_object_container *self, br_token type, char *pattern, br_token_value *tv);

#ifdef __cplusplus
};
#endif

#endif
#endif


