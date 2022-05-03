/*
 * $BC<"make -f testview.mak %s.obj;">
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <dos.h>

#include "brender.h"
#include "dosio.h"
#include "fmt.h"

#define S0 BR_SCALAR(0.0)
#define S1 BR_SCALAR(1.0)

#define MAX_ARG_SIZE 80

/*
 * linked list structure used to process command line args (@file)
 */

static struct list_node {
        struct list_node *next;
        void *item;
};

void LoadPalette(char *file_name)
{
    int i;
    static br_pixelmap *palette = NULL;

    /* Load palette... */

    if (palette!=NULL)
        BR_WARNING0("Overwriting previous palette");

    palette = BrPixelmapLoad(file_name);
    if (palette==NULL)
        BR_ERROR1("Unable to load palette from '%s'",file_name);

    /* ...and install it */

    DOSGfxPaletteSet(palette);
}

void LoadShadeTable(char *file_name)
{
    br_pixelmap *shade_table[10];
    int i,n;

    /* Load shade tables... */

    n = BrPixelmapLoadMany(file_name,shade_table,10);
    if (!n)
        BR_ERROR1("Failed to load any shade tables from '%s'",file_name);

    /* ...and add them to registry */
    for (i = 0; i<n; i++)
        printf("Added Table '%s'\n",shade_table[i]->identifier);

    BrTableAddMany(shade_table,n);
}

void LoadTexture(char *file_name)
{
    br_pixelmap *texture[100];
    int i,n;
    char *identifier;

    /* Load textures... */

    n = BrPixelmapLoadMany(file_name,texture,100);
    if (!n)
        BR_ERROR1("Failed to load any textures from '%s'",file_name);

    /* ...and add them to registry */

    for (i = 0; i<n; i++) {
        if ((identifier = strtok(NULL,",")) != NULL)
            texture[i]->identifier = identifier;
        printf("Added Map '%s'\n",texture[i]->identifier);
        BrMapAdd(texture[i]);
    }
}

void LoadMaterial(char *file_name)
{
    br_material *material[100];
    int i,n;

    /* Load materials... */

    n = BrMaterialLoadMany(file_name,material,100);
    if (!n)
        BR_ERROR1("Failed to load any materials from '%s'",file_name);

    /* ...and add them to registry */

    for (i = 0; i<n; i++)
        printf("Added Material '%s'\n",material[i]->identifier);

    BrMaterialAddMany(material,n);
}

void LoadMaterialScript(char *file_name)
{
    br_material *material[100];
    int i,n;

    /* Load materials... */

    n = BrFmtScriptMaterialLoadMany(file_name,material,100);
    if (!n)
        BR_ERROR1("Failed to load any materials from '%s'",file_name);

    /* ...and add them to registry */

    for (i = 0; i<n; i++)
        printf("Added Material (Script) '%s'\n",material[i]->identifier);

    BrMaterialAddMany(material,n);
}

void LoadModel(char *file_name)
{
    br_model *model[100];
    int i,n;

    /* Load models... */

    n = BrModelLoadMany(file_name,model,100);
    if (!n)
        BR_ERROR1("Failed to load any models from '%s'",file_name);

    /* ...and add them to registry */

    for (i = 0; i<n; i++)
        printf("Added Model '%s'\n",model[i]->identifier);

    BrModelAddMany(model,n);
}

br_actor* LoadActors(char *file_name)
{
    br_actor *root;

#if 0
    br_actor *actors[100];
    int num_actors;
    int i;

    /* Load as many actors as you find in the file */

    num_actors = BrActorLoadMany(file_name, actors, 100);
    if (num_actors==0) {
        BR_ERROR1("Failed to load any actors from '%s'",file_name);
    }
#else
    root = BrActorLoad(file_name);
    if (root==NULL) {
        BR_ERROR1("Failed to load any actors from '%s'",file_name);
    }
#endif

#if 0
    /* Generate a root actor and put everything under it */

    root = BrActorAllocate (BR_ACTOR_NONE, NULL);
    for (i=0; i<num_actors; i++) {
         BrActorRelink(root,actors[i]);
    }
#endif

    return root;
}

#define MAXFRAMES 100
#define MAXTRANSFORMS 2000

int total_frames;

struct ani_transform {
	br_actor *actor;
	br_matrix34 mat;
};

struct ani_frame {
	struct ani_transform * transforms;
	int ntransforms;
};

struct ani_frame frames[MAXFRAMES];
struct ani_transform transforms[MAXTRANSFORMS];
br_actor *ani_root;

void LoadVUE(char *file_name, br_actor *root)
{
    FILE *file;
    int f = -1,n,i;
    char buff[1024];
    char name[80];
    br_model *model;
    float v[4][3];
	struct ani_transform *t = transforms;

    file = fopen(file_name,"r");

    while (!feof(file)) {
		fgets(buff,1024,file);

		/* Is it a frame specification line? */

		if(sscanf(buff,"frame %d",&i) > 0) {
			if(f > 0)
				frames[f].ntransforms = t - frames[f].transforms;
			f = i;
			frames[f].transforms = t;
		}

		/* Or a transform specification? */

		if (sscanf(buff,
	    	"transform \"%[^\"]\" %f %f %f %f %f %f %f %f %f %f %f %f",
		    name,
		    &v[0][0],&v[0][1],&v[0][2],
		    &v[1][0],&v[1][1],&v[1][2],
		    &v[2][0],&v[2][1],&v[2][2],
		    &v[3][0],&v[3][1],&v[3][2])>0
		) {
	    	int k,l;

		    /* Found a transform so convert it to scalars */

			for (k = 0; k<4; k++)
				for (l = 0; l<3; l++) 
					t->mat.m[k][l] = BrFloatToScalar(v[k][l]);

			t->actor = BrActorSearch(root, name);

			if(t->actor != NULL)
				t++;
		}
	}

    fclose(file);

	if(f > 0)
		frames[f].ntransforms = t - frames[f].transforms;

	total_frames = f+1;
}

/*
 * Internal functions
 */
static char *GetLine(void *fh)
{
        char ch,i,*buffer;
        /*
         * read one line of text from file
         */
        buffer = BrMemAllocate(MAX_ARG_SIZE,BR_MEMORY_STRING);

        if(BrFileGetLine(buffer,MAX_ARG_SIZE,fh) == 0)
                return NULL;

        return buffer;
}

static void Push(struct list_node **head,void *item)
{
        struct list_node *new_rec;
        /*
         * push a pointer to an item onto the head of a list
         */
        new_rec = BrMemAllocate(sizeof(struct list_node),BR_MEMORY_SCRATCH);
        new_rec->next = *head;
        new_rec->item = item;

        *head = new_rec;
}

static void Pop(struct list_node **head,void **dest)
{
        struct list_node *curr_node;
        /*
         * Pop pointer to item from head of list
         */
        curr_node = *head;
        *dest = (*head)->item;
        *head = (*head)->next;
        
        BrMemFree(curr_node);
}

static void ProcessArg(int *argc,char ***argv,int *n,int *flag)
{
        void *fh;
        int open_mode = BR_FS_MODE_TEXT;
        int i,count;
        char *next_line,*sub_arg;
        char **new_argv;
        char *delim = "\n\r ";
        /*
         * Process command line @file
         */
        struct list_node *head=NULL;

        fh = BrFileOpenRead( (*argv)[*n]+1,0,NULL,&open_mode);

        if (fh == NULL)
            BR_ERROR1("Could not open command line file '%s' for reading",
		      (*argv)[*n]);

        for (count = 0;;) {

            /*
             * for each line in @file
             */

            if ((next_line = GetLine(fh)) == NULL)
                    break;

            if (*next_line != '#') {
                sub_arg = strtok(next_line,delim);

                while(sub_arg != NULL) {
                    if (*sub_arg=='#') break;

                    /*
                     * store valid entry and get next
                     */ 

                    Push(&head,sub_arg);
                    count++;
                    
                    sub_arg = strtok(NULL,delim);
                }

                /*
                 * check for comments in @file
                 */

                if (sub_arg != NULL && *sub_arg == '#')
                    BrMemFree(sub_arg);
            } else
                BrMemFree(next_line);
        }

        new_argv = malloc(sizeof(*new_argv)*(count+*argc+2));

        /*
         * generate new argv table of all command line args processed so far
         */

        for (i = 0; i<*n; i++)
                new_argv[i] = (*argv)[i];

        for (i = *n+count-1; i >= *n; i--)
                Pop(&head,&new_argv[i]);

        for (i = *n+1; i < *argc+1; i++)
                new_argv[i+count-1] = (*argv)[i];

        *argc += count-1;

        if (*flag)
            BrMemFree(*argv);
        else
            *flag = 1;

        *argv = new_argv;
        (*n)--;
        
        BrFileClose(fh);
}

br_uint_32 BR_CALLBACK ActorSearchCallback(br_actor *actor, void *arg)
{
	if(strcmp((char *)arg, actor->identifier))
		return BrActorEnum(actor, ActorSearchCallback, arg); 

	return (br_uint_32)actor;
}

char *usage[] = {
    "testview - a BRender model viewer",
    "",
    "-p <palette>\tLoad palette",
    "-h <table>\tLoad shade table",
    "-t <texture>\tLoad texture pixelmap",
    "-s <material>\tLoad material",
    "-S <material>\tLoad material script file",
    "-m <model>\tLoad model",
    "-a <actor>\tLoad actor",
    "-c \"camera\"\tSelect camera",
    "-l \"light\"\tTurn on light",
    "-v <vue-file>\tLoad a .VUE file",
	"-z <float>,<float>\tHither,Yon for default camera",
    0
};

main(int argc,char *argv[])
{
    br_pixelmap *screen,*back_screen,*depth_buffer;
    br_actor *root = NULL;
    br_actor *actors = NULL;
    br_actor *camera = NULL;
    br_actor *light = NULL;
	float default_hither = 1.0;
	float default_yon = 2000.0;
    int n,new_cm;

    if (argc<2) {
        char **ptr = usage;

        while (*ptr)
            fprintf(stderr,"%s\n",*ptr++);

        exit(0);
    }
    
    /* Start BRender */

    BrBegin();

	root = BrActorAllocate(BR_ACTOR_NONE,NULL);

    screen = DOSGfxBegin(NULL);
    if (screen == NULL)
		BR_ERROR("Can't open screen mode");

    back_screen = BrPixelmapMatch (screen,BR_PMMATCH_OFFSCREEN);
	back_screen->origin_x = back_screen->width/2;
	back_screen->origin_y = back_screen->height/2;

    depth_buffer = BrPixelmapMatch (back_screen, BR_PMMATCH_DEPTH_16);
    BrZbBegin (back_screen->type, depth_buffer->type);

    /* Parse arguments */

    /*
     * Process command line to expand @files
     */

    for (n = 1, new_cm = 0; n<argc; n++)
            if (argv[n][0] == '@')
                    ProcessArg(&argc,&argv,&n,&new_cm);

    while (++argv,--argc) {
        if (!strcmp(argv[0],"-p")) {
            if (argc<2)
                BR_ERROR0("Specify palette after -p");
            LoadPalette(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-h")) {
            if (argc<2)
                BR_ERROR0("Specify shade table after -h");
            LoadShadeTable(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-t")) {
            if (argc<2)
                BR_ERROR0("Specify texture file after -t");
            LoadTexture(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-s")) {
            if (argc<2)
                BR_ERROR0("Specify material file after -s");
            LoadMaterial(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-S")) {
            if (argc<2)
                BR_ERROR0("Specify material script file after -S");
            LoadMaterialScript(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-m")) {
            if (argc<2)
                BR_ERROR0("Specify model file after -s");
            LoadModel(argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-a")) {
            if (argc<2)
                BR_ERROR0("Specify actor file after -a");
			actors = LoadActors(argv[1]);
            BrActorAdd(root, actors);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-c")) {
            if (argc<2)
                BR_ERROR0("Specify camera after -c");
            if (root==NULL)
                BR_ERROR0("Can't find camera when no actor loaded");
            camera = (br_actor *)BrActorEnum(root, ActorSearchCallback, argv[1]);
            if (camera==NULL)
                BR_ERROR1("Couldn't find camera '%s'",argv[1]);
            ++argv,--argc;
            continue;
        }
        if (!strcmp(argv[0],"-l")) {
            if (argc<2)
                BR_ERROR0("Specify light after -l");
            if (root==NULL)
                BR_ERROR0("Can't find light when no actor loaded");
            light = (br_actor *)BrActorEnum(root, ActorSearchCallback, argv[1]);
            if (light==NULL)
                BR_ERROR1("Couldn't find light '%s'",argv[1]);
            BrLightEnable(light);
            ++argv,--argc;
            continue;
        }

        if (!strcmp(argv[0],"-v")) {
            if (argc<2)
                BR_ERROR0("Specify .VUE file after -v");
            LoadVUE(argv[1],actors);
            ++argv,--argc;
            continue;
        }

        if (!strcmp(argv[0],"-z")) {
            if (argc<2)
                BR_ERROR0("Specify hither,yon after -z");

            sscanf(argv[1],"%f,%f",&default_hither,&default_yon);

            ++argv,--argc;
            continue;
        }

        BR_ERROR1("Erroneous command line option '%s'",argv[0]);
    }

    if (root==NULL) {
        BR_ERROR0("No actor loaded");
    }

    if (light==NULL) {
        printf ("Making my own light\n");
        light = BrActorAllocate(BR_ACTOR_LIGHT,NULL);
        ((br_light *)light->type_data)->type = BR_LIGHT_POINT;
        ((br_light *)light->type_data)->colour = BR_COLOUR_RGB(255,255,255);
#if 0
        ((br_light *)light->type_data)->attenuation_c = BR_SCALAR(100.0);
        ((br_light *)light->type_data)->attenuation_l = BR_SCALAR(0.1);
        ((br_light *)light->type_data)->attenuation_q = BR_SCALAR(0.1);
#endif
        BrActorAdd(root,light);
        BrLightEnable(light);
    }
        
    
    if (camera==NULL) {
        printf ("Making my own camera\n");
        camera = BrActorAllocate(BR_ACTOR_CAMERA,NULL);
        camera->identifier = "camera";
        ((br_camera *)camera->type_data)->type = BR_CAMERA_PERSPECTIVE;
        ((br_camera *)camera->type_data)->field_of_view = BR_ANGLE_DEG(55.0);
        ((br_camera *)camera->type_data)->hither_z = BrFloatToScalar(default_hither);
        ((br_camera *)camera->type_data)->yon_z = BrFloatToScalar(default_yon);
        ((br_camera *)camera->type_data)->aspect = BR_SCALAR(1.6);
        BrActorAdd(root,camera);
    }

    /* this is the rendering/mouse interface loop */

    DOSKeyBegin();
	DOSMouseBegin();
    {
        float x = 0;
        float y = 0;
        float z = 0;
        long dx = 0;
        long dy = 0;
        unsigned long buttons;
        int flash = 0;
        int current_frame = 0;
		int frame_delta = 0;
        float sens = 0;
        br_matrix34 rotation;
        br_matrix34 delta_rotation;
        br_matrix34 centre;
        br_matrix34 look;
        BrMatrix34Identity (&rotation);
        BrMatrix34Identity (&delta_rotation);
        BrMatrix34Identity (&centre);
        BrMatrix34Identity (&look);

  		for(;;) {

			/*
			 * Key controls
			 */
			if(DOSKeyTest(SC_ESC,0,0))
				break;

			if(DOSKeyTest(SC_Q,0,0))
				break;

			if(DOSKeyTest(SC_MINUS,0,REPT_FIRST_DOWN))
				current_frame--;

			if(DOSKeyTest(SC_EQUALS,0,REPT_FIRST_DOWN))
				current_frame++;

			if(DOSKeyTest(SC_COMMA,0,0))
				frame_delta = -1;

			if(DOSKeyTest(SC_DOT,0,0))
				frame_delta =  1;

			if(DOSKeyTest(SC_SLASH,0,0))
				frame_delta =  0;

			/*
			 * Play any VUE file
			 */
			if(total_frames != 0) {
				int i;

				/*
				 * Clamp frame number to valid range
				 */
				if(current_frame < 0)
					current_frame = total_frames-1;

				if(current_frame >= total_frames)
					current_frame = 0;

				/*
				 * Apply animation to world
				 */
				for(i=0; i < frames[current_frame].ntransforms; i++)
					frames[current_frame].transforms[i].actor->t.t.mat =
						frames[current_frame].transforms[i].mat;

				/*
				 * Move to next frame
				 */
				current_frame += frame_delta;
			}

            BrMatrix34Copy (&camera->t.t.mat, &rotation);
            BrMatrix34PreTranslate (&camera->t.t.mat, BR_SCALAR(x),
				    BR_SCALAR(y), BR_SCALAR(z)); 
            BrMatrix34Post (&camera->t.t.mat, &centre);

            BrMatrix34Copy (&light->t.t.mat, &camera->t.t.mat);
           
            BrPixelmapFill(back_screen,16);
            BrPixelmapFill(depth_buffer,0xffffffff);

            if (flash) {
                flash = 0;
            } else {
                BrZbSceneRender(root,camera,back_screen,depth_buffer);
            }
        
            dx = 0;
            dy = 0;
            DOSMouseRead(&dx,&dy,&buttons);

            sens = sqrt (x*x + y*y + z*z) + 10;

            if (DOSKeyTest(SC_SPACE,0,0)) {
                BrMatrix34Copy (&centre, &camera->t.t.mat);
                BrMatrix34Identity (&rotation);
                x = 0;
                y = 0;
                z = 0;
                flash = 1;              
            } else if (buttons == 0) {
                BrMatrix34RollingBall(&delta_rotation,-dx,dy,800);
                BrMatrix34Pre(&rotation,&delta_rotation);
            } else if (buttons & BR_MSM_BUTTONL) {
                z -= dy*sens/300.0;
                BrMatrix34PreRotateZ (&rotation, BR_ANGLE_DEG(dx/10.0));
            } else if (buttons & BR_MSM_BUTTONR) {
                x -= dx*sens/200.0;
                y += dy*sens/200.0;
            }

            BrPixelmapDoubleBuffer(screen,back_screen);
        }
    }
	DOSMouseEnd();
    DOSKeyEnd();


    DOSGfxEnd();
    BrZbEnd();
    BrEnd();
}



