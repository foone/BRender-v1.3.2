/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: zsrmacro.h 1.1 1997/12/10 16:53:18 jon Exp $
 * $Locker: $
 *
 */
#ifndef _ZSRMACRO_H_
#define _ZSRMACRO_H_

#ifdef __cplusplus
extern "C" {
#endif

#define VIEW_Z(v) ((v)->comp[C_W])

/*
 * Calculate the sort value for a triangle
 */
#define SORT_VALUE_TRIANGLE(type,v0,v1,v2) do { \
	zprim=VIEW_Z(v0); \
	switch(type) { \
		case BR_SORT_MIN: \
			if(VIEW_Z(v1) < zprim) zprim = VIEW_Z(v1); \
			if(VIEW_Z(v2) < zprim) zprim = VIEW_Z(v2); \
			break; \
		case BR_SORT_MAX: \
			if(VIEW_Z(v1)>zprim) zprim=VIEW_Z(v1); \
			if(VIEW_Z(v2)>zprim) zprim=VIEW_Z(v2); \
			break; \
		case BR_SORT_AVERAGE: \
			zprim=BR_ADD(zprim,BR_ADD(VIEW_Z(v1),VIEW_Z(v2))); \
			zprim=BR_MUL(zprim,BR_SCALAR(1.0/3.0)); \
			break; \
		case BR_SORT_FIRST_VERTEX: \
			break; \
	} \
} while(0)

/*
 * Calculate the sort value for an edge
 */
#define SORT_VALUE_EDGE(type,z0,z1) do { \
	zprim=VIEW_Z(v0); \
	switch(type) { \
		case BR_SORT_MIN: \
			if(VIEW_Z(v1)<zprim) zprim=VIEW_Z(v1); \
			break; \
		case BR_SORT_MAX: \
			if(VIEW_Z(v1)>zprim) zprim=VIEW_Z(v1); \
			break; \
		case BR_SORT_AVERAGE: \
			zprim=BR_ADD(zprim,VIEW_Z(v1)); \
			zprim=BR_MUL(zprim,BR_SCALAR(1.0/2.0)); \
			break; \
		case BR_SORT_FIRST_VERTEX: \
			break; \
	} \
} while(0)

#define SORT_VALUE_POINT(type,z0) zprim = VIEW_Z(v0)

/*
 * Insert a primitive at the head of the first bucket
 */
#define INSERT_PRIMITIVE_FIRST_BUCKET(ot,prim,depth) do {	\
	(prim)->next=(ot)->table[0];							\
	(ot)->table[0]=(prim);									\
} while(0)

/*
 * Insert a primitive at the head of the correct bucket
 */
#define INSERT_PRIMITIVE_HEAD(ot,prim,depth) do {				\
	int bucket;													\
	(depth)=BR_SUB((depth),(ot)->min_z);						\
	if((depth)<BR_SCALAR(0.0)) {								\
		bucket = 0;												\
	} else {													\
		bucket = BrScalarToInt(BR_MUL((ot)->scale,(depth)));	\
		if(bucket >= (ot)->size)								\
			bucket = (ot)->size-1;								\
	}															\
	(prim)->next=(ot)->table[bucket];							\
	(ot)->table[bucket]=(prim);									\
} while(0)

/*
 * Insert a primitive into the current order table
 */
#define INSERT_PRIMITIVE(ot, prim, depth) do {			\
	if((ot)->flags & BR_ORDER_TABLE_CONVEX)				\
		INSERT_PRIMITIVE_FIRST_BUCKET(ot,prim,depth);	\
	else												\
		INSERT_PRIMITIVE_HEAD(ot,prim,depth);			\
} while(0)

#ifdef __cplusplus
};
#endif
#endif

