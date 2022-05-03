/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pool.h 1.1 1997/12/10 16:41:19 jon Exp $
 * $Locker: $
 *
 * Fixed size block pools
 */
#ifndef _POOL_H_
#define _POOL_H_

#define BR_POOL_DEBUG 0
#define BR_POOL_ALIGN 7

typedef struct br_pool_block {
		struct br_pool_block *next;
} br_pool_block;

typedef struct br_pool {
		br_pool_block *free;
		br_uint_32	block_size;
		br_uint_32	chunk_size;
		int			mem_type;
#if BR_POOL_DEBUG
		br_uint_32 max_count;
		br_uint_32 count;
#endif
} br_pool;


/*
 * Speedup macros
 */
#if 0
#if !POOL_DEBUG
br_pool_block *__bp; /* Hmm, this global is not optimizer friendly */

#define BrPoolAllocate(pool)\
	(void *)(((pool)->free?0:BrPoolAddChunk(pool)),\
	(__bp = (pool)->free),\
	((pool)->free = __bp->next,__bp))

#define BrPoolFree(pool,bp)\
	((bp)->next = (pool)->free,(pool)->free = (bp))
#endif
#endif

#endif



