/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv.h 1.1 1997/12/10 16:46:46 jon Exp $
 * $Locker: $
 *
 * Private device driver structure
 */
#ifndef _DRV_H_
#define _DRV_H_

#define BR_OBJECT_PRIVATE
#define BR_DEVICE_PRIVATE
#define BR_PRIMITIVE_LIBRARY_PRIVATE
#define BR_PRIMITIVE_STATE_PRIVATE
#define BR_BUFFER_STORED_PRIVATE

#ifndef _BRDDI_H_
#include "brddi.h"
#endif

#ifndef _PRIMINFO_H_
#include "priminfo.h"
#endif

#ifndef _WORK_H_
#include "work.h"
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

#ifndef _PLIB_H_
#include "plib.h"
#endif

#ifndef _PSTATE_H_
#include "pstate.h"
#endif

#ifndef _SBUFFER_H_
#include "sbuffer.h"
#endif

#ifndef _PRIMINFO_H_
#include "priminfo.h"
#endif

#ifndef _MATCH_H_
#include "match.h"
#endif

#ifndef _TIMESTMP_H_
#include "timestmp.h"
#endif

#ifndef _RASTPARAM_H_
#include "rastparm.h"
#endif

/*
 * Macros that exapnd to the first two arguments of a template entry
 * Builtin or device token
 */
#define BRT(t)	BRT_##t,0
#define DEV(t)	0,#t

/*
 * Definitions for pulling in different renderers
 */
#define PART_8		0x0001
#define PART_15		0x0002
#define PART_24		0x0004

#define PART_8Z		0x0008
#define PART_15Z	0x0010
#define PART_24Z	0x0020

#define PART_BUMP	0x0040

#define PART_8Z4 	0x0000
#define PART_15Z4	0x0000
#define PART_24Z4	0x0000

#define PART_16		0x0080
#define PART_16Z	0x0100
#define PART_16Z4	0x0200

#define PART_8Z_FOG	0x0400
#define PART_8Z_BLEND	0x0800
#define PART_8Z_FOG_BLEND	0x1000

#define PART_DITHER	0x2000

#define PART_15Z_OR_16Z	0x0110

#define PART_15_OR_16   0x0082


/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _DRV_IP_H_
#include "drv_ip.h"
#endif

/*
 * MMX prototypes are seperated out for the moment for easier development
 */
#ifndef _MMX_IP_H_
#include "mmx_ip.h"
#endif

#endif

#ifdef __cplusplus
};
#endif
#endif

