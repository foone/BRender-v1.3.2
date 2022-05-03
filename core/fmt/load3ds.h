/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: load3ds.h 1.1 1997/12/10 16:41:01 jon Exp $
 * $Locker: $
 *
 * Definitions for 3D-Studio .3DS format loader
 */
#ifndef _LOAD3DS_H_
#define _LOAD3DS_H_

/*
 * Compile-time options
 */
#define USE_MATERIAL_OPACITY	(FALSE)
#define USE_BR_MATF_TWO_SIDED	(TRUE)
#define USE_BR_MATF_DECAL		(TRUE)
#define USE_BR_MATF_PERSPECTIVE	(TRUE)

#define TEST_WIREFRAME			(FALSE)
#define REGISTRY_ADD			(FALSE)
#define REPORT_MESSAGES			(TRUE)
#define SHOW_KEYFRAME_DATA		(FALSE)

/*
 * Grey-scale conversion factors
 */
#define RED_GREYSCALE_FACTOR	(0.3)
#define GREEN_GREYSCALE_FACTOR	(0.6)
#define BLUE_GREYSCALE_FACTOR	(0.1)

/*
 * Flags added to the BRender face flags to indicate texture wrapping
 */
#define FACEF_UWRAP	0x40
#define FACEF_VWRAP	0x80

/*
 * Miscellaneous definitions
 */
#define FENCE					(65535)
#define HALF_IMAGE_PLANE_MM		(21.2237408104802371)
#define MAX_DISPLAY_DEPTH		(10)

/*
 * General-purpose integer type
 */
typedef br_int_32 Int_t;

/*
 * Four-byte float type
 */
typedef float Float_t;

/*
 * Boolean type
 */
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
 * Chunk identification tags
 */
typedef enum ChunkIdTag_t {

    UNIDENTIFIED                                = 0x0000,

    COLOR_F                                     = 0x0010,
    COLOR_24                                    = 0x0011,
    INT_PERCENTAGE                              = 0x0030,
    FLOAT_PERCENTAGE                            = 0x0031,
    MAT_MAPNAME                                 = 0xA300,

        M3DMAGIC                                = 0x4D4D,
            M3D_VERSION                         = 0x0002,
            MDATA                               = 0x3D3D,
                MESH_VERSION                    = 0x3D3E,
                MASTER_SCALE                    = 0x0100,
                LO_SHADOW_BIAS                  = 0x1400,
                HI_SHADOW_BIAS                  = 0x1410,
                SHADOW_MAP_SIZE                 = 0x1420,
                SHADOW_SAMPLES                  = 0x1430,
                SHADOW_RANGE                    = 0x1440,
                SHADOW_FILTER                   = 0x1450,
                AMBIENT_LIGHT                   = 0x2100,
                O_CONSTS                        = 0x1500,
                BIT_MAP                         = 0x1100,
                SOLID_BGND                      = 0x1200,
                V_GRADIENT                      = 0x1300,
                USE_BIT_MAP                     = 0x1101,
                USE_SOLID_BGND                  = 0x1201,
                USE_V_GRADIENT                  = 0x1301,
                FOG                             = 0x2200,
                    FOG_BGND                    = 0x2210,
                DISTANCE_CUE                    = 0x2300,
                    DCUE_BGND                   = 0x2310,
                USE_FOG                         = 0x2201,
                USE_DISTANCE_CUE                = 0x2301,
                DEFAULT_VIEW                    = 0x3000,
                    VIEW_TOP                    = 0x3010,
                    VIEW_BOTTOM                 = 0x3020,
                    VIEW_LEFT                   = 0x3030,
                    VIEW_RIGHT                  = 0x3040,
                    VIEW_FRONT                  = 0x3050,
                    VIEW_BACK                   = 0x3060,
                    VIEW_USER                   = 0x3070,
                    VIEW_CAMERA                 = 0x3080,
                MAT_ENTRY                       = 0xAFFF,
                    MAT_NAME                    = 0xA000,
                    MAT_AMBIENT                 = 0xA010,
                    MAT_DIFFUSE                 = 0xA020,
                    MAT_SPECULAR                = 0xA030,
                    MAT_SHININESS               = 0xA040,
                    MAT_TRANSPARENCY            = 0xA050,
                    MAT_XPFALL                  = 0xA052,
                    MAT_USE_XPFALL              = 0xA240,
                    MAT_REFBLUR                 = 0xA053,
                    MAT_USE_REFBLUR             = 0xA250,
                    MAT_SELF_ILLUM              = 0xA080,
                    MAT_TWO_SIDE                = 0xA081,
                    MAT_DECAL                   = 0xA082,
                    MAT_ADDITIVE                = 0xA083,
                    MAT_SHADING                 = 0xA100,
                    MAT_TEXMAP                  = 0xA200,
                    MAT_SXP_TEXT_DATA           = 0xA320,
                    MAT_OPACMAP                 = 0xA210,
                    MAT_SXP_OPAC_DATA           = 0xA322,
                    MAT_REFLMAP                 = 0xA220,
                    MAT_ACUBIC                  = 0xA310,
                    MAT_BUMPMAP                 = 0xA230,
                    MAT_SXP_BUMP_DATA           = 0xA324,
                NAMED_OBJECT                    = 0x4000,
                    N_TRI_OBJECT                = 0x4100,
                        POINT_ARRAY             = 0x4110,
                        POINT_FLAG_ARRAY        = 0x4111,
                        FACE_ARRAY              = 0x4120,
                            MSH_MAT_GROUP       = 0x4130,
                            SMOOTH_GROUP        = 0x4150,
                        TEX_VERTS               = 0x4140,
                        MESH_MATRIX             = 0x4160,
                        MESH_TEXTURE_INFO       = 0x4170,
                        PROC_NAME               = 0x4181,
                        PROC_DATA               = 0x4182,
                    N_DIRECT_LIGHT              = 0x4600,
                       DL_OFF                   = 0x4620,
                       DL_SPOTLIGHT             = 0x4610,
                           DL_SHADOWED          = 0x4630,
                           DL_LOCAL_SHADOW2     = 0x4641,
                           DL_SEE_CONE          = 0x4650,
                    N_CAMERA                    = 0x4700,
                       CAM_SEE_CONE             = 0x4710,
                    OBJ_HIDDEN                  = 0x4010,
                    OBJ_VIS_LOFTER              = 0x4011,
                    OBJ_DOESNT_CAST             = 0x4012,
                    OBJ_MATTE                   = 0x4013,
                    OBJ_FAST                    = 0x4014,
                    OBJ_PROCEDURAL              = 0x4015,
                    OBJ_FROZEN                  = 0x4016,
        KFDATA                                  = 0xB000,
            KFHDR                               = 0xB00A,
            KFSEG                               = 0xB008,
            KFCURTIME                           = 0xB009,
            OBJECT_NODE_TAG                     = 0xB002,
            CAMERA_NODE_TAG                     = 0xB003,
            TARGET_NODE_TAG                     = 0xB004,
            LIGHT_NODE_TAG                      = 0xB005,
            SPOTLIGHT_NODE_TAG                  = 0xB007,
            L_TARGET_NODE_TAG                   = 0xB006,
                NODE_HDR                        = 0xB010,
                PIVOT                           = 0xB013,
                INSTANCE_NAME                   = 0xB011,
                MORPH_SMOOTH                    = 0xB015,
                BOUNDBOX                        = 0xB014,
                POS_TRACK_TAG                   = 0xB020,
                COL_TRACK_TAG                   = 0xB025,
                ROT_TRACK_TAG                   = 0xB021,
                SCL_TRACK_TAG                   = 0xB022,
                MORPH_TRACK_TAG                 = 0xB026,
                FOV_TRACK_TAG                   = 0xB023,
                ROLL_TRACK_TAG                  = 0xB024,
                HOT_TRACK_TAG                   = 0xB027,
                FALL_TRACK_TAG                  = 0xB028,

        MLIBMAGIC                               = 0x3DAA,

} ChunkIdTag_t;

/*
 * Chunk flags
 */
typedef enum ChunkFlag_t {
    GOT_COLOR_F                                 = 0x00000001, /* generic    */
    GOT_COLOR_24                                = 0x00000002, /* generic    */
    GOT_INT_PERCENTAGE                          = 0x00000001, /* generic    */
    GOT_FLOAT_PERCENTAGE                        = 0x00000002, /* generic    */
    GOT_MAT_MAPNAME                             = 0x00000004, /* generic    */

        GOT_M3DMAGIC                            = 0x00000000, /* req, once  */
            GOT_M3D_VERSION                     = 0x00000001, /* req, once  */
            GOT_MDATA                           = 0x00000002, /* req, once  */
                GOT_MESH_VERSION                = 0x00000001, /* req, once  */
                GOT_MASTER_SCALE                = 0x00000002, /* req, once  */
                GOT_LO_SHADOW_BIAS              = 0x00000004, /* opt, once  */
                GOT_HI_SHADOW_BIAS              = 0x00000008, /* opt, once  */
                GOT_SHADOW_MAP_SIZE             = 0x00000010, /* opt, once  */
                GOT_SHADOW_SAMPLES              = 0x00000020, /* opt, once  */
                GOT_SHADOW_RANGE                = 0x00000040, /* opt, once  */
                GOT_SHADOW_FILTER               = 0x00000080, /* opt, once  */
                GOT_AMBIENT_LIGHT               = 0x00000100, /* opt, once  */
                    /* COLOR_<X>                                 req, once  */
                GOT_O_CONSTS                    = 0x00000200, /* opt, once  */
                GOT_BIT_MAP                     = 0x00000400, /* opt, once  */
                GOT_SOLID_BGND                  = 0x00000800, /* opt, once  */
                GOT_V_GRADIENT                  = 0x00001000, /* opt, once  */
                    /* COLOR_<X>                                 req, three */
                GOT_USE_BIT_MAP                 = 0x00002000, /* opt, once  */
                GOT_USE_SOLID_BGND              = 0x00004000, /* opt, once  */
                GOT_USE_V_GRADIENT              = 0x00008000, /* opt, once  */
                GOT_FOG                         = 0x00010000, /* opt, once  */
                    /* COLOR_<X>                                 req, once  */
                    GOT_FOG_BGND                = 0x00000000, /* opt, flag  */
                GOT_DISTANCE_CUE                = 0x00020000, /* opt, once  */
                    GOT_DCUE_BGND               = 0x00000000, /* opt, flag  */
                GOT_USE_FOG                     = 0x00040000, /* opt, once  */
                GOT_USE_DISTANCE_CUE            = 0x00080000, /* opt, once  */
                GOT_DEFAULT_VIEW                = 0x00100000, /* opt, once  */
                    GOT_VIEW_TOP                = 0x00000001, /* xopt, once */
                    GOT_VIEW_BOTTOM             = 0x00000002, /* xopt, once */
                    GOT_VIEW_LEFT               = 0x00000004, /* xopt, once */
                    GOT_VIEW_RIGHT              = 0x00000008, /* xopt, once */
                    GOT_VIEW_FRONT              = 0x00000010, /* xopt, once */
                    GOT_VIEW_BACK               = 0x00000020, /* xopt, once */
                    GOT_VIEW_USER               = 0x00000040, /* xopt, once */
                    GOT_VIEW_CAMERA             = 0x00000080, /* xopt, once */
                GOT_MAT_ENTRY                   = 0x00000000, /* opt, any   */
                    GOT_MAT_NAME                = 0x00000001, /* req, once  */
                    GOT_MAT_AMBIENT             = 0x00000002, /* req, once  */
                        /* COLOR_<X>                             req, once  */
                    GOT_MAT_DIFFUSE             = 0x00000004, /* req, once  */
                        /* COLOR_<X>                             req, once  */
                    GOT_MAT_SPECULAR            = 0x00000008, /* req, once  */
                        /* COLOR_<X>                             req, once  */
                    GOT_MAT_SHININESS           = 0x00000010, /* req, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                    GOT_MAT_TRANSPARENCY        = 0x00000020, /* req, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                    GOT_MAT_XPFALL              = 0x00000040, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                    GOT_MAT_USE_XPFALL          = 0x00000080, /* opt, once  */
                    GOT_MAT_REFBLUR             = 0x00000100, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                    GOT_MAT_USE_REFBLUR         = 0x00000200, /* opt, once  */
                    GOT_MAT_SELF_ILLUM          = 0x00000000, /* opt, flag  */
                    GOT_MAT_TWO_SIDE            = 0x00000400, /* opt, once  */
                    GOT_MAT_DECAL               = 0x00000800, /* opt, once  */
                    GOT_MAT_ADDITIVE            = 0x00000000, /* opt, flag  */
                    GOT_MAT_SHADING             = 0x00001000, /* req, once  */
                    GOT_MAT_TEXMAP              = 0x00002000, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                        /* MAT_MAPNAME                           req, once  */
                    GOT_MAT_SXP_TEXT_DATA       = 0x00004000, /* opt, once  */
                    GOT_MAT_OPACMAP             = 0x00008000, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                        /* MAT_MAPNAME                           req, once  */
                    GOT_MAT_SXP_OPAC_DATA       = 0x00010000, /* opt, once  */
                    GOT_MAT_REFLMAP             = 0x00020000, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                        /* MAT_MAPNAME                           req, once  */
                    GOT_MAT_ACUBIC              = 0x00040000, /* opt, once  */
                    GOT_MAT_BUMPMAP             = 0x00080000, /* opt, once  */
                        /* <X>_PERCENTAGE                        req, once  */
                        /* MAT_MAPNAME                           req, once  */
                    GOT_MAT_SXP_BUMP_DATA       = 0x00100000, /* opt, once  */
                GOT_NAMED_OBJECT                = 0x00000000, /* opt, any   */
                    GOT_N_TRI_OBJECT            = 0x00000001, /* xopt, once */
                        GOT_POINT_ARRAY         = 0x00000001, /* req, once  */
                        GOT_POINT_FLAG_ARRAY    = 0x00000002, /* opt, once  */
                        GOT_FACE_ARRAY          = 0x00000004, /* req, once  */
                            GOT_MSH_MAT_GROUP   = 0x00000000, /* opt, any   */
                            GOT_SMOOTH_GROUP    = 0x00000001, /* opt, once  */
                        GOT_TEX_VERTS           = 0x00000008, /* opt, once  */
                        GOT_MESH_MATRIX         = 0x00000010, /* opt, once  */
                        GOT_MESH_TEXTURE_INFO   = 0x00000020, /* opt, once  */
                        GOT_PROC_NAME           = 0x00000040, /* opt, once  */
                        GOT_PROC_DATA           = 0x00000080, /* opt, once  */
                    GOT_N_DIRECT_LIGHT          = 0x00000002, /* xopt, once */
                       /* COLOR_<X>                              req, once  */
                       GOT_DL_OFF               = 0x00000001, /* opt, once  */
                       GOT_DL_SPOTLIGHT         = 0x00000002, /* opt, once  */
                           GOT_DL_SHADOWED      = 0x00000001, /* opt, once  */
                           GOT_DL_LOCAL_SHADOW2 = 0x00000002, /* opt, once  */
                           GOT_DL_SEE_CONE      = 0x00000004, /* opt, once  */
                    GOT_N_CAMERA                = 0x00000004, /* xopt, once */
                       GOT_CAM_SEE_CONE         = 0x00000001, /* opt, once  */
                    GOT_OBJ_HIDDEN              = 0x00000002, /* opt, once  */
                    GOT_OBJ_VIS_LOFTER          = 0x00000004, /* opt, once  */
                    GOT_OBJ_DOESNT_CAST         = 0x00000008, /* opt, once  */
                    GOT_OBJ_MATTE               = 0x00000010, /* opt, once  */
                    GOT_OBJ_FAST                = 0x00000020, /* opt, once  */
                    GOT_OBJ_PROCEDURAL          = 0x00000040, /* opt, once  */
                    GOT_OBJ_FROZEN              = 0x00000080, /* opt, once  */
        GOT_KFDATA                              = 0x00000004, /* opt, once  */
            GOT_KFHDR                           = 0x00000001, /* req, once  */
            GOT_KFSEG                           = 0x00000002, /* req, once  */
            GOT_KFCURTIME                       = 0x00000004, /* req, once  */
            GOT_OBJECT_NODE_TAG                 = 0x00000000, /* opt, any   */
            GOT_CAMERA_NODE_TAG                 = 0x00000000, /* opt, any   */
            GOT_TARGET_NODE_TAG                 = 0x00000000, /* opt, any   */
            GOT_LIGHT_NODE_TAG                  = 0x00000000, /* opt, any   */
            GOT_SPOTLIGHT_NODE_TAG              = 0x00000000, /* opt, any   */
            GOT_L_TARGET_NODE_TAG               = 0x00000000, /* opt, any   */
                GOT_NODE_HDR                    = 0x00000001, /* req, once  */
                GOT_PIVOT                       = 0x00000002, /* req, once  */
                GOT_INSTANCE_NAME               = 0x00000004, /* opt, once  */
                GOT_MORPH_SMOOTH                = 0x00000008, /* opt, once  */
                GOT_BOUNDBOX                    = 0x00000010, /* req, once  */
                GOT_POS_TRACK_TAG               = 0x00000020, /* req, once  */
                GOT_COL_TRACK_TAG               = 0x00000040, /* req, once  */
                GOT_ROT_TRACK_TAG               = 0x00000080, /* req, once  */
                GOT_SCL_TRACK_TAG               = 0x00000100, /* req, once  */
                GOT_MORPH_TRACK_TAG             = 0x00000200, /* opt, once  */
                GOT_FOV_TRACK_TAG               = 0x00000400, /* req, once  */
                GOT_ROLL_TRACK_TAG              = 0x00000800, /* req, once  */
                GOT_HOT_TRACK_TAG               = 0x00001000, /* req, once  */
                GOT_FALL_TRACK_TAG              = 0x00002000  /* req, once  */

} ChunkFlag_t;

/*
 * RGB triple
 */
typedef struct Color_t {
    Float_t red;
    Float_t green;
    Float_t blue;
} Color_t;

/*
 * 3DS material shading types
 */
typedef enum MatShading_t {
    WIREFRAME,
    FLAT,
    GOURAUD
} MatShading_t;

/*
 * An entry in a linked list of pixmaps
 */
typedef struct PixmapList_t {
    br_pixelmap         *pixelmap;
    struct PixmapList_t *next;
} PixmapList_t;

/*
 * A pixmap reference
 */
typedef struct PixmapRef_t {
    Bool_t  is_reflection_map;
    char    *mat_mapname;
    Float_t strength;
} PixmapRef_t;

/*
 * A 3DS material entry
 */
typedef struct MatEntry_t {
    char         *mat_name;
    Color_t      mat_ambient;
    Color_t      mat_diffuse;
    Color_t      mat_specular;
    Float_t      mat_transparency;
    Float_t      mat_shininess;
    MatShading_t mat_shading;
    Bool_t       mat_two_side;
    Bool_t       mat_decal;
    PixmapRef_t  pixmap_ref;
} MatEntry_t;

/*
 * An entry in a linked list of 3DS materials
 */
typedef struct MaterialList_t {
    MatShading_t          mat_shading;
    br_material           *material;
    struct MaterialList_t *next;
} MaterialList_t;

/*
 * An array of points
 */
typedef struct PointArray_t {
    br_uint_16 n_vertices;
    br_vector3 *vertices;
} PointArray_t;

/*
 * Intermediate face structure
 */
typedef struct Face_t {
    br_uint_16 vertices[3];
    br_uint_16 flags;
} Face_t;

/*
 * An entry in a linked list of material groups
 */
typedef struct MshMatGroup_t {
    MaterialList_t *material_link;
    br_uint_16     n_indexes;
    br_uint_16     *indexes;
    struct MshMatGroup_t *next;
} MshMatGroup_t;

/*
 * An array of faces, material groups and smoothing groups
 */
typedef struct FaceArray_t {
    br_uint_16    n_faces;
    Face_t        *faces;
    MshMatGroup_t *msh_mat_groups;
    br_uint_16    *smooth_group;
} FaceArray_t;

/*
 * An array of texture coordinates
 */
typedef struct TexVerts_t {
    br_uint_16  n_texverts;
    br_vector2  *texverts;
} TexVerts_t;

/*
 * An intermediate triangle mesh structure 
 */
typedef struct NTriObj_t {
    PointArray_t point_array;
    FaceArray_t  face_array;
    TexVerts_t   tex_verts;
    br_matrix34  mesh_matrix;
} NTriObj_t;

/*
 * Spotlight parameters
 */
typedef struct DlSpotlight_t {
    br_vector3 target;
    Float_t    cone_inner;
    Float_t    cone_outer;
} DlSpotlight_t;

/*
 * Light parameters
 */
typedef struct NDLight_t {
    br_vector3    posn;
    Color_t       color;
    Bool_t        is_off;
    Bool_t        is_spotlight;
    DlSpotlight_t dl_spotlight;
} NDLight_t;

/*
 * Camera parameters
 */
typedef struct NCamera_t {
    br_vector3 posn;
    br_vector3 target;
    Float_t    bank_angle;
    Float_t    focal_length;
} NCamera_t;

/*
 * Light
 */
typedef struct Light_t {
    br_uint_8 type;
    br_colour colour;
    br_angle cone_outer;
    br_angle cone_inner;
    Bool_t is_off;
} Light_t;

/*
 * Camera
 */
typedef struct Camera_t {
    br_angle field_of_view;
} Camera_t;

/*
 * Model
 */
typedef struct Model_t {
    br_model    *fill_model;
    br_model    *wire_model;
} Model_t;

/*
 * 3DS named object types
 */
typedef enum NamedObjType_t {
    NONE,
    LIGHT,
    CAMERA,
    MODEL
} NamedObjType_t;

/*
 * An entry in a linked list of named objects
 */
typedef struct NamedObj_t {
    char              *name;
    NamedObjType_t    type;
    union {
        Light_t       *light;
        Model_t       *model;
        Camera_t      *camera;
    } data;
    br_matrix34 to_world;
    br_matrix34 from_world;
    struct NamedObj_t *next;
} NamedObj_t;

/*
 * Node header
 */
typedef struct NodeHdr_t {
    Int_t      index;
    Int_t      parent;
    NamedObj_t *named_obj;
} NodeHdr_t;

/*
 * An entry in a linked list of node tags, each
 * of which contain additional model information
 */
typedef struct NodeTag_t {
    NodeHdr_t  node_hdr;
    char       *instance_name;
    Bool_t     has_pivot;
    br_vector3 pivot;
    Bool_t     has_boundbox;
    br_bounds  boundbox;
    struct NodeTag_t *next;
} NodeTag_t;

/*
 * An item on the stack
 */
typedef struct Stack_t {
    ChunkIdTag_t id_tag;
    br_uint_32   length;
    br_uint_32   done;
    ChunkFlag_t  flags;
    union { Color_t        color;
            Float_t        percent;
            char           *string;
            MatShading_t   mat_shading;
            PixmapRef_t    pixmap_ref;
            MatEntry_t     mat_entry;
            PointArray_t   point_array;
            FaceArray_t    face_array;
            br_uint_16     *smooth_group;
            MshMatGroup_t  *msh_mat_group;
            TexVerts_t     tex_verts;
            br_matrix34    mesh_matrix;
            NTriObj_t      n_tri_obj;
            DlSpotlight_t  dl_spotlight;
            NDLight_t      n_d_light;
            NCamera_t      n_camera;
            NamedObj_t     *named_obj;
            NodeHdr_t      node_hdr;
            br_vector3     pivot;
            br_bounds      boundbox;
            NodeTag_t      *node_tag;
          } data;
} Stack_t;

/*
 * Parser states
 */
typedef enum State_t {
    OK,
    PARSE_ERROR,
    OUT_OF_MEMORY
} State_t;

#endif

