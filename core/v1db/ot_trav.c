//#include <brender.h>

#include "v1db.h"
#include "shortcut.h"
#include "brassert.h"
#include "datafile.h"
#include "priminfo.h"


#if defined(__WATCOMC__)
#define __inline
#endif

static zs_order_table_traversal_cbfn *callback;


static __inline br_error GeometryV1BucketsTraverse(struct br_geometry *self, struct br_renderer *renderer,struct br_primitive **buckets, br_int_32 nbuckets)
{
	int i;
	struct br_primitive *p;
	struct br_renderer_state_stored *last_state;
	br_error r;
	br_token last_type;
	ot_vertex v[3];
	
	
	if(nbuckets <= 0)
		return BRE_OK;

	/*
	 * Use 'self' as an object handle that will never
	 * be a stored object
	 */
//	last_state = (br_renderer_state_stored *)self;
//	last_type = BR_NULL_TOKEN;

//	rend.block = NULL;

	/*
	 * Render bucket table from last to first
	 */
	for(buckets += nbuckets-1; nbuckets--; buckets--) {
		for(p = *buckets; p ; p = p->next) {
			/*
			 * Reload state for primitive, if it has changed
			 */
#if 0
			if(p->stored != last_state || p->type != last_type) {

				if(rend.block)
					PrimitiveStateRenderEnd(renderer->state.pstate, rend.block);
	
				if(p->stored)
					RendererStateRestore(renderer, p->stored, BR_STATE_PRIMITIVE | BR_STATE_CACHE);

				/*
				 * Reference the primtive block
				 */
				r = PrimitiveStateRenderBegin(renderer->state.pstate,
					&rend.block, &rend.block_changed, &rend.range_changed, BR_FALSE, p->type);

				last_type = p->type;
				last_state = p->stored;

				if(r != BRE_OK)
					continue;
			}
#endif
			/*
			 * Render the primitive
			 */
//			rend.block->render(rend.block,p->v[0],p->v[1],p->v[2]);

			ASSERT((p->type==BR_PRIMITIVE_POINT)||(p->type==BR_PRIMITIVE_LINE)||(p->type==BR_PRIMITIVE_TRIANGLE));

			switch(p->type){			
				case BR_PRIMITIVE_TRIANGLE:
#if BASED_FIXED
					v[2].screenX=((brp_vertex*)p->v[2])->comp_x[C_SX]-BR_SCALAR(0.5);
					v[2].screenY=((brp_vertex*)p->v[2])->comp_x[C_SY]-BR_SCALAR(0.5);
					v[2].distanceZ=((brp_vertex*)p->v[2])->comp_x[C_W];
#else
					v[2].screenX=((brp_vertex*)p->v[2])->comp_f[C_SX]-BR_SCALAR(0.5);
					v[2].screenY=((brp_vertex*)p->v[2])->comp_f[C_SY]-BR_SCALAR(0.5);
					v[2].distanceZ=((brp_vertex*)p->v[2])->comp_f[C_W];
#endif
#if BASED_FIXED
					v[1].screenX=((brp_vertex*)p->v[1])->comp_x[C_SX]-BR_SCALAR(0.5);
					v[1].screenY=((brp_vertex*)p->v[1])->comp_x[C_SY]-BR_SCALAR(0.5);
					v[1].distanceZ=((brp_vertex*)p->v[1])->comp_x[C_W];
#else
					v[1].screenX=((brp_vertex*)p->v[1])->comp_f[C_SX]-BR_SCALAR(0.5);
					v[1].screenY=((brp_vertex*)p->v[1])->comp_f[C_SY]-BR_SCALAR(0.5);
					v[1].distanceZ=((brp_vertex*)p->v[1])->comp_f[C_W];
#endif			
#if BASED_FIXED
					v[0].screenX=((brp_vertex*)p->v[0])->comp_x[C_SX]-BR_SCALAR(0.5);
					v[0].screenY=((brp_vertex*)p->v[0])->comp_x[C_SY]-BR_SCALAR(0.5);
					v[0].distanceZ=((brp_vertex*)p->v[0])->comp_x[C_W];
#else		
					v[0].screenX=((brp_vertex*)p->v[0])->comp_f[C_SX]-BR_SCALAR(0.5);
					v[0].screenY=((brp_vertex*)p->v[0])->comp_f[C_SY]-BR_SCALAR(0.5);
					v[0].distanceZ=((brp_vertex*)p->v[0])->comp_f[C_W];
#endif
					break;
				case BR_PRIMITIVE_LINE:
					v[1].screenX=BrFixedToScalar(((brp_vertex*)p->v[1])->comp_x[C_SX]-0x8000);
					v[1].screenY=BrFixedToScalar(((brp_vertex*)p->v[1])->comp_x[C_SY]-0x8000);
#if BASED_FIXED
					v[1].distanceZ=BrFixedToScalar(((brp_vertex*)p->v[1])->comp_x[C_W]);
#else
					v[1].distanceZ=((brp_vertex*)p->v[1])->comp_f[C_W];
#endif 
#if 0
					v[1].screenX=((brp_vertex*)p->v[1])->comp_f[C_SX]-0x8000;
					v[1].screenY=((brp_vertex*)p->v[1])->comp_f[C_SY]-0x8000;
					v[1].distanceZ=((brp_vertex*)p->v[1])->comp_f[C_W];
#endif			
				case BR_PRIMITIVE_POINT:
					v[0].screenX=BrFixedToScalar(((brp_vertex*)p->v[0])->comp_x[C_SX]-0x8000);
					v[0].screenY=BrFixedToScalar(((brp_vertex*)p->v[0])->comp_x[C_SY]-0x8000);
#if BASED_FIXED
					v[0].distanceZ=BrFixedToScalar(((brp_vertex*)p->v[0])->comp_x[C_W]);
#else
					v[0].distanceZ=((brp_vertex*)p->v[0])->comp_f[C_W];
#endif
#if 0
					v[0].screenX=((brp_vertex*)p->v[0])->comp_f[C_SX]-0x8000;
					v[0].screenY=((brp_vertex*)p->v[0])->comp_f[C_SY]-0x8000;
					v[0].distanceZ=((brp_vertex*)p->v[0])->comp_f[C_W];
#endif
			}
			callback(p->type,&v[0],&v[1],&v[2]);

		}
	}

//	if(rend.block)
//		PrimitiveStateRenderEnd(renderer->state.pstate, rend.block);
	
	return BRE_FAIL;
}




static __inline void TraverseOrderTableList(void)
{
	br_order_table *order_table;

	order_table=v1db.order_table_list;

	while(order_table!=NULL) {
		GeometryV1BucketsTraverse(v1db.format_buckets,v1db.renderer, order_table->table, order_table->size);
//		order_table->visits=0;
		order_table=order_table->next;

	}
}


static __inline void TraversePrimaryOrderTable(void)
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
		TraverseOrderTableList();
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

		GeometryV1BucketsTraverse(v1db.format_buckets,v1db.renderer, order_table->table,order_table->size);
//		order_table->visits=0;
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
			GeometryV1BucketsTraverse(v1db.format_buckets,v1db.renderer,bucket,1);

		/*
		 * Render order tables whose sort Z values lie in this bucket
		 */
		max_z-=bucket_size;
		while(order_table!=NULL) {

			if(order_table->sort_z < max_z) break;

			GeometryV1BucketsTraverse(v1db.format_buckets,v1db.renderer,order_table->table,order_table->size);
//			order_table->visits=0;
			order_table=order_table->next;

		}

	}

	/*
	 * Render any remaining order tables in the list
	 */
	while(order_table!=NULL) {

		GeometryV1BucketsRender(v1db.format_buckets,v1db.renderer,order_table->table,order_table->size);
//		order_table->visits=0;
		order_table=order_table->next;

	}

	/*
	 * Reset visit count
	 */
//	v1db.primary_order_table->visits=0;
}


void BR_PUBLIC_ENTRY ZsOrderTableTraversal(zs_order_table_traversal_cbfn *cbfn)
{

//	RendererPartSet(v1db.renderer, BRT_OUTPUT, 0, BRT_COLOUR_BUFFER_O, (br_uint_32) v1db.colour_buffer);

	callback=cbfn;
	
	if(v1db.format_buckets == NULL)
		BR_ERROR0("Renderer does not support buckets");

	if(v1db.primary_order_table) {

		/*
		 * Render primitives in the primary order table
		 * and the list of order tables
		 */
		TraversePrimaryOrderTable();
	} else {
		/*
		 * Render primitives in the list of order table
		 */
		TraverseOrderTableList();
	}

	/*
	 * Tell the renderer to flush
	 */
//	RendererFlush(v1db.renderer, BR_FALSE);

//	v1db.render_root = NULL;
}
