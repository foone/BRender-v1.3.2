/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: heap.c 1.1 1997/12/10 16:52:07 jon Exp $
 * $Locker: $
 *
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "shortcut.h"
#include "brassert.h"

#include "vecifns.h"

#include "zsrmacro.h"

BR_RCS_ID("$Id: heap.c 1.1 1997/12/10 16:52:07 jon Exp $");

static br_boolean heapCheck(br_primitive_heap *heap, br_size_t s)
{
	return (heap->current + s) <= (heap->base + heap->size);
}

static br_primitive *heapPrimitiveAdd(br_primitive_heap *heap, br_token type)
{
	br_primitive *p;

	p = (br_primitive *)heap->current;

	heap->current += sizeof(br_primitive);

	p->type = type;
	p->stored = rend.renderer->last_restored;

	return p;
}

static brp_vertex *heapVertexAdd(br_primitive_heap *heap, brp_vertex *src, br_boolean share)
{
	brp_vertex *v;
	int n;

	if(share) {
		/*
		 * If src is one of the original temp_vertices, see if there is
		 * already a copy in the heap
		 */
		if(src >= rend.temp_vertices) {
			n = src - rend.temp_vertices;
			if(n < rend.nvertices) {
				if(rend.vertex_heap_pointers[n] == NULL) {
					/*
					 * First time this temp has been seen, add it to
					 * head and remember a pointer to it
					 */
					rend.vertex_heap_pointers[n] = (brp_vertex *)heap->current;
					*rend.vertex_heap_pointers[n] = *src;
					heap->current += sizeof(brp_vertex);
				}
				return rend.vertex_heap_pointers[n];
			}
		}
	}

	v = (brp_vertex *)heap->current;
	*v = *src;
	heap->current += sizeof(brp_vertex);

	return v;
}

void BR_ASM_CALL OpHeapAddTriangle(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	br_primitive *p;
	br_scalar zprim;

	if(!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front) {
		zprim = BR_SCALAR(0.0);
	}else{
		if(rend.renderer->state.surface.force_back){
			zprim = BR_SCALAR_MAX;
		}else{
			SORT_VALUE_TRIANGLE(rend.renderer->state.hidden.order_table->type,v0,v1,v2);
		}
	}

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_TRIANGLE);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, rend.renderer->state.cache.share_vertex_0);
	p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, v1, rend.renderer->state.cache.share_other_vertices);
	p->v[2] = heapVertexAdd(rend.renderer->state.hidden.heap, v2, rend.renderer->state.cache.share_other_vertices);

	/*
	 * Insert into current table
	 */
	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[3];
		z[0] = VIEW_Z(v0);
		z[1] = VIEW_Z(v1);
		z[2] = VIEW_Z(v2);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);

	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

void BR_ASM_CALL OpHeapAddLine(struct brp_block *block, brp_vertex *v0, brp_vertex *v1)
{
	br_primitive *p;
	br_scalar zprim;

	/*
	 * Build primitive in heap
	 */
	if(!heapCheck(rend.renderer->state.hidden.heap,sizeof(br_primitive) + 2 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front){
		zprim = BR_SCALAR(0.0);
	} else {
		if(rend.renderer->state.surface.force_back){
			zprim = BR_SCALAR_MAX;
		} else {
			SORT_VALUE_EDGE(rend.renderer->state.hidden.order_table->type,v0,v1);
		}
	}

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_LINE);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, rend.renderer->state.cache.share_vertex_0);
	p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, v1, rend.renderer->state.cache.share_other_vertices);

	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[2];
		z[0] = VIEW_Z(v0);
		z[1] = VIEW_Z(v1);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);
	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

void BR_ASM_CALL OpHeapAddPoint(struct brp_block *block, brp_vertex *v0)
{
	br_primitive *p;
	br_scalar zprim;

	/*
	 * Build primitive in heap
	 */
	if(!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front){
		zprim = BR_SCALAR(0.0);
	} else {
		if(rend.renderer->state.surface.force_back) {
			zprim = BR_SCALAR_MAX;
		} else {
			SORT_VALUE_POINT(rend.renderer->state.hidden.order_table->type,v0);
		}
	}

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_POINT);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, v0, rend.renderer->state.cache.share_vertex_0);

	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[1];
		z[0] = VIEW_Z(v0);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);

	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

/*
 * Version of above that also convert components to destination types
 */
void BR_ASM_CALL OpHeapAddTriangleConvert(struct brp_block *block, brp_vertex *v0, brp_vertex *v1, brp_vertex *v2)
{
	br_primitive *p;
	br_scalar zprim;
	brp_vertex outv[3];

	if(!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 3 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front) {
		zprim = BR_SCALAR(0.0);
	} else {
		if(rend.renderer->state.surface.force_back) {
			zprim = BR_SCALAR_MAX;
		} else {
			SORT_VALUE_TRIANGLE(rend.renderer->state.hidden.order_table->type,v0,v1,v2);
		}
	}

	ConvertVertex(outv+0, v0);
	ConvertVertex(outv+1, v1);
	ConvertVertex(outv+2, v2);

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_TRIANGLE);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+0,0);
	p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+1,0);
	p->v[2] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+2,0);


	/*
	 * Insert into current table
	 */
	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[3];
		z[0] = VIEW_Z(v0);
		z[1] = VIEW_Z(v1);
		z[2] = VIEW_Z(v2);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);

	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

void BR_ASM_CALL OpHeapAddLineConvert(struct brp_block *block, brp_vertex *v0, brp_vertex *v1)
{
	br_primitive *p;
	br_scalar zprim;
	brp_vertex outv[2];

	/*
	 * Build primitive in heap
	 */
	if(!heapCheck(rend.renderer->state.hidden.heap,sizeof(br_primitive) + 2 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front) {
		zprim = BR_SCALAR(0.0);
	} else {
		if(rend.renderer->state.surface.force_back) {
			zprim = BR_SCALAR_MAX;
		} else {
			SORT_VALUE_EDGE(rend.renderer->state.hidden.order_table->type,v0,v1);
		}
	}

	ConvertVertex(outv+0, v0);
	ConvertVertex(outv+1, v1);

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_LINE);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+0,0);
	p->v[1] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+1,0);

	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[2];
		z[0] = VIEW_Z(v0);
		z[1] = VIEW_Z(v1);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);
	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

void BR_ASM_CALL OpHeapAddPointConvert(struct brp_block *block, brp_vertex *v0)
{
	br_primitive *p;
	br_scalar zprim;
	brp_vertex outv[1];

	/*
	 * Build primitive in heap
	 */
	if(!heapCheck(rend.renderer->state.hidden.heap, sizeof(br_primitive) + 1 * sizeof(brp_vertex)))
#if DEBUG
        BR_ERROR0("Primitive heap space overflow");
#else
		return;
#endif

	if(rend.renderer->state.surface.force_front) {
		zprim = BR_SCALAR(0.0);
	} else {
		if(rend.renderer->state.surface.force_back) {
			zprim = BR_SCALAR_MAX;
		} else {
			SORT_VALUE_POINT(rend.renderer->state.hidden.order_table->type,v0);
		}
	}

	ConvertVertex(outv+0, v0);

	/*
	 * Build primitive in heap
	 */
	p = heapPrimitiveAdd(rend.renderer->state.hidden.heap, BRT_POINT);
	p->v[0] = heapVertexAdd(rend.renderer->state.hidden.heap, outv+0, 0);

	if(rend.renderer->state.hidden.insert_fn) {
		br_scalar z[1];
		z[0] = VIEW_Z(v0);

		rend.renderer->state.hidden.insert_fn(p, 
			rend.renderer->state.hidden.insert_arg1,
			rend.renderer->state.hidden.insert_arg2,
			rend.renderer->state.hidden.insert_arg3,
			rend.renderer->state.hidden.order_table,z);

	} else {
		INSERT_PRIMITIVE(rend.renderer->state.hidden.order_table, p, zprim);
	}
}

