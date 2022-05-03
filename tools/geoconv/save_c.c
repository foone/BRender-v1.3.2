/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: save_c.c 1.1 1997/12/10 16:58:16 jon Exp $
 * $Locker: $
 *
 * Save brender data in C source code form
 *
 * $BC<"make -f geoconv.mak %s.obj;">
 */
#include <stddef.h>

/*
 * Brender includes
 */
#include "brender.h"

#define FractionToFloat(f) BrScalarToFloat(BrFractionToScalar(f))

void SortOutputPointers(void *table,int num);

static void WriteModelC(void *fh, br_model *m)
{
	int i;
	char *name = m->identifier?m->identifier:"unnamed";

	/*
	 * Vertices
	 */
	if(m->nvertices) {
		BrFilePrintf(fh,
			"/*\n"
			" * %d Vertices\n"
			" */\n"
			"static struct br_vertex %s_vertices[] = {\n",
			m->nvertices,name);

		for(i = 0; i < m->nvertices; i++) {

			BrFilePrintf(fh,"\t{BR_VECTOR3(%g,%g,%g),"
					   "BR_VECTOR2(%g,%g),",
				BrScalarToFloat(m->vertices[i].p.v[0]),
				BrScalarToFloat(m->vertices[i].p.v[1]),
				BrScalarToFloat(m->vertices[i].p.v[2]),

				BrScalarToFloat(m->vertices[i].map.v[0]),
				BrScalarToFloat(m->vertices[i].map.v[1]));

			BrFilePrintf(fh,"%3d,%3d,%3d,%3d,",
				m->vertices[i].index,
				m->vertices[i].red,
				m->vertices[i].grn,
				m->vertices[i].blu);

			BrFilePrintf(fh,"},\t/* %4d */\n",i);
		}

		BrFilePrintf(fh,"};\n\n");
	}

	/*
	 * Faces
	 */
	if(m->nfaces) {
		BrFilePrintf(fh,
			"/*\n"
			" * %d Faces\n"
			" */\n"
			"static struct br_face %s_faces[] = {\n",
			m->nfaces,name);

		for(i = 0; i < m->nfaces; i++) {
			BrFilePrintf(fh,"\t{{%d,%d,%d}",
				m->faces[i].vertices[0],
				m->faces[i].vertices[1],
				m->faces[i].vertices[2]);

				BrFilePrintf(fh,",{0},NULL,%d,%d,",
					m->faces[i].smoothing,m->faces[i].flags);

				BrFilePrintf(fh,"},\t/* %4d */\n",i);
		}
		BrFilePrintf(fh,"};\n\n");
	}

	/*
	 * Model
	 */
	BrFilePrintf(fh,"struct br_model %s = {\n",name);
	BrFilePrintf(fh,"\t\"%s\",\n",name);
	BrFilePrintf(fh,"\t%s_vertices,\n",name);
	BrFilePrintf(fh,"\t%s_faces,\n",name);

	BrFilePrintf(fh,"\t%d,%d, /* nV nF */\n",m->nvertices,m->nfaces);

	BrFilePrintf(fh,"\tBR_VECTOR3(%g,%g,%g), /* pivot */\n",
		BrScalarToFloat(m->pivot.v[0]),
		BrScalarToFloat(m->pivot.v[1]),
		BrScalarToFloat(m->pivot.v[2]));

	BrFilePrintf(fh,"\t0x%04x | BR_MODF_KEEP_ORIGINAL, /* flags */\n",m->flags);

	BrFilePrintf(fh,"};\n");
}

/*
 * Write out C source code for a model
 */
void OutputModelsC(char *filename)
{
	br_model **mtable;
	int nmodels,i;
	void *fh;

	nmodels = BrModelCount(NULL);
	mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);

	nmodels = BrModelFindMany(NULL,mtable,nmodels);

	SortOutputPointers(mtable,nmodels);

	fh = BrFileOpenWrite(filename,1);

	if(fh == NULL)
		BR_ERROR1("could not open '%s' for writing",filename);

	for(i=0; i < nmodels; i++)
		WriteModelC(fh,mtable[i]);

	BrFileClose(fh);
	BrMemFree(mtable);
}

