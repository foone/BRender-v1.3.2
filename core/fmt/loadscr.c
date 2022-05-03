/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadscr.c 1.2 1998/03/05 20:23:50 jon Exp $
 * $Locker: $
 *
 * Load a Material Script file
 */
#include "brddi.h"

BR_RCS_ID("$Id: loadscr.c 1.2 1998/03/05 20:23:50 jon Exp $")

/*
 * Default material fields
 */
br_material _DefaultScriptMaterial = {
	0,
	NULL,
	BR_COLOUR_RGB(255,255,255),	/* colour			*/
	255,						/* opacity			*/

	BR_UFRACTION(0.10),			/* Indexed ka		*/
	BR_UFRACTION(0.70),			/*         kd		*/
	BR_UFRACTION(0.0),			/*         ks		*/
	
	BR_SCALAR(20),				/* power			*/
	BR_MATF_LIGHT,				/* flags			*/
	{{
		BR_VECTOR2(1,0),		/* map transform	*/
		BR_VECTOR2(0,1),
		BR_VECTOR2(0,0),
	}},
	BR_MATM_DEPTH_TEST_LE |
	BR_MATM_BLEND_MODE_STANDARD |
	BR_MATM_MAP_WIDTH_LIMIT_WRAP |
	BR_MATM_MAP_HEIGHT_LIMIT_WRAP,	/* mode				*/
	0,63,						/* index base/range	*/
};

enum {
	/*
	 * Keywords
	 */
	T_MATERIAL = T_KEYWORD,

	/*
	 * Members of material
	 */
	T_IDENTIFIER,
	T_NAME,
	T_FLAGS,
	T_COLOUR,
	T_OPACITY,
	T_AMBIENT,
	T_KA,
	T_DIFFUSE,
	T_KD,
	T_SPECULAR,
	T_KS,
	T_POWER,
	T_MAP_TRANSFORM,
	T_INDEX_BASE,
	T_INDEX_RANGE,
	T_COLOUR_MAP,
	T_SCREEN_DOOR,
	T_INDEX_SHADE,
	T_INDEX_BLEND,
    T_FOG_MIN,
    T_FOG_MAX,
    T_FOG_COLOUR,
    T_INDEX_FOG,
    T_SUBDIVIDE_TOLERANCE,
    T_DEPTH_TEST,
    T_BLEND_MODE,
    T_MAP_WIDTH_LIMIT,
    T_MAP_HEIGHT_LIMIT,

	/*
	 * Material flags
	 */
	T_LIGHT,
	T_PRELIT,
	T_SMOOTH,
	T_GOURAUD,
	T_ENVIRONMENT,
	T_ENVIRONMENT_I,
	T_ENVIRONMENT_LOCAL,
	T_ENVIRONMENT_L,
	T_PERSPECTIVE,
	T_DECAL,
	T_ALWAYS_VISIBLE,
	T_TWO_SIDED,
	T_FORCE_FRONT,
	T_FORCE_BACK,
	T_DITHER,
	T_EXTRA_SURFACE,
	T_EXTRA_PRIMITIVE,
    T_MAP_ANTIALIASING,
    T_MAP_INTERPOLATION,
    T_MIP_INTERPOLATION,
    T_FOG_LOCAL,
    T_SUBDIVIDE,
	T_QUAD_MAPPING,
	T_INHIBIT_DEPTH_WRITE,

	/*
	 * Depth test modes
	 */
	T_GT,
	T_GE,
	T_EQ,
	T_NE,
	T_LE,
	T_LT,
	T_NV,
	T_AL,

	/*
	 * Blend modes
	 */
	T_STANDARD,
	T_SUMMED,
	T_DIMMED,
	T_PREMULTIPLIED,

	/*
	 * Width and height limit modes
	 */
	T_WRAP,
	T_CLAMP,
	T_MIRROR,
};

static br_lexer_keyword keywords[] = {
    { "material",                   T_MATERIAL,             },
    { "identifier",                 T_IDENTIFIER,           },
    { "name",                       T_NAME,                 },
    { "flags",                      T_FLAGS,                },
    { "colour",                     T_COLOUR,               },
    { "opacity",                    T_OPACITY,              },
    { "ambient",                    T_AMBIENT,              },
    { "ka",                         T_KA,                   },
    { "diffuse",                    T_DIFFUSE,              },
    { "kd",                         T_KD,                   },
    { "specular",                   T_SPECULAR,             },
    { "ks",                         T_KS,                   },
    { "power",                      T_POWER,                },
    { "map_transform",              T_MAP_TRANSFORM,        },
    { "index_base",                 T_INDEX_BASE,           },
    { "index_range",                T_INDEX_RANGE,          },
    { "colour_map",                 T_COLOUR_MAP,           },
    { "screen_door",                T_SCREEN_DOOR,          },
    { "index_shade",                T_INDEX_SHADE,          },
    { "index_blend",                T_INDEX_BLEND,          },
    { "depth_test",                 T_DEPTH_TEST,           },
    { "blend_mode",                 T_BLEND_MODE,           },
    { "map_width_limit",            T_MAP_WIDTH_LIMIT,      },
    { "map_height_limit",           T_MAP_HEIGHT_LIMIT,     },
    { "light",                      T_LIGHT,                },
    { "prelit",                     T_PRELIT,               },
    { "smooth",                     T_SMOOTH,               },
    { "gouraud",                    T_GOURAUD,              },
    { "environment",                T_ENVIRONMENT,          },
    { "environment_i",              T_ENVIRONMENT_I,        },
    { "environment_local",          T_ENVIRONMENT_LOCAL,    },
    { "environment_l",              T_ENVIRONMENT_L,        },
    { "perspective",                T_PERSPECTIVE,          },
    { "decal",                      T_DECAL,                },
    { "always_visible",             T_ALWAYS_VISIBLE,       },
    { "two_sided",                  T_TWO_SIDED,            },
    { "force_z_0",                  T_FORCE_FRONT,          },
    { "force_front",                T_FORCE_FRONT,          },
    { "force_back",                 T_FORCE_BACK,           },
    { "dither",                     T_DITHER,               },
    { "extra_surface",              T_EXTRA_SURFACE,        },
    { "extra_primitive",            T_EXTRA_PRIMITIVE,      },
    { "map_antialiasing",           T_MAP_ANTIALIASING,     },
    { "map_interpolation",          T_MAP_INTERPOLATION,    },
    { "mip_interpolation",          T_MIP_INTERPOLATION,    },
    { "fog_local",                  T_FOG_LOCAL,            },
    { "fog_min",                    T_FOG_MIN,              },
    { "fog_max",                    T_FOG_MAX,              },
    { "fog_colour",                 T_FOG_COLOUR,           },
    { "index_fog",                  T_INDEX_FOG,            },
    { "subdivide_tolerance",        T_SUBDIVIDE_TOLERANCE,  },
    { "subdivide",                  T_SUBDIVIDE,            },
    { "quad_mapping",               T_QUAD_MAPPING,         },
    { "inhibit_depth_write",        T_INHIBIT_DEPTH_WRITE,  },
	{ "gt",                         T_GT                    },
	{ "ge",                         T_GE                    },
	{ "eq",                         T_EQ                    },
	{ "ne",                         T_NE                    },
	{ "le",                         T_LE                    },
	{ "lt",                         T_LT                    },
	{ "nv",                         T_NV                    },
	{ "al",                         T_AL                    },
	{ "greater",                    T_GT                    },
	{ "greater_or_equal",           T_GE                    },
	{ "equal",                      T_EQ                    },
	{ "not_equal",                  T_NE                    },
	{ "less_or_equal",              T_LE                    },
	{ "less",                       T_LT                    },
	{ "never",                      T_NV                    },
	{ "always",                     T_AL                    },
	{ "standard",                   T_STANDARD,             },
	{ "summed",                     T_SUMMED,               },
	{ "dimmed",                     T_DIMMED,               },
	{ "premultiplied",              T_PREMULTIPLIED,        },
	{ "wrap",                       T_WRAP,                 },
	{ "clamp",                      T_CLAMP,                },
	{ "mirror",                     T_MIRROR,               },
};

static void BrLexerPrintPos(struct br_lexer *l)
{
	char pos[256];

	BrLexerPosition(l,pos, BR_ASIZE(pos));
        BR_WARNING1("In file %s", pos);
}

STATIC br_uint_32 ParseMatFlags(br_lexer *l)
{
	br_uint_32 f = 0;

	BrLexerExpect(l,T_LSQUARE);

	if(BrLexerCurrent(l) == T_RSQUARE) {
		BrLexerAdvance(l);
		return 0;
	}

	/*
	 * Read flag keywords until ]
	 */
	for(;;) {
		switch(BrLexerCurrent(l)) {
        case T_LIGHT:                   f |= BR_MATF_LIGHT;                     break;
        case T_PRELIT:                  f |= BR_MATF_PRELIT;                    break;
		case T_SMOOTH:
        case T_GOURAUD:                 f |= BR_MATF_SMOOTH;                    break;
		case T_ENVIRONMENT_I:
        case T_ENVIRONMENT:             f |= BR_MATF_ENVIRONMENT_I;             break;
		case T_ENVIRONMENT_L:
        case T_ENVIRONMENT_LOCAL:       f |= BR_MATF_ENVIRONMENT_L;             break;
        case T_PERSPECTIVE:             f |= BR_MATF_PERSPECTIVE;               break;
        case T_DECAL:                   f |= BR_MATF_DECAL;                     break;
        case T_ALWAYS_VISIBLE:          f |= BR_MATF_ALWAYS_VISIBLE;            break;
        case T_TWO_SIDED:               f |= BR_MATF_TWO_SIDED;                 break;
        case T_FORCE_FRONT:             f |= BR_MATF_FORCE_FRONT;               break;
        case T_FORCE_BACK:              f |= BR_MATF_FORCE_BACK;                break;
        case T_DITHER:                  f |= BR_MATF_DITHER;                    break;
        case T_MAP_ANTIALIASING:        f |= BR_MATF_MAP_ANTIALIASING;          break;
        case T_MAP_INTERPOLATION:       f |= BR_MATF_MAP_INTERPOLATION;         break;
        case T_MIP_INTERPOLATION:       f |= BR_MATF_MIP_INTERPOLATION;         break;
        case T_FOG_LOCAL:               f |= BR_MATF_FOG_LOCAL;                 break;
        case T_SUBDIVIDE:               f |= BR_MATF_SUBDIVIDE;                 break;
        case T_QUAD_MAPPING:            f |= BR_MATF_QUAD_MAPPING;              break;
        case T_INHIBIT_DEPTH_WRITE:     f |= BR_MATF_INHIBIT_DEPTH_WRITE;       break;
		default:
			BrLexerError(l, "Unknown material flag");
		}
		BrLexerAdvance(l);

		if(BrLexerCurrent(l) == T_RSQUARE)
			break;

		BrLexerExpect(l,T_COMMA);
	}

	BrLexerExpect(l,T_RSQUARE);

	return f;
}

STATIC br_material *ParseMaterial(br_lexer *l)
{
	br_material *mat;
	br_vector3 v3;
#if 0 
	br_token_value token_buffer[512];
	int i;
#endif

	mat = BrMaterialAllocate(NULL);
	*mat = _DefaultScriptMaterial;

	/*
	 * material = [
	 */
	BrLexerExpect(l,T_MATERIAL);
	BrLexerExpect(l,T_EQUAL);
	BrLexerExpect(l,T_LSQUARE);

	while(BrLexerCurrent(l) != T_RSQUARE) {
		switch(BrLexerCurrent(l)) {
		case T_IDENTIFIER:
		case T_NAME:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
			mat->identifier = BrResStrDup(mat,BrLexerString(l));
			BrLexerAdvance(l);
			break;

		case T_FLAGS:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->flags = ParseMatFlags(l);
			break;

		case T_COLOUR:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);

			if(BrParseVector(l, &v3.v[0],3) != 3)
				BrLexerError(l,"Colour has too few entries");

			mat->colour = BR_COLOUR_RGB(
				BrScalarToInt(v3.v[0]),
				BrScalarToInt(v3.v[1]),
				BrScalarToInt(v3.v[2]));
			break;

		case T_OPACITY:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->opacity = (br_uint_8)BrParseInteger(l);
			break;

		case T_KA:
		case T_AMBIENT:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->ka = (br_ufraction)BrParseScalar(l);
			break;

		case T_KD:
		case T_DIFFUSE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->kd = (br_ufraction)BrParseScalar(l);
			break;

		case T_KS:
		case T_SPECULAR:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->ks = (br_ufraction)BrParseScalar(l);
			break;

		case T_POWER:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->power = BrParseScalar(l);
			break;

		case T_MAP_TRANSFORM:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			BrParseMatrix(l, &mat->map_transform.m[0][0],2,3);
			break;

		case T_DEPTH_TEST:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			switch (BrLexerCurrent(l)) {
			case T_GT:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_GT;
				break;
			case T_GE:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_GE;
				break;
			case T_EQ:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_EQ;
				break;
			case T_NE:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_NE;
				break;
			case T_LE:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_LE;
				break;
			case T_LT:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_LT;
				break;
			case T_NV:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_NV;
				break;
			case T_AL:
				mat->mode = (mat->mode & ~BR_MATM_DEPTH_TEST_MASK) | BR_MATM_DEPTH_TEST_AL;
				break;
			default:
				BrLexerError(l, "Unknown depth test mode");
			}
			BrLexerAdvance(l);
			break;

		case T_BLEND_MODE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			switch (BrLexerCurrent(l)) {
			case T_STANDARD:
				mat->mode = (mat->mode & ~BR_MATM_BLEND_MODE_MASK) | BR_MATM_BLEND_MODE_STANDARD;
				break;
			case T_SUMMED:
				mat->mode = (mat->mode & ~BR_MATM_BLEND_MODE_MASK) | BR_MATM_BLEND_MODE_SUMMED;
				break;
			case T_DIMMED:
				mat->mode = (mat->mode & ~BR_MATM_BLEND_MODE_MASK) | BR_MATM_BLEND_MODE_DIMMED;
				break;
			case T_PREMULTIPLIED:
				mat->mode = (mat->mode & ~BR_MATM_BLEND_MODE_MASK) | BR_MATM_BLEND_MODE_PREMULTIPLIED;
				break;
			default:
				BrLexerError(l, "Unknown blend mode");
			}
			BrLexerAdvance(l);
			break;

		case T_MAP_WIDTH_LIMIT:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			switch (BrLexerCurrent(l)) {
			case T_WRAP:
				mat->mode = (mat->mode & ~BR_MATM_MAP_WIDTH_LIMIT_MASK) | BR_MATM_MAP_WIDTH_LIMIT_WRAP;
				break;
			case T_CLAMP:
				mat->mode = (mat->mode & ~BR_MATM_MAP_WIDTH_LIMIT_MASK) | BR_MATM_MAP_WIDTH_LIMIT_CLAMP;
				break;
			case T_MIRROR:
				mat->mode = (mat->mode & ~BR_MATM_MAP_WIDTH_LIMIT_MASK) | BR_MATM_MAP_WIDTH_LIMIT_MIRROR;
				break;
			default:
				BrLexerError(l, "Unknown width limit mode");
			}
			BrLexerAdvance(l);
			break;

		case T_MAP_HEIGHT_LIMIT:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			switch (BrLexerCurrent(l)) {
			case T_WRAP:
				mat->mode = (mat->mode & ~BR_MATM_MAP_HEIGHT_LIMIT_MASK) | BR_MATM_MAP_HEIGHT_LIMIT_WRAP;
				break;
			case T_CLAMP:
				mat->mode = (mat->mode & ~BR_MATM_MAP_HEIGHT_LIMIT_MASK) | BR_MATM_MAP_HEIGHT_LIMIT_CLAMP;
				break;
			case T_MIRROR:
				mat->mode = (mat->mode & ~BR_MATM_MAP_HEIGHT_LIMIT_MASK) | BR_MATM_MAP_HEIGHT_LIMIT_MIRROR;
				break;
			default:
				BrLexerError(l, "Unknown width limit mode");
			}
			BrLexerAdvance(l);
			break;

		case T_INDEX_BASE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->index_base = (br_uint_8)BrParseInteger(l);
			break;

		case T_INDEX_RANGE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
			mat->index_range = (br_uint_8)BrParseInteger(l);
			break;

        case T_SUBDIVIDE_TOLERANCE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
            mat->subdivide_tolerance = (br_int_32)BrParseInteger(l);
			break;

		case T_COLOUR_MAP:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
			mat->colour_map = BrMapFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;

		case T_SCREEN_DOOR:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
			mat->screendoor = BrTableFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;

		case T_INDEX_SHADE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
			mat->index_shade = BrTableFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;

		case T_INDEX_BLEND:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
			mat->index_blend = BrTableFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;
#if 0
		case T_EXTRA_SURFACE:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);

			BrStringToTokenValue(token_buffer, sizeof(token_buffer),BrLexerString(l));

			mat->extra_surf = index_blend = BrTableFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;
#endif
        case T_FOG_MIN:
            BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
            mat->fog_min = BrParseScalar(l);
			break;

        case T_FOG_MAX:
			BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);
            mat->fog_max = BrParseScalar(l);
			break;

        case T_FOG_COLOUR:
            BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL);

			if(BrParseVector(l, &v3.v[0],3) != 3)
                BrLexerError(l,"Fog colour has too few entries");

            mat->fog_colour = BR_COLOUR_RGB(
				BrScalarToInt(v3.v[0]),
				BrScalarToInt(v3.v[1]),
				BrScalarToInt(v3.v[2]));
			break;

        case T_INDEX_FOG:
            BrLexerAdvance(l); BrLexerExpect(l,T_EQUAL); BrLexerCheck(l,T_STRING);
            mat->index_fog = BrTableFind(BrLexerString(l));
			BrLexerAdvance(l);
			break;

		default:
			BrLexerPrintPos(l);
			BR_WARNING1("Incorrect material member '%s' ignored",BrLexerString(l));
			while (BrLexerCurrent(l) != T_SEMICOLON)
				BrLexerAdvance(l);
		}
		BrLexerExpect(l,T_SEMICOLON);
	}

	/*
	 * ];
	 */
	BrLexerAdvance(l);
	BrLexerExpect(l,T_SEMICOLON);

	return mat;
};

static void BR_CALLBACK lexerError(struct br_lexer *l, char *string)
{
	char pos[256];

	BrLexerPosition(l,pos, BR_ASIZE(pos));
	BR_FAILURE2("%s%s", pos, string);
}

#if 0
#include <stdio.h>
void BR_CALLBACK dumpLine(char *str, void *arg)
{
	puts(str);
}
#endif

br_uint_32 BR_PUBLIC_ENTRY BrFmtScriptMaterialLoadMany(char *filename,br_material **materials,br_uint_16 num)
{
	int count;
	br_lexer *l;
	br_error r;

	l = BrLexerAllocate(keywords, BR_ASIZE(keywords));
	if(l == NULL)
		return 0;
		
	BrLexerCommentSet(l,'#');
	BrLexerErrorSet(l,lexerError);
#if 0
	BrLexerDumpSet(l, dumpLine, NULL);
#endif

	r = BrLexerPushFile(l,filename);
	if(r != BRE_OK) {
	 	BrLexerFree(l);
		return 0;
	}

	/*
	 * Start up the pipleline
	 */
	BrLexerAdvance(l);

	for (count=0; BrLexerCurrent(l) != T_EOF && count<num; count++)
		materials[count] = ParseMaterial(l);

	BrLexerFree(l);
	return count;
}

br_material * BR_PUBLIC_ENTRY BrFmtScriptMaterialLoad(char *filename)
{
	br_material *ptr;

	return (BrFmtScriptMaterialLoadMany(filename,&ptr,1) != 1)?NULL:ptr;
}
