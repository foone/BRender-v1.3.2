/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pm.h 1.1 1997/12/10 16:41:25 jon Exp $
 * $Locker: $
 *
 * Internal types and structures for pixelmap support
 */
#ifndef _PM_H_
#define _PM_H_

/*
 * Pull in all the public definitions/declarations and framework
 */
#ifndef _BRDDI_H_
#include "brddi.h"
#endif

/*
 * Static workspace for pixelmap supprot library
 */
struct br_pixelmap_state {
    /*
     * Resource anchor
     */
    void *res;

    /*
     * Templates for memory pixelmap operations
     */
    br_tv_template *device_pixelmap_template;
    br_tv_template *pixelmap_match_template;
};

/*
 * Global renderer state
 */
#ifdef __cplusplus
extern "C" {
#endif
extern struct br_pixelmap_state BR_ASM_DATA _pixelmap;

#ifdef __cplusplus
};
#endif

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _PM_IP_H_
#include "pm_ip.h"
#endif

#endif

#endif



