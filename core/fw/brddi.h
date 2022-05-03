/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brddi.h 1.2 1998/11/12 13:14:23 johng Exp $
 * $Locker: $
 *
 * Master include file for devices
 */
#ifndef _BRDDI_H_
#define _BRDDI_H_

#define __BRENDER_DDI__ BR_VERSION(1,1,0)

/*
 * Pull in all the public definitions/declarations
 */
#ifndef _BRENDER_H_
#include "brender.h"
#endif

#ifndef _POOL_H_
#include "pool.h"
#endif

#ifndef _BREXCEPT_H_
#include "brexcept.h"
#endif

#ifndef _REGISTER_H_
#include "register.h"
#endif

#ifndef _TOKEN_H_
#include "token.h"
#endif

#ifndef _TOKENMAP_H_
#include "tokenmap.h"
#endif

#ifndef _LEXER_H_
#include "lexer.h"
#endif

#ifndef _BRIMAGE_H_
#include "brimage.h"
#endif

#ifndef _DEV_OBJS_H_
#include "dev_objs.cgh"
#endif

/*
 * Pull in DDI prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef _BRDDI_P_H_
#include "brddi_p.h"
#endif

#endif
#endif



