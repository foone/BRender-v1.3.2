/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: brender.h 1.2 1998/11/12 13:19:07 johng Exp $
 * $Locker: $
 *
 * Master include file for BRender
 *
 */
#ifndef _BRENDER_H_
#define _BRENDER_H_

/*
 * Version number
 */
#define BR_VERSION(major,minor,point) ((major) * 65536 + (minor) * 256 + (point))

// #define __BRENDER__  BR_VERSION(1,3,0)
#define __BRENDER__ 0x010300

#ifndef _COMPILER_H_
#include "compiler.h"
#endif		   

#ifndef _BRLIMITS_H_
#include "brlimits.h"
#endif		   

/**
 ** Framework
 **/
#ifndef _BRFILE_H_
#include "brfile.h"
#endif

#ifndef _BRMEM_H_
#include "brmem.h"
#endif

#ifndef _BRSTDLIB_H_
#include "brstdlib.h"
#endif

#ifndef _BRDIAG_H_
#include "brdiag.h"
#endif

#ifndef _BRERR_H_
#include "brerror.h"
#endif

/**
 ** Maths support
 **/
#ifndef _FIXED_H_
#include "fixed.h"
#endif		   

#ifndef _SCALAR_H_
#include "scalar.h"
#endif		   

#ifndef _COLOUR_H_
#include "colour.h"
#endif		   

#ifndef _MATRIX_H_
#include "matrix.h"
#endif

#ifndef _VECTOR_H_
#include "brvector.h"
#endif

#ifndef _ANGLES_H_
#include "angles.h"
#endif

#ifndef _QUAT_H_
#include "quat.h"
#endif

#ifndef _TRANSFRM_H_
#include "transfrm.h"
#endif


/**
 ** Pixelmap support
 **/
#ifndef _PIXELMAP_H_
#include "pixelmap.h"
#endif

#ifndef _BRFONT_H_
#include "brfont.h"
#endif

/**
 ** Device support
 **/
#ifndef _TOKEN_H_
#include "token.h"
#endif

#ifndef _RENDER_H_
#include "render.h"
#endif

#ifndef _POOL_H_
#include "pool.h"
#endif

#ifndef _CUSTSUPT_H_
#include "custsupt.h"
#endif

#ifndef _ORDER_H_
#include "order.h"
#endif


// Associative arrays

#ifndef _ASSOCARR_H_
#include "assocarr.h"
#endif


#include "brlists.h" 

	/*
	 * Function prototypes
	 */
#ifndef _NO_PROTOTYPES

#ifndef _FW_P_H_
#include "fw_p.h"
#endif


#ifndef _MATH_P_H_
#include "math_p.h"
#endif

#ifndef _PM_P_H_
#include "pm_p.h"
#endif


#ifndef _SYSCON_P_H_
#include "syscn_p.h"
#endif



#endif

/**
 ** V1 Database support
 **/

#ifndef __BR_V1DB__
#define __BR_V1DB__ 1
#endif

#if __BR_V1DB__
#ifndef _BRV1DB_H_
#include "brv1db.h"
#endif
#endif

#include "compend.h"


// Associative arrays

#include "assoca_p.h"


#endif

