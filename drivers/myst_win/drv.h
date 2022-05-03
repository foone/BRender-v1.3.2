/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: drv.h 1.1 1997/12/10 16:46:01 jon Exp $
 * $Locker: $
 *
 * Private device driver structure
 */
#ifndef _DRV_H_
#define _DRV_H_


#ifdef _WIN32
#include <windows.h>
#include "msi95.h"

typedef unsigned char *MSImemptr;

#else

#include <i86.h>

#pragma pack(push)
#pragma pack(1)
#include <msidos.h>
#pragma pack(pop)

typedef unsigned char far *MSImemptr;

#define TRUE 1
#define FALSE 0

#endif


#include "log.h"



/* Dave's macro to rapidly clear all the templates whose maps mustn't be
 * held over between driver instances */
#define CLEAR_TEMPLATE(a) \
  memset(&a##Template, 0, sizeof(a##Template)); \
  a##Template.n_entries = BR_ASIZE(a##TemplateEntries); \
  a##Template.entries = a##TemplateEntries; \



#define BR_OBJECT_PRIVATE
#define BR_DEVICE_PRIVATE
#define BR_OUTPUT_FACILITY_PRIVATE
#define BR_DEVICE_PIXELMAP_PRIVATE
#define BR_PRIMITIVE_STATE_PRIVATE
#define BR_PRIMITIVE_LIBRARY_PRIVATE
#define BR_BUFFER_STORED_PRIVATE
#define BR_DEVICE_CLUT_PRIVATE

#ifndef _BRDDI_H_
#include "brddi.h"
#endif

#ifndef _DEVICE_H_
#include "device.h"
#endif

#ifndef _OUTFCTY_H_
#include "outfcty.h"
#endif

#ifndef _DEVPIXMP_H_
#include "devpixmp.h"
#endif

#ifndef _OBJECT_H_
#include "object.h"
#endif

#ifndef _PRIMINFO_H_
#include "priminfo.h"
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

#ifndef _DEVCLUT_H_
#include "devclut.h"
#endif

#ifndef _MATCH_H_
#include "match.h"
#endif

#ifndef _TIMESTMP_H_
#include "timestmp.h"
#endif

/*
 * Macros that exapnd to the first two arguments of a template entry
 * Builtin or device token
 */
#define BRT(t)	BRT_##t,0
#define DEV(t)	0,#t

/*
 * Macro for driver wide resource anchor
 */
#define DRIVER_RESOURCE (DriverDeviceMyst.res)

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

