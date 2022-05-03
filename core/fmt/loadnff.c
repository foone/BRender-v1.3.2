/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadnff.c 1.1 1997/12/10 16:41:02 jon Exp $
 * $Locker: $
 *
 * Loader for Eric Haines' NFF format
 */
#include "brender.h"

BR_RCS_ID("$Id: loadnff.c 1.1 1997/12/10 16:41:02 jon Exp $")

/*
 * Current line buffer
 */
#define MAX_LINE 256

static char line[MAX_LINE];

/*
 * Maintain a list of unique vertices
 */
static int used_vertices = 0;

#define VTX_POOL_SIZE	1024
#define VTX_HASH_SIZE	65536

typedef struct nff_vertex {
	struct nff_vertex *next;
	struct nff_vertex *hash_next;
	br_vector3 v;
	int n;
} nff_vertex;

static nff_vertex *temp_vertex_pool = NULL;
static nff_vertex *temp_vertex_hash[VTX_HASH_SIZE];
static nff_vertex *temp_vertex_chain;
static int pool_vertices = 0;
static void **pool_chain = NULL;

static int AddVertex(br_scalar x, br_scalar y, br_scalar z)
{
	nff_vertex *vp;
	int hash;
	void **pp;

	/*
	 * make hash value from X,Y & Z bit patterns
	 *
	 * Assumes that br_scalar is at least as big as 2 shorts
	 */
	hash = ((br_uint_16 *)&x)[0] + ((br_uint_16 *)&x)[1] +
		   ((br_uint_16 *)&y)[0] + ((br_uint_16 *)&y)[1] +
		   ((br_uint_16 *)&z)[0] + ((br_uint_16 *)&z)[1];

	hash = hash % VTX_HASH_SIZE;

	/*
	 * See if vertex already exists
	 */
	for(vp=temp_vertex_hash[hash]; vp; vp = vp->hash_next)
		if(vp->v.v[0] == x && vp->v.v[1] == y && vp->v.v[2] == z)
			return vp->n;

	/*
	 * Create new vertex
	 */
	if(pool_vertices <=0) {
		/*
		 * Refill pool (pool blocks are linked by pointers in first
		 */
		pp = BrResAllocate(NULL, VTX_POOL_SIZE * sizeof(*temp_vertex_pool) + sizeof (void *),BR_MEMORY_APPLICATION);
		*pp = pool_chain;
		pool_chain = pp;
		temp_vertex_pool = (nff_vertex *)(pp+1);

		pool_vertices = VTX_POOL_SIZE;
	}

	vp = temp_vertex_pool++;
	pool_vertices--;

	/*
	 * Fill in new vertex
	 */
	vp->v.v[0] = x;
	vp->v.v[1] = y;
	vp->v.v[2] = z;

	/*
	 * Add new vertex to appropriate hash chain
	 */
	vp->hash_next = temp_vertex_hash[hash];
	temp_vertex_hash[hash] = vp;

	/*
	 * Add vertex to general chain
	 */
	vp->next = temp_vertex_chain;
	temp_vertex_chain = vp;

	return vp->n = used_vertices++;
}

/*
 * Maintain a list of faces
 */
static int used_faces = 0;
static int allocated_faces = 0;

#define FACEALLOC_MIN	256
 
struct nff_face {
	int v[3];
	int patch;
};

static struct nff_face *temp_faces = NULL;

static int AddFace(int v1, int v2, int v3,int patch)
{
	struct nff_face *fp;

	/*
	 * Create new face
	 */
	if(used_faces >= allocated_faces) {
		/*
		 * Allocate a new buffer if this one will overflow
		 */
		if(allocated_faces < FACEALLOC_MIN)
			allocated_faces = FACEALLOC_MIN;
		else
			allocated_faces *= 2;

		fp = BrResAllocate(NULL, sizeof(*fp) * allocated_faces, BR_MEMORY_APPLICATION);
		BrMemCpy(fp,temp_faces,used_faces * sizeof(*fp));
		BrResFree(temp_faces);
		temp_faces = fp;
	}

	temp_faces[used_faces].v[0] = v1;
	temp_faces[used_faces].v[1] = v2;
	temp_faces[used_faces].v[2] = v3;
	temp_faces[used_faces].patch = patch;

	return used_faces++;
}

/*
 * Read a line from input file, detecting EOF
 */
static void NextLine(void *fh)
{
	if(BrFileGetLine(line,MAX_LINE,fh) == 0)
		BR_ERROR0("Unexpected end of file");
}

br_model * BR_PUBLIC_ENTRY BrFmtNFFLoad(char *name)
{
	void *fh;
	int i,n;
	char *cp;
	br_model *model;
 	nff_vertex *vp;
	void **pp,**next_pp;
	int open_mode = BR_FS_MODE_TEXT;

	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	/*
	 * Read lines from the input file
	 */
	while(!BrFileEof(fh)) {

		BrFileGetLine(line,MAX_LINE,fh);
		cp = line;

		/*
		 * Eat white space of front of line
		 */

		/*
		 * Switch on the primitive
		 */
		switch(*cp++) {
		/*
		 * Comment for the rest of the line
		 */
		case '#':
			continue;
			break;

		/*
		 * viewing vectors and angles
		 */
		case 'v': {
				float x,y,z,tx,ty,tz,ux,uy,uz,angle,hither;
				int rx,ry;

				NextLine(fh);
				BrSScanf(line,"from %g %g %g",&x,&y,&z);
				NextLine(fh);
				BrSScanf(line,"at %g %g %g",&tx,&ty,&tz);
				NextLine(fh);
				BrSScanf(line,"up %g %g %g",&ux,&uy,&uz);
				NextLine(fh);
				BrSScanf(line,"angle %g",&angle);
				NextLine(fh);
				BrSScanf(line,"hither %g",&hither);
				NextLine(fh);
				BrSScanf(line,"resolution %d %d",&rx,&ry);
			}
			break;

		/*
		 * background color
		 */
		case 'b': {	
				float r,g,b;

				n = BrSScanf(cp,"%g %g %g",&r,&g,&b);
			}
			break;

		/*
		 * positional light location
		 */
		case 'l': {
				float x,y,z,r,g,b;
				n = BrSScanf(cp,"%g %g %g %g %g %g",
					&x,&y,&z,&r,&g,&b);
			}
			break;

		/*
		 * object material properties
		 */
		case 'f': {
				float r,g,b,kd,ks,shine,t,ior;

				n = BrSScanf(cp,"%g %g %g %g %g %g %g %g",
						&r,&g,&b,&kd,&ks,&shine,&t,&ior);
			}
			break;

		/*
		 * cone or cylinder primitive
		 */
		case 'c': {
				float bx,by,bz,br,ax,ay,az,ar;

				NextLine(fh);
				n = BrSScanf(line,"%g %g %g %g",&bx,&by,&bz,&br);
				NextLine(fh);
				n = BrSScanf(line,"%g %g %g %g",&ax,&ay,&az,&ar);
			}
			break;

		/*
		 * sphere primitive
		 */
		case 's': {
				float x,y,z,r;

				n = BrSScanf(cp,"%g %g %g %g",&x,&y,&z,&r);
			}
			break;

		/*
		 * polygon or patch primitive
		 */
		case 'p': {
				float vx,vy,vz,nx,ny,nz;
				int num_vertices,patch = 0;
				int verts[3];

				if(*cp == 'p') {
					cp++;
					patch = 1;
				}

				/*
				 * Find number of vertices from rest of line
				 */

				BrSScanf(cp,"%d",&num_vertices);

				if(num_vertices <3)
					BR_ERROR1("polygon has too few vertices: %d",num_vertices);

				/*
				 * Process each line of polygon
				 */
				for(i=0; i< num_vertices; i++) {

					NextLine(fh);

					n = BrSScanf(line,"%g %g %g %g %g %g",
						&vx,&vy,&vz,&nx,&ny,&nz);

					if(n != (patch?6:3))
						BR_ERROR1("Wrong number of components for vertex - %d",n);

					if(i < 2) {
						verts[i] = AddVertex(BrFloatToScalar(vx),
											 BrFloatToScalar(vy),
											 BrFloatToScalar(vz));
					} else {
						verts[2] = AddVertex(BrFloatToScalar(vx),
											 BrFloatToScalar(vy),
											 BrFloatToScalar(vz));

						AddFace(verts[0],verts[1],verts[2],patch);
						verts[1] = verts[2];
					}
				}
			}

			break;

		/*
		 * Unknown
		 */
		default:
			BR_ERROR1("FmtNFFLoad: Unknown NFF primitive - '%c'",*(cp-1));
			break;
		}
	}

	/*
	 * Convert temporary buffers into a model for p3d
	 */
	model = BrModelAllocate("nff_object",used_vertices,used_faces);

	for(i=0, vp =temp_vertex_chain; i < used_vertices && vp; i++, vp=vp->next) {
		model->vertices[vp->n].p.v[0] = vp->v.v[0];
		model->vertices[vp->n].p.v[1] = vp->v.v[1];
		model->vertices[vp->n].p.v[2] = vp->v.v[2];
	}

	for(i=0; i < used_faces; i++) {
		model->faces[i].vertices[0] = temp_faces[i].v[0];
		model->faces[i].vertices[1] = temp_faces[i].v[1];
		model->faces[i].vertices[2] = temp_faces[i].v[2];

		model->faces[i].material = NULL;
#if 0
		model->faces[i].smoothing = 1; 
#else
		model->faces[i].smoothing =  temp_faces[i].patch;
#endif
	}

	/*
	 * Release temporary buffers
	 */
	BrResFree(temp_faces);

	for(pp = pool_chain; pp; pp = next_pp) {
		next_pp = *pp; 
		BrResFree(pp);
	}

	return model;
}

