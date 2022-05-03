/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: prepmesh.c 1.10 1998/10/22 18:41:15 johng Exp $
 * $Locker: $
 *
 * Precompute information for triangle meshes:
 *	Face normals
 *	Vertex normals
 *	Edge numbers
 *	Bounding radius
 */

#include "v1db.h"
#include "vecifns.h"
#include "brassert.h"
#include "shortcut.h"
#include "brmath.h"

#include "formats.h"

#define MODU_FACE_COPY_FLAGS \
	(BR_MODU_FACE_COLOURS|\
	BR_MODU_FACE_EQUATIONS)

#define MODU_VERTEX_COPY_FLAGS \
	(BR_MODU_VERTEX_POSITIONS|\
	BR_MODU_VERTEX_COLOURS|\
	BR_MODU_VERTEX_MAPPING|\
	BR_MODU_VERTEX_NORMALS)


BR_RCS_ID("$Id: prepmesh.c 1.10 1998/10/22 18:41:15 johng Exp $")

/*
 * Temporary structure used whilst processing normals and groups
 */
struct prep_vertex {
			br_vector3 n;
			br_uint_16 v;
			br_uint_16 f;
};


/*
 * Alignment for block in prepared area
 */
#define PREP_BOUNDARY 32
#define PREP_ALIGN(x) (((x)+(PREP_BOUNDARY-1)) & ~(PREP_BOUNDARY-1))

/*
 * Scratch space for edges - the temporary edge structures are hashed
 * on their first vertex - all similar vertices are the singly linked in
 * a chain
 */
static struct pm_temp_edge {
	struct pm_temp_edge  *next; /* next in chain */
	br_uint_16 first;		/* First vertex */
	br_uint_16 last;			/* Last Vertex */
	char other;			/* Edge is used in other direction */
};

static struct pm_temp_edge *pm_edge_table;
static struct pm_temp_edge **pm_edge_hash;

static char *pm_edge_scratch;
static int num_edges = 0;

static int addEdge(br_uint_16 first, br_uint_16 last)
{
	struct pm_temp_edge *tep;

	/*
	 * See if edge exists and can be used in other direction
	 */
	for(tep = pm_edge_hash[last]; tep; tep = tep->next) {

		if(tep->last == first && tep->other == 0) { 
			/*
			 * Yup, flag as used and return index
			 */
			tep->other = 1;
			return tep - pm_edge_table;
		}
	}

	/*
	 * Create new edge
	 */
	tep = pm_edge_table + num_edges;

	tep->first = first;
	tep->last = last;
	tep->other = 0;
	tep->next = pm_edge_hash[first];
	pm_edge_hash[first] = tep;
	
	return num_edges++;
}


/*
 * Build edge references for a group
 */
static void prepareEdges(struct v11group *group, br_model *model)
{
	br_size_t scratch_size;
	br_face *mfp;
//	struct v11face *fp;
	br_vector3_u16 *vertex_numbers;
	br_vector3_u16 *edges;
//	br_vector4 * plane_equation;

	int f;

	scratch_size =
		group->nfaces * 3 * sizeof(*pm_edge_table) +
		group->nvertices * sizeof(*pm_edge_hash);

	pm_edge_scratch = BrScratchAllocate(scratch_size);

	/*
	 * Divvy up scratch area and clear hash pointers
	 */
	pm_edge_hash = (struct pm_temp_edge **)pm_edge_scratch;
	pm_edge_table = (struct pm_temp_edge *)
		(pm_edge_scratch+group->nvertices * sizeof(*pm_edge_hash));

	BrMemSet(pm_edge_hash, 0, group->nvertices * sizeof(*pm_edge_hash));

	/*
	 * Accumulate shared edges from each face
	 */
	num_edges = 0;
//	fp = group->faces;
	vertex_numbers=group->vertex_numbers;
	edges=group->edges;

	for(f = 0; f < group->nfaces; f++, vertex_numbers++,edges++) {
        /*
         * Get a pointer back to original model face
         */
		mfp = model->faces + group->face_user[f];

		if(!(mfp->flags & BR_FACEF_COPLANAR_0))
			edges->v[0] = addEdge(vertex_numbers->v[0],vertex_numbers->v[1])+1;
		else 
			edges->v[0] = 0;

		if(!(mfp->flags & BR_FACEF_COPLANAR_1))
			edges->v[1] = addEdge(vertex_numbers->v[1],vertex_numbers->v[2])+1;
		else 
			edges->v[1] = 0;

		if(!(mfp->flags & BR_FACEF_COPLANAR_2))
			edges->v[2] = addEdge(vertex_numbers->v[2],vertex_numbers->v[0])+1;
		else 
			edges->v[2] = 0;
	}

	group->nedges = num_edges+1;

	BrScratchFree(pm_edge_scratch);
}

/*
 * Build per-face edge indices for each group in a model
 */
void BrPrepareEdges(br_model *model)
{
	int g;
	struct v11model *v11m;

    /*
     * Do nothing if model has not been prepared
     */
	if(model->prepared)
		v11m = model->prepared;
	else
		return;

	for(g=0; g < v11m->ngroups; g++)
		prepareEdges(v11m->groups+g, model);
}

/*
 * Comparison function for qsorting pointers to faces
 */
static int BR_CALLBACK FacesCompare(const void *p1, const void *p2)
{
	const br_face *f1 = *(br_face **)p1, *f2 = *(br_face **)p2;

	if(f1->material > f2->material)
		return 1;
	if(f1->material < f2->material)
		return -1;

	return 0;
}

/*
 * Compare temp vertices by  X,Y,Z
 */
static br_model *compareModel;

static int BR_CALLBACK TVCompare_XYZ(const void *p1, const void *p2)
{
	const struct prep_vertex *tv1 = *(struct prep_vertex **)p1;
	const struct prep_vertex *tv2 = *(struct prep_vertex **)p2;
	struct br_vertex *v1,*v2;
	int i;

	ASSERT(compareModel != NULL);

	if(tv1->v == tv2->v)
		return 0;

	v1 = compareModel->vertices+tv1->v;
	v2 = compareModel->vertices+tv2->v;

	for(i =0 ; i < 3; i++) {
		if(v1->p.v[i] > v2->p.v[i])
			return 1;
		if(v1->p.v[i] < v2->p.v[i])
			return -1;
	}

	return 0;
}

/*
 * Compare temp vertices by Material, X,Y,Z,U,V Normal
 */

static int BR_CALLBACK TVCompare_MXYZUVN(const void *p1, const void *p2)
{
	const struct prep_vertex *tv1 = *(struct prep_vertex **)p1;
	const struct prep_vertex *tv2 = *(struct prep_vertex **)p2;
	struct br_vertex *v1,*v2;
	int i;

	ASSERT(compareModel != NULL);

	/*
	 * Compare material
	 */
	if(compareModel->faces[tv1->f].material > compareModel->faces[tv2->f].material)
		return 1;
	if(compareModel->faces[tv1->f].material < compareModel->faces[tv2->f].material)
		return -1;

	if(tv1->v != tv2->v) {
		v1 = compareModel->vertices+tv1->v;
		v2 = compareModel->vertices+tv2->v;

		/*
		 * Compare X,Y,Z
		 */
		for(i =0 ; i < 3; i++) {
			if(v1->p.v[i] > v2->p.v[i])
				return 1;
			if(v1->p.v[i] < v2->p.v[i])
				return -1;
		}

		/*
		 * Compare U,V
		 */
		for(i =0 ; i < 2; i++) {
			if(v1->map.v[i] > v2->map.v[i])
				return 1;
			if(v1->map.v[i] < v2->map.v[i])
				return -1;
		}
	}

	/*
	 * Compare Normal
	 */
	for(i =0 ; i < 3; i++) {
		if(tv1->n.v[i] > tv2->n.v[i])
			return 1;
		if(tv1->n.v[i] < tv2->n.v[i])
			return -1;
	}

	return 0;
}

/*
 * Compare temp vertices by material, vertex pointer, Normal
 */
static int BR_CALLBACK TVCompare_MVN(const void *p1, const void *p2)
{
	const struct prep_vertex *tv1 = *(struct prep_vertex **)p1;
	const struct prep_vertex *tv2 = *(struct prep_vertex **)p2;
	int i;

	ASSERT(compareModel != NULL);

   	/*
	 * Compare Material
	 */
	if(compareModel->faces[tv1->f].material > compareModel->faces[tv2->f].material)
		return 1;
	if(compareModel->faces[tv1->f].material < compareModel->faces[tv2->f].material)
		return -1;

	if(tv1->v > tv2->v)
		return 1;
	if(tv1->v < tv2->v)
		return -1;

	/*
	 * Compare Normal
	 */
	for(i =0 ; i < 3; i++) {
		if(tv1->n.v[i] > tv2->n.v[i])
			return 1;
		if(tv1->n.v[i] < tv2->n.v[i])
			return -1;
	}

	return 0;
}

/*
 * Convert a scalar to a fraction, clamping the result to the range (-1,1]
 */
br_fraction BrScalarToFractionClamp(br_scalar s)
{
	if(s <= BR_SCALAR(-1.0))
		return BR_FRACTION(-1.0)+BR_SCALAR_EPSILON;
	else if(s >= BR_SCALAR(1.0))
		return BR_FRACTION(1.0);
	else
		return BrScalarToFraction(s);
}

/*
 * Fill in face normals for all faces
 */
static void PrepareFaceNormals(br_model *model)
{
	br_vertex *vertices = model->vertices;
	br_vector4 v4;
	br_face *fp;
	int f;

	ASSERT(model->faces && model->vertices);

	fp = model->faces;
	for(f=0; f< model->nfaces; f++, fp++) {

		BrPlaneEquation(&v4,
				&vertices[fp->vertices[0]].p,
				&vertices[fp->vertices[1]].p,
				&vertices[fp->vertices[2]].p);

		fp->n.v[0] = BrScalarToFractionClamp(v4.v[0]);
		fp->n.v[1] = BrScalarToFractionClamp(v4.v[1]);
		fp->n.v[2] = BrScalarToFractionClamp(v4.v[2]);
		fp->d = v4.v[3];
	}
}

/*
 * Given a group of temp_vertices that are deemed to be co-incident,
 * accumulate the normals for each vertex by adding all
 * other normals from faces that share at least 1 smoothing group
 */
static void Smoothing(br_model *model, br_scalar crease_limit,
	struct prep_vertex **start, struct prep_vertex **end)
{
	struct prep_vertex **outer, **inner;

	for(outer = start ; outer < end; outer++) {
		for(inner = start ; inner < end; inner++) {
			if((inner == outer) || (model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing)) {
				BrVector3Accumulate(&(*outer)->n, &model->faces[(*inner)->f].n);
			}
		}
	}
}

/*
 * Similar as above, but also applies a crease angle between faces
 */
static void SmoothingCreased(br_model *model, br_scalar crease_limit,
	struct prep_vertex **start, struct prep_vertex **end)
{
	br_vector3 o_n;
	struct prep_vertex **outer, **inner;

	for(outer = start ; outer < end; outer++) {
		BrVector3CopyF(&o_n, &model->faces[(*outer)->f].n);
		for(inner = start ; inner < end; inner++) {
			if((inner == outer) || ((model->faces[(*outer)->f].smoothing & model->faces[(*inner)->f].smoothing) &&
                                BrFVector3Dot(&model->faces[(*inner)->f].n, &o_n) > crease_limit)) {
                                        BrVector3Accumulate(&(*outer)->n, &model->faces[(*inner)->f].n);
			}
		}
	}
}

/*
 * Copies info. from a br_vertex to fmt_vertex
 */
static void CopyVertex(struct v11group *group, int v, struct prep_vertex *src, br_model *model)
{
	br_vertex *srcv = model->vertices + src->v;
	br_vector3 temp;

	group->position[v].v[0] = srcv->p.v[0] - model->pivot.v[0];
	group->position[v].v[1] = srcv->p.v[1] - model->pivot.v[1];
	group->position[v].v[2] = srcv->p.v[2] - model->pivot.v[2];

	group->map[v] = srcv->map;

	BrVector3Normalise(&group->normal[v], &src->n);

	group->vertex_colours[v] = BR_COLOUR_RGBA(srcv->red, srcv->grn, srcv->blu, srcv->index);

	group->vertex_user[v] = src->v;
}

/*
 * Copies info. from a br_face to v11face
 */
static void CopyFace(struct v11group *group, int f, br_face *src, br_model *model)
{
	group->vertex_numbers[f].v[0] = src->vertices[0];
	group->vertex_numbers[f].v[1] = src->vertices[1];
	group->vertex_numbers[f].v[2] = src->vertices[2];

	group->face_colours[f] = BR_COLOUR_RGBA(src->red, src->grn, src->blu, src->index);

	group->eqn[f].v[0] = BrFractionToScalar(src->n.v[0]);
	group->eqn[f].v[1] = BrFractionToScalar(src->n.v[1]);
	group->eqn[f].v[2] = BrFractionToScalar(src->n.v[2]);
	group->eqn[f].v[3] = src->d;

	group->face_flags[f] = src->flags;

	group->face_user[f] = src - model->faces;
}

/*
 * Convert user's face+vertex meshes into internal face & vertex meshes
 */
static void PrepareGroups(br_model *model)
{
	br_qsort_cbfn *vertex_compare_smoothing;
	br_qsort_cbfn *vertex_compare_groups;
	void (*smoothing_fn)(br_model *, br_scalar,struct prep_vertex **, struct prep_vertex **);
	br_size_t block_size;
	struct prep_vertex *temp_verts, *gtvp, **sorted_vertices;
	struct br_face *fp;
	struct br_vertex *vp;
	int g,f,v,i,ntemps,count,nf,nv,ng, old_count;
	br_scalar crease_limit;
	struct v11model *v11m;
	struct v11group *v11g;
//	struct v11face *v11f;
	br_vector3_u16 *vertex_numbers;
	br_vector3_u16 *edges;
	br_vector4 *eqn;
//	struct fmt_vertex *v11v;
	br_vector3 *position;
	br_vector2 *map;
	br_vector3 *normal;
	br_fvector3 last_n;

    br_colour *v11fcolours, *v11vcolours;
    br_uint_16 *v11fuser, *v11vuser;
    br_uint_8 *v11fflags;
	struct br_face **sorted_faces;
	char *cp;

	/*
	 * Select sorting functions
	 */
	if(model->flags & BR_MODF_DONT_WELD) {
		vertex_compare_smoothing = TVCompare_XYZ;
		vertex_compare_groups = TVCompare_MVN;
	} else {
		vertex_compare_smoothing = TVCompare_XYZ;
		vertex_compare_groups = TVCompare_MXYZUVN;
	}

	/*
	 * Select function for smoothing 
	 */
	if(model->flags & BR_MODF_CREASE) {
		crease_limit = BR_COS(model->crease_angle);
		smoothing_fn = SmoothingCreased;
	} else {
		crease_limit = BR_SCALAR(0.0);
		smoothing_fn = Smoothing;
	}

	/*
     * XXX HACK
	 * Use static varable to tell qsort functions which model to use
	 */
	compareModel = model;

	/*
	 * Create a per face vertex table, and a table of pointers
	 * to each element, which is then sorted in various ways
	 *
	 */
	ntemps = model->nfaces * 3;

	block_size = ntemps * (sizeof(*temp_verts) + sizeof(*sorted_vertices)) + model->nfaces * sizeof(sorted_faces);
	temp_verts = BrScratchAllocate(block_size);

	BrMemSet(temp_verts, 0, block_size);

	sorted_vertices = (struct prep_vertex **)(temp_verts + ntemps);
	sorted_faces = (struct br_face **)(sorted_vertices + ntemps);

	gtvp = temp_verts;
	for(i=0, f = 0, fp = model->faces; f< model->nfaces; f++, fp++) {
		sorted_faces[f] = fp;
		for( v = 0; v < 3; v++, i++, gtvp++) {

			ASSERT(gtvp < temp_verts + model->nfaces*3);

			if (model->flags & BR_MODF_CUSTOM_NORMALS) {
				gtvp->n.v[0] = BrFractionToScalar(model->vertices[fp->vertices[v]].n.v[0]);
				gtvp->n.v[1] = BrFractionToScalar(model->vertices[fp->vertices[v]].n.v[1]);
				gtvp->n.v[2] = BrFractionToScalar(model->vertices[fp->vertices[v]].n.v[2]);
			}

			gtvp->v = fp->vertices[v];
			gtvp->f = f;

			sorted_vertices[i] = gtvp;
		}

		/*
		 * Faces that are not in any smoothing group smooth with all
		 * other faces
		 */
		if (fp->smoothing == 0)
			fp->smoothing = (br_uint_16)~0;
	}

	ASSERT(i == ntemps);
	ASSERT(gtvp == temp_verts + ntemps);
	
	/*
	 * Sort face pointers by material
	 */
	BrQsort(sorted_faces, model->nfaces, sizeof(*sorted_faces), FacesCompare);

	/*
	 * Propagate normals based on smoothing groups (and creasing)
	 */
	if (!(model->flags & BR_MODF_CUSTOM_NORMALS)) {

		BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices),vertex_compare_smoothing);

		for(v=0, i=0; v < ntemps-1; v++) {

			if(vertex_compare_smoothing(sorted_vertices+v,sorted_vertices+v+1)) {
				/*
				 * Process a group of vertices
				 */
				smoothing_fn(model, crease_limit, sorted_vertices+i, sorted_vertices+v+1);
				i = v+1;
			}
		}

		smoothing_fn(model, crease_limit, sorted_vertices+i, sorted_vertices+ntemps);
	}

	/*
	 * Resort temp. vertices into groups
	 */
	BrQsort(sorted_vertices, ntemps, sizeof(*sorted_vertices),vertex_compare_groups);

	/*
	 * Count distinct vertices in model
	 */
	for(v=0, nv=1; v < ntemps-1; v++)
		if(vertex_compare_groups(sorted_vertices+v,sorted_vertices+v+1))
			nv++;

	/*
	 * Count groups
	 */
	for(f=1, ng=1; f < model->nfaces; f++)
		if(sorted_faces[f]->material != sorted_faces[f-1]->material)
			ng++;

	/*
	 * Prepared data will have same number of faces as input
	 */
	nf = model->nfaces;

	/*
	 * Allocate prepared data area
	 */
	block_size= PREP_ALIGN(sizeof(struct v11model)) +
				PREP_ALIGN(ng * sizeof(struct v11group)) +

				PREP_ALIGN(nf * sizeof(*v11g->vertex_numbers)) +
				PREP_ALIGN(nf * sizeof(*v11g->edges)) +
				PREP_ALIGN(nf * sizeof(*v11g->eqn)) +

				PREP_ALIGN(nv * sizeof(*v11g->position))+
				PREP_ALIGN(nv * sizeof(*v11g->map))+
				PREP_ALIGN(nv * sizeof(*v11g->normal))+

				PREP_ALIGN(nf * sizeof(br_colour)) +
                PREP_ALIGN(nv * sizeof(br_colour)) +
                PREP_ALIGN(nf * sizeof(br_uint_8)) +
				nv * sizeof(br_int_16) + nf * sizeof(br_int_16);

	if(model->prepared && block_size > ((struct v11model *)(model->prepared))->size) {
		BrResFree(model->prepared);
		model->prepared = NULL;
	}

	if(model->prepared == NULL) {
		model->prepared = BrResAllocate(model, block_size, BR_MEMORY_PREPARED_MODEL);
		((struct v11model *)(model->prepared))->size = block_size;
	}

	cp = model->prepared;

	v11m = (void *)cp; cp+= PREP_ALIGN(sizeof(struct v11model));
	v11g = (void *)cp; cp+= PREP_ALIGN(ng * sizeof(struct v11group));

//	v11f = (void *)cp; cp+= PREP_ALIGN(nf * sizeof(struct v11face));
	vertex_numbers	=	(void *)cp; cp+= PREP_ALIGN(nf * sizeof(*v11g->vertex_numbers));
	edges			=	(void *)cp; cp+= PREP_ALIGN(nf * sizeof(*v11g->edges));
	eqn				=	(void *)cp; cp+= PREP_ALIGN(nf * sizeof(*v11g->eqn));

//	v11v = (void *)cp; cp+= PREP_ALIGN(nv * sizeof(struct fmt_vertex));
	position	= (void *)cp; cp+= PREP_ALIGN(nv * sizeof(*v11g->position));
	map			= (void *)cp; cp+= PREP_ALIGN(nv * sizeof(*v11g->map));
	normal		= (void *)cp; cp+= PREP_ALIGN(nv * sizeof(*v11g->normal));

    v11vcolours = (void *)cp; cp+= PREP_ALIGN(nv * sizeof(br_colour));
	v11fcolours = (void *)cp; cp+= PREP_ALIGN(nf * sizeof(br_colour));

	v11fflags = (void *)cp; cp+= PREP_ALIGN(nf * sizeof(br_uint_8));

    v11vuser = (void *)cp; cp+= nv * sizeof(br_uint_16);
	v11fuser = (void *)cp; cp+= nf * sizeof(br_uint_16);

	v11m->groups = v11g;
	v11m->ngroups = ng;
	
	/*
	 * Build prepared data
	 */

	/*
	 * Group Faces
	 */
//	v11g[0].faces = v11f;
	v11g[0].vertex_numbers=vertex_numbers;
	v11g[0].edges=edges;
	v11g[0].eqn=eqn;

    v11g[0].face_colours = v11fcolours;
    v11g[0].face_user = v11fuser;

    v11g[0].face_flags = v11fflags;

	v11g[0].stored = sorted_faces[0]->material?sorted_faces[0]->material->stored:NULL;
	v11g[0].nfaces = 0;

	for(f=0, g=0; f < model->nfaces-1; f++) {

		CopyFace(v11g+g, v11g[g].nfaces, sorted_faces[f], model);
		v11g[g].nfaces++;

		if(sorted_faces[f]->material != sorted_faces[f+1]->material) {
			g++;
//    		v11g[g].faces = v11f+f+1;
			v11g[g].vertex_numbers=vertex_numbers+f+1;
			v11g[g].edges=edges+f+1;
			v11g[g].eqn=eqn+f+1;
		
            v11g[g].face_colours = v11fcolours+f+1;
            v11g[g].face_user = v11fuser+f+1;
            v11g[g].face_flags = v11fflags+f+1;

			v11g[g].stored = sorted_faces[f+1]->material?sorted_faces[f+1]->material->stored:NULL;
			v11g[g].nfaces = 0;
		}
	}

	CopyFace(v11g+g, v11g[g].nfaces, sorted_faces[f], model);
	v11g[g].nfaces++;

	/*
	 * Group Vertices
	 */
//	v11g[0].vertices = v11v;
	v11g[0].position=position;
	v11g[0].map=map;
	v11g[0].normal=normal;

	v11g[0].vertex_colours = v11vcolours;
	v11g[0].vertex_user = v11vuser;

	v11g[0].nvertices = 1;
	CopyVertex(v11g, 0, sorted_vertices[0], model);

	if (model->flags & BR_MODF_UPDATEABLE) {

		last_n.v[0] = BrScalarToFractionClamp(v11g[0].normal[0].v[0]);
		last_n.v[1] = BrScalarToFractionClamp(v11g[0].normal[0].v[1]);
		last_n.v[2] = BrScalarToFractionClamp(v11g[0].normal[0].v[2]);

		model->vertices[sorted_vertices[0]->v].n = last_n;
	}

	for(v=0, g=0, count=0; v < ntemps-1; v++) {

		if(model->faces[sorted_vertices[v]->f].material !=
			model->faces[sorted_vertices[v+1]->f].material ) {
            g++;
//			v11g[g].vertices = v11v+count+1;
			v11g[g].position = position+count+1;
			v11g[g].map = map+count+1;
			v11g[g].normal = normal+count+1;

        	v11g[g].vertex_colours = v11vcolours+count+1;
        	v11g[g].vertex_user = v11vuser+count+1;
			v11g[g].nvertices = 0;
		}

		old_count = count;
		if(vertex_compare_groups(sorted_vertices+v,sorted_vertices+v+1)) {

			count++;
			sorted_vertices[v]->v = count;
			CopyVertex(v11g+g, v11g[g].nvertices, sorted_vertices[v+1], model);

			if (model->flags & BR_MODF_UPDATEABLE) {

				last_n.v[0] = BrScalarToFractionClamp(v11g[g].normal[v11g[g].nvertices].v[0]);
				last_n.v[1] = BrScalarToFractionClamp(v11g[g].normal[v11g[g].nvertices].v[1]);
				last_n.v[2] = BrScalarToFractionClamp(v11g[g].normal[v11g[g].nvertices].v[2]);
			}

			v11g[g].nvertices++;
		}

		if (model->flags & BR_MODF_UPDATEABLE)
			model->vertices[sorted_vertices[v+1]->v].n = last_n;

		sorted_vertices[v]->v = old_count;
	}

	sorted_vertices[v]->v = count;

	/*
	 * Attach Faces to vertices
	 */
	for(g=0; g < ng; g++) {
		for(f=0; f < v11g[g].nfaces; f++) {
			i = v11g[g].position - position;
			v = v11g[g].face_user[f] * 3;

			v11g[g].vertex_numbers[f].v[0] = temp_verts[v+0].v - i;
			v11g[g].vertex_numbers[f].v[1] = temp_verts[v+1].v - i;
			v11g[g].vertex_numbers[f].v[2] = temp_verts[v+2].v - i;
		}
	}

	/*
	 * Release scratchpad
	 */
	BrScratchFree(temp_verts);
}

/*
 * Find bounding radius of model
 */
static void PrepareBoundingRadius(br_model *model)
{
	float d,max = 0.0F;
	int v;
	br_vertex *vp;

	for(v=0, vp = model->vertices ; v< model->nvertices; v++, vp++) {
		d = BrScalarToFloat(vp->p.v[0]) * BrScalarToFloat(vp->p.v[0]) +
			BrScalarToFloat(vp->p.v[1]) * BrScalarToFloat(vp->p.v[1]) +
			BrScalarToFloat(vp->p.v[2]) * BrScalarToFloat(vp->p.v[2]);

		if(d>max)
			max = d;
	}

	model->radius = BrFloatToScalar(BrFloatSqrt(max));
}

static br_scalar PrepareCentredBoundingRadius(br_model *model,const br_vector3 * centre)
{
	float d,max = 0.0F;
	int v;
	br_vertex *vp;
	float rp[3];

	for(v=0, vp = model->vertices ; v< model->nvertices; v++, vp++) {

		rp[0]=BrScalarToFloat(vp->p.v[0]) - BrScalarToFloat(centre->v[0]);
		rp[1]=BrScalarToFloat(vp->p.v[1]) - BrScalarToFloat(centre->v[1]);
		rp[2]=BrScalarToFloat(vp->p.v[2]) - BrScalarToFloat(centre->v[2]);

		d = rp[0]*rp[0] + rp[1]*rp[1] + rp[2]*rp[2];

		if(d>max)
			max = d;
	}

	return BrFloatToScalar(BrFloatSqrt(max));
}

/*
 * Find bounding box of model
 */
static void PrepareBoundingBox(br_model *model)
{
	int axis,v;
	br_vertex *vp;
	br_scalar x;

	/*
	 * Initialise bounding box to first vertex
	 */
	for(axis = 0; axis < 3; axis++ )
		model->bounds.min.v[axis] = model->bounds.max.v[axis] =
			model->vertices[0].p.v[axis];

	/*
	 * Expand bounding box with remaining vertices
	 */
	for(v=1, vp = model->vertices+1 ; v< model->nvertices; v++, vp++) {
		for(axis = 0; axis < 3; axis++) {

			x = vp->p.v[axis];

			if(x > model->bounds.max.v[axis])
				model->bounds.max.v[axis] = x;

			if(x < model->bounds.min.v[axis])
				model->bounds.min.v[axis] = x;
		}
	}
}


void RegenerateFaceNormals(struct v11model *v11m)
{
	int g,f;
//	struct v11face *fp;
	br_vector4 * eqn;
	br_vector3_u16 *vertex_numbers;

	for(g=0; g < v11m->ngroups; g++) {
		eqn=v11m->groups[g].eqn;
		vertex_numbers=v11m->groups[g].vertex_numbers;
		for(f=0; f < v11m->groups[g].nfaces; f++, eqn++,vertex_numbers++) {
			
			BrPlaneEquation(eqn,
					&v11m->groups[g].position[vertex_numbers->v[0]],
					&v11m->groups[g].position[vertex_numbers->v[1]],
					&v11m->groups[g].position[vertex_numbers->v[2]]);
		}
	}
}

void RegenerateVertexNormals(struct v11model *v11m)
{
	int g,v,f;
//	struct v11face *fp;
	br_vector3_u16 *vertex_numbers;
	br_vector4 *eqn;

//	struct fmt_vertex *vp;
	br_vector3 * normal;
	br_vector3 *normals;

	for(g=0; g < v11m->ngroups; g++) {

		/*
		 * Build a scratch workspace for accumulated normals
		 */
		normals = BrScratchAllocate(sizeof(*normals) * v11m->groups[g].nvertices);
		BrMemSet(normals,0,sizeof(*normals) * v11m->groups[g].nvertices);

		/*
		 * Accumulate normals from each face
		 */
//		fp=v11m->groups[g].faces;
		vertex_numbers=v11m->groups[g].vertex_numbers;
		eqn=v11m->groups[g].eqn;

		for(f=0 ; f < v11m->groups[g].nfaces; f++, vertex_numbers++,eqn++) {
			BrVector3Accumulate(normals+vertex_numbers->v[0],(br_vector3 *)eqn);
			BrVector3Accumulate(normals+vertex_numbers->v[1],(br_vector3 *)eqn);
			BrVector3Accumulate(normals+vertex_numbers->v[2],(br_vector3 *)eqn);
			
		}

		/*
		 * Normalise accumulated normals into vertices
		 */
		normal = v11m->groups[g].normal;
		for(v=0; v < v11m->groups[g].nvertices; v++,normal++)
			BrVector3Normalise(normal,normals+v);

		BrScratchFree(normals);
	}
}

/*
 * Do all model preprocessing
 */
void BR_PUBLIC_ENTRY BrModelUpdate(br_model *model, br_uint_16 flags)
{
	int g,f,v;
	struct v11model *v11m;
//	struct fmt_vertex *fvp;
	br_vector3 * position;
	br_vector2 * map;
	br_vector3 * normal;
	br_vector4 * eqn;
	br_face * fp;
	br_vertex * vp;

	br_vector3 centre;
	br_scalar centred_radius;
	br_bounds3 old_bounds;
	br_boolean centre_valid=BR_FALSE;

	/*
	 * Do not do anything if model is marked as being pre-prepared
	 */
	if(model->flags & BR_MODF_PREPREPARED)
		return;

	/*
	 * Choke if model has no faces or vertices
	 */
	if(model->faces == NULL || model->vertices == NULL)
			BR_FAILURE1("BrModelUpdate: model has no faces or vertices (%s)",
				model->identifier?model->identifier:"<NULL>");

	v11m = model->prepared;
	if(v11m){
		// initialise the centre and radius to the old centre and radius
		centre=v11m->centre;
        centred_radius=v11m->centred_radius;
		centre_valid=BR_TRUE; // assume the old radius is either correct or will be overwritten with a correct value
	}

				
	/*
	 * Re-copy the vertices if the pivot has moved
	 */
	if(flags & BR_MODU_PIVOT)
		flags |= BR_MODU_VERTEX_POSITIONS;

	/*
 	 * Backwards compatibility
	 */
	if(model->flags & (BR_MODF_GENERATE_TAGS | BR_MODF_KEEP_ORIGINAL)) {
		model->flags |= BR_MODF_UPDATEABLE;
	}

	/*
	 * Bounds
	 *
	 */
	if(!(model->flags & BR_MODF_CUSTOM_BOUNDS) && (flags & (BR_MODU_VERTICES|BR_MODU_VERTEX_POSITIONS))) {
		PrepareBoundingRadius(model);
		PrepareBoundingBox(model);
// use the average of the bounds as an approximation to the centre of the model (that gives the minimal bounding radius)
		centre.v[0]=BR_CONST_DIV(model->bounds.min.v[0]+model->bounds.max.v[0],2);
		centre.v[1]=BR_CONST_DIV(model->bounds.min.v[1]+model->bounds.max.v[1],2);
		centre.v[2]=BR_CONST_DIV(model->bounds.min.v[2]+model->bounds.max.v[2],2);
		centred_radius=PrepareCentredBoundingRadius(model,&centre);
		centre_valid=BR_TRUE; 
	}else if((model->flags & BR_MODF_CUSTOM_BOUNDS) && (flags & (BR_MODU_VERTICES|BR_MODU_VERTEX_POSITIONS))) {
		old_bounds=model->bounds;
		PrepareBoundingBox(model);
// use the average of the bounds as an approximation to the centre of the model (that gives the minimal bounding radius)
		centre.v[0]=BR_CONST_DIV(model->bounds.min.v[0]+model->bounds.max.v[0],2);
		centre.v[1]=BR_CONST_DIV(model->bounds.min.v[1]+model->bounds.max.v[1],2);
		centre.v[2]=BR_CONST_DIV(model->bounds.min.v[2]+model->bounds.max.v[2],2);
		model->bounds=old_bounds;
		centred_radius=PrepareCentredBoundingRadius(model,&centre);
		centre_valid=BR_TRUE; 
	}

	/*
	 * Catch internal update - just look up new material stored pointers
	 */
	if (flags & _BR_MODU_RESERVED &&
		!(model->flags & BR_MODF_USED_PREPARED_USER)) {

		v11m = model->prepared;

		for(g=0; g < v11m->ngroups; g++) {

			ASSERT(v11m->groups[g].face_user[0] < model->nfaces);

			fp = model->faces + v11m->groups[g].face_user[0];

			v11m->groups[g].stored = fp->material != NULL? fp->material->stored: NULL;
		}

	/*
	 * See if the only thing needed is to copy new parts over
	 */
	} else if (model->prepared && 
		!(flags & ~(MODU_VERTEX_COPY_FLAGS | MODU_FACE_COPY_FLAGS)) &&
		!(model->flags & BR_MODF_USED_PREPARED_USER)) {

		v11m = model->prepared;

		/*
		 * Transcribe any vertex info
		 */
		if(model->vertices && (flags & MODU_VERTEX_COPY_FLAGS)) {
			for(g=0; g < v11m->ngroups; g++) {
				for(v=0; v < v11m->groups[g].nvertices; v++) {
//					fvp = v11m->groups[g].vertices+v;
					position=v11m->groups[g].position+v;
					normal=v11m->groups[g].normal+v;
					map=v11m->groups[g].map+v;

					ASSERT(v11m->groups[g].vertex_user[v] < model->nvertices);
					vp = model->vertices + v11m->groups[g].vertex_user[v];

					if(flags & BR_MODU_VERTEX_POSITIONS) {
						position->v[0] = vp->p.v[0] - model->pivot.v[0];
						position->v[1] = vp->p.v[1] - model->pivot.v[1];
						position->v[2] = vp->p.v[2] - model->pivot.v[2];
					}

					if(flags & BR_MODU_VERTEX_COLOURS)
                        v11m->groups[g].vertex_colours[v] =
						    BR_COLOUR_RGBA(vp->red, vp->grn, vp->blu, vp->index);

					if(flags & BR_MODU_VERTEX_MAPPING)
						*map = vp->map;

					if((flags & BR_MODU_VERTEX_NORMALS) && (model->flags & BR_MODF_CUSTOM_NORMALS)) {
						normal->v[0] = BrFractionToScalar(vp->n.v[0]);
						normal->v[1] = BrFractionToScalar(vp->n.v[1]);
						normal->v[2] = BrFractionToScalar(vp->n.v[2]);
                    }
				}
			}
		}

		/*
		 * Transcribe any face info
		 */
		if(model->faces && (flags & MODU_FACE_COPY_FLAGS)) {

			for(g=0; g < v11m->ngroups; g++) {
				for(f=0; f < v11m->groups[g].nfaces; f++) {
					eqn=v11m->groups[g].eqn+v;

					ASSERT(v11m->groups[g].face_user[f] < model->nfaces);
					fp = model->faces+v11m->groups[g].face_user[f];

					if(flags & BR_MODU_FACE_COLOURS)
				        v11m->groups[g].face_colours[f] = BR_COLOUR_RGBA(fp->red, fp->grn, fp->blu, fp->index);

					if((flags & BR_MODU_FACE_EQUATIONS) && (model->flags & BR_MODF_CUSTOM_EQUATIONS)) {
						eqn->v[0] = BrFractionToScalar(fp->n.v[0]);
						eqn->v[1] = BrFractionToScalar(fp->n.v[1]);
						eqn->v[2] = BrFractionToScalar(fp->n.v[2]);
						eqn->v[3] = fp->d;
                    }
				}
			}
		}

		if(flags & BR_MODU_VERTEX_POSITIONS) {

			if(!(model->flags & BR_MODF_CUSTOM_NORMALS))
				RegenerateVertexNormals(v11m);

			if(!(model->flags & BR_MODF_CUSTOM_EQUATIONS))
				RegenerateFaceNormals(v11m);
		}

		v11m->bounds = model->bounds;
		v11m->radius = model->radius;
		if(centre_valid && centred_radius<model->radius){
			v11m->centre=centre;
			v11m->centred_radius=centred_radius;
		}else{
			v11m->centre.v[0]=0;
			v11m->centre.v[1]=0;
			v11m->centre.v[2]=0;
			v11m->centred_radius=model->radius;
		}
	} else {
		/*
		 *	Regenerate from scratch...
		 */

		/*
		 * If model has no faces or vertices, do nothing
		 */
		if(model->faces == NULL || model->vertices == NULL)
			return;

		/*
		 * Generate all face normals
		 */
		if (!(model->flags & BR_MODF_CUSTOM_EQUATIONS))
			PrepareFaceNormals(model);

		/*
		 * Check that faces do not reference non-existant vertices and set
		 * flag to say of model uses default material
		 */
		fp = model->faces;
		model->flags &= ~MODF_USES_DEFAULT;

		for(f=0; f< model->nfaces; f++, fp++) {

			if(fp->material == NULL)
				model->flags |= MODF_USES_DEFAULT;

			for(v=0; v < 3; v++)
				if(fp->vertices[v] >= model->nvertices)
					BR_ERROR2("face references invalid vertex f=%d v=%d",f,v);
		}

		PrepareGroups(model);
		BrPrepareEdges(model);

		v11m = model->prepared;

		v11m->bounds = model->bounds;
		v11m->radius = model->radius;
		if(centre_valid && centred_radius<model->radius){
			v11m->centre=centre;
			v11m->centred_radius=centred_radius;
		}else{
			v11m->centre.v[0]=0;
			v11m->centre.v[1]=0;
			v11m->centre.v[2]=0;
			v11m->centred_radius=model->radius;
		}
	}

	/*
	 * Release original faces and vertices if model is not updateable
	 */
	if(!(model->flags & BR_MODF_UPDATEABLE)) {
		if(model->faces)
			BrResFree(model->faces);

		if(model->vertices)
			BrResFree(model->vertices);

		model->faces = NULL;
		model->vertices = NULL;

		model->nfaces = 0;
		model->nvertices = 0;
	}

	/*
	 * Generate stored object, if renderer is available
	 */
	if(v1db.renderer && v1db.format_model &&
		!(model->flags & BR_MODF_UPDATEABLE) &&
		(model->flags & BR_MODF_FACES_ONLY)) {

		br_error r;
		struct br_geometry_stored *sg;
		br_boolean b;
		br_token_value tv[] = {
			{BRT_CAN_SHARE_B, BR_TRUE},
			{0},
		};

		/*
		 * Release any existing stored object
		 */
		if(model->stored) {
			ObjectFree(model->stored);
			model->stored = NULL;
		}

		/*
		 * Try and generate a stored version of the model (as faces)
		 */
		r = GeometryV1ModelStoredNew(v1db.format_model, v1db.renderer, &sg, model->prepared, BRT_TRIANGLE, tv);
		
		if((r == BRE_OK) && (sg != NULL)) {
			/*
			 * It worked, remember the pointer
			 */
			model->stored = sg;

			/*
			 * If the resulting buffer does not share any of the prepared data, ditch it
			 */
			r = ObjectQuery(sg,(br_uint_32 *)&b,BRT_SHARED_B);

			if(r==BRE_OK && b == BR_FALSE) {
				BrResFree(model->prepared);
				model->prepared = NULL;
			}
		}
	}
}

/*
 * Clear up a prepared model
 */
void BrModelClear(struct br_model *model)
{
	if(model->stored) {
		ObjectFree(model->stored);
		model->stored = NULL;
	}
}

void BrModelClearPrepared(struct br_model *model)
{
	if(model->prepared) {
		BrResFree(model->prepared);
		model->prepared = NULL;
	}
}
