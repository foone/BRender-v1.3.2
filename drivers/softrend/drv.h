/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv.h 1.1 1997/12/10 16:51:46 jon Exp $
 * $Locker: $
 *
 * Private device driver structure
 */
#ifndef _DRV_H_
#define _DRV_H_

#ifdef __cplusplus
extern "C" {
#endif

#define BR_OBJECT_PRIVATE
#define BR_DEVICE_PRIVATE
#define BR_RENDERER_FACILITY_PRIVATE
#define BR_RENDERER_STATE_PRIVATE
#define BR_RENDERER_STATE_STORED_PRIVATE
#define BR_RENDERER_PRIVATE
#define BR_GEOMETRY_V1_MODEL_PRIVATE
#define BR_GEOMETRY_V1_BUCKETS_PRIVATE
#define BR_GEOMETRY_PRIMITIVES_PRIVATE
#define BR_GEOMETRY_LIGHTING_PRIVATE

#ifndef _BRDDI_H_
#include "brddi.h"
#endif

#ifndef _TIMESTMP_H_
#include "timestmp.h"
#endif

#ifndef _PRIMINFO_H_
#include "priminfo.h"
#endif

#ifndef _OBJECT_H_
#include "object.h"
#endif

#ifndef _TEMPLATE_H_
#include "template.h"
#endif

#ifndef _DEVICE_H_
#include "device.h"
#endif

#ifndef _STATE_H_
#include "state.h"
#endif

#ifndef _RENDFCTY_H_
#include "rendfcty.h"
#endif

#ifndef _RENDERER_H_
#include "renderer.h"
#endif

#ifndef _SSTATE_H_
#include "sstate.h"
#endif

#ifndef _GV1MODEL_H_
#include "gv1model.h"
#endif

#ifndef _GV1BUCKT_H_
#include "gv1buckt.h"
#endif

#ifndef _GLIGHTING_H_
#include "lighting.h"
#endif

#ifndef _GPRIM_H_
#include "gprim.h"
#endif

#ifndef _FORMATS_H_
#include "formats.h"
#endif

#ifndef _REND_H_
#include "rend.h"
#endif

/*
 * Macros that exapnd to the first two arguments of a template entry
 * Builtin or device token
 */
#define BRT(t)	BRT_##t,0
#define DEV(t)	0,#t

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _DRV_IP_H_
#include "drv_ip.h"
#endif

#endif

#ifdef __cplusplus
};
#endif
#endif

