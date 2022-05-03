/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadasc.c 1.1 1997/12/10 16:41:01 jon Exp $
 * $Locker: $
 *
 * Loader for 3D-Studio .ASC format
 *
 */
#include "brender.h"
#include "fmt.h"

BR_RCS_ID("$Id: loadasc.c 1.1 1997/12/10 16:41:01 jon Exp $")

/*
 * Various keywords than can appear in 3D-Studio .ASC file
 */
#define K_NOP			0
#define K_AMBIENT_LIGHT	1
#define K_NAMED_OBJECT	2
#define K_TRI_MESH		3
#define K_VERTEX		4
#define K_FACE			5
#define K_SMOOTHING		6
#define K_MATERIAL		7
#define K_DIRECT_LIGHT	8
#define K_POSITION		9
#define K_LIGHT_COLOR	10
#define K_CAMERA		11
#define K_TARGET		12
#define K_BANK_ANGLE	13

#define K_EOF			255

static struct {
	char *word;
	short length;
	short token;
#define K(word,token) {word,sizeof(word)-1,token,},
} Keywords[] = {
	K("Ambient light color:",	K_AMBIENT_LIGHT)
	K("Named object: ",			K_NAMED_OBJECT)
	K("Tri-mesh,",				K_TRI_MESH)
	K("Vertex list:",			K_NOP)
	K("Vertex",					K_VERTEX)
	K("Face list:",				K_NOP)
	K("Face",					K_FACE)
	K("Smoothing:",				K_SMOOTHING)
	K("Material:",				K_MATERIAL)
	K("Direct light",			K_DIRECT_LIGHT)
	K("Position:",				K_POSITION)
	K("Light color:",			K_LIGHT_COLOR)
	K("Camera ",				K_CAMERA)
	K("Target ",				K_TARGET)
	K("Bank angle: ",			K_BANK_ANGLE)
};
#undef K

#define MAX_LINE 512
#define MAX_NAME 64

#define TABLE_SIZE 16

static br_model *LoadModels;
static br_material *LoadMaterials;
static br_camera *LoadCameras;
static br_light *LoadLights;
static br_actor *LoadActors;

/*
 * Load a 3D-Studio .ASC file and produce -
 *
 * 	Models
 *
 * TODO:
 * 	Actors
 * 	Materials
 * 	Cameras
 * 	Lights
 */
br_uint_32 BR_PUBLIC_ENTRY BrFmtASCLoad(char *name,
		br_model **mtable, br_uint_16 max_models)
{
	void *fh;
	char line[MAX_LINE];
	char object_name[MAX_NAME+1] = "";
	char material_name[MAX_NAME+1] = "";
	char *line_tail;
	int	token,cvert,cface,vnum,i;
	float vx,vy,vz,vu,vv;
	int fnum,fa,fb,fc,fab,fbc,fca;
	br_model *mp = NULL;
	int nmodels = 0,nvertices,nfaces;
	int open_mode = BR_FS_MODE_TEXT;

	/*
	 * Open input file
	 */
	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	/*
	 * Read lines from input file
	 */
	while(!BrFileEof(fh)) {
		/*
		 * Pull next line and look at the front to see what it contains:
		 */
		if(BrFileGetLine(line,MAX_LINE,fh) == 0) {
			token = K_NOP;
		} else {
			token = K_NOP;
			for(i=0; i< BR_ASIZE(Keywords); i++) {
				if(!BrStrNICmp(Keywords[i].word,line,Keywords[i].length)) {
					token = Keywords[i].token;
					line_tail = line+Keywords[i].length;
					break;
				}
			}
		}

		/*
		 * Perform action for current line
		 */
		switch(token) {
		case K_NAMED_OBJECT:

			/*
			 * Extract name 
			 */
			BrSScanf(line_tail,"\"%64[^\"]",object_name);
			break;

		case K_MATERIAL:

			BrSScanf(line_tail,"\"%64[^\"]",material_name);

			/*
			 * make sure a face has been read
			 */	
			if(cface > 0)
				mp->faces[cface-1].material = BrMaterialFind(material_name);
			break;

		case K_TRI_MESH:

			/*
			 * Fail if no enough space in output table
			 */
			if(nmodels >= max_models)
				return nmodels;

			/*
			 * Allocate a new model and buffers
			 */
			BrSScanf(line_tail," Vertices: %d Faces: %d",&nvertices,&nfaces);

			mp = BrModelAllocate((object_name[0] != '0')?object_name:NULL,nvertices,nfaces);

			cvert = 0;
			cface = 0;

			/*
			 * Record model in output table
			 */
			mtable[nmodels++] = mp;
			break;

		case K_VERTEX:
			/*
			 * Add vertex to current mesh
			 */
			vx = vy = vz = vu = vv = 0.0F;

			BrSScanf(line_tail,"%d: X: %f Y: %f Z: %f U: %f V: %f",&vnum,&vx,&vy,&vz,&vu,&vv);
			/*
			 * Only process if things are consistent
			 */
			if(cvert != vnum || cvert >= mp->nvertices)
				break;
			
			mp->vertices[cvert].p.v[0] = BrFloatToScalar(vx);
			mp->vertices[cvert].p.v[1] = BrFloatToScalar(vy);
			mp->vertices[cvert].p.v[2] = BrFloatToScalar(vz);
			mp->vertices[cvert].map.v[0] = BrFloatToScalar(vu);
			mp->vertices[cvert].map.v[1] = BrFloatToScalar(vv);

			cvert++;
			break;

		case K_FACE:
			/*
			 * Add face to current mesh
			 */
			BrSScanf(line_tail,"%d: A:%d B:%d C:%d AB:%d BC:%d CA:%d",
					  &fnum,&fa,&fb,&fc,&fab,&fbc,&fca);
			/*
			 * Only process if things are consistent
			 */
			if(cface != fnum || cface >= mp->nfaces)
				break;

			mp->faces[cface].vertices[0] = fa;
			mp->faces[cface].vertices[1] = fb;
			mp->faces[cface].vertices[2] = fc;
			mp->faces[cface].material = 0;	/* default material */
			mp->faces[cface].flags = (fab?0:1) | (fbc?0:2) | (fca?0:4);

			cface++;
			break;

		case K_SMOOTHING:
			/*
			 * Add smoothing group to last face
			 */	
			if(cface > 0) {
				mp->faces[cface-1].smoothing = 0;

				while(i = BrStrToL(line_tail,&line_tail,0)) {

					mp->faces[cface-1].smoothing |= 1<<((i-1) % 16);

					while(*line_tail == ',' || *line_tail == ' ')
						line_tail++;
				}
			}
			break;

			/*
			 * Ignore everything else
			 */
		case K_NOP:
		default:
			;
		}
	}

	BrFileClose(fh);
	return nmodels;
}
