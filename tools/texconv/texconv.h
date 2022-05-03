/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: texconv.h 1.1 1997/12/10 16:59:14 jon Exp $
 * $Locker: $
 *
 */
#ifndef _TEXCONV_H_
#define _TEXCONV_H_

#define TYPE_GUESS -1

extern char *res_anchor;
extern br_uint_32 palette_type;
extern br_uint_8 alpha_threshold;
extern br_int_32 base;
extern br_int_32 range;

#ifndef _LISTS_H_
#include "lists.h"		/* linked list fns */
#endif

#ifndef _CLA_H_
#include "cla.h"		/* command line argument processing */
#endif

#ifndef _PMAPS_H_
#include "pmaps.h"		/* generic pixelmap cbfn */
#endif

#ifndef _CONVERT_H_
#include "convert.h"		/* pixelmap conversion */
#endif

#ifndef _LOADPIX_H_
#include "loadpix.h"		/* pix/gif/bmp/iff/tga load */
#endif

#ifndef _LOG_H_
#include "log.h"
#endif

#ifndef _SAVEPIX_H_
#include "savepix.h"		/* pix/pal/iamge save */
#endif

#ifndef _SAVETGA_H_
#include "savetga.h"		/* tga save */
#endif

#endif
