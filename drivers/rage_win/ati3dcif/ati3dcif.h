//  Filename: ati3dcif.h 
//  $Revision: 1.1 $
//
//  Description: ATI 'C'interface (CIF) header to GT 3D hardware acceleration
//
//  Trade secret of ATI Technologies, Inc.
//  Copyright 1995, ATI Technologies, Inc., (unpublished) 
//  
//  All rights reserved.  This notice is intended as a precaution against
//  inadvertent publication and does not imply publication or any waiver
//  of confidentiality.  The year included in the foregoing notice is the
//  year of creation of the work.
//  
//  

#ifndef ATI_ATI3DCIF_H
#define ATI_ATI3DCIF_H


// Current Restrictions
//          1. Only support EV_VTCF floating point vertex type
//          2. Only one context may be exist at a time
//          3. RenderSwitch does not have meaning ( consequence of 2. ) 
//          4. C3D_ETFILT_MIPTRI_MAG2BY2 is not supported yet.
//          5. Coordinates should be pre-clipped by software to:
//              -2048.0f +2047.0f in X
//              -4096.0f +4095.0f in Y

#ifdef __cplusplus
extern "C" {
#endif

#if ! defined (BUILD_AS_DLL)
#define DLLEXPORT 
#endif

#ifdef  DOS_BUILD
#include "afxdos.h"
#include "i3debug.h"
#endif

#ifndef  DOS_BUILD
#pragma pack(push)  // push packing state
#pragma pack(8)     // CIF requires structure data is naturally aligned
#endif

// Fundemental types
//
typedef unsigned int    C3D_BOOL;       // 0 indicates FALSE, >0 indicates TRUE
typedef unsigned int    C3D_INT32;      // 32 bit integer data
typedef unsigned int    C3D_UINT32;     // unsigned 32 bit integer data
typedef unsigned short  C3D_UINT16;     // unsigned 16 bit integer data
typedef unsigned char   C3D_UINT8;      // unsigned 8  bit integer data
typedef float           C3D_FLOAT32;    // 32 bit floating point data

typedef unsigned int*   C3D_PBOOL;      // points at C3D_BOOL;   
typedef unsigned int*   C3D_PINT32;     // points at C3D_INT32;
typedef unsigned int*   C3D_PUINT32;    // points at C3D_UINT32;
typedef unsigned short* C3D_PUINT16;    // points at C3D_UINT16;
typedef unsigned char*  C3D_PUINT8;     // points at C3D_UINT8;
typedef float*          C3D_PFLOAT32;   // points at C3D_FLOAT32;
typedef void *          C3D_PVOID;      // points at generic data

//ensure that enums are 32 bits in size.  Allows optimization of non C3D enums.
#ifdef  DOS_BUILD
#define C3D_FORCE_SIZE     0x7F      
#else
#define C3D_FORCE_SIZE     0x7FFFFFFF      
#endif

// Error codes
//
typedef enum {
    C3D_EC_OK           = 0,  // Success
    C3D_EC_GENFAIL      = 1,  // Generic Failure
    C3D_EC_MEMALLOCFAIL = 2,  // Memory allocation failure
    C3D_EC_BADPARAM     = 3,  // Invalid parameter passed to function
    C3D_EC_UNUSED0      = 4,  // Reserved
    C3D_EC_BADSTATE     = 5,  // Object entered invalid state
    C3D_EC_NOTIMPYET    = 6,  // Functionality Not Implemented Yet
    C3D_EC_UNUSED1      = 7,  // Reserved
    C3D_EC_CHIPCAPABILITY = 8, //Feature is not on this chip version (see C3D_CAPS)
    C3D_EC_NUM          = 9,
    C3D_EC_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EC, * C3D_PEC;

// Rect: specs a rectangular region in 2 space
//
typedef struct {
    C3D_INT32 top;
    C3D_INT32 left;
    C3D_INT32 bottom;
    C3D_INT32 right;
} C3D_RECT , * C3D_PRECT;

// Color
//
typedef union {
    struct {
        unsigned r: 8;  // 8 red bits
        unsigned g: 8;  // 8 green bits
        unsigned b: 8;  // 8 blue bits
        unsigned a: 8;  // 8 alpha bits
    };
    C3D_UINT32 u32All;
} C3D_COLOR , * C3D_PCOLOR;

// Palette Color
//
//values for the flag palette entry.  Set the flag
// C3D_LOAD_PALETTE_ENTRY to use thse palette entry, 
//C3D_NO_LOAD_PALETTE_ENTRY to skip the entry.
//This allows "virtual palettes" of other than 16/256 size.

#define C3D_LOAD_PALETTE_ENTRY  0x40
#define C3D_NO_LOAD_PALETTE_ENTRY  0x00

typedef union {
    struct {
        unsigned r: 8;  // 8 red bits
        unsigned g: 8;  // 8 green bits
        unsigned b: 8;  // 8 blue bits
        unsigned flags: 8;  //flag bits - see above defines
    };
    C3D_UINT32 u32All;
} C3D_PALETTENTRY , * C3D_PPALETTENTRY;

// Module Capability Flags: These module capability flags should allow a client
// to function properly with any release of the 3D CIF library. C3D_CAPS_BASE 
// indicates the availability of all functionality supported in the first 
// release of 3DCIF build 17. Each flag indicates the existance of module 
// features added after the first release.  NB that u32CIFCaps field of the 
// C3D_3DCIFINFO struct did not exist.  Before using the u32CIFCaps field make 
// sure the library is more recent than build 17
//
#define C3D_CAPS1_BASE           0x00000000  // base line functionality
#define C3D_CAPS1_FOG            0x00000001  // fog support
#define C3D_CAPS1_POINT          0x00000002  // point primitive
#define C3D_CAPS1_RECT           0x00000004  // screen aligned rect primitive
#define C3D_CAPS1_Z_BUFFER       0x00000008  // depth buffer support
#define C3D_CAPS1_CI4_TMAP       0x00000010  // 4 bit color index texture support
#define C3D_CAPS1_CI8_TMAP       0x00000020  // 8 bit color index texture support
#define C3D_CAPS1_LOAD_OBJECT    0x00000040  // bus-master data loading
#define C3D_CAPS1_DITHER_EN      0x00000080  // dithering can be turned on/off
#define C3D_CAPS1_ENH_PERSP      0x00000100  // enhanced perspective levels avail
#define C3D_CAPS1_SCISSOR        0x00000200  // set clip without setting origin
#define C3D_CAPS1_PROFILE_IF     0x01000000  // is the profile interface avail

// Module info structure
//
typedef struct {
    C3D_UINT32 u32Size;             // Sz of struct must be init'ed by client
    C3D_UINT32 u32FrameBuffBase;    // Host pointer to frame buffer base
    C3D_UINT32 u32OffScreenHeap;    // Host pointer to offscreen heap
    C3D_UINT32 u32OffScreenSize;    // Size of offscreen heap
    C3D_UINT32 u32TotalRAM;         // Total amount of RAM on the card
    C3D_UINT32 u32ASICID;           // ASIC Id. code
    C3D_UINT32 u32ASICRevision;     // ASIC revision
    C3D_UINT32 u32CIFCaps1;          // ATI3DCIF module capabilities (see flags)
    C3D_UINT32 u32CIFCaps2;          // ATI3DCIF module capabilities (see flags)
    C3D_UINT32 u32CIFCaps3;          // ATI3DCIF module capabilities (see flags)
    C3D_UINT32 u32CIFCaps4;          // ATI3DCIF module capabilities (see flags)
    C3D_UINT32 u32CIFCaps5;          // ATI3DCIF module capabilities (see flags)
} C3D_3DCIFINFO, * PC3D_3DCIFINFO;

//
// Vertex enumeration type : used to select vertex data format to be referenced 
//                           when rendering primitives
//  
// Nomenclature:             V for Vertex (Always X,Y,Z)
//                           C for Color  (Always R,G,B,A)
//                           T for Texture(Always S,T,W)
// 

typedef enum {
    C3D_EV_VF       = 0,
    C3D_EV_VCF      = 1,
    C3D_EV_VTF      = 2,
    C3D_EV_VTCF     = 3,
    C3D_EV_NUM      = 4,
    C3D_EV_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EVERTEX, * C3D_PEVERTEX;

//
// Vertex data types
//
typedef struct {                        
    C3D_FLOAT32 x, y, z;                // FLOATING point type
} C3D_VF, * C3D_PVF;                    // identified by C3D_EV_VF

typedef struct { 
    C3D_FLOAT32 x, y, z;                // FLOATING point type
    C3D_FLOAT32 r, g, b, a;             // identified by C3D_EV_VCF
} C3D_VCF, * C3D_PVCF;

typedef struct { 
    C3D_FLOAT32 x, y, z;                // FLOATING point type
    C3D_FLOAT32 s, t, w;                // identified by C3D_EV_VTF
} C3D_VTF, * C3D_PVTF;

typedef struct { 
    C3D_FLOAT32 x, y, z;                // FLOATING point type
    C3D_FLOAT32 s, t, w;                // identified by C3D_EV_VTCF
    C3D_FLOAT32 r, g, b, a; 
} C3D_VTCF, * C3D_PVTCF;


// Primitive Sets:  A primitive set is a sequence of vertex data used
// to represent a stream of geometric primitives:  Quads,Tris or Lines.  
// A Primitive Set can take two basic forms: Strips or Lists.  
// A Primitive List is a doubly indirect array of pointers to actual vertex data.  
// A Primitive Strip is  a pointer to contiguous array of vertex data 
// where adjacent primitives share vertex data.
//
// (e.g. a triangle strip with four vertices represents 2 triangles 
//

// Primitive Set Pointers
typedef void *  C3D_VSTRIP;
typedef void ** C3D_VLIST;

// Primitive Type:  Specifies the geometric interpretation of a vertex set
// during rasterization. eg: if C3D_EPRIM_TRI is set in a rendering context
// then subsequent calls to ATI3DCIF_RenderPrimList will interpret the specified
// vertices as triangles and will consume 3 vertices for each triangle drawn.
//
typedef enum {
    C3D_EPRIM_LINE  = 0,
    C3D_EPRIM_TRI   = 1,
    C3D_EPRIM_QUAD  = 2,
    C3D_EPRIM_RECT  = 3,
    C3D_EPRIM_POINT = 4,
    C3D_EPRIM_NUM   = 5,     // invalid enumeration val
    C3D_EPRIM_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EPRIM, * C3D_PEPRIM;

// Shading
//
typedef enum {
    C3D_ESH_NONE     = 0,
    C3D_ESH_SOLID    = 1,   // shade using the clrSolid from the RC
    C3D_ESH_FLAT     = 2,   // shade using the last vertex to flat shade.
    C3D_ESH_SMOOTH   = 3,   // shade using linearly interpolating vert clr
    C3D_ESH_NUM      = 4,    // invalid enumeration val
    C3D_ESH_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ESHADE, * C3D_PESHADE;

// Alpha Blending
//
//      clr_dst = clr_src * f(eAlphaSrcFactor) + clr_dst * f(eAlphaDstFactor)

// alpha blending source factor select
typedef enum {
    C3D_EASRC_ZERO              = 0,// Blend factor is (0, 0, 0)
    C3D_EASRC_ONE               = 1,// Blend factor is (1, 1, 1)
    C3D_EASRC_DSTCLR            = 2,// Blend factor is (Rd, Gd, Bd)
    C3D_EASRC_INVDSTCLR         = 3,// Blend factor is (1-Rd, 1-Gd, 1-Bd)
    C3D_EASRC_SRCALPHA          = 4,// Blend factor is (As, As, As)
    C3D_EASRC_INVSRCALPHA       = 5,// Blend factor is (1-As, 1-As, 1-As)
    C3D_EASRC_NUM               = 6, // invalid enumeration val
    C3D_EASRC_FORCE_U32    = C3D_FORCE_SIZE
 } C3D_EASRC, *C3D_PEASRC;

// alpha blending destination factor select
//
typedef enum {
    C3D_EADST_ZERO          = 0,    // Blend factor is (0, 0, 0)
    C3D_EADST_ONE           = 1,    // Blend factor is (1, 1, 1)
    C3D_EADST_SRCCLR        = 2,    // Blend factor is (Rs, Gs, Bs)
    C3D_EADST_INVSRCCLR     = 3,    // Blend factor is (1-Rs, 1-Gs, 1-Bs)
    C3D_EADST_SRCALPHA      = 4,    // Blend factor is (As, As, As)
    C3D_EADST_INVSRCALPHA   = 5,    // Blend factor is (1-As, 1-As, 1-As)
    C3D_EADST_NUM           = 6,     // invalid enumeration val
    C3D_EADST_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EADST, *C3D_PEADST;

// Texture mapping
//
typedef void *      C3D_HTX;          // handle to a texture in the framebuffer
typedef C3D_HTX *   C3D_PHTX;         // pointer to handle to texture
typedef void *      C3D_HTXPAL;       // handle to a texture palette
typedef C3D_HTXPAL *C3D_PHTXPAL;      // pointer to handle to a texture palette


typedef enum {
    C3D_ECI_TMAP_TRUE_COLOR = 0,         //  no palette
    C3D_ECI_TMAP_4BIT_HI    = 1,         //  16 entry palette
    C3D_ECI_TMAP_4BIT_LOW   = 2,         //  16 entry palette
    C3D_ECI_TMAP_8BIT       = 3,         //  256 entry palette
    C3D_ECI_TMAP_NUM        = 4,         //  invalid enumeration
    C3D_ECI_TMAP_FORCE_U32  = C3D_FORCE_SIZE
} C3D_ECI_TMAP_TYPE;

// Texel lighting
typedef enum {
    C3D_ETL_NONE        = 0,        //  TEXout = Tclr
    C3D_ETL_MODULATE    = 1,        //  TEXout = Tclr*CInt
    C3D_ETL_ALPHA_DECAL = 2,        //  TEXout = (Tclr*Talp)+(CInt*(1-Talp))
    C3D_ETL_NUM         = 3,         //  invalid enumeration
    C3D_ETL_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ETLIGHT, * C3D_PETLIGHT;

// specifies the level of perspective correction done by the hardware.  This
// setting trades off image quality for better frame rates 
typedef enum {
    C3D_ETPC_NONE       = 0,        //  no correction, best frame rate
    C3D_ETPC_ONE        = 1,        //   
    C3D_ETPC_TWO        = 2,        //   
    C3D_ETPC_THREE      = 3,        //  recommended correction
    C3D_ETPC_FOUR       = 4,        //   
    C3D_ETPC_FIVE       = 5,        //   
    C3D_ETPC_SIX        = 6,        //   
    C3D_ETPC_SEVEN      = 7,        //   
    C3D_ETPC_EIGHT      = 8,        //   
    C3D_ETPC_NINE       = 9,        //  full correction, worst frame rate
    C3D_ETPC_NUM        = 10,        //  invalid enumeration
    C3D_ETPC_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ETPERSPCOR, * C3D_PETPERSPCOR;

// texture filtering modes
// ( NB: that GT supports 2x2 MIN only if 2x2 MAG is turned on )
// ( NB: modes identified by MIP are only valid with mip mapped textures )
typedef enum {
    C3D_ETFILT_MINPNT_MAGPNT    = 0,// pick nearest texel (pnt) min/mag
    C3D_ETFILT_MINPNT_MAG2BY2   = 1,// pnt min/bi-linear mag
    C3D_ETFILT_MIN2BY2_MAG2BY2  = 2,// 2x2 blend min/bi-linear mag
    C3D_ETFILT_MIPLIN_MAGPNT    = 3,// 1x1 blend min(between maps)/pnt mag
    C3D_ETFILT_MIPLIN_MAG2BY2   = 4,// 1x1 blend min(between maps)/bi-linear mag
    C3D_ETFILT_MIPTRI_MAG2BY2   = 5,// (2x2)x(2x2)(between maps)/bi-linear mag
    C3D_ETFILT_NUM              = 6, //
    C3D_ETFILT_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ETEXFILTER, *C3D_PETEXFILTER;

// texel op modes: determines how which operations are applied to texels as 
//      they are read from the texture.  By design theses are orthogonal to 
//      filtering modes. 
//
typedef enum {
    C3D_ETEXOP_NONE          = 0,  // 
    C3D_ETEXOP_CHROMAKEY     = 1,  // select texels not equal to the chroma key
    C3D_ETEXOP_ALPHA         = 2,  // pass texel alpha to the alpha blender
    C3D_ETEXOP_ALPHA_MASK    = 3,  // lw bit 0: tex not drawn otw: alpha int
    C3D_ETEXOP_NUM           = 4,   //
    C3D_ETEXOP_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ETEXOP, * C3D_PETEXOP;

// specifies the pixel format
typedef enum {                      //                                     (sz)
    C3D_EPF_RGB1555 = 3,            // 1b Alpha, 5b Red, 5b Green, 5b Blue (16)
    C3D_EPF_RGB565  = 4,            // 0b Alpha, 5b Red, 6b Green, 5b Blue (16) 
    C3D_EPF_RGB8888 = 6,            // 8b Alpha, 8b Red, 8b Green, 8b Blue (32) 
    C3D_EPF_RGB332  = 7,            // 0b Alpha, 3b Red, 3b Green, 2b Blue (08) 
    C3D_EPF_Y8      = 8,            // 8b Y                                (08) 
    C3D_EPF_YUV422  = 11,           // YUV 422 Packed (YUYV) MS FOURCC_UYVY(16)
    C3D_EPF_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EPIXFMT, * C3D_PEPIXFMT;
 
// specifies the texel format
typedef enum {                      //                                     (sz)
    C3D_ETF_CI4 = 1,                // 4BPP Pseudocolor (not supported for SCALING)
    C3D_ETF_CI8 = 2,                // 8BPP Pseudocolor 
    C3D_ETF_RGB1555 = 3,            // 1b Alpha, 5b Red, 5b Green, 5b Blue (16)
    C3D_ETF_RGB565  = 4,            // 0b Alpha, 5b Red, 6b Green, 5b Blue (16) 
    C3D_ETF_RGB8888 = 6,            // 8b Alpha, 8b Red, 8b Green, 8b Blue (32) 
    C3D_ETF_RGB332  = 7,            // 0b Alpha, 3b Red, 3b Green, 2b Blue (08)     
    C3D_ETF_Y8      = 8,            // 8b Y                                (08) 
    C3D_ETF_YUV422  = 11,           // YUV 422 Packed (YUYV) MS FOURCC_UYVY(16)
    C3D_ETF_RGB4444 = 15,            // 4b Alpha, 4b Red, 4b Green, 4b Blue (16) 
    C3D_ETF_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ETEXFMT, * C3D_PETEXFMT;


// the TMAP structure specifies how the hardware should interpret a texture 
// stored in the frambuffer.
//
// apv32Offsets is an array of host pointers to the individual maps which 
// compose a texture. Where if (bMipMap==TRUE) apv32Offsets contains one or 
// more elements the first (index 0) points at the base map and subsequent 
// elements point at the sequentially smaller maps.  For the case of 
// bMipMap == FALSE the array contains one valid element(the first) 
// and the rest are ignored.  When bMipMap == TRUE the array contains 2n+1
// pointers to mip levels, where 
// n = max( log_base_2( width_of_base_map ), log_base_2( height_of_base_map ) )
// 
//  eg: for a mip map with a 256x128        eg: for a mip map with a 512x512
//  base map the elements of apvLevels[]    base map the elements of apvLevels[]
//  points at  the following mip levels     point at the following mip levels
//    Total of NINE mip levels                Total of TEN mip levels
//    apvLevels[0] = 256x128;                 apvLevels[0] = 512x512;
//    apvLevels[1] = 128x64;                  apvLevels[1] = 256x256;  
//    apvLevels[2] = 64x32;                   apvLevels[2] = 128x128;
//    apvLevels[3] = 32x16;                   apvLevels[3] = 64x64;
//    apvLevels[4] = 16x8;                    apvLevels[4] = 32x32;
//    apvLevels[5] = 8x4;                     apvLevels[5] = 16x16;
//    apvLevels[6] = 4x2;                     apvLevels[6] = 8x8;
//    apvLevels[7] = 2x1;                     apvLevels[7] = 4x4;
//    apvLevels[8] = 1x1;                     apvLevels[8] = 2x2;
//                                            apvLevels[9] = 1x1;
#define cu32MAX_TMAP_LEV 11

typedef struct {
    C3D_UINT32      u32Size;                    // size of structure
    C3D_BOOL        bMipMap;                    // is texture a mip map
    C3D_PVOID       apvLevels[cu32MAX_TMAP_LEV];// array of pointer to map level
    C3D_UINT32      u32MaxMapXSizeLg2;          // log 2 X size of largest map
    C3D_UINT32      u32MaxMapYSizeLg2;          // log 2 Y size of largest map
    C3D_ETEXFMT     eTexFormat;                 // texel format
    C3D_COLOR       clrTexChromaKey;            // specify texel transp. clr
    C3D_HTXPAL      htxpalTexPalette;           // pointer to texture palette
} C3D_TMAP, * C3D_PTMAP;


// Z Testing mode: 
//
typedef enum {
    C3D_EZMODE_OFF          = 0,    // ignore z
    C3D_EZMODE_TESTON       = 1,    // test z, but do not update the z buffer
    C3D_EZMODE_TESTON_WRITEZ= 2,    // test z and update the z buffer
    C3D_EZMODE_MAX          = 3,
    C3D_EZMODE_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EZMODE, * C3D_PEZMODE;

// Z Comparison Function:
//      
typedef enum {
    C3D_EZCMP_NEVER     = 0,    // Z compare never passes
    C3D_EZCMP_LESS      = 1,    
    C3D_EZCMP_LEQUAL    = 2,
    C3D_EZCMP_EQUAL     = 3,
    C3D_EZCMP_GEQUAL    = 4,
    C3D_EZCMP_GREATER   = 5,
    C3D_EZCMP_NOTEQUAL  = 6,
    C3D_EZCMP_ALWAYS    = 7,    // Z compare always passes
    C3D_EZCMP_MAX       = 8,
    C3D_EZCMP_FORCE_U32    = C3D_FORCE_SIZE
} C3D_EZCMP, * C3D_PEZCMP;

//
// Rendering Context

typedef void* C3D_HRC;
typedef void* C3D_PRSDATA;

// Rendering context field enumerations 
//                                  // DATA TYPE       DEFAULT VALUE
typedef enum {                  
    C3D_ERS_FG_CLR          = 0,    // C3D_COLOR       {0,0,0,0}
    C3D_ERS_VERTEX_TYPE     = 1,    // C3D_EVERTEX     C3D_EV_VTCF
    C3D_ERS_PRIM_TYPE       = 2,    // C3D_EPRIM       C3D_EPRIM_TRI
    C3D_ERS_SOLID_CLR       = 3,    // C3D_COLOR       {0,0,0,0}
    C3D_ERS_SHADE_MODE      = 4,    // C3D_ESHADE      C3D_ESH_SMOOTH
    C3D_ERS_TMAP_EN         = 5,    // C3D_BOOL        FALSE
    C3D_ERS_TMAP_SELECT     = 6,    // C3D_HTX         NULL
    C3D_ERS_TMAP_LIGHT      = 7,    // C3D_ETLIGHT     C3D_ETL_NONE
    C3D_ERS_TMAP_PERSP_COR  = 8,    // C3D_ETPERSPCOR  C3D_ETPC_THREE
    C3D_ERS_TMAP_FILTER     = 9,    // C3D_ETEXFILTER  C3D_ETFILT_MINPNT_MAG2BY2
    C3D_ERS_TMAP_TEXOP      = 10,   // C3D_ETEXOP      C3D_ETEXOP_NONE
    C3D_ERS_ALPHA_SRC       = 11,   // C3D_EASRC       C3D_EASRC_ONE
    C3D_ERS_ALPHA_DST       = 12,   // C3D_EADST       C3D_EADST_ZERO
    C3D_ERS_SURF_DRAW_PTR   = 13,   // C3D_PVOID       Desktop Surface
    C3D_ERS_SURF_DRAW_PITCH = 14,   // C3D_UINT32      Desktop Surface
    C3D_ERS_SURF_DRAW_PF    = 15,   // C3D_EPIXFMT     Desktop Surface
    C3D_ERS_SURF_VPORT      = 16,   // C3D_RECT        Desktop Surface
    C3D_ERS_FOG_EN          = 17,   // C3D_BOOL        FALSE
    C3D_ERS_DITHER_EN       = 18,   // C3D_BOOL        TRUE
    C3D_ERS_Z_CMP_FNC       = 19,   // C3D_EZCMP       C3D_EZCMP_ALWAYS
    C3D_ERS_Z_MODE          = 20,   // C3D_EZMODE      C3D_EZMODE_OFF
    C3D_ERS_SURF_Z_PTR      = 21,   // C3D_PVOID       NULL
    C3D_ERS_SURF_Z_PITCH    = 22,   // C3D_UINT32      Desktop Surface
    C3D_ERS_SURF_SCISSOR    = 23,   // C3D_RECT        Desktop clipping
    C3D_ERS_NUM             = 24,    // invalid enumeration
    C3D_ERS_FORCE_U32    = C3D_FORCE_SIZE
} C3D_ERSID, * C3D_PERSID;

// 
// Inlines
//
#define SET_CIF_COLOR(exp, red, grn, blu, alp)           \
    (exp).r             = (C3D_UINT8)(red);              \
    (exp).g             = (C3D_UINT8)(grn);              \
    (exp).b             = (C3D_UINT8)(blu);              \
    (exp).a             = (C3D_UINT8)(alp);

//
// CIF3D Module Functions
//
#if defined (BUILD_AS_DLL) 
//export both the function as is and a function pointer
//This allows a GetProcAddress() to work without name mangling
#define PROTOTYPE(function_name, return_type, parameters)   \
return_type DLLEXPORT WINAPI function_name parameters;      \
typedef return_type (WINAPI *function_name##_t) parameters; \
extern DLLEXPORT function_name##_t function_name##_lib
#else
#define PROTOTYPE(function_name, return_type, parameters)   \
typedef return_type (WINAPI *function_name##_t) parameters; \
return_type DLLEXPORT WINAPI function_name parameters;      
#endif


#if defined (DOS_BUILD) || defined (NT_BUILD)
// only needed for DOS but does nobody any harm
PROTOTYPE (ATI3DCIF_DosInit, C3D_EC, ( PVOID pPHXINFO ));
#else
// initializes the ATI3DCIF driver module
//
PROTOTYPE (ATI3DCIF_Init, C3D_EC, (void));
#endif


// terminates the ATI3DCIF driver module
//
PROTOTYPE (ATI3DCIF_Term, C3D_EC, (void));

// returns information about the 3DCIF module
//         
PROTOTYPE (ATI3DCIF_GetInfo, C3D_EC, (PC3D_3DCIFINFO p3DCIFInfo));

//
// Texture Management Functions
//

// registers a texture with the CIF module.  
//
// ptmapToReg is a client specified info on texture to be reg'ed
// phtmap is the returned handle to the registered texture 
PROTOTYPE (ATI3DCIF_TextureReg, C3D_EC, (C3D_PTMAP ptmapToReg,C3D_PHTX  phtmap));


// Unregisters a texture with the CIF module.  
//
PROTOTYPE (ATI3DCIF_TextureUnreg, C3D_EC, (C3D_HTX htxToUnreg));


// Create a texture palette
//
// epalette ==> kind of palette
// pclrPalette ==> 16 or 256 entries (colors and flags)
// phtpalCreated ==> place to return handle to created palette
PROTOTYPE (ATI3DCIF_TexturePaletteCreate, C3D_EC, (C3D_ECI_TMAP_TYPE epalette,C3D_PPALETTENTRY pclrPalette, C3D_PHTXPAL phtpalCreated));

// Destroy a texture palette
//
PROTOTYPE (ATI3DCIF_TexturePaletteDestroy, C3D_EC, (C3D_HTXPAL htxpalToDestroy));

// Animate a texture palette.  Low overhead function to update palette entries
//
PROTOTYPE (ATI3DCIF_TexturePaletteAnimate, C3D_EC, (C3D_HTXPAL htxpalToAnimate,C3D_UINT32 u32StartIndex,C3D_UINT32 u32NumEntries,C3D_PPALETTENTRY pclrPalette));

// Loads an object into the framebuffer using the bus-master DMA hardware.
// Available only if C3D_CAPS_LOAD_OBJECT bit is set in the u32CIFCaps field
// of the C3D_3DCIFINFO struct.
//
// pvObjSrc ==> pointer to object in system mem
// pvObjDst ==> pointer to dst in framebuffer
// u32Size ==> size of object to load in bytes
PROTOTYPE (ATI3DCIF_LoadObject, C3D_EC, (C3D_PVOID   pvObjSrc,C3D_PVOID   pvObjDst,C3D_UINT32  u32Size ));

//
// Context Functions
//

// Creates a ATI3DCIF rendering context initialized to the default state
// specified above
PROTOTYPE (ATI3DCIF_ContextCreate, C3D_HRC, (void));

// Destroys a ATI3DCIF rendering context. 
//
PROTOTYPE (ATI3DCIF_ContextDestroy, C3D_EC, (C3D_HRC     hRC));

// Set State in a ATI3DCIF rendering context  
//
// hRC ==> handle to a created rendering context
// eRStateID ==> id of state to set
// pRStateData ==> data to set

#ifdef GAME_DEBUG
PROTOTYPE (ATI3DCIF_DoContextSetState, C3D_EC, (C3D_HRC hRC, C3D_ERSID   eRStateID, C3D_PRSDATA pRStateData));
#else
PROTOTYPE (ATI3DCIF_ContextSetState, C3D_EC, (C3D_HRC hRC, C3D_ERSID   eRStateID, C3D_PRSDATA pRStateData));
#endif

//
// Rendering Functions
//

// Prepares hardware to draw using the context referred to by the specified 
// handle.  RenderBegin must be called prior to any other Render* functions.
// RenderBegin will(may?) fail if the Primary Surface has been locked through 
// DirectDraw.(?)  Typically called prior to rendering each frame
//
PROTOTYPE (ATI3DCIF_RenderBegin, C3D_EC, (C3D_HRC     hRC));

// Ends 3D hardware drawing operations.  Frees up the graphics hardware for 2D
// operations. 
// Typically called after rendering each frame
//
PROTOTYPE (ATI3DCIF_RenderEnd, C3D_EC, (void));


// Switches to a new rendering context.  ATI3DCIF_RenderSwitch(hrc) is a 
// lighter weight, functional equivalent to the the combination of 
// ATI3DCIF_RenderEnd() followed by ATI3DCIF_RenderBegin(hrc)
// Only valid while in the 3D rendering state.
//
PROTOTYPE (ATI3DCIF_RenderSwitch, C3D_EC, (C3D_HRC     hRC));

// RenderPrimStrip in the rendering context specified with the render begin call
// Only valid while in the 3D rendering state.
//
#ifdef GAME_DEBUG
PROTOTYPE (ATI3DCIF_DoRenderPrimStrip, C3D_EC, (C3D_VSTRIP  vStrip, C3D_UINT32  u32NumVert));
#else
PROTOTYPE (ATI3DCIF_RenderPrimStrip, C3D_EC, (C3D_VSTRIP  vStrip, C3D_UINT32  u32NumVert));
#endif

// RenderPrimList in the rendering context specified with the render begin call
// Only valid while in the 3D rendering state.
//
#ifdef GAME_DEBUG
PROTOTYPE (ATI3DCIF_DoRenderPrimList, C3D_EC, (C3D_VLIST   vList, C3D_UINT32  u32NumVert));
#else
PROTOTYPE (ATI3DCIF_RenderPrimList, C3D_EC, (C3D_VLIST   vList, C3D_UINT32  u32NumVert));
#endif

#ifndef  DOS_BUILD
#pragma pack(pop)               // restore packing state
#endif

#ifdef __cplusplus
}
#endif


#ifdef RCS_H_KEYS
    static char *rcsid_h_ati3dcif_ati = "$Revision: 1.1 $" __FILE__;
#endif

#endif // ATI_ATI3DCIF_H

//
// $Author: jon $
// $Revision: 1.1 $
// $Date: 1997/12/10 17:04:47 $
// $Modtime:   05 Dec 1996 14:05:56  $
// $Log: ati3dcif.h $
// Revision 1.1  1997/12/10 17:04:47  jon
// Initial revision
// Revision 1.1  1997/12/01 11:48:48  jon
// Initial revision
// Revision 1.1  1997/12/01 11:42:00  jon
// Initial revision
// 
//    Rev 2.28   05 Dec 1996 14:18:30   xwu
// Add in GAME_DEBUG for GAME_DEBUG build
// 
//    Rev 2.27   Aug 28 1996 17:08:36   cjiang
// Restore the development tree since the changes made between
// Aug. 15 and Aug 23 are lost on the server but exist on
// my machine. 
// 
//    Rev 2.26   Aug 21 1996 15:50:48   mmintz
// Chenged header to export functions
// 
//    Rev 2.25   Aug 15 1996 11:31:22   philr
// Changed PROTOTYPE definitions under mmintz direction.
// 
//    Rev 2.24   Aug 01 1996 16:59:40   mmintz
// 
//    Rev 2.23   Jul 26 1996 14:05:30   mmintz
// new static library
// 
//    Rev 2.22   Jul 23 1996 15:33:06   mmintz
// 
//    Rev 2.21   Jul 16 1996 11:42:04   mmintz
// changed enum force value
// 
//    Rev 2.20   Jul 16 1996 09:34:28   mmintz
// OPEN GL changes and added Memory control register
// 
//    Rev 2.19   Jun 27 1996 10:15:06   mmintz
// interface for scissors and stipple
// 
//    Rev 2.18   Jun 21 1996 16:39:44   mmintz
// ci4 work
// 
//    Rev 2.17   Jun 05 1996 13:26:52   mmintz
// added new error - chip capability to indicate that the chip cannot 
// perform some functino (like Z buffering) even though the
// driver is aware of the feature.
// 
//    Rev 2.16   Jun 03 1996 08:39:10   mmintz
// z buffer
// 
//    Rev 2.15   May 15 1996 18:09:00   tomp
// Added new interface for palettized textures, Z buffer and Dither Control
// 
//    Rev 2.14   May 01 1996 16:37:12   philr
// changed UINT32 to C3D_UINT32
// 
//    Rev 2.13   Apr 26 1996 02:16:00   tomp
// fixed typo in C3D_ERSID enumeration
// 
//    Rev 2.12   Apr 26 1996 02:02:56   tomp
//     Added 4 new states to enable fogging
//         C3D_ERS_FG_CLR
//         C3D_ERS_FOG_EN
//         C3D_ERS_Z_CMP_FNC
//         C3D_ERS_Z_MODE   
//     Modified ATI3DCIF.H interface header to support the followin
//     new features when they are available in hardware:
//         -Palettized Textures
//         -Z Testing
//         -Object Loading
//     Added better documentation for TMap structure; examples on how to set
//     up for mip maps.
// 
//    Rev 2.11   Mar 19 1996 21:21:26   philr
// Attempted to remove null chars at end which disturb Watcom
// 
//    Rev 2.10   Mar 04 1996 02:06:12   tomp
// Merged DOS ATI3DCIF code
// 
//    Rev 2.9   Mar 03 1996 15:55:28   tomp
// Added push and pop pragma to preserve the structure packing state
// 
//    Rev 2.8   Mar 02 1996 22:31:04   tomp
// Added structure packing pragma
// 
//    Rev 2.7   Feb 29 1996 11:46:58   tomp
// Added Texel Op modes to the 3D CIF.  "Texel Op" is the name I'm using refer
//     to the group of operations (other than filtering) the GT can apply to 
//     texels as they are read from a texture, such as:
//     1. Texel Chroma Key
//     2. Texel Alpha Mask
//     3. Texel Alpha Blending
// Added support for all filtering modes, but tri-linear
// Fixed a couple bugs found in the d6 release of 3D CIF        
// 
//    Rev 2.6   Feb 15 1996 00:23:22   tomp
// D6 release
// 
// Optimized
//     Alpha Blender is turned off for the nop combinations of SRC and DST funct
//     Alpha interpolators are not setup when neither the SRC nor the DST funct
//     depend on source alpha
//
// Fixed
//     texture chorma keying bug
//     
// Added new Pixel and Texel Formats   
// 
//    Rev 2.5   Jan 29 1996 18:00:38   tomp
// Added to list of restrictions based on testing done prior to beta II release
// 
//    Rev 2.4   Jan 08 1996 23:57:42   tomp
// Converted to the beta 3DCIF interface
// 
//    Rev 2.3   Dec 29 1995 16:19:38   tomp
// Cleaned up and added more comments
// 
//    Rev 2.2   Dec 22 1995 03:49:56   tomp
// Updated interface.  Removed dependencies on external types and on Direct
// Draw.  Replaced implicit Lock/explicit Unlock with Begin/End Render calls
// Added support for filtering mode selection.
// 
//    Rev 2.1   Dec 20 1995 17:47:54   tomp
// Inital Revision
