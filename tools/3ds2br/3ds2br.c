/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: 3ds2br.c 1.2 1998/07/28 14:33:36 jon Exp $
 * $Locker: $
 *
 * 3D-Studio -> BRender command line tool
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "brender.h"

/*
 * Definitions
 */
#define	MAX_FILE_LINE_LEN	(255)
#define SHOW_TOKENS			(FALSE)

typedef int Bool_t;

#ifdef TRUE
#undef TRUE
#endif
#define TRUE  (1)

#ifdef FALSE
#undef FALSE
#endif
#define FALSE (0)

/*
 * Command flags
 */
typedef enum CommandFlag_t {
    cf_none,
    cf_help,
    cf_verbose,
    cf_mod,
    cf_mat,
    cf_scr,
    cf_act,
    cf_flat,
    cf_no_lights,
    cf_no_cameras,
    cf_log,
    cf_hither,
    cf_yon,
    cf_pc,
	cf_scale,
} CommandFlag_t;

/*
 * Command line parser state
 */
typedef struct FlagParseState_t {
    char *command;
	char *input_filename;
    char *mat_filename;
    char *mod_filename;
    char *act_filename;
    char *scr_filename;
    char *log_filename;
	char *hither_string;
	char *yon_string;
	char *scale_string;
    CommandFlag_t last_flag;
	Bool_t verbose;
	br_fmt_options *fmt_options;
	br_3ds_options *options;
} FlagParseState_t;

/*
 * Prototypes
 */
int ParseAtFile(FlagParseState_t *fp_state, char *filename);

/*
 * Globals
 */
FILE *log;

/*
 * Display usage
 */
void DisplayUsage(char *command)
{
    fprintf(stdout,
            "Usage: 3ds2br <input file name> <options>\n"
	    "       [ -h ]"
	    "    Display this message\n"
	    "       [ -v ]"
	    "    Turns verbose mode on\n"
	    "       [ -mod <model file name> ]"
	    "    Save models to file\n" 
	    "       [ -mat <material file name> ]"
	    "    Save materials to file\n" 
	    "       [ -scr <material script file name> ]"
	    "    Save materials to script file\n" 
	    "       [ -act <actor file name> ]"
	    "    Save actor hierarchy to file\n" 
	    "       [ -log <log file name> ]"
	    "    Save log to file\n"
        "       [ -flat ]"
        "    Ignore keyframer hierarchy and build a flat one instead\n"
        "       [ -nopivot ]"
        "    Ignore keyframer pivot point\n"
        "       [ -nomatrix ]"
        "    Ignore mesh matrix\n"
        "       [ -noaxis ]"
        "    Do not remap axes to correspond to 3DS user interface\n"
	    "       [ -nl ]"
	    "    Replace lights with dummy actors\n"
	    "       [ -nc ]"
	    "    Replace cameras with dummy actors\n"
	    "       [ -hither <distance > ]"
	    "    Sets all camera hither distances to this value\n"
	    "       [ -yon <distance > ]"
	    "    Sets all camera yon distances to this value\n"
        "       [ -pc ]"
        "    Set perspective correction for all textured materials\n"
        "       [ -scale <factor> ]"
        "    Apply scaling to models and hierarchy\n"
        "       [ -modelpivot ]"
        "    Use model pivot points instead of moving vertices\n",
        command); 
}

/*
 *
 */
Bool_t ConditionalSetString(char **pointer, char *arg, char *err_msg)
{
    if (*pointer != NULL) {
        printf(err_msg, arg);
        return FALSE;
    } else {
        *pointer = strdup(arg);
        if (*pointer == NULL) {
            printf("Out of memory!\n");
            return FALSE;
        }
    }
    return TRUE;
}

/*
 *
 */
Bool_t NextToken(FlagParseState_t *fp_state, char *tok_string)
{
    Bool_t is_ok = TRUE;

    switch (fp_state->last_flag) {
    case cf_none:
        switch (tok_string[0]) {
        case '\0':
            is_ok = FALSE;
            break;
        case '@':
            is_ok = ParseAtFile(fp_state, tok_string + 1);
            fp_state->last_flag = cf_none;
            break;
        case '-':
	    if (!strcmp(tok_string, "-h")) {
#if SHOW_TOKENS
		printf("Help flag found\n");
#endif
		DisplayUsage(fp_state->command);
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-v")) {
#if SHOW_TOKENS
		printf("Verbose flag found\n");
#endif
		fp_state->verbose = TRUE;

		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-mod")) {
		fp_state->last_flag = cf_mod;
	    } else if (!strcmp(tok_string, "-mat")) {
		fp_state->last_flag = cf_mat;
	    } else if (!strcmp(tok_string, "-scr")) {
		fp_state->last_flag = cf_scr;
	    } else if (!strcmp(tok_string, "-act")) {
		fp_state->last_flag = cf_act;
	    } else if (!strcmp(tok_string, "-log")) {
		fp_state->last_flag = cf_log;
	    } else if (!strcmp(tok_string, "-flat")) {
#if SHOW_TOKENS
                printf("Flat hierarchy flag: '%s'\n", tok_string);
#endif
		fp_state->options->flags |= BR_3DS_FLAT_HIERARCHY;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-nopivot")) {
		fp_state->options->flags &= ~BR_3DS_APPLY_PIVOT;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-modelpivot")) {
		fp_state->options->flags |= BR_3DS_USE_MODEL_PIVOT;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-nomatrix")) {
		fp_state->options->flags &= ~BR_3DS_APPLY_MESH_MATRIX;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-noaxis")) {
		fp_state->options->flags &= ~BR_3DS_CORRECT_AXES;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-nl")) {
#if SHOW_TOKENS
                printf("No lights flag: '%s'\n", tok_string);
#endif
		fp_state->options->flags |= BR_3DS_DUMMY_LIGHTS;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-nc")) {
#if SHOW_TOKENS
                printf("No cameras flag: '%s'\n", tok_string);
#endif
		fp_state->options->flags |= BR_3DS_DUMMY_CAMERAS;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-pc")) {
#if SHOW_TOKENS
                printf("Perspective correction flag found\n");
#endif
		fp_state->options->flags |= BR_3DS_SET_PERSPECTIVE;
		fp_state->last_flag = cf_none;
	    } else if (!strcmp(tok_string, "-hither")) {
		fp_state->last_flag = cf_hither;
	    } else if (!strcmp(tok_string, "-yon")) {
		fp_state->last_flag = cf_yon;
	    } else if (!strcmp(tok_string, "-scale")) {
		fp_state->last_flag = cf_scale;
	    } else {
                BR_FAILURE1("Unrecognised flag '%s'", tok_string);
		is_ok = FALSE;
	    }
            break;
        default:
#if SHOW_TOKENS
            BR_FAILURE1("Unannounced argument: '%s'", tok_string);
#endif
	    is_ok = ConditionalSetString(
	        &(fp_state->input_filename),
		tok_string, "Extra input filename %s\n");
	    fp_state->last_flag = cf_none;
	    break;
	}
	break;
    case cf_mod:
#if SHOW_TOKENS
        printf("Mod flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->mod_filename),
            tok_string, "Extra mod file name %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_mat:
#if SHOW_TOKENS
        printf("Mat flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->mat_filename),
            tok_string, "Extra mat file name %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_scr:
#if SHOW_TOKENS
        printf("Scr flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->scr_filename),
            tok_string, "Extra scr file name %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_act:
#if SHOW_TOKENS
        printf("act flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->act_filename),
            tok_string, "Extra act file name %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_log:
#if SHOW_TOKENS
        printf("log flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->log_filename),
            tok_string, "Extra log filename option %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_hither:
#if SHOW_TOKENS
        printf("hither flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->hither_string),
            tok_string, "Extra hither option %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_yon:
#if SHOW_TOKENS
        printf("yon flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->yon_string),
            tok_string, "Extra yon option %s\n");
	fp_state->last_flag = cf_none;
	break;
    case cf_scale:
#if SHOW_TOKENS
        printf("scale flag: '%s'\n", tok_string);
#endif
	is_ok = ConditionalSetString(
	    &(fp_state->scale_string),
            tok_string, "Extra scale option %s\n");
	fp_state->last_flag = cf_none;
	break;
    default:
	BR_FAILURE("Internal error: unexpected parser state");
	is_ok = FALSE;
	break;
    }
    return is_ok;
}

/*
 * Parse command string
 */
Bool_t ParseCmdString(FlagParseState_t *fp_state, char *cmd_string)
{
    Bool_t is_ok = TRUE;
    char *p, *arg_begin, tmp_c;
    
    p = cmd_string;
    while (is_ok && *p != '\0') {
        while (isspace(*p)) {
           p++;
        }
        arg_begin = p;
	while (*p != '\0' && !isspace(*p)) {
           p++;
        }
        if (*arg_begin != '\0') {
            tmp_c = *p;
            *p = '\0';
            is_ok = NextToken(fp_state, arg_begin);
            *p = tmp_c;
        }
    }
    return is_ok;
}

/*
 * Parse from a file
 */
Bool_t ParseAtFile(FlagParseState_t *fp_state, char *filename)
{
    Bool_t is_ok = TRUE;
    FILE *fp;
    char *line_buffer;
    int line_buffer_length;

    line_buffer_length = MAX_FILE_LINE_LEN;
    line_buffer = malloc(line_buffer_length);
    if (line_buffer == NULL) {
        is_ok = FALSE;
    }
    if (is_ok) {
	fp = fopen(filename, "r");
	if (fp == NULL) {
		BR_FAILURE1("Cannot open file '%s'", filename);
	    is_ok = FALSE;
	}
    }
    while (is_ok && !ferror(fp) && !feof(fp)) {
	if (fgets(line_buffer, line_buffer_length, fp) != NULL){
            /* Insert checks for buffer overrun here */
            ParseCmdString(fp_state, line_buffer);
        }
    }
    if (line_buffer != NULL) {
        free(line_buffer);
    }
    return is_ok;
}

/*
 * Parse command line
 */
Bool_t ParseArgv(FlagParseState_t *fp_state, char *argv[])
{
    Bool_t is_ok = TRUE;

    while (is_ok && *argv != NULL) { /* This is ANSI standard now */
        is_ok = NextToken(fp_state, *argv);
        argv++;
    }
    return is_ok;
}

/*
 * Message report callback
 */
void BR_CALLBACK ReportCallback(char *string)
{
	fprintf(log, string);
}

/*
 * Main
 */
int main(int argc, char **argv)
{
	br_fmt_options fmt_options;
	br_3ds_options options;
	br_fmt_results *results;
    FlagParseState_t fp_state;
    Bool_t is_ok = TRUE;

	BrBegin();

	BR_BANNER("3DS2BR","1996","$Revision: 1.2 $");

	fp_state.verbose = FALSE;
    fp_state.input_filename = NULL;

	options.flags = BR_3DS_APPLY_PIVOT | BR_3DS_APPLY_MESH_MATRIX | BR_3DS_CORRECT_AXES;
	options.report = NULL;

    fp_state.mod_filename = NULL;
    fp_state.mat_filename = NULL;
    fp_state.scr_filename = NULL;
    fp_state.act_filename = NULL;
    fp_state.log_filename = NULL;

    log = NULL;

    options.hither = BR_SCALAR(1.0);
    fp_state.hither_string = NULL;

    options.yon = BR_SCALAR(2000.0);
    fp_state.yon_string = NULL;

	options.scale = BR_SCALAR(1.0);
	fp_state.scale_string = NULL;

    fp_state.command = argv[0];
    fp_state.options = &options;
    fp_state.last_flag = cf_none;
    is_ok = ParseArgv(&fp_state, argv +1);
    if (is_ok && fp_state.input_filename == NULL) {
		DisplayUsage(argv[0]);
        BR_FAILURE("No input filename specified");
    }
    if (is_ok && fp_state.hither_string != NULL) {
		options.flags |= BR_3DS_SET_HITHER;
        options.hither = BrFloatToScalar(atof(fp_state.hither_string));
        /* FIXME: atof has no error checking.  We could do with some. */
    }
    if (is_ok && fp_state.yon_string != NULL) {
		options.flags |= BR_3DS_SET_YON;
        options.yon = BrFloatToScalar(atof(fp_state.yon_string));
        /* FIXME: atof has no error checking.  We could do with some. */
    }
    if (is_ok && fp_state.scale_string != NULL) {
		options.flags |= BR_3DS_APPLY_SCALE;
        options.scale = BrFloatToScalar(atof(fp_state.scale_string));
        /* FIXME: atof has no error checking.  We could do with some. */
    }
    if (is_ok) {
        if (fp_state.log_filename != NULL) {
            log = fopen(fp_state.log_filename, "w");
	    	if (log == NULL) {
	        	BR_FAILURE1("Couldn't open log file '%s'\n",fp_state.log_filename);
            }
		} else {
            log = stdout;
        }
    }

	if (fp_state.verbose) {
		options.report = ReportCallback;
	}

	fmt_options.convert_flags = 0;

	if((fp_state.mat_filename != NULL) || (fp_state.scr_filename != NULL))
		fmt_options.convert_flags |= BR_FMT_MATERIALS | BR_FMT_PIXELMAPS;

	if(fp_state.act_filename != NULL)
		fmt_options.convert_flags |= BR_FMT_ACTORS | BR_FMT_MODELS | BR_FMT_MATERIALS;

	if(fp_state.mod_filename != NULL)
		fmt_options.convert_flags |= BR_FMT_MODELS | BR_FMT_MATERIALS;

	results = BrFmt3DSLoad(fp_state.input_filename, &fmt_options, &options);

	if(results == NULL) BR_FAILURE1("Cannot convert file '%s'",fp_state.input_filename);

	if(results->nmaterials > 0) {
		if(fp_state.mat_filename != NULL) {
			BrMaterialSaveMany(fp_state.mat_filename, results->materials, results->nmaterials);
			fprintf(log, "Saved all %d materials to file '%s'\n", results->nmaterials, fp_state.mat_filename);
		}
		if(fp_state.scr_filename != NULL) {
			BrFmtScriptMaterialSaveMany(fp_state.scr_filename, results->materials, results->nmaterials);
			fprintf(log, "Saved all %d materials to file '%s' as scripts\n", results->nmaterials, fp_state.scr_filename);
		}
	}

	if(results->nactors == 1) {
		BrActorSave(fp_state.act_filename, results->actors[0]);
		fprintf(log, "Saved actor hierarchy to file '%s'\n", fp_state.act_filename);
	}

	if(results->nmodels > 0) {
		BrModelSaveMany(fp_state.mod_filename, results->models, results->nmodels);
			fprintf(log, "Saved all %d models to file '%s'\n", results->nmodels, fp_state.mod_filename);
	}

	BrResFree(results);
	BrEnd();
    
    return 0;
}
