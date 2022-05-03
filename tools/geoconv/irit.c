/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: irit.c 1.1 1997/12/10 16:58:11 jon Exp $
 * $Locker: $
 *
 * Load and save IRIT test data format
 *
 * $BC<"make -f geoconv.mak %s.obj;">
 */
#include <stddef.h>

/*
 * Brender includes
 */
#include "brender.h"

br_uint_32 BR_PUBLIC_ENTRY BrFmtIritModelLoadMany(char *name,
		br_model **mtable, br_uint_16 max_models)
{
	return 0;
}

static IritWriteModel(void *fh, br_model *model)
{
	int f,v;
	br_face *fp;

	BrFilePrintf(fh,"\n[OBJECT %s\n",model->identifier?model->identifier:"BRENDER_MODEL");

	for(f=0, fp = model->faces; f < model->nfaces; f++,fp++) {
		BrFilePrintf(fh,"\t[POLYGON 3\n");
		for(v=0; v < 3; v++)
			BrFilePrintf(fh,"\t\t[%s %10f %10f %10f]\n",
				(fp->flags & (1<<v))?"[INTERNAL]":"",
				BrScalarToFloat(model->vertices[fp->vertices[v]].p.v[0]),
				BrScalarToFloat(model->vertices[fp->vertices[v]].p.v[1]),
				BrScalarToFloat(model->vertices[fp->vertices[v]].p.v[2]));
		BrFilePrintf(fh,"\t]\n");
	}

	BrFilePrintf(fh,"]\n");
}

br_uint_32 BR_PUBLIC_ENTRY BrFmtIritModelSaveMany(char *name,
		br_model **mtable, br_uint_16 nmodels)
{
	void *fh;
	int i;

	/*
	 * Open input file
	 */
	fh = BrFileOpenWrite(name,1);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for writing",name);

	for(i=0; i < nmodels; i++)
		IritWriteModel(fh, mtable[i]);

	BrFileClose(fh);

	return nmodels;
}

