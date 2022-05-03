/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: otable.c 1.1 1997/12/10 16:41:32 jon Exp $
 * $Locker: $
 *
 * order table support code
 */
#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"

#include "math_ip.h"
#include "zsmacro.h"

BR_RCS_ID("$Id: otable.c 1.1 1997/12/10 16:41:32 jon Exp $")

/*
 * Smallest possible distance between Z bounds
 */
#define MINIMUM_RANGE 0.001

/*
 * Allocate an order table
 */
br_order_table * BR_PUBLIC_ENTRY BrZsOrderTableAllocate(br_uint_16 size,br_uint_32 flags,br_uint_16 type)
{
	br_order_table *order_table;

	UASSERT_MESSAGE("Number of buckets are <= 0", size>0);

	/*
	 * Allocate memory for order table structure
	 */
	order_table=BrResAllocate(v1db.res,sizeof(br_order_table),BR_MEMORY_ORDER_TABLE);

	/*
	 * Allocate memory for bucket pointer table
	 */
	order_table->table=BrResAllocate(order_table,size*sizeof(br_primitive *),BR_MEMORY_ORDER_TABLE);

	/*
	 * Fill in other order table members
	 */
	order_table->size=size;
	order_table->next=NULL;
	order_table->flags=flags;
	order_table->type=type;
	order_table->visits=0;
	order_table->min_z=BR_SCALAR(0.0);
	order_table->max_z=BR_SCALAR(0.0);
	order_table->sort_z=BR_SCALAR(0.0);
	order_table->scale=BR_SCALAR(0.0);

	return order_table;
}

/*
 * Free an order table
 */
void BR_PUBLIC_ENTRY BrZsOrderTableFree(br_order_table *order_table)
{
    UASSERT_MESSAGE("BrZsOrderTableFree NULL pointer to an order table", order_table!=NULL);

	BrResFree(order_table);
}

/*
 * Set an actor's order table
 */
br_order_table * BR_PUBLIC_ENTRY BrZsActorOrderTableSet(br_actor *actor, br_order_table *order_table)
{
	UASSERT(actor!=NULL);

	actor->render_data=(void *)order_table;

	return order_table;
}


/*
 * Get a pointer to an actor's order table
 */
br_order_table * BR_PUBLIC_ENTRY BrZsActorOrderTableGet(br_actor *actor)
{
	UASSERT(actor!=NULL);

	return (br_order_table *)actor->render_data;
}


/*
 * Clear an order table
 */
br_order_table * BR_PUBLIC_ENTRY BrZsOrderTableClear(br_order_table *order_table)
{
    UASSERT(order_table!=NULL);

	BrMemSet(order_table->table,0,order_table->size*sizeof(br_primitive *));

	return order_table;
}

/*
 * Insert a primitive into an order table
 */
void BR_PUBLIC_ENTRY BrZsOrderTablePrimitiveInsert(br_order_table *order_table,
												   br_primitive *primitive,
												   br_uint_16 bucket)
{
	UASSERT(primitive!=NULL);
	UASSERT(order_table!=NULL);
	UASSERT(bucket<order_table->size);

	primitive->next=order_table->table[bucket];
	order_table->table[bucket]=primitive;
}

/*
 * Select a bucket, given vertices, bounds, size and sort type
 */
br_uint_16 BR_PUBLIC_ENTRY BrZsPrimitiveBucketSelect(br_scalar *z,
													 br_uint_16 type,
													 br_scalar min_z,
													 br_scalar max_z,
													 br_uint_16 size,
													 br_uint_16 sort_type)
{
	br_uint_16 bucket;
	br_scalar zprim,range,scale;

	UASSERT(size>0);
    UASSERT_MESSAGE("BrZsPrimitiveBucketSelect NULL pointer", z != NULL);

	/*
 	 * Determine sort value
	 */
	switch(type) {

		case BR_PRIMITIVE_TRIANGLE:
			SORT_VALUE_TRIANGLE(sort_type,z[0],z[1],z[2]);
			break;

		case BR_PRIMITIVE_LINE:
			SORT_VALUE_EDGE(sort_type,z[0],z[1]);
			break;

		case BR_PRIMITIVE_POINT:
			zprim=z[0];
			break;

	}

	/*
	 * Calculate Z scale factor
	 */
    UASSERT_MESSAGE("Divide by zero error, max_z - min_z = 0", range != 0 );
	range=BR_SUB(max_z,min_z);
	if(range>BR_SCALAR(MINIMUM_RANGE)) scale=BR_DIV(BrIntToScalar(size),range);
	else scale=BR_SCALAR(1.0/(float)MINIMUM_RANGE);

	/*
	 * Select bucket and clamp to range
	 */
	zprim=BR_SUB(zprim,min_z);
	if(zprim<BR_SCALAR(0.0)) zprim=BR_SCALAR(0.0);
	bucket=BrScalarToInt(BR_MUL(scale,zprim));
	if(bucket>=size) bucket=size-1;

	return bucket;
}


/*
 * Enable the primary order table
 */
void BR_PUBLIC_ENTRY BrZsOrderTablePrimaryEnable(br_order_table *order_table)
{
	if(order_table) v1db.primary_order_table=order_table;
	else v1db.primary_order_table=v1db.default_order_table; 
}

/*
 * Disable the primary order table
 */
void BR_PUBLIC_ENTRY BrZsOrderTablePrimaryDisable(void)
{
	v1db.primary_order_table=NULL;
}

/*
 * Add an order table to the list of order tables
 */
void InsertOrderTableList(br_order_table *order_table)
{
	br_order_table *previous_table,*current_table;

	ASSERT(order_table!=NULL);

	/*
	 * Do not insert primary order table
	 */
	if(order_table==v1db.primary_order_table) return;

	if(v1db.order_table_list==NULL) {

		/*
		 * Add as the first order table
		 */
		v1db.order_table_list=order_table;
		order_table->next=NULL;

	} else {

		current_table=v1db.order_table_list;
		if(current_table->sort_z<order_table->sort_z) {

			/*
			 * Insert at head of list
			 */
			order_table->next=current_table;
			v1db.order_table_list=order_table;

		} else {

			/*
			 * Search for position
			 */
			previous_table=current_table;
			current_table=current_table->next;

			while(current_table!=NULL) {
				if(current_table->sort_z>=order_table->sort_z) {
					previous_table=current_table;
					current_table=current_table->next;
				} else break;
			}
		
			previous_table->next=order_table;
			order_table->next=current_table;

		}
	}
}


/*
 * Set order table bounds and Z scale factor given a source bounds
 */
void SetOrderTableBounds(br_bounds *bounds, br_order_table *order_table)
{
	br_uint_32 i;
	br_scalar range,element;
	br_scalar min_z,max_z;
	br_vector3 *min,*max;

	/*
	 * Set new bounds if necessary
	 */
	if((order_table->flags & BR_ORDER_TABLE_NEW_BOUNDS) ||
	  ((order_table->flags & BR_ORDER_TABLE_INIT_BOUNDS) && (order_table->visits==1))) {

		/*
		 * Calculate Z bounds in screen space by accumulating
		 * maximal and minimal products
		 */
		min=&bounds->min;
		max=&bounds->max;
		BrModelToScreenQuery(&v1db.model_to_screen);
		min_z=max_z=v1db.model_to_screen.m[3][3];

		for(i=0;i<3;i++) {
			element=v1db.model_to_screen.m[i][3];
			if(element>0) {
				max_z=BR_ADD(max_z,BR_MUL(element,max->v[i]));
				min_z=BR_ADD(min_z,BR_MUL(element,min->v[i]));
			}
			else {
				max_z=BR_ADD(max_z,BR_MUL(element,min->v[i]));
				min_z=BR_ADD(min_z,BR_MUL(element,max->v[i]));
			}
		}

		order_table->min_z=min_z;
		order_table->max_z=max_z;

	}

	SetOrderTableRange(order_table);
}

void SetOrderTableRange(br_order_table *order_table)
{
	br_scalar range=BR_SUB(order_table->max_z,order_table->min_z);

	/*
	 * Calculate Z scale factor
	 */
	if(range>BR_SCALAR(MINIMUM_RANGE))
		order_table->scale=BR_DIV(BrIntToScalar(order_table->size),range);
	else
		order_table->scale=BR_SCALAR(1.0/(float)MINIMUM_RANGE);

	/*
	 * Set order table sort value
	 */
        if((order_table->flags & BR_ORDER_TABLE_SORT_NEAR)!=0){
            order_table->sort_z=order_table->min_z;
        }else{
            if((order_table->flags & BR_ORDER_TABLE_SORT_FAR)!=0){
                order_table->sort_z=order_table->max_z;
            }else{
                if((order_table->flags & BR_ORDER_TABLE_SORT_CENTRE)!=0)
                    order_table->sort_z=BR_ADD(order_table->min_z,BR_MUL(range,BR_SCALAR(0.5)));
            }
        }
}

/*
 * Render primitives in the list of order tables
 */
void RenderOrderTableList(void)
{
	br_order_table *order_table;

	order_table=v1db.order_table_list;

	while(order_table!=NULL) {
		GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer, order_table->table, order_table->size);
		order_table->visits=0;
		order_table=order_table->next;

	}
}


/*
 * Render primitives in the root order table and
 * in the list of order tables
 */
void RenderPrimaryOrderTable(void)
{
	br_uint_16 m,size;
	br_scalar bucket_size;
	br_scalar min_z,max_z;
	br_primitive *prim,**bucket;
	br_order_table *order_table;

	ASSERT(v1db.primary_order_table);
	ASSERT(v1db.primary_order_table->size>0);

	/*
	 * If primary order table has never been visited,
	 * just render the list of order tables
	 */
	if(v1db.primary_order_table->visits==0) {
		RenderOrderTableList();
		return;
	}

	min_z=v1db.primary_order_table->min_z;
	max_z=v1db.primary_order_table->max_z;
	size=v1db.primary_order_table->size;
	bucket=v1db.primary_order_table->table+(size-1);

	/*
	 * Render order tables in list whose sort Z values
	 * are behind the far bound of the root order table
	 */
	order_table=v1db.order_table_list;
	while(order_table!=NULL) {

		if(order_table->sort_z < max_z) break;

		GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer, order_table->table,order_table->size);
		order_table->visits=0;
		order_table=order_table->next;

	}

	/*
	 * Render the root order table together with any order tables
	 * in the list whose sort Z values lie within the bounds of
	 * the root order table
	 */ 
	bucket_size=BR_DIV(BR_SUB(max_z,min_z),BrIntToScalar(size));

	for(m=0;m<size;m++,bucket--) {

		/*
		 * Render bucket
		 */
		if(*bucket)
			GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer,bucket,1);

		/*
		 * Render order tables whose sort Z values lie in this bucket
		 */
		max_z-=bucket_size;
		while(order_table!=NULL) {

			if(order_table->sort_z < max_z) break;

			GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer,order_table->table,order_table->size);
			order_table->visits=0;
			order_table=order_table->next;

		}

	}

	/*
	 * Render any remaining order tables in the list
	 */
	while(order_table!=NULL) {

		GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer,order_table->table,order_table->size);
		order_table->visits=0;
		order_table=order_table->next;

	}

	/*
	 * Reset visit count
	 */
	v1db.primary_order_table->visits=0;
}

