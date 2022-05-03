/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: save3ds.c 1.1 1997/12/10 16:58:14 jon Exp $
 * $Locker: $
 *
 * $BC<"make -f geoconv.wtc %s.obj;">
 *
 * Save brender data to 3DS files using the Autodesk 3DS File Toolkit
 */
#if SAVE_3DS

#include <stddef.h>
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

/*
 * Brender includes
 */
#include "brender.h"

/*
 * 3DS Includes
 */
#pragma pack(1);
#include "3dsftk.h"
#pragma pack();

void ErrorCheck3ds(void);
br_uint_32 BR_CALLBACK Output3DSModel(struct br_model *model, void *dummy);

void Output3DSModels(char *filename)
{
	char name[_MAX_PATH+1],*cp;

	/*
	 * Strip extension from filename
	 */
	strncpy(name,filename,_MAX_PATH);

	for(cp = name+strlen(name); cp > name; cp--) {
		if(*cp == '.')
			*cp = '\0';
		else if(*cp == ':' || *cp == '/' || *cp == '\\')
			break;
	}

	/*
	 * Start 3DS file
	 */
	Init3ds(name);
	ErrorCheck3ds();

	SetScale3ds(1.0);
	ErrorCheck3ds();

	/*
	 * Output all Models
	 */
	BrModelEnum(NULL,Output3DSModel,NULL);

	/*
	 * Close up 3DS file
	 */
	Close3ds();
	ErrorCheck3ds();
}


br_uint_32 BR_CALLBACK Output3DSModel(struct br_model *model, void *dummy)
{
	int i;
	static int model_num = 0;

	mesh3ds *mesh = NULL;

	/*
	 * Allocate memory for the mesh
	 */
	InitMeshObj3ds(&mesh,model->nvertices,model->nfaces,
		InitTextArray3ds| InitSmoothArray3ds);
	ErrorCheck3ds();

	/*
	 * Transcribe model->mesh
	 */
	sprintf(mesh->name,"Brender-%02d",model_num++);

#if 1
	if(model->identifier && (strlen(model->identifier) != 0))
		strncpy(mesh->name,model->identifier,10);
	else
		sprintf(mesh->name,"Brender-%02d",model_num++);

	for(i=0; i< model->nvertices; i++) {
		mesh->vertexarray[i].x = BrScalarToFloat(model->vertices[i].p.v[0]);
		mesh->vertexarray[i].y = BrScalarToFloat(model->vertices[i].p.v[1]);
		mesh->vertexarray[i].z = BrScalarToFloat(model->vertices[i].p.v[2]);

		mesh->textarray[i].u = model->vertices[i].map.v[0];
		mesh->textarray[i].v = model->vertices[i].map.v[1];
	}	

	for(i=0; i< model->nfaces; i++) {
		mesh->facearray[i].v1 = model->faces[i].vertices[0];
		mesh->facearray[i].v2 = model->faces[i].vertices[1];
		mesh->facearray[i].v3 = model->faces[i].vertices[2];
		mesh->facearray[i].flag = (model->faces[i].flags & 0x7) ^ 0x7;

		mesh->smootharray[i] = model->faces[i].smoothing;
	}	
#endif

//	FillMatrix3ds(mesh);

	memset(mesh->locmatrix,0,sizeof(mesh->locmatrix));
	mesh->locmatrix[0] = 1.0;
	mesh->locmatrix[4] = 1.0;
	mesh->locmatrix[8] = 1.0;

	ErrorCheck3ds();

	/*
	 * Write mesh to output file
	 */
	if(AddMeshObject3ds(mesh) != Success3ds)
		ErrorCheck3ds();

	/*
	 * Release allocated mesh
	 */
	if(RelMeshObj3ds(&mesh) != Success3ds)
		ErrorCheck3ds();

	return 0;
}

void ErrorCheck3ds(void)
{
	if(ErrorExit3ds())
		BR_ERROR1("%s",ErrorMessage3ds(GetLastError3ds()));
}

#endif /* SAVE_3DS */
