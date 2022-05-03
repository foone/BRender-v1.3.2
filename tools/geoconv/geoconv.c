/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: geoconv.c 1.4 1998/09/24 15:53:17 jon Exp $
 * $Locker: $
 *
 * Command line interface for geometry file format convertion
 *
 * $BC<"make -f geoconv.mak %s.obj;">
 */
#define _NO_VECTOR_MACROS
#define BR_MPREP_ALL BR_MODU_ALL

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>

#include "brender.h"
#include "dosio.h"
#include "fmt.h"
#include "oldmodel.h"

#define VERBOSE 0

#define TYPE_GUESS -1

#define WRAP_U  0x40
#define WRAP_V  0x80

/*
 * Known output file types
 */
void OutputModels(char *filename);
void OutputModelsTxt(char *filename);
void OutputModelsC(char *filename);
void OutputMaterials(char *filename);
void OutputMaterialsTxt(char *filename);
void OutputActors(char *filename);
void OutputActorsTxt(char *filename);
void OutputUnimplemented(char *filename);
void OutputASCFile(char *filename);

#if SAVE_3DS
void Output3DSModels(char *filename);
#endif

void OutputIRITModels(char *filename);
old_model * OldModel(br_model *model);
void MergeFromOldModel(old_model *model, br_model *output);
int FwAddEdge(br_uint_16 first, br_uint_16 last);

static struct pm_temp_edge {
	struct pm_temp_edge  *next; /* next in chain */
	br_uint_16 first;            /* First vertex */
	br_uint_16 last;                     /* Last Vertex */
	char other;                     /* Edge is used in other direction */
};

static struct pm_temp_edge *pm_edge_table;
static struct pm_temp_edge **pm_edge_hash;

static char *pm_edge_scratch;
static int num_edges = 0;

br_uint_32 BR_PUBLIC_ENTRY BrFmtIritModelSaveMany(char *name,
		br_model **mtable, br_uint_16 nmodels);

struct {
	char *name;
	void (*function)(char *name);
} OutputFileTypes[] = {

	/*
	 * Brender formats - file extentions
	 */
	{"dat",                 OutputModels},
	{"brm",                 OutputModels},
	{"brt",                 OutputMaterials},
	{"bra",                 OutputActors},
	{"brc",                 OutputUnimplemented},
	{"brl",                 OutputUnimplemented},
	{"brn",                 OutputUnimplemented},

	/*
	 * Brender formats - verbose names
	 */
	{"models",              OutputModels},
	{"materials",   OutputMaterials},
	{"actors",              OutputActors},
	{"animation",   OutputUnimplemented},

	{"txt-models",          OutputModelsTxt},
	{"txt-materials",       OutputMaterialsTxt},
	{"txt-actors",          OutputActorsTxt},
	{"txt-animation",       OutputUnimplemented},

	{"c",                   OutputModelsC},
	{"c-models",    OutputModelsC},
	{"c-materials", OutputUnimplemented},
	{"c-actors",    OutputUnimplemented},

	/*
	 * 3DS formats
	 */
        {"asc",         OutputASCFile},

	/*
	 * Foreign formats
	 */
#if SAVE_3DS
	{"3ds",                 Output3DSModels},
	{"3ds-materials",OutputUnimplemented},
#endif
	{"irit",                OutputIRITModels},
};

/*
 * Use the model->user pointer to save store real flags
 */
#define MODEL_FLAGS(model) (*(br_uint_32 *)(&(model)->user))

/*
 * Current output type
 */
int OutputType = TYPE_GUESS;

/*
 * A hook function used to create unknown materials on demand
 */
br_material * BR_CALLBACK LoadMaterialFFHook(char *name);

/*
 * Known input file types
 */
void InputBrender(char *filename);
void InputNFF(char *filename);
void InputASC(char *filename);

struct {
	char *name;
	void (*function)(char *name);
} InputFileTypes[] = {
	{"txt",         InputBrender},
	{"dat",         InputBrender},
	{"nff",         InputNFF},
	{"asc",         InputASC},
#if 0
	{"3ds",         Input3DS},
#endif
};

/*
 * Current import type
 */
int InputType = TYPE_GUESS;

/*
 * Sorting types
 */
enum output_sort_types {
	SORT_OUT_NONE,
	SORT_OUT_NAME,
};

int OutputSortType = SORT_OUT_NONE;

/*
 * Structure used whilst merging near vertices
 */
struct merge_vertex {
	float a[3];
	int n;
};

struct apply_map {
	int type;
	int axes[2];
};

static int LookupAxis(char *s);

/*
 * Various processing operations invoked from command line parameters
 */
br_uint_32 BR_CALLBACK ModelApplyMaterial(br_model *model, br_material *mat);
br_uint_32 BR_CALLBACK ModelApplyMap(br_model *model, struct apply_map *am);
br_uint_32 BR_CALLBACK ModelAxisRemap(br_model *model, int *axis_map);
br_uint_32 BR_CALLBACK ModelNormalise(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelCentre(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelFlipFaces(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelUnifyNormals(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelMergeVertices(br_model *model, float *tolerance);
br_uint_32 BR_CALLBACK ModelTranslate(br_model *model, float *translate);
br_uint_32 BR_CALLBACK ModelScale(br_model *model, float *scale);
br_uint_32 BR_CALLBACK ModelCleanup(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelSetFlags(br_model *model, int *flags);
br_uint_32 BR_CALLBACK ModelClearFlags(br_model *model, int *flags);
br_uint_32 BR_CALLBACK ModelSetSmoothing(br_model *model, void *dummy);
br_uint_32 BR_CALLBACK ModelRename(br_model *model, char *name);
br_uint_32 BR_CALLBACK MaterialRename(br_material *material, char *name);
br_uint_32 BR_CALLBACK ModelFixWrapping(br_model *model, int *mask);
br_uint_32 BR_CALLBACK ModelFixWrapping3DS(br_model *model, void *dummy);

struct model_list_totals  {
	int vertices;
	int edges;
	int faces;
};

br_uint_32 BR_CALLBACK ModelList(br_model *model, struct model_list_totals *totals);

br_model *ModelMerge(br_model *dest, br_model *src, br_matrix34 *xfm);

br_uint_32 BR_CALLBACK MaterialList(br_material *material, void *dummy);

/*
 * Current pattern
 */
char *CurrentPattern = NULL;

void ViewModel(br_model * model) {
   br_pixelmap * screen, *back;
   br_pixelmap * depth;
   br_actor *world, *object, *light, *camera;
   br_material ** materials;
   br_camera *camera_data;
   br_scalar camera_dist;
   int i;
				  
   world = BrActorAllocate(BR_ACTOR_NONE,NULL);
   camera = BrActorAdd(world,BrActorAllocate(BR_ACTOR_CAMERA,NULL));
   light = BrActorAdd(world,BrActorAllocate(BR_ACTOR_LIGHT,NULL));
   BrLightEnable(light);
   camera->t.type = BR_TRANSFORM_MATRIX34;
   camera_data = (br_camera *)camera->type_data;
   camera_dist = BR_DIV(model->radius, BR_SIN(camera_data->field_of_view/2));
   BrMatrix34Translate(&camera->t.t.mat,BR_SCALAR(0.0),BR_SCALAR(0.0),camera_dist);
   camera_data->hither_z = camera_dist - model->radius;
   camera_data->yon_z = camera_dist + model->radius;
   object = BrActorAdd(world,BrActorAllocate(BR_ACTOR_MODEL,NULL));
   object->model = model;
   object->material = NULL;
   materials = BrMemAllocate((sizeof(*materials) * ((object->model)->nfaces)) , BR_MEMORY_MATERIAL_INDEX);
   ModelSaveMaterials(object->model,materials);
   ModelApplyMaterial(object->model,NULL);
   object->t.type = BR_TRANSFORM_MATRIX34;
   BrMatrix34RotateY(&object->t.t.mat,BR_ANGLE_DEG(30.0));
   screen = DOSGfxBegin("MCGA,W:320,H:200,B:8");
   DOSGfxPaletteSet(BrPixelmapLoad("std.pal"));
   back = BrPixelmapMatch(screen, BR_PMMATCH_OFFSCREEN);
   back->origin_x = back->width /2;
   back->origin_y = back->height /2;
   depth = BrPixelmapMatch(back, BR_PMMATCH_DEPTH_16);
   BrZbBegin(screen->type, BR_PMT_DEPTH_16);
   DOSKeyBegin();
   while (!(DOSKeyTest(SC_SPACE,0,REPT_FIRST_DOWN))) {
      BrPixelmapFill(back,0);
      BrPixelmapFill(depth,0xFFFFFFFF);
      BrMatrix34PreRotateX(&object->t.t.mat,BR_ANGLE_DEG(0.5));
      BrMatrix34PreRotateY(&object->t.t.mat,BR_ANGLE_DEG(0.3));
      BrMatrix34PreRotateZ(&object->t.t.mat,BR_ANGLE_DEG(0.1));
      BrZbSceneRender(world,camera,back,depth);
      BrPixelmapDoubleBuffer(screen,back);
   }
   DOSKeyEnd();
   ModelRestoreMaterials(object->model,materials);
   BrActorRemove(object);
   BrActorRemove(camera);
   BrLightDisable(light);
   BrActorRemove(light);
   BrActorFree(object);
   BrActorFree(camera);
   BrActorFree(light);
   BrActorFree(world);
   BrZbEnd();
   DOSGfxEnd();
   BrPixelmapFree(back);
   BrPixelmapFree(depth);
}


int main(int argc, char **argv)
{
	char    *cp;

	BR_BANNER("GEOCONV","1994-1995","$Revision: 1.4 $");

	/*
	 * Initialise framework
	 */
	BrBegin();

	/*
	 * Process argument list in order
	 */
	while (argv++, --argc) {
		if (**argv == '-') {
			/*
			 * Got one or more flags
			 */
			for (cp = *argv + 1; *cp; cp++) {

				if (strchr("IoOastPrMNSFDLwRv", *cp)) {
					argv++;
					if(--argc == 0)
						BR_ERROR1("The %c option requires an argument", *cp);
				}

				switch (*cp) {

				default:
					BR_ERROR1("unknown option '%c'\n", *cp);
					break;

				/*
				 * Usage
				 */
				case '?':
					fprintf(stdout,
"Usage: geoconv {options}\n"
"Options are treated as commands executed in left-to-right order -\n"
"\n"
"   <input-file>                Load a file into current data\n"
"   -I <input-type>             Set input file type\n"
"   -o <file>                   Generate output file from current data\n"
"   -O <output-type>            Set type of data to generate\n"
"   -n                          Normalise models to radius of 1.0\n"
"   -c                          Centre models on 0,0,0\n"
"   -f                          Flip face normals\n"
"   -F <flag>                   Set or clear model flag\n"
#if 0
"   -u                          Unify face normals\n"
#endif
"   -w <axes>                   Fix wrapped texture coordinates\n"
"   -p                          Remove identical vertices\n"
"   -P <float>                  Merge vertices within a given tolerance\n"
"   -C                          Remove degenerate faces, unused vertices\n"
"                               and duplicate faces\n"
"   -m                          Collapse current data to one actor and one model\n"
"   -r <pattern>                Restrict subsequent operations to things\n" 
"                               matching <pattern>\n"
"   -l                          List current data\n"
"   -g                          Set each model to a different smoothing group\n"
"   -S <sort-type>              Set sorting on output\n"
"   -N <material-name>          Set all models faces to use named material\n"
"   -N default		          	Set all models faces to use default material\n"
"   -M <map-type>,<axis>,<axis> Apply a default U,V mapping to models\n"
"   -s <float>                  Uniform scale applied to models\n"
"   -s <float>,<float>,<float>  Non-uniform scale applied to models\n"
"   -t <float>,<float>,<float>  Translation applied to models\n"
"   -a <axis>,<axis>,<axis>     Remap axes\n"
"   -D <name>                   Rename all models\n"
"   -L <name>                   Rename materials\n"
"   -R <old-name> <new-name>    Rename a model matching <old-name> which may\n"
"                               include wildcards (* and ?), to <new-name>.\n"
"   -v <name>                   View a model using the default material.\n"
);

					fprintf(stdout,
"\n"
"   <input-type> =\n"
"            dat    - P3D/Q3D model files\n"
"            nff    - Eric Haines' Neutral File Format\n"
"            asc    - 3D Studio .ASC file\n"
#if 0
"            3ds    - 3D Studio file\n"
"            geo    - Videoscape-3D (and Argonaut variants)\n"
"            obj    - Wavefront object file\n"
"            dxf    - Autocad DXF Interchange files\n"
"            vue    - 3D Studio VUE animation file\n"
#endif
"\n"
"   If a type is not specified, it will be guessed from the extension\n"
"   of the filename.\n");
					fprintf(stdout,
"\n"
"   <output-type> =\n"
"            models      (.dat or .brm)\n"
"            c-models        Source code for model data structures\n"
"            c-prep-models    Source code for prepared models\n"
#if 0
"            materials  (.brt)\n"
"            actors     (.bra)\n"
"            cameras    (.brc)\n"
"            lights     (.brl)\n"
"            animation  (.brn)\n"
#endif

#if 0 
"            3ds        3D Studio format\n"
"            3ds-materials\n"
#endif
);
					fprintf(stdout,
"\n"
"   <axis> =\n"
"           x y z    - positive input axes\n"
"           +x +y +z - positive input axes\n"
"           -x -y -z - negative input axes\n");
					fprintf(stdout,
"\n"
"   <map-axes> =\n"
"           u \ U    - fix wrapping along u axis\n"
"           v \ V    - fix wrapping along v axis\n"
"           uv \ UV  - fix wrapping along uv axes\n");
					fprintf(stdout,
"\n"
"   <map-type> =\n"
"           none\n"
"           disc\n"
"           plane\n"
"           cylinder\n"
"           sphere\n");
					fprintf(stdout,
"\n"
"   <material-name> =\n"
"           <string>\n"
"           default\n");
					fprintf(stdout,
"\n"
"   <sort-type> =\n"
"           none\n"
"           name\n");
					fprintf(stdout,
"\n"
"   <flag> =\n"
"           +d Set   BR_MODF_DONT_WELD\n"
"           -d Clear BR_MODF_DONT_WELD\n"
"           +o Set   BR_MODF_KEEP_ORIGNAL\n"
"           -o Clear BR_MODF_KEEP_ORIGNAL\n"
"           +t Set   BR_MODF_GENERATE_TAGS\n"
"           -t Clear BR_MODF_GENERATE_TAGS\n"
"           +q Set   BR_MODF_QUICK_UPDATE\n"
"           -q Clear BR_MODF_QUICK_UPDATE\n");

					break;

				/*
				 * -r <pattern>
				 */
				case 'r':
					if(!stricmp(*argv,"*"))
						CurrentPattern = NULL;
					else
						CurrentPattern = *argv;
					break;
				
				/*
				 * -R <name> <new-name>
				 */
				case 'R': {
				   char name[30];
				   char newname[30];
				   br_model * tempmodel;

				   sscanf(*(argv++),"%s",name);
				   if (--argc <= 0) 
				      BR_ERROR0("-R requires two arguments");
				   else {
				      sscanf(*argv,"%s",newname);
				      tempmodel = BrModelFind(name);
				      if (!tempmodel) 
					 BR_ERROR1("No model matching '%s' to rename.",name);
				      else {
					 BrResFree(tempmodel->identifier);
					 tempmodel->identifier = BrMemStrDup(newname);
				      }
				   }
				}
				break;

				/*
				 * -I <input-type>
				 */
				case 'I': {
					int i;

					for(i=0; i < BR_ASIZE(InputFileTypes) ; i++)
						if(!stricmp(*argv,InputFileTypes[i].name))
							break;

					if(i >= BR_ASIZE(InputFileTypes))
						BR_ERROR1("Unknown input type \"%s\"",*argv);

					InputType = i;
					break;
				}

				/*
				 * -O <output-type>
				 */
				case 'O': {
					int i;

					for(i=0; i < BR_ASIZE(OutputFileTypes) ; i++)
						if(!stricmp(*argv,OutputFileTypes[i].name))
							break;

					if(i >= BR_ASIZE(OutputFileTypes))
						BR_ERROR1("Unknown output type \"%s\"",*argv);

					OutputType = i;
					break;
				}

				/*
				 * -o <output_file>
				 */
				case 'o': {
					int i,ot = OutputType;
					char *cp;

					if(ot == TYPE_GUESS && (cp = strrchr(*argv,'.'))) {
						/*
						 * Guess import type based on extension
						 */
						for(i=0; i < BR_ASIZE(OutputFileTypes); i++)
							if(!stricmp(OutputFileTypes[i].name,cp+1))
								ot = i;
					}

					/*
					 * Complain if, by this point, we don't know how to process file
					 */
					if(ot == TYPE_GUESS)
						BR_ERROR0("Unknown output file format");

					OutputFileTypes[ot].function(*argv);
					break;
				}
				/*
				 * -n
				 */
				case 'n':
					BrModelEnum(CurrentPattern,ModelNormalise,NULL);
					break;

				/*
				 * -N <material-type>
				 */
				case 'N': {
					br_material *mat;
					br_material_find_cbfn *old_hook;
		
					if(!stricmp(*argv,"default"))
						mat = NULL;
					else {
						old_hook = BrMaterialFindHook(LoadMaterialFFHook);
						mat = BrMaterialFind(*argv);
						BrMaterialFindHook(old_hook);
					}

					BrModelEnum(CurrentPattern,ModelApplyMaterial,mat);
					break;
				}

				/*
				 * -D <name>
				 */
				case 'D':
					BrModelEnum(CurrentPattern,ModelRename,*argv);
					break;

				/*
				 * -L <name>
				 */
				case 'L':
					BrMaterialEnum(CurrentPattern,MaterialRename,*argv);
					break;

				/*
				 * -c
				 */
				case 'c':
					BrModelEnum(CurrentPattern,ModelCentre,NULL);
					break;

				/*
				 * -f
				 */
				case 'f':
					BrModelEnum(CurrentPattern,ModelFlipFaces,NULL);
					break;

				/*
				 * -u
				 */
				case 'u':
					BrModelEnum(CurrentPattern,ModelUnifyNormals,NULL);
					break;

				/*
				 * -g
				 */
				case 'g': {
					int current = 0;
					BrModelEnum(CurrentPattern,ModelSetSmoothing,&current);
					}
					break;

				/*
				 * -p
				 */
				case 'p': {
					float tolerance = 0.0;

					BrModelEnum(CurrentPattern,ModelMergeVertices,&tolerance);
					break;
				}

				/*
				 * -v
				 */
				case 'v' : {
				  char name[30];
				  br_model * model;

				  sscanf(*argv,"%s",name);
				  
				  model = BrModelFind(name);
				  if (!model)
				    BR_ERROR1("No such model to view: '%s'",name);
				  
				  ViewModel(model);

				  break;
				}
				
				/*
				 * -w
				 */
				case 'w': {
					int mask = 0;
					char *cp;

					for(cp = *argv; *cp; cp++) {
						if(*cp == 'u' || *cp == 'U')
							mask |= WRAP_U;

						if(*cp == 'v' || *cp == 'V')
							mask |= WRAP_V;
					
						if(*cp != 'v' && *cp !='V' && *cp != 'u' && *cp != 'U')
						   BR_ERROR1("Invalid map axis: '%c'",*cp);
					}

					BrModelEnum(CurrentPattern,ModelFixWrapping,&mask);
					break;
				}

				/*
				 * -C
				 */
				case 'C':
					BrModelEnum(CurrentPattern,ModelCleanup,NULL);
					break;
				/*
				 * -P
				 */
				case 'P': {
					float tolerance = atof(*argv);

					BrModelEnum(CurrentPattern,ModelMergeVertices,&tolerance);
					break;
				}

				/*
				 * -m
				 */
				case 'm': {
					br_model **mtable;
					br_model *new;
					int nmodels;
					int i;

					/*
					 * Build a table of all the current models
					 */
					nmodels = BrModelCount(CurrentPattern);
					
					if(nmodels <= 0)
						BR_ERROR0("No models loaded for merge");

					mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);
	
					BrModelFindMany(CurrentPattern,mtable,nmodels);

					/*
					 * Merge all remaining models into first on list
					 */
					for(i=1; i < nmodels; i++) {
						BrModelRemove(mtable[i]);
						ModelMerge(mtable[0],mtable[i],NULL);
						BrModelFree(mtable[i]);
					}
					BrModelPrepare(mtable[0],BR_MPREP_ALL);

					BrMemFree(mtable);

					break;
				}

				/*
				 * -l
				 */
				case 'l': {
					int n;
					struct model_list_totals totals = { 0,0,0};

					n = BrModelCount(CurrentPattern);

					if(n > 0) {
						printf("Models: %d\n",n);
						printf(
"                  Verts Faces Edges DOTQ Radius Bounds\n");

						BrModelEnum(CurrentPattern,ModelList,&totals);

						if(n > 1)
							printf(
"\n TOTAL              %5d %5d %5d\n",
								totals.vertices,
								totals.faces,
								totals.edges
								);
					}

					n = BrMaterialCount(NULL);

					if(n > 0) {
						printf("Materials: %d\n",n);
						BrMaterialEnum(CurrentPattern,MaterialList,NULL);
					}

					break;
				}

				/*
				 * -s <scale>
				 */
				case 's': {
					int i;
					float scale[3];

					i = sscanf(*argv,"%g,%g,%g",
						&scale[0],&scale[1],&scale[2]);

					if(i == 1) {
						scale[2] = scale[0];
						scale[1] = scale[0];
					} else if(i != 3)
						BR_ERROR0("incorrect number of parameters for -s");
					
					BrModelEnum(CurrentPattern,ModelScale,scale);
					break;
				}

				/*
				 * -t <tx>,<ty>,<tz>
				 */
				case 't': {
					int i;
					float translate[3];

					i = sscanf(*argv,"%g,%g,%g",
						&translate[0],&translate[1],&translate[2]);

					if(i != 3)
						BR_ERROR0("incorrect number of parameters to -t");

					BrModelEnum(CurrentPattern,ModelTranslate,translate);
					break;
				}

				/*
				 * -a <axis>,<axis>,<axis>
				 */
				case 'a': {
					char aname[3][3];
					int axes[3];
					int i;

					i = sscanf(*argv,"%2[^,],%2[^,],%2[^,]",aname[0],aname[1],aname[2]);
					if(i != 3)
						BR_ERROR0("Incorrect number of axes");

					for(i=0; i<3; i++)
						axes[i] = LookupAxis(aname[i]);

					BrModelEnum(CurrentPattern,ModelAxisRemap,axes);

					break;
				}

				/*
				 * -M <map-type>,<axis>,<axis>
				 */
				case 'M': {
					int i;
					char map_name[10];
					char aname[2][3];
					struct apply_map am;

					i = sscanf(*argv,"%9[^,],%2[^,],%2[^,]",map_name,aname[0],aname[1]);

					if(i != 3)
						BR_ERROR0("Incorrect number of arguments");

					for(i=0; i<2; i++)
						am.axes[i] = LookupAxis(aname[i]);

					if(am.axes[0] == am.axes[1])
						BR_ERROR0("Mapping axes must be different");

					if(!stricmp("plane",map_name))
						am.type = BR_APPLYMAP_PLANE;
					else if(!stricmp("sphere",map_name))
						am.type = BR_APPLYMAP_SPHERE;
					else if(!stricmp("cylinder",map_name))
						am.type = BR_APPLYMAP_CYLINDER;
					else if(!stricmp("disc",map_name))
						am.type = BR_APPLYMAP_DISC;
					else if(!stricmp("none",map_name))
						am.type = BR_APPLYMAP_NONE;
					else
						BR_ERROR1("Unknown map type '%s'",map_name);

					BrModelEnum(CurrentPattern,ModelApplyMap,&am);
				
					break;
				}
			

				/*
				 * -S <sort-type>
				 */
				case 'S': 
					if(!strcmp(*argv,"none"))
						OutputSortType = SORT_OUT_NONE;
					else if(!strcmp(*argv,"name"))
						OutputSortType = SORT_OUT_NAME;
					else
						BR_ERROR0("unknown sort type");
					break;

				/*
				 * -F <flag>
				 */
				case 'F': {
					int flag_mask = 0;
					char *cp;
					br_model_enum_cbfn *alter_flag;

					/*
					 * Work out if setting or clearing
					 */
					switch((*argv)[0]) {
					case '+':
						alter_flag = ModelSetFlags;
						break;

					case '-':
						alter_flag = ModelClearFlags;
						break;

					default:
						BR_ERROR0("<flag> must start with + or -");
					}

					/*
					 * Work out flag bits
					 */
					if((*argv)[1] == '\0')
						BR_ERROR0("No model flag type");

					for(cp = (*argv)+1; *cp; cp++) {
						switch(toupper(*cp)) {
						case 'D':
							flag_mask |= BR_MODF_DONT_WELD;
							break;
						case 'O':
							flag_mask |= BR_MODF_KEEP_ORIGINAL;
							break;
						case 'T':
							flag_mask |= BR_MODF_GENERATE_TAGS;
							break;
						case 'Q':
							flag_mask |= BR_MODF_QUICK_UPDATE;
							break;

						default:
							BR_ERROR1("Unknown model flag '%c'",*cp);
						}
					}

					/*
					 * Go through models
					 */
					BrModelEnum(CurrentPattern,alter_flag,&flag_mask);
				}
				}
			}
		} else {
			int i,it = InputType;
			char *cp,*input_name = *argv;

			/**
			 ** process input file
			 **/

			if(it == TYPE_GUESS && (cp = strrchr(input_name,'.'))) {
				/*
				 * Guess import type based on extension
				 */
				for(i=0; i < BR_ASIZE(InputFileTypes); i++)
					if(!stricmp(InputFileTypes[i].name,cp+1))
						it = i;
			}

			/*
			 * Complain if, by this point, we don't know how to process file
			 */
			if(it == TYPE_GUESS)
				BR_ERROR0("Unknown input file format");

			InputFileTypes[it].function(input_name);
		}
	}

	/*
	 * Close down and go home
	 */
	BrEnd();

	return 0;
}

static int LookupAxis(char *s)
{
	int i;
	static struct {
		char *name;
		int a;
	} axis_names[] = {
		{"x",BR_FITMAP_PLUS_X},
		{"y",BR_FITMAP_PLUS_Y},
		{"z",BR_FITMAP_PLUS_Z},

		{"+x",BR_FITMAP_PLUS_X},
		{"+y",BR_FITMAP_PLUS_Y},
		{"+z",BR_FITMAP_PLUS_Z},

		{"-x",BR_FITMAP_MINUS_X},
		{"-y",BR_FITMAP_MINUS_Y},
		{"-z",BR_FITMAP_MINUS_Z},
	};

	for(i=0; i< BR_ASIZE(axis_names); i++)
		if(!stricmp(axis_names[i].name,s))
			return axis_names[i].a;

	BR_ERROR1("Unknown axis: '%s'",s);
	return 0;
}

void HideModelFlags(br_model *model)
{
	MODEL_FLAGS(model) = model->flags;

	model->flags = BR_MODF_KEEP_ORIGINAL;
}

void RestoreModelFlags(br_model *model)
{
	model->flags = MODEL_FLAGS(model);
}

/*
 * A finf failed hook for BrmaterialFind - creates a default
 * material of the given name
 *
 * This means that material names will be preserved from input -> output
 */

br_material * BR_CALLBACK LoadMaterialFFHook(char *name)
{
	br_material *mat;

	mat = BrMaterialAllocate(name);

	BrMaterialAdd(mat);

	return mat;
}


void InputNFF(char *filename)
{
	br_model *mp;

#if VERBOSE
	printf("InputNFF(%s)\n",filename);
#endif
	if((mp = BrFmtNFFLoad(filename)) == NULL)
		BR_ERROR1("Could not load file \'%s\'",filename);

	HideModelFlags(mp);

	BrModelAdd(mp);
}

void InputASC(char *filename)
{
	br_model *mtable[1024];
	int nmodels,i;
	br_material_find_cbfn *old_hook;

#if VERBOSE
	printf("InputASC(%s)\n",filename);
#endif
       /*
	 * Hook BrMaterialFind for the duration of the laod so that any
	 * unknnown materials have defaults created
	 */
	old_hook = BrMaterialFindHook(LoadMaterialFFHook);
	nmodels = BrFmtASCLoad(filename,mtable,BR_ASIZE(mtable));
	BrMaterialFindHook(old_hook);

	if(nmodels == 0)
		BR_ERROR1("Could not load any models from '%s'",filename);
		
	for(i=0; i < nmodels; i++)
		HideModelFlags(mtable[i]);

	BrModelAddMany(mtable,nmodels);
}

void InputBrender(char *filename)
{
	br_model *mtable[1024];
	int nmodels,i;
	br_material_find_cbfn *old_hook;

#if VERBOSE
	printf("InputBrender(%s)\n",filename);
#endif

	/*
	 * Hook BrMaterialFind for the duration of the load so that any
	 * unknnown materials have defaults created
	 */
	old_hook = BrMaterialFindHook(LoadMaterialFFHook);
	nmodels = BrModelLoadMany(filename,mtable,BR_ASIZE(mtable));
	BrMaterialFindHook(old_hook);

	if(nmodels == 0)
		BR_ERROR1("Could not load any models from '%s'",filename);
		
	/*
	 * Hide the flags
	 */
	for(i=0; i < nmodels; i++)
		HideModelFlags(mtable[i]);

	BrModelAddMany(mtable,nmodels);
}

static int BR_CALLBACK CompareItemNames( const void * v1, const void * v2)
{
	return strcmp( **(char ***)v1, **(char ***)v2);
}

void SortOutputPointers(void *table,int num)
{
	switch(OutputSortType) {
	case SORT_OUT_NONE:
		/*
		 * Leave table as is
		 */
		break;

	case SORT_OUT_NAME:
		/*
		 * Sort by name
		 */
		BrQsort(table,num,sizeof(char **),CompareItemNames);
		break;
	}
}

void OutputModels(char *filename)
{
	br_model **mtable;
	int nmodels,n,i,j;
	br_model * modelptr;

#if VERBOSE
	printf("OutputModels(%s)\n",filename);
#endif

	nmodels = BrModelCount(CurrentPattern);

	mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);

	nmodels = BrModelFindMany(CurrentPattern,mtable,nmodels);

	SortOutputPointers(mtable,nmodels);

	for(i=0; i < nmodels; i++)
		RestoreModelFlags(mtable[i]);

	j = nmodels / 2;

	// Reverse list of parts since BrModelFindMany returns reversed
	// list as compared to input.
	for(i=0; i < j; i++) {
	   modelptr = mtable[i];
	   mtable[i] = mtable[(nmodels-i)-1];
	   mtable[(nmodels-i)-1] = modelptr;
	}
	
	BrModelSaveMany(filename,mtable,nmodels);

	for(i=0; i < nmodels; i++)
		HideModelFlags(mtable[i]);

	BrMemFree(mtable);
}

void OutputModelsTxt(char *filename)
{
	br_model **mtable;
	int nmodels,n,i;
	int old_mode;

#if VERBOSE
	printf("OutputModelsTxt(%s)\n",filename);
#endif

	nmodels = BrModelCount(CurrentPattern);

	mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);

	nmodels = BrModelFindMany(NULL,mtable,nmodels);

	SortOutputPointers(mtable,nmodels);

	for(i=0; i < nmodels; i++)
		RestoreModelFlags(mtable[i]);

	old_mode = BrWriteModeSet(BR_FS_MODE_TEXT);
	BrModelSaveMany(filename,mtable,nmodels);
	BrWriteModeSet(old_mode);

	for(i=0; i < nmodels; i++)
		HideModelFlags(mtable[i]);

	BrMemFree(mtable);
}

void OutputMaterials(char *filename)
{
	br_material **mtable;
	int nmaterials,n,i,j;
	br_material *matptr;

#if VERBOSE
	printf("OutputMaterials(%s)\n",filename);
#endif
	nmaterials = BrMaterialCount(NULL);

	mtable = BrMemCalloc(sizeof(*mtable),nmaterials,BR_MEMORY_APPLICATION);

	nmaterials = BrMaterialFindMany(NULL,mtable,nmaterials);
	SortOutputPointers(mtable,nmaterials);

	j = nmaterials /2;

	// Reverse list of materials since BrMaterialFindMany returns 
	// reversed list as compared to input.
	for(i=0; i < j; i++) {
	   matptr = mtable[i];
	   mtable[i] = mtable[(nmaterials-i)-1];
	   mtable[(nmaterials-i)-1] = matptr;
	}

	BrMaterialSaveMany(filename,mtable,nmaterials);

	BrMemFree(mtable);
}

void OutputMaterialsTxt(char *filename)
{
	br_material **mtable;
	int nmaterials,n;
	int old_mode;

#if VERBOSE
	printf("OutputMaterials(%s)\n",filename);
#endif
	nmaterials = BrMaterialCount(NULL);

	mtable = BrMemCalloc(sizeof(*mtable),nmaterials,BR_MEMORY_APPLICATION);

	nmaterials = BrMaterialFindMany(CurrentPattern,mtable,nmaterials);
	SortOutputPointers(mtable,nmaterials);

	old_mode = BrWriteModeSet(BR_FS_MODE_TEXT);
	BrMaterialSaveMany(filename,mtable,nmaterials);
	BrWriteModeSet(old_mode);

	BrMemFree(mtable);
}

void OutputIRITModels(char *filename)
{
	br_model **mtable;
	int nmodels,n,i;
	int old_mode;

#if VERBOSE
	printf("OutputIRITModels(%s)\n",filename);
#endif

	nmodels = BrModelCount(CurrentPattern);

	mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);

	nmodels = BrModelFindMany(CurrentPattern,mtable,nmodels);

	SortOutputPointers(mtable,nmodels);

	for(i=0; i < nmodels; i++)
		RestoreModelFlags(mtable[i]);

	BrFmtIritModelSaveMany(filename,mtable,nmodels);

	for(i=0; i < nmodels; i++)
		HideModelFlags(mtable[i]);

	BrMemFree(mtable);
}

void OutputActors(char *filename)
{
#if VERBOSE
	printf("OutputActors(%s)\n",filename);
#endif
}

void OutputActorsTxt(char *filename)
{
#if VERBOSE
	printf("OutputActorsTxt(%s)\n",filename);
#endif
}

void OutputUnimplemented(char *filename)
{
	BR_ERROR0("Unimplemented Output type");
}

/*
 * Produce a 3D-Stduio .ASC file
 */
void OutputASCFile(char *filename)                   
{
        br_model **mtable;
	int nmodels,i,j;
	int a,b,c,ab,bc,ca,flag,smooth;
  	float vx,vy,vz,vu,vv;
	FILE *fp;

	fp=fopen(filename,"w");
        
        nmodels = BrModelCount(CurrentPattern);

	mtable = BrMemCalloc(sizeof(*mtable),nmodels,BR_MEMORY_APPLICATION);

        nmodels = BrModelFindMany(CurrentPattern,mtable,nmodels);         
        
        for(j = 0 ; j < nmodels ; j ++){

        fprintf(fp,"Named object: \"%s\"\n", mtable[j]->identifier);   
        fprintf(fp,"Tri-mesh, Vertices: %d  Faces: %d\n" , mtable[j]->nvertices,mtable[j]->nfaces);
      
        vu = mtable[j]->vertices[0].map.v[0];
        vv = mtable[j]->vertices[0].map.v[1];

        if(vu || vv)  fprintf(fp,"Mapped\n");      
        fprintf(fp,"Vertex list:\n");

        for(i=0 ; i < mtable[j]->nvertices ; i++){
        
           vx = mtable[j]->vertices[i].p.v[0];
           vy = mtable[j]->vertices[i].p.v[1];
           vz = mtable[j]->vertices[i].p.v[2];
           vu = mtable[j]->vertices[i].map.v[0];
           vv = mtable[j]->vertices[i].map.v[1];

           if(vu || vv){
              fprintf(fp,"Vertex %d X: %f  Y: %f  Z: %f  U: %f  V: %f\n" , i ,  vx , vy , vz , vu , vv);
           }
           else{
              fprintf(fp,"Vertex %d X: %f  Y: %f  Z: %f\n" , i ,  vx , vy , vz);
           }
        }

        fprintf(fp,"Face list:\n");
   
        for(i=0 ; i < mtable[j]->nfaces ; i++){

           a = mtable[j]->faces[i].vertices[0];
           b = mtable[j]->faces[i].vertices[1];
           c = mtable[j]->faces[i].vertices[2];

           flag = mtable[j]->faces[i].flags;    

           switch(flag){

              case 0 : ab = 1; bc = 1; ca = 1;
                       break;
              case 1 : ab = 0; bc = 1; ca = 1;
                       break;
              case 2 : ab = 1; bc = 0; ca = 1;
                       break;
              case 3 : ab = 0; bc = 0; ca = 1;
                       break;
              case 4 : ab = 1; bc = 1; ca = 0;
                       break;
              case 5 : ab = 0; bc = 1; ca = 0;
                       break;
              case 6 : ab = 1; bc = 0; ca = 0;
                       break;
              case 7 : ab = 0; bc = 0; ca = 0;
                       break;
           }

           fprintf(fp,"Face %d: A: %d  B: %d  C: %d  AB: %d  BC: %d  CA: %d\n" , i , a , b , c , ab , bc , ca);

           if(mtable[j]->faces[i].material)
           if(mtable[j]->faces[i].material->identifier)
              fprintf(fp,"material: \"%s\" \n" , mtable[j]->faces[i].material->identifier);

           if(!(mtable[j]->faces[i].smoothing == (br_uint_16)~0))
              fprintf(fp,"Smoothing: %d \n" , (int)(log10(mtable[j]->faces[i].smoothing)/log10(2)) +1);
           }       
        }

   	fclose(fp);
	BrMemFree(mtable);
}


#if 0
void PrintMatrix34(br_matrix34 *t)
{
	int i,j;

	for(i=0; i<4; i++) {
		for(j=0; j<3; j++)
			printf("%11.5f ",BrScalarToFloat(t->m[i][j]));
		printf("\n");
	}
}
#endif

br_uint_32 BR_CALLBACK ModelApplyMaterial(br_model *model, br_material *mat)
{
	int i;

	for(i=0; i< model->nfaces; i++)
		model->faces[i].material = mat;

	BrModelPrepare(model,BR_MPREP_ALL);

	return 0;
}

void ModelSaveMaterials(br_model *model, br_material **mat)
{
	int i;

	for(i=0; i< model->nfaces; i++)
		mat[i] = model->faces[i].material; 

	BrModelPrepare(model,BR_MPREP_ALL);
}

void ModelRestoreMaterials(br_model *model, br_material **mat)
{
	int i;

	for(i=0; i< model->nfaces; i++)
		model->faces[i].material = mat[i];

	BrModelPrepare(model,BR_MPREP_ALL);
}

br_uint_32 BR_CALLBACK ModelApplyMap(br_model *model, struct apply_map *am)
{
	br_matrix34 map_xform;

	/*
	 * Generate and apply the mapping transform
	 */
	BrModelFitMap(model,am->axes[0],am->axes[1],&map_xform);
	BrModelApplyMap(model,am->type,&map_xform);

	BrModelPrepare(model,BR_MPREP_ALL);

	return 0;
}

br_uint_32 BR_CALLBACK ModelAxisRemap(br_model *model, int *axis_map)
{
	int i,j;
	br_vertex *vp;
	br_vertex old_v;

#if VERBOSE
	printf("ModelAxisRemap(%s,[%d,%d,%d])\n",model->identifier,
		axis_map[0],axis_map[1],axis_map[2]);
#endif

	for(i=0,vp=model->vertices; i<model->nvertices; i++,vp++) {
		old_v = *vp;

		for(j=0; j<3; j++)
			if(axis_map[j] < 3)
				vp->p.v[j] =  old_v.p.v[axis_map[j]];
			else
				vp->p.v[j] = -old_v.p.v[axis_map[j]-3];
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK  ModelNormalise(br_model *model, void *dummy)
{
	int v;
	br_scalar max_r,r;

#if VERBOSE
	printf("ModelNormalise(%s)\n",model->identifier);
#endif

	max_r = BR_SCALAR(0.0);
	
	for(v = 0; v < model->nvertices; v++) {
		r = BrVector3Length(&model->vertices[v].p);
	
		if(r> max_r)
			max_r = r;
	}
	
	if(max_r == BR_SCALAR(0.0))
		BR_ERROR0("Model has zero radius");

	max_r = BR_DIV(BR_SCALAR(1.0),max_r);

	for(v = 0; v < model->nvertices; v++) {
		BrVector3Scale(&model->vertices[v].p,&model->vertices[v].p,max_r);
	}
	
	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

#if 0

/*
 * Use average of vertices
 */
br_uint_32 BR_CALLBACK ModelCentre(br_model *model, void *dummy)
{
	int v;
	br_scalar fx,fy,fz;
	br_scalar vx,vy,vz;

#if VERBOSE
	printf("ModelCentre(%s)\n",model->identifier);
#endif

	/*
	 * Work out average of vertices, and translate object to there
	 */
	fx = fy = fz = 0.0;
	
	for(v = 0; v < model->nvertices; v++) {
		fx += BrScalarToFloat(model->vertices[v].p.v[0]);
		fy += BrScalarToFloat(model->vertices[v].p.v[1]);
		fz += BrScalarToFloat(model->vertices[v].p.v[2]);
	}
	
	vx = BrFloatToScalar(fx / model->nvertices);
	vy = BrFloatToScalar(fy / model->nvertices);
	vz = BrFloatToScalar(fz / model->nvertices);

	for(v = 0; v < model->nvertices; v++) {
		model->vertices[v].p.v[0] = BR_SUB(model->vertices[v].p.v[0],vx);
		model->vertices[v].p.v[1] = BR_SUB(model->vertices[v].p.v[1],vy);
		model->vertices[v].p.v[2] = BR_SUB(model->vertices[v].p.v[2],vz);
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}
#else
/*
 * Use centre of bounds
 */
br_uint_32 BR_CALLBACK ModelCentre(br_model *model, void *dummy)
{
	int v;
	br_vector3 t;

#if VERBOSE
	printf("ModelCentre(%s)\n",model->identifier);
#endif

	BrVector3Add(&t,&model->bounds.max,&model->bounds.min);
	BrVector3Scale(&t,&t,BR_SCALAR(-0.5));

	for(v = 0; v < model->nvertices; v++) {
		BrVector3Accumulate(&model->vertices[v].p,&t);
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}
#endif

br_uint_32 BR_CALLBACK ModelFlipFaces(br_model *model, void *dummy)
{
	int f,t;

#if VERBOSE
	printf("ModelFlipFaces(%s)\n",model->identifier);
#endif

	/*
	 * Reverse order of vertices in each face
	 */
	for(f = 0; f < model->nfaces; f++) {
		t = model->faces[f].vertices[0];
		model->faces[f].vertices[0] = model->faces[f].vertices[2];
		model->faces[f].vertices[2] = t;
		t = model->faces[f].flags;
		model->faces[f].flags &= BR_FACEF_COPLANAR_1;
		if(t & BR_FACEF_COPLANAR_0)
			model->faces[f].flags |= BR_FACEF_COPLANAR_2;
		if(t & BR_FACEF_COPLANAR_2)
			model->faces[f].flags |= BR_FACEF_COPLANAR_0;
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK ModelUnifyNormals(br_model *model, void *dummy)
{
#if VERBOSE
	printf("ModelUnifyNormals(%s)\n",model->identifier);
#endif

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

/*
 * Flags to ModelFixWrapping()
 */
#define WRAP_U  0x40
#define WRAP_V  0x80

/*
 * Scratch structures used by ModelFixWrapping()
 */
struct wrap_face_info {
	br_uint_8 edge_dir[3];
	br_uint_8 vflags[3];
};

struct wrap_edge_info {
	br_uint_16 old_vertices[2];
	br_uint_16 vertices[2];
	br_uint_8 count;
	br_uint_8 flags;
	br_uint_8 vflags[2];
};

/*
 * Temp. workspace used to build new vertices
 */
static br_vertex *WrapNewVertices;
static int WrapTotalVertices;
static int WrapNextVertex;

/*
 * Adds a new vertex to model that has had it's
 * wrapping fixed
 */
 
static int WrapAddVertex(br_vertex *new_vp)
{
	int i;
	struct br_vertex *vp;

	/*
	 * Try and find vertex in current set
	 */
	vp = WrapNewVertices;

	for(i=0; i < WrapNextVertex; i++, vp++) {
		if( new_vp->p.v[0]   == vp->p.v[0] &&
			new_vp->p.v[1]   == vp->p.v[1] &&
			new_vp->p.v[2]   == vp->p.v[2] &&
			new_vp->map.v[0] == vp->map.v[0] &&
			new_vp->map.v[1] == vp->map.v[1])
			return i;
	}

	/*
	 * Add to end of list
	 */
	if(WrapNextVertex >= WrapTotalVertices)
		BR_ERROR0("WrapAddVertex: no space");

	WrapNewVertices[WrapNextVertex] = *new_vp;

	return WrapNextVertex++;
}

/*
 * ModelFixWrapping()
 *
 * Processes a model and attempts to fix up seams where the texture
 * coordinates implicitly wrap.
 *
 * Takes a pointer to an integer mask of [WRAP_U , WRAP_V]
 * Each bit indicates that wrapping should be explored in that
 * texture axis.
 */

br_uint_32 BR_CALLBACK ModelFixWrapping(br_model *input, int *mask)
{
	old_model * model;
	br_vertex *vp0,*vp1;
	old_face *fp;
	int f,e,e0,e1,v,vw,num_wraps,v0,v1;
	struct wrap_edge_info *edge_info,*eip;
	struct wrap_face_info *face_info,*fip;
	br_vertex tempv;

	model = OldModel(input);
	edge_info = BrMemCalloc(model->nedges,sizeof(*edge_info),BR_MEMORY_APPLICATION);
	face_info = BrMemCalloc(model->nfaces,sizeof(*face_info),BR_MEMORY_APPLICATION);

	/*
	 * Go through faces to build edge table
	 */
	fp = model->faces;
	fip = face_info;
	for(f=0 ; f < model->nfaces; f++, fp++,fip++) {
		for(e0 = 0; e0 < 3; e0++) {
			e1 = (e0 == 2)?0:(e0 + 1);
			eip = edge_info + fp->edges[e0];
			eip->count++;
			if(eip->count == 1) {
				/*
				 * First time edge has been used
				 */
				eip->vertices[0] = fp->vertices[e0];
				eip->vertices[1] = fp->vertices[e1];

				eip->old_vertices[0] = eip->vertices[0];
				eip->old_vertices[1] = eip->vertices[1];
			}

			/*
			 * See which direction edge is used
			 */
			if(eip->vertices[0] == fp->vertices[e0] &&
			   eip->vertices[1] == fp->vertices[e1]) {
				fip->edge_dir[e0] = 0;
			} else if(
			   eip->vertices[0] == fp->vertices[e1] &&
			   eip->vertices[1] == fp->vertices[e0]) {
				fip->edge_dir[e0] = 1;
			} else
				BR_ERROR1("Face %d has a bad edge\n",f);
		}
	}

	/*
	 * For each edge - find if U or V wraps
	 */
	num_wraps = 0;

	eip = edge_info;
	for(e=0; e< model->nedges; e++, eip++) {
		vp0 = model->vertices + eip->vertices[0];
		vp1 = model->vertices + eip->vertices[1];

		if((*mask & WRAP_U) && (BR_ABS(vp0->map.v[0]-vp1->map.v[0]) > BR_SCALAR(0.5))) {
			vw = (vp0->map.v[0] > vp1->map.v[0])?1:0;

			/*
			 * Mark lower vertex as wrapping - keep a count
			 * new wraps
			 */
			if(!(eip->vflags[vw] & WRAP_U)) {
				eip->vflags[vw] |= WRAP_U;
				num_wraps++;
			}
			/*
			 * mark face
			 */
			eip->flags |= WRAP_U;
		}

		if((*mask & WRAP_V) && (BR_ABS(vp0->map.v[1]-vp1->map.v[1]) > BR_SCALAR(0.5))) {
			vw = (vp0->map.v[1] > vp1->map.v[1])?1:0;

			if(!(eip->vflags[vw] & WRAP_V)) {
				eip->vflags[vw] |= WRAP_V;
				num_wraps++;
			}
			/*
			 * mark face
			 */
			eip->flags |= WRAP_V;
		}
	}

	/*
	 * Go through faces and accumulate per face-vertex flags
	 */
	fp = model->faces;
	fip = face_info;

	for(f=0; f< model->nfaces; f++, fp++, fip++) {
		for(e0 = 0; e0 < 3; e0++) {
			e1 = (e0 == 2)?0:(e0 + 1);
			eip = edge_info + fp->edges[e0];

			if(fip->edge_dir[e0])
				v0 = 1, v1 = 0;
			else
				v0 = 0, v1 = 1;

			fip->vflags[e0] |= eip->vflags[v0];
			fip->vflags[e1] |= eip->vflags[v1];
		}
	}

#if 0
	/*
	 * Dump faces, edges and vertices
	 */
	fp = model->faces;
	fip = face_info;

	printf("Before:\n");

	for(f=0; f< model->nfaces; f++, fp++, fip++)
		printf("Face %d V:[%d,%d,%d] E:[%d,%d,%d] DIRN:[%d,%d,%d] VF:[%02X,%02X,%02X]\n",
			f,
			fp->vertices[0],fp->vertices[1],fp->vertices[2],
			fp->edges[0],fp->edges[1],fp->edges[2],
			fip->edge_dir[0],fip->edge_dir[1],fip->edge_dir[2],
			fip->vflags[0],fip->vflags[1],fip->vflags[2]);

	printf("\n");
	eip = edge_info;
	for(e=0; e< model->nedges; e++, eip++)
		printf("Edge: %d V:[%d,%d] C:%d F:%02x VF:[%02X,%02X] OV:[%d,%d]\n",
			e,eip->vertices[0],eip->vertices[1],
			eip->count,eip->flags,
			eip->vflags[0],eip->vflags[1],
			eip->old_vertices[0],eip->old_vertices[1]);

	printf("\n");
	
	vp0 = model->vertices;
	for(v=0; v< model->nvertices; v++, vp0++)
		printf("Vertex %d: P:(%g,%g,%g) M:(%g,%g)\n",
			v,
			BrScalarToFloat(vp0->p.v[0]),BrScalarToFloat(vp0->p.v[1]),BrScalarToFloat(vp0->p.v[2]),
			BrScalarToFloat(vp0->map.v[0]),BrScalarToFloat(vp0->map.v[1]));


	printf("\n");
#endif

	/*
	 * Allocate a new vertex array (may be bigger than required)
	 */
	WrapTotalVertices = model->nvertices + num_wraps;
	WrapNewVertices = BrResAllocate(model,WrapTotalVertices * sizeof(*WrapNewVertices),BR_MEMORY_VERTICES);
	memcpy(WrapNewVertices,model->vertices,sizeof(*WrapNewVertices) * model->nvertices);

	WrapNextVertex = model->nvertices;

	/*
	 * Go through faecs and generate new vertices
	 */
	fp = model->faces;
	fip = face_info;

	for(f=0; f< model->nfaces; f++, fp++, fip++) {
		for(v=0; v < 3; v++) {
			if(!(fip->vflags))
				continue;
			/*
			 * Make a copy of the vertex, and wrap
			 * mapping coordinates
			 */
			tempv = model->vertices[fp->vertices[v]];

			if(fip->vflags[v] & WRAP_U)
				tempv.map.v[0] = tempv.map.v[0] + BR_SCALAR(1.0);
				
			if(fip->vflags[v] & WRAP_V)
				tempv.map.v[1] = tempv.map.v[1] + BR_SCALAR(1.0);

			fp->vertices[v] = WrapAddVertex(&tempv);
		}
	}

#if 0
	/*
	 * Dump faces and edges
	 */
	fp = model->faces;
	fip = face_info;

	printf("After:\n");

	for(f=0; f< model->nfaces; f++, fp++, fip++)
		printf("Face %d V:[%d,%d,%d] E:[%d,%d,%d] DIRN:[%d,%d,%d]\n",
			f,
			fp->vertices[0],fp->vertices[1],fp->vertices[2],
			fp->edges[0],fp->edges[1],fp->edges[2],
			fip->edge_dir[0],fip->edge_dir[1],fip->edge_dir[2]);

	printf("\n");
	eip = edge_info;
	for(e=0; e< model->nedges; e++, eip++)
		printf("Edge: %d V:[%d,%d] C:%d F:%02x VF:[%02X,%02X] OV:[%d,%d]\n",
			e,eip->vertices[0],eip->vertices[1],
			eip->count,eip->flags,
			eip->vflags[0],eip->vflags[1],
			eip->old_vertices[0],eip->old_vertices[1]);

	printf("\n");
	
	vp0 = WrapNewVertices;
	for(v=0; v< WrapNextVertex; v++, vp0++)
		printf("Vertex %d: P:(%g,%g,%g) M:(%g,%g)\n",
			v,
			BrScalarToFloat(vp0->p.v[0]),BrScalarToFloat(vp0->p.v[1]),BrScalarToFloat(vp0->p.v[2]),
			BrScalarToFloat(vp0->map.v[0]),BrScalarToFloat(vp0->map.v[1]));

	printf("\n");

	printf("New Vertices = %d (out of %d) Old = %d\n",WrapNextVertex,WrapTotalVertices,model->nvertices);
#endif
	

	BrMemFree(face_info);
	BrMemFree(edge_info);

	/*
	 * Connect new vertex array to faces
	 */
	BrResFree(model->vertices);

	model->vertices = WrapNewVertices;
	model->nvertices = WrapNextVertex;
	WrapNewVertices = NULL;

	MergeFromOldModel(model,input);
	BrModelPrepare(input,BR_MPREP_ALL);

	BrResFree(model);

	return 0;       
}

/*
 * Take face wrap flags from 3DS info and fix up wrapping
 */
br_uint_32 BR_CALLBACK ModelFixWrapping3DS(br_model *model, void *dummy)
{
	br_face *fp;
	br_vertex tempv;
	int f,v;

	/*
	 * Allocate a new vertex array (bigger than required)
	 */
	WrapTotalVertices = model->nfaces * 3;
	WrapNewVertices = BrResAllocate(model,WrapTotalVertices * sizeof(*WrapNewVertices),BR_MEMORY_VERTICES);
	memcpy(WrapNewVertices,model->vertices,sizeof(*WrapNewVertices) * model->nvertices);

	WrapNextVertex = model->nvertices;

	/*
	 * Go through faces and generate new vertices
	 */
	fp = model->faces;

	for(f=0; f< model->nfaces; f++, fp++) {
		if(!(fp->flags & (WRAP_U | WRAP_V)))
			continue;

		for(v=0; v < 3; v++) {
			/*
			 * Make a copy of the vertex, and wrap
			 * mapping coordinates
			 */
			tempv = model->vertices[fp->vertices[v]];

			if((fp->flags & WRAP_U) &&
				(tempv.map.v[0] < BR_SCALAR(0.5)))
				tempv.map.v[0] = tempv.map.v[0] + BR_SCALAR(1.0);
#if 0
			if((fp->flags & WRAP_V) &&
				(tempv.map.v[1] < BR_SCALAR(0.5)))
				tempv.map.v[1] = tempv.map.v[1] + BR_SCALAR(1.0);
#endif
			fp->vertices[v] = WrapAddVertex(&tempv);
		}
	}

	/*
	 * Connect new vertex array to faces
	 */
	BrResFree(model->vertices);

	model->vertices = WrapNewVertices;
	model->nvertices = WrapNextVertex;
	WrapNewVertices = NULL;

	BrModelPrepare(model,BR_MPREP_ALL);

	return 0;       
}

/*
 * qsort function to compare faces
 */
static int BR_CALLBACK CompareFacePtrs( const void * v1, const void * v2)
{
	const br_face **p1 = (const br_face **)v1,**p2 = (const br_face **)v2;

	if((*p1)->vertices[0] < (*p2)->vertices[0])
		return -1;
	else if((*p1)->vertices[0] > (*p2)->vertices[0])
		return 1;

	else if((*p1)->vertices[1] < (*p2)->vertices[1])
		return -1;
	else if((*p1)->vertices[1] > (*p2)->vertices[1])
		return  1;

	else if((*p1)->vertices[2] < (*p2)->vertices[2])
		return -1;
	else if((*p1)->vertices[2] > (*p2)->vertices[2])
		return  1;

	else if((*p1)->material < (*p2)->material)
		return -1;
	else if((*p1)->material > (*p2)->material)
		return  1;
	else
		return 0;
}

br_uint_32 BR_CALLBACK ModelCleanup(br_model *model, void *dummy)
{
	int i,j,n;
	br_face *fp,*fp_last,**sorted_faces;
	br_vertex *v0,*v1,*v2;
	int *vertex_counts;
	int *vertex_mapping;
	char *face_flags;

#if VERBOSE
	printf("ModelCleanup(%s)\n",model->identifier);
#endif

	/**
	 ** Remove degenerate faces
	 **/
	n = model->nfaces;

	for(i=0, fp=model->faces; i< model->nfaces; ) {
		/*
		 * If any two vertices on the face are the same, remove it
		 *
		 * Compare actual x,y,z values in case of duplicate vertices
		 */
		v0 = model->vertices + fp->vertices[0];
		v1 = model->vertices + fp->vertices[1];
		v2 = model->vertices + fp->vertices[2];

		if((v0->p.v[0] == v1->p.v[0] &&
		    v0->p.v[1] == v1->p.v[1] &&
		    v0->p.v[2] == v1->p.v[2]) ||

		   (v0->p.v[0] == v2->p.v[0] &&
		    v0->p.v[1] == v2->p.v[1] &&
		    v0->p.v[2] == v2->p.v[2]) ||

		   (v2->p.v[0] == v1->p.v[0] &&
		    v2->p.v[1] == v1->p.v[1] &&
		    v2->p.v[2] == v1->p.v[2])) {

			/*
			 * Move face at and of array to this slot (and don't advance
			 * to next face)
			 */
			model->nfaces--;
			*fp = model->faces[model->nfaces];
		} else {
			/*
			 * Advance to next face
			 */
			i++;
			fp++;
		}
	}

	if(n != model->nfaces)
		printf("Remove %d degenerate faces\n",n-model->nfaces);

	/**
	 ** Remove identical faces
	 **/
	sorted_faces = BrMemCalloc(sizeof(*sorted_faces),model->nfaces,BR_MEMORY_APPLICATION);
	face_flags = BrMemCalloc(sizeof(*face_flags),model->nfaces,BR_MEMORY_APPLICATION);
	
	/*
	 * Sort model's faces by vertex numbers and material pointer
	 */
	for(i=0; i< model->nfaces; i++)
		sorted_faces[i] = model->faces+i;

	BrQsort(sorted_faces, model->nfaces, sizeof(*sorted_faces), CompareFacePtrs);

	/*
	 * Go through sorted faces marking those that should be removed
	 */
	fp_last = sorted_faces[0];
	face_flags[0] = 0;

	for(i=1; i< model->nfaces; i++) {
		fp = sorted_faces[i];
	
		face_flags[fp-model->faces] =
			fp->vertices[0] == fp_last->vertices[0] &&
			fp->vertices[1] == fp_last->vertices[1] &&
			fp->vertices[2] == fp_last->vertices[2] &&
			fp->material    == fp_last->material;

		fp_last = fp;
	}

	/*
	 * Strip out identical faces
	 */
	for(i=0,j=0; i< model->nfaces; i++) {
		if(face_flags[i] != 0)
			continue;
		
		model->faces[j] = model->faces[i];

		j++;
	}

	if(model->nfaces > j)
		printf("Remove %d identical faces\n",model->nfaces-j);

	model->nfaces = j;

	BrMemFree(face_flags);
	BrMemFree(sorted_faces);        

	/**
	 ** Remove unused vertices
	 **/

	/*
	 * Make a count of vertex references
	 */
	vertex_counts = BrMemCalloc(sizeof(*vertex_counts),model->nvertices,BR_MEMORY_APPLICATION);
	vertex_mapping = BrMemCalloc(sizeof(*vertex_mapping),model->nvertices,BR_MEMORY_APPLICATION);

	for(i=0, fp=model->faces; i< model->nfaces; i++, fp++) {
		vertex_counts[fp->vertices[0]]++;
		vertex_counts[fp->vertices[1]]++;
		vertex_counts[fp->vertices[2]]++;
	}

	/*
	 * Remove vertices with a count of 0, and make a mapping table
	 * of old->new
	 */
	j = 0;
	for(i=0; i< model->nvertices; i++) {
		if(vertex_counts[i] == 0)
			continue;
		/* 
		 * This vertex is used - move it to new position
		 * and record mapping
		 */
		vertex_mapping[i] = j;
		model->vertices[j] = model->vertices[i];
		j++;
	}

	if(model->nvertices > j)
		printf("Remove %d unreferenced vertices\n",model->nvertices-j);

	model->nvertices = j;
	
	/*
	 * Update faces to use new vertex numbers
	 */
	for(i=0, fp=model->faces; i< model->nfaces; i++, fp++) {
		fp->vertices[0] = vertex_mapping[fp->vertices[0]];
		fp->vertices[1] = vertex_mapping[fp->vertices[1]];
		fp->vertices[2] = vertex_mapping[fp->vertices[2]];
	}

	BrMemFree(vertex_mapping);
	BrMemFree(vertex_counts);

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

/*
 * qsort function to compare merge vertices
 */
static int BR_CALLBACK CompareMergeVertex( const void * v1, const void * v2)
{
	const struct merge_vertex *p1 = v1,*p2 = v2;

	if(p1->a[0] < p2->a[0])
		return -1;
	else if(p1->a[0] > p2->a[0])
		return 1;
	else
		return 0;
}

br_uint_32 BR_CALLBACK ModelMergeVertices(br_model *model, float *pt)
{
	struct br_vertex *vp;
	struct br_face *fp;
	struct merge_vertex *sorted;
	struct merge_vertex *ip,*op;
	int *output;
	int *mapping;
	float t = *pt,mt = -t, sqt = t*t;
	int axis_0,axis_1,axis_2;
	float d0,d1,d2;
	int i,j,otail = 0,ohead = 0;

#if VERBOSE
	printf("ModelMergeVertices(%f)\n",t);
#endif

	sorted = BrMemCalloc(sizeof(*sorted),model->nvertices,BR_MEMORY_APPLICATION);
	output = BrMemCalloc(sizeof(*output),model->nvertices,BR_MEMORY_APPLICATION);
	mapping = BrMemCalloc(sizeof(*mapping),model->nvertices,BR_MEMORY_APPLICATION);
	
	/*
	 * Fix order of axes for the moment - XXX should pick
	 */
	axis_0 = 0;
	axis_1 = 1;
	axis_2 = 2;

	/*
	 * Build a sorted table of the vertices
	 */
	for(i=0, vp = model->vertices, ip =sorted;
		i < model->nvertices;
		i++, ip++, vp++) {
		ip->a[0] = BrScalarToFloat(vp->p.v[axis_0]);
		ip->a[1] = BrScalarToFloat(vp->p.v[axis_1]);
		ip->a[2] = BrScalarToFloat(vp->p.v[axis_2]);
		ip->n = i;
	}

	BrQsort(sorted, model->nvertices, sizeof(*sorted), CompareMergeVertex);

	/*
	 * Do a plane sweep along chosen axis
	 */

	/* 
	 * For each input vertex ...
	 */
	for(i = 0, ip = sorted; i< model->nvertices; i++, ip++) {
		/* 
		 * For each vertex in current window ...
		 */
		for(j = ohead-1; j >= otail; j--) {
			op = sorted+output[j];

			/*
			 * If out of tolerance on sweep axis, then no further
			 * vertices in window will be within tolerance
			 */
			d0 = ip->a[0] - op->a[0];

			if(d0 > t || d0 < mt) {
				otail = j+1;
				break;
			}

			/*
			 * Do remaining axes - and check manhattan distance
			 */                     
			d1 = ip->a[1] - op->a[1];
			if(d1 > t || d1 < mt)
				continue;
	
			d2 = ip->a[2] - op->a[2];
			if(d2 > t || d2 < mt)
				continue;

			/*
			 * Check euclidian distance
			 */
			if(d0*d0 + d1*d1 + d2*d2 > sqt)
				continue;

			/*
			 * Merge input with current output
			 */
			mapping[ip->n] = j;
			goto next_input;
		}


		/*
		 * Add vertex to output set
		 */
		mapping[ip->n] = ohead;
		output[ohead++] = i;

next_input: ;

	}

	if(ohead < model->nvertices) {

		printf("Removed %d vertices\n",model->nvertices-ohead);

		/*
		 * Go through faces and remap vertex numbers
		 */
		for(i=0, fp=model->faces ; i< model->nfaces; i++, fp++) {
			fp->vertices[0] = mapping[fp->vertices[0]];
			fp->vertices[1] = mapping[fp->vertices[1]];
			fp->vertices[2] = mapping[fp->vertices[2]];
		}

		/*
		 * Build new vertex table
		 */
		vp = BrResAllocate(model, sizeof(*vp) * ohead,BR_MEMORY_APPLICATION);

		for(i = 0; i< ohead; i++)
			vp[i] = model->vertices[sorted[output[i]].n];

		BrResFree(model->vertices);
		model->vertices = vp;
		model->nvertices = ohead;
		
	}

	/*
	 * Release workspace
	 */
	BrMemFree(sorted);
	BrMemFree(output);
	BrMemFree(mapping);

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK ModelTranslate(br_model *model, float *translate)
{
	int v;

#if VERBOSE
	printf("ModelTranslate(%s) tx=%f ty=%f tz=%f\n",
		model->identifier,
		translate[0],translate[1],translate[2]);
#endif

	for(v = 0; v < model->nvertices; v++) {
		model->vertices[v].p.v[0] = BR_ADD(model->vertices[v].p.v[0],BrFloatToScalar(translate[0]));
		model->vertices[v].p.v[1] = BR_ADD(model->vertices[v].p.v[1],BrFloatToScalar(translate[1]));
		model->vertices[v].p.v[2] = BR_ADD(model->vertices[v].p.v[2],BrFloatToScalar(translate[2]));
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}                                         

br_uint_32 BR_CALLBACK ModelScale(br_model *model, float *scale)
{
	int v;

#if VERBOSE
	printf("ModelScale(%s) sx=%f sy=%f sz=%f\n",
		model->identifier,
		scale[0],scale[1],scale[2]);
#endif

	for(v = 0; v < model->nvertices; v++) {
		model->vertices[v].p.v[0] = BR_MUL(model->vertices[v].p.v[0],BrFloatToScalar(scale[0]));
		model->vertices[v].p.v[1] = BR_MUL(model->vertices[v].p.v[1],BrFloatToScalar(scale[1]));
		model->vertices[v].p.v[2] = BR_MUL(model->vertices[v].p.v[2],BrFloatToScalar(scale[2]));
	}

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK ModelList(br_model *input, struct model_list_totals *totals)
{
	old_model * model = OldModel(input);
	printf(" %-16s %5d %5d %5d %c%c%c%c %6.5g (%4.3g,%4.3g,%4.3g) to (%4.3g,%4.3g,%4.3g)\n",
		model->identifier?model->identifier:"<NULL>",
		model->nvertices,
		model->nfaces,
		model->nedges,

		(MODEL_FLAGS(model) & BR_MODF_DONT_WELD)?'D':'-',
		(MODEL_FLAGS(model) & BR_MODF_KEEP_ORIGINAL)?'O':'-',
		(MODEL_FLAGS(model) & BR_MODF_GENERATE_TAGS)?'T':'-',
		(MODEL_FLAGS(model) & BR_MODF_QUICK_UPDATE)?'Q':'-',
		BrScalarToFloat(model->radius),
		BrScalarToFloat(model->bounds.min.v[0]),
		BrScalarToFloat(model->bounds.min.v[1]),
		BrScalarToFloat(model->bounds.min.v[2]),
		BrScalarToFloat(model->bounds.max.v[0]),
		BrScalarToFloat(model->bounds.max.v[1]),
		BrScalarToFloat(model->bounds.max.v[2])
);

	/*
	 * If required, add counts to running total
	 */
	if(totals) {
		totals->vertices += model->nvertices;
		totals->edges += model->nedges;
		totals->faces += model->nfaces;
	}

	BrResFree(model);
	return 0;
}

/*
 * Add source model's vertices and faces to destination
 * model - optionally push each of the source models vertices
 * though a supplied transform (if not NULL)
 */
br_model *ModelMerge(br_model *dest, br_model *src, br_matrix34 *xfm)
{
	br_face *new_faces,*dest_fp;
	br_vertex *new_vertices,*dest_vp;
	int i;

#if VERBOSE
	printf("ModelMerge(\"%s\",\"%s\")\n",dest->identifier,src->identifier);
#endif

	/*
	 * make new arrays to hold both model's data
	 */
	new_vertices = BrResAllocate(dest,2 * sizeof(*new_vertices) * (dest->nvertices+src->nvertices), BR_MEMORY_APPLICATION);
	new_faces = BrResAllocate(dest,2* sizeof(*new_faces) * (dest->nfaces+src->nfaces), BR_MEMORY_APPLICATION);
	
	dest_fp = new_faces;
	dest_vp = new_vertices;

	/*
	 * Add first model
	 */
	for(i=0; i < dest->nvertices; i++,dest_vp++)
		*dest_vp = dest->vertices[i];

	for(i=0; i < dest->nfaces; i++,dest_fp++)
		*dest_fp = dest->faces[i];
	
	/*
	 * Add second model
	 */
	for(i=0; i < src->nvertices; i++,dest_vp++)
		*dest_vp = src->vertices[i];

	for(i=0; i < src->nfaces; i++,dest_fp++) {
		*dest_fp = src->faces[i];

		dest_fp->vertices[0] += dest->nvertices;
		dest_fp->vertices[1] += dest->nvertices;
		dest_fp->vertices[2] += dest->nvertices;
	}

	/*
	 * Release old tables
	 */
	if(dest->vertices)
		BrResFree(dest->vertices);

	if(dest->faces)
		BrResFree(dest->faces);

	/*
	 * Change dest model to point at the new merged data
	 */
	dest->vertices = new_vertices;
	dest->nvertices += src->nvertices;

	dest->faces = new_faces;
	dest->nfaces += src->nfaces;

	return dest;
}

br_uint_32 BR_CALLBACK MaterialList(br_material *material, void *dummy)
{
	printf(" %s\n",material->identifier?material->identifier:"<NULL>");

	return 0;
}

br_uint_32 BR_CALLBACK ModelSetFlags(br_model *model, int *flags)
{
	MODEL_FLAGS(model) |= *flags;

	return 0;
}

br_uint_32 BR_CALLBACK ModelClearFlags(br_model *model, int *flags)
{
	MODEL_FLAGS(model) &= ~*flags;

	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK ModelSetSmoothing(br_model *model, int *current_group)
{
	int f,s;

	s = 1 << (*current_group % 16);

	for(f=0; f< model->nfaces; f++)
		model->faces[f].smoothing = s;

	(*current_group)++;
	
	BrModelPrepare(model,BR_MPREP_ALL);
	return 0;
}

br_uint_32 BR_CALLBACK ModelRename(br_model *model, char *name)
{
	if(model->identifier)
		BrResFree(model->identifier);

	if(name)
		model->identifier = BrResStrDup(model,name);
	else
		model->identifier = NULL;
	
	return 0;
}

br_uint_32 BR_CALLBACK MaterialRename(br_material *material, char *name)
{
	if(material->identifier)
		BrResFree(material->identifier);

	if(name)
		material->identifier = BrResStrDup(material,name);
	else
		material->identifier = NULL;
	
	return 0;
}
	   
/*
 * Build edge references for each face
 */
old_model * OldModel(br_model *model)
{
	br_size_t scratch_size;
	old_face *fp;
	int f,g;
	old_model * output;

	output = BrResAllocate(NULL,sizeof(old_model),BR_MEMORY_MODEL);
	
	output->identifier = BrMemStrDup(model->identifier);
	
	output->nvertices = model->nvertices;
	output->vertices = BrResAllocate(output,output->nvertices *
				sizeof(br_vertex),BR_MEMORY_VERTICES);

	for (f=0; f < model->nvertices; f++) {
	  output->vertices[f] = model->vertices[f];
	}

	output->nfaces = model->nfaces;
	output->faces = BrResAllocate(output,output->nfaces *
			     sizeof(old_face),BR_MEMORY_FACES);

	for (f=0; f < model->nfaces; f++) {
	   output->faces[f].material = model->faces[f].material;
	   output->faces[f].smoothing = model->faces[f].smoothing;
	   output->faces[f].flags = model->faces[f].flags;
	   output->faces[f].n = model->faces[f].n;
	   output->faces[f].d = model->faces[f].d;

	   for (g=0; g < 3; g++) {
	      output->faces[f].vertices[g] = model->faces[f].vertices[g];
	   }

	}

	output->pivot = model->pivot;
	output->flags = model->flags;
	output->custom = model->custom;
	output->user = model->user;
	output->radius = model->radius;
	output->bounds = model->bounds;

	scratch_size =
		output->nfaces * 3 * sizeof(*pm_edge_table) +
		output->nvertices * sizeof(*pm_edge_hash);

	pm_edge_scratch = BrScratchAllocate(scratch_size);

	/*
	 * Divvy up scratch area and clear hash pointers
	 */
	pm_edge_hash = (struct pm_temp_edge **)pm_edge_scratch;
	pm_edge_table = (struct pm_temp_edge *)
		(pm_edge_scratch+output->nvertices * sizeof(*pm_edge_hash));

	memset(pm_edge_hash,0,output->nvertices * sizeof(*pm_edge_hash));

	/*
	 * Accumulate shared edges from each face
	 */
	num_edges = 0;
	fp = output->faces;

	for(f = 0; f < output->nfaces; f++, fp++) {
		fp->edges[0] = FwAddEdge(fp->vertices[0],fp->vertices[1]);
		fp->edges[1] = FwAddEdge(fp->vertices[1],fp->vertices[2]);
		fp->edges[2] = FwAddEdge(fp->vertices[2],fp->vertices[0]);
	}

	output->nedges = num_edges;

	BrScratchFree(pm_edge_scratch);

	return output;
}

void MergeFromOldModel(old_model * model, br_model * output)
{
	int f,g;

	output->identifier = BrMemStrDup(model->identifier);

	if (output->nvertices != model->nvertices) {
	   BrResFree(output->vertices);
	   output->nvertices = model->nvertices;
	   output->vertices = BrResAllocate(output,sizeof(br_vertex)*
			       output->nvertices,BR_MEMORY_VERTICES);
	}
   
	for (f=0; f < model->nvertices; f++) {
	  output->vertices[f] = model->vertices[f];
	}

	if (output->nfaces != model->nfaces) {
	   BrResFree(output->faces);
	   output->nfaces = model->nfaces;
	   output->faces = BrResAllocate(output,sizeof(br_face)*
			       output->nfaces,BR_MEMORY_FACES);
	}
	  
      
	for (f=0; f < model->nfaces; f++) {
	   output->faces[f].material = model->faces[f].material;
	   output->faces[f].smoothing = model->faces[f].smoothing;
	   output->faces[f].flags = model->faces[f].flags;
	   output->faces[f].n = model->faces[f].n;
	   output->faces[f].d = model->faces[f].d;

	   for (g=0; g < 3; g++) {
	      output->faces[f].vertices[g] = model->faces[f].vertices[g];
	   }
	}

	output->pivot = model->pivot;
	output->flags = model->flags;
	output->custom = model->custom;
	output->user = model->user;
	output->radius = model->radius;
	output->bounds = model->bounds;
}

static int FwAddEdge(br_uint_16 first, br_uint_16 last)
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
