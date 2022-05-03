/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: quat.h 1.1 1997/12/10 16:41:19 jon Exp $
 * $Locker: $
 *
 * Type descibing a unit quaternion
 */
#ifndef _QUAT_H_
#define _QUAT_H_

typedef struct br_quat {
	br_scalar x;
	br_scalar y;
	br_scalar z;
	br_scalar w;
} br_quat;

#define BR_QUAT(x,y,z,w) {BR_SCALAR(x),BR_SCALAR(y),BR_SCALAR(z),BR_SCALAR(w)}

#endif


