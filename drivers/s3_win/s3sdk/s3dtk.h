/*********************************************************************

  S3D ToolKit library.

  Copyright S3 Inc, 1995 - 1996.

  Developer : Alex Lyashevsky.

*********************************************************************/

#ifndef __S3DTK_H

#define __S3DTK_H

#define S3DTKVALUE float
#define S3DTK_TRUE  1
#define S3DTK_FALSE 0

/*
  S3D ToolKit library is aimed to ease programming the S3 graphical
  controller advanced 3D/2D capabilities. It is conceptually a HAL
  of S3D engine with several utility functions.

  Environment and OS requirement:

  1. DOS + Rational System Extender.
  2. WIN95 + S3KRNL32.DLL + S3KERNEL.VXD.

  The library consists of four groups of functions :

  1. Set/GetState functions.
  2. 3D primitive functions.
  3. 2D primitive functions.
  4. Initialization and service functions.

  Outline for multi-threading Apps.

  1. Renderers created in different processes and in different thread of
  the same process could rely on the contention resolving scheme built in
  the ToolKit library. There is no need for explicit call to S3DK_EnterCritical
  if an user calls any ToolKit rendering functions( 2D or 3D ).

  2. If however user will decide to use S3D_EnterCtritical/ ReleaseCritical
  (     it might be if he/she uses the library by single primitive basis but needs
  to prevent another app to use S3 2D/3D HW per primitives' packet basis )
  there is no restriction to use them.
  An user could call EnterCritical several times without any harm - only the
  first call will be taken into account. Similarly he/she can call
  ReleaseCritical pretty flexible. If there was a previous EnterCritical call
  from the same thread the critical section will be freed for entering by any
  other threads. If there was not at least one EnterCritical from the thread
  there will not be any action.


*/

#define S3DTK_ERR 0
#define S3DTK_OK  1

#define S3DTK_ON 1
#define S3DTK_OFF 0




/*
   Set/GetState functions' KEY parameters
*/


typedef enum {

/*
   SYSTEM INFORMATION
*/

/* Returns version number in the lowest 2 bytes ( Get ).*/
  S3DTK_VERSION,

/*
  VIDEO/MEMORY CONTROL AND INFORMATION  ( DOS ONLY )
*/

/* Sets possible SVGA video mode( see the list below )( Set )( DOS only ).
 Returns current video mode ( Get )( DOS only ). */
  S3DTK_VIDEOMODE,
/* Returns linear address of the video memory( Get ) ( DOS only ). */
  S3DTK_VIDEOMEMORYADDRESS,
/* Returns size of the video memory( Get ) ( DOS only ).  */
  S3DTK_VIDEOMEMORYSIZE,

/*
   BUFFERIZATION
*/

/*
// Sets current rendering surface using pointer on the S3DTK_SURFACE
// structure( see STRUCTURES section ) as a second parameter ( Set ).
// Returns information of the current rendering surface using
// the second parameters as pointer to memory where the content
// of the S3DTK_SURFACE structure will be copied ( Get ).
*/
  S3DTK_DRAWSURFACE,
/*
//      Sets current visible surface using pointer on the S3DTK_SURFACE
// structure( see STRUCTURES section ) as a second parameter ( Set ).
// Returns information of the current visible surface using
// the second parameters as pointer to memory where the content
//  of the S3DTK_SURFACE structure will be copied ( Get ).
*/


  S3DTK_DISPLAYSURFACE,

/*
// Returns 1 if Vsinc interrupt status pending( display has been
// fully refreshed), otherwise returns 0. ( Get ). This call should be
// used to prevent current visible surface being overdrawn
// by next one or skipping previous frame.
*/

  S3DTK_DISPLAYADDRESSUPDATED,

/*
// Returns immediately after nearest Vsinc( Set ). The call intends
// to guarantee that the change of visible surfaces
// would not happen in the middle of the display refresh.
//  S3DTK_VSYNCWAITFORNEXT,
*/

  S3DTK_GRAPHICS_ENGINE_IDLE,
/*
// Returns TRUE if S3 graphical engine idle. The call gives the correct
// time point for switching back anf fron buffer.
*/

  S3DTK_DMA_OFF,
  S3DTK_DMA_ON,

/*
// Turns OFF and ON DMA in DMA data transfer mode
*/

/*
   RENDERING TYPE
*/

/* Sets/returns current rendering type( see list below )( Set/Get ). */
  S3DTK_RENDERINGTYPE,

/*
   Z BUFFER CONTROL
*/
/* Sets/Returns   Z-buffer offset in video memory( Set/Get ). */
  S3DTK_ZBUFFERSURFACE,
/*
// Sets/returns z-buffer compare mode( see the list below )
// ( Set/Get ).
*/
  S3DTK_ZBUFFERCOMPAREMODE,
/*
// Sets Z-buffering off or on with value eq
// 0 or 1 respectively( SET ).
// Note : ZBUFFERENABLE should be called after seting rendering buffer
// otherwise the call will return an error.
*/
  S3DTK_ZBUFFERENABLE,

/*
// Enables or disables Z-buffer updating.
// By default it is enable. But might be disable for transparency
*/

  S3DTK_ZBUFFERUPDATEENABLE,

/*
   TEXTURE CONTROL
*/

/*
// Sets current texture using pointer on the S3DTK_SURFACE
// structure( see STRUCTURES section ) as a second parameter ( Set ).
// Returns information of the current texture using the second parameters
// as pointer to memory where the content
// of the S3DTK_SURFACE structure will be copied ( Get ).
*/
  S3DTK_TEXTUREACTIVE,
/*
// Sets/returns current texture filtering mode( see the list below )
// ( Set/Get ).
*/
  S3DTK_TEXFILTERINGMODE,
/*
// Sets/returns current texture blending mode( see the list below )
// ( Set/Get ).
*/
  S3DTK_TEXBLENDINGMODE,
  S3DTK_TEXMAXMIPMAPLEVEL,

/*
   ALPHABLENDING                          
*/

/*
// Sets/returns current way of using ALPHA channel
// ( see the list below ) ( Set/Get ).
*/
  S3DTK_ALPHABLENDING,  

/*
   FOG
*/
/*
// Sets current color for fogging in RGBX888 format 
// and turn fogging on. If value eq S3DTK_FOGOFF
// fogging will be tuned off( Set ).
// Returns current fog color or -1 if fogging
// is off( Get ).
*/
  S3DTK_FOGCOLOR,
/*
// If S3DTK_ON is set the Toolkit won't compute D level
// but will wait for D from higher level SW insead, otherwise
// ( S3DTK_OFF - default ) D will be computed with Toolkit.
*/

  S3DTK_D_LEVEL_SUPPLIED,

/*
   Set/Get clipping sub-rect area inside a current rendering surface
*/
  S3DTK_CLIPPING_AREA,

/*
   Flip and wait until done (default is TRUE) - DOS only
*/
  S3DTK_FLIP_WAIT

} S3DTK_STATEKEY;


/* SUPPORTED VIDEO MODES */

#define S3DTK_MODE320x200x8       0x13
#define S3DTK_MODE320x200x15      0x10D
#define S3DTK_MODE512x384x8       0x215
#define S3DTK_MODE512x384x15      0x216
#define S3DTK_MODE640x400x8       0x100
#define S3DTK_MODE640x480x8       0x101
#define S3DTK_MODE640x480x15      0x110
#define S3DTK_MODE640x480x24      0x112
#define S3DTK_MODE800x600x8       0x103
#define S3DTK_MODE800x600x15      0x113
#define S3DTK_MODE800x600x24      0x115
#define S3DTK_MODE1024x768x8      0x105
#define S3DTK_MODE1024x768x15     0x116
#define S3DTK_MODE1024x768x24     0x118 

#define S3DTK_MODESET_NO_CLEAR    0x8000    /* OR the mode with this flag to */
                                            /* set mode without clear the    */
                                            /* video memory                  */


/* SUPPORTED SURFACE FORMATS */
#define S3DTK_VIDEORGB8           0         /* palettized */
#define S3DTK_VIDEORGB15          0x4L      /* 5, 5, 5    */
#define S3DTK_VIDEORGB24          0x8L      /* 8, 8 ,8    */
#define S3DTK_SYSTEM              0x80000000L   /* surface is in system memory*/
#define S3DTK_VIDEO               0x00000000L   /* surface is in video memory */
#define S3DTK_TEXTURE             0x40000000L   /* surface for texture        */
#define S3DTK_Z16                 0x10000000L   /* artificial Z-buffer format */


/* POSSIBLE RENDERING TYPES */
#define S3DTK_GOURAUD               0L
#define S3DTK_LITTEXTURE            0x8000000L
#define S3DTK_UNLITTEXTURE          0x10000000L
#define S3DTK_LITTEXTUREPERSPECT    0x28000000L
#define S3DTK_UNLITTEXTUREPERSPECT  0x30000000L


/* Z COMPARE MODES */
#define S3DTK_ZNEVERPASS            0L
#define S3DTK_ZSRCGTZFB             0x100000L                
#define S3DTK_ZSRCEQZFB             0x200000L                
#define S3DTK_ZSRCGEZFB             0x300000L                
#define S3DTK_ZSRCLSZFB             0x400000L                
#define S3DTK_ZSRCNEZFB             0x500000L                
#define S3DTK_ZSRCLEZFB             0x600000L                
#define S3DTK_ZALWAYSPASS           0x700000L


/* SUPPORTED TEXTURE COLOR FORMATS */
#define FORMAT_SHIFT  5
#define S3DTK_TEXARGB8888      ( 0 << FORMAT_SHIFT )
#define S3DTK_TEXARGB4444      ( 1L << FORMAT_SHIFT )
#define S3DTK_TEXARGB1555      ( 2L << FORMAT_SHIFT )
#define S3DTK_TEXPALETTIZED8   ( 6L << FORMAT_SHIFT )


/* TEXTURE FILTERING MODE */
#define FILTER_SHIFT  12
#define S3DTK_TEXM1TPP         ( 0L << FILTER_SHIFT )  /* MIP_NEAREST */
#define S3DTK_TEXM2TPP         ( 1L << FILTER_SHIFT )  /* LINEAR MIP NEAREST */
#define S3DTK_TEXM4TPP         ( 2L << FILTER_SHIFT )  /* MIP LINEAR */
#define S3DTK_TEXM8TPP         ( 3L << FILTER_SHIFT )  /* LINEAR MIP LINEAR */
#define S3DTK_TEX1TPP          ( 4L << FILTER_SHIFT )  /* NEAREST ( NO MIP ) */
#define S3DTK_TEXV2TPP         ( 5L << FILTER_SHIFT )  /* thought to be VIDEO but actually is linear filter */
#define S3DTK_TEX4TPP          ( 6L << FILTER_SHIFT )  /* LINEAR ( NO MIP ) */

/* TEXTURE BLENDING MODE */
#define BLEND_MODE_SHIFT 15

#define S3DTK_TEXMODULATE         ( 1L << BLEND_MODE_SHIFT )
#define S3DTK_TEXDECAL            ( 2L << BLEND_MODE_SHIFT )



/* ALPHA BLENDING TYPES */
#define ALPHA_BLEND_CONTROL_SHIFT   18
#define S3DTK_ALPHAOFF               0
#define S3DTK_ALPHATEXTURE           2 << ALPHA_BLEND_CONTROL_SHIFT
#define S3DTK_ALPHASOURCE            3 << ALPHA_BLEND_CONTROL_SHIFT

/*
   FOG
*/
#define S3DTK_FOGOFF        0xFFFFFFFF

/* ERROR LIST */

/* 3D capabilities are not supported */
#define S3DTK_3DCAPNOTSUPPORTED       -1
/* cant convert from linear to phys address */
#define S3DTK_CANTCONVERT             -2
/* invalid format */
#define S3DTK_INVALIDFORMAT           -3
/* null pointer */
#define S3DTK_NULLPOINTER             -4
/* invalid rendering type */
#define S3DTK_INVALIDRENDETINGTYPE    -5
/* invalid value*/
#define S3DTK_INVALIDVALUE            -6
/* unsupported video mode */
#define S3DTK_UNSUPPORTEDVIDEOMODE    -7
/* invalid filtering mode */
#define S3DTK_INVALIDFILTERINGMODE    -8
/* unsupported key */
#define S3DTK_UNSUPPORTEDKEY          -9
/* unsupported method */
#define S3DTK_UNSUPPORTEDMETHOD       -10
/* surface is not set */
#define S3DTK_INVALIDSURFACE          -11
/* can't find S3KERNEL VxD */
#define S3DTK_NOS3KERNEL              -12




/*
   STRUCTURES
*/


/*
  LIB_INIT Flags
*/
#define S3DTK_INITPIO                    0 /* programming device using registers */
#define S3DTK_INITDMA                    1 /* programming device using command DMA */
#define S3DTK_INIT2D_SERIALIZATION_ON    2 /* serialize access to S3D endine */
#define S3DTK_INIT_DMA_SIZE_64K           0x10 /* if bit 0 is set this bit defines the DMA size */
                                          /* either needed or provided. if 0 - 4K( default ), 1 - 64k */
/* DO not use */
#define S3DTK_INIT_VIDEO_BUFFER_ADDR_PROVIDED 4 /* addrs for video buff and MMI0 space are */
                                           /* provided by this call */
/* DO not use */
#define S3DTK_INIT_DMA_BUFFER_ADDR_PROVIDED 8 /* address of contigiuos memblock for DMA */
                                              /* is supplied */

typedef struct _S3DTK_LIB_INIT {

  ULONG libFlags;
  ULONG libVideoBufferLinAddr;
  ULONG libMMIOSpaceLinAddr;

} S3DTK_LIB_INIT, * S3DTK_LPLIB_INIT;

/*
  RENDERER_INITSTRUCT Flags Bits

*/

#define S3DTK_GENERIC         0   /* float + do not clip UV + do not clip XY */
#define S3DTK_FORMAT_FLOAT    0
#define S3DTK_FORMAT_FIXED    1L  /* not implemented */
#define S3DTK_VERIFY_UVRANGE  2L  /* is needed only for perspactive texture mapping */
#define S3DTK_VERIFY_XYRANGE  4L  /* is needed if upper layer sw does not consider to clip
                                    along the view port window 
                                                                  */


#define S3DTK_USER_GENERIC 0
#define S3DTK_USER_WIN95   1

#define S3DTK_APP_GENERIC 0
#define S3DTK_APP_D3D     1

typedef struct {

  ULONG initFlags;
  ULONG initUserID;  /* reserved */
  ULONG initAppID;  /* reserved */

} S3DTK_RENDERER_INITSTRUCT, * S3DTK_LPRENDERER_INITSTRUCT;

typedef struct {
  
  ULONG sfOffset;                         /* should be filled */
  ULONG sfWidth;                          /* should be filled */
  ULONG sfHeight;                         /* should be filled */
  ULONG sfFormat;                         /* should be filled */
  ULONG reserved[ 5 ];

} S3DTK_SURFACE, FAR * S3DTK_LPSURFACE;

#ifndef WIN32
typedef struct {

  long int left;
  long int top;
  long int right;
  long int bottom;

} S3DTK_RECTAREA, FAR * S3DTK_LPRECTAREA;

#else

typedef RECT S3DTK_RECTAREA;

typedef S3DTK_RECTAREA FAR *  S3DTK_LPRECTAREA;
#endif

typedef struct _s3dtk_vertex_lit
{

  S3DTKVALUE  X;
  S3DTKVALUE  Y;
  S3DTKVALUE  Z;
  S3DTKVALUE  W; /* not used */
  BYTE        B;
  BYTE        G;
  BYTE        R;
  BYTE        A;

} S3DTK_VERTEX_LIT, FAR * S3DTK_LPVERTEX_LIT;

typedef struct _s3dtk_vertex_tex {

S3DTKVALUE X;
S3DTKVALUE Y;
S3DTKVALUE Z;
S3DTKVALUE W;
BYTE B;
BYTE G;
BYTE R;
BYTE A;
S3DTKVALUE D;
S3DTKVALUE U;
S3DTKVALUE V;
 } S3DTK_VERTEX_TEX, FAR * S3DTK_LPVERTEX_TEX;


#ifdef __cplusplus
extern "C"
{
#endif


/*
  S3DTK FUNCTIONS LIST
*/

/*  Acceptable primitive types */

#define S3DTK_TRILIST  0
#define S3DTK_TRISTRIP 1
#define S3DTK_TRIFAN   2

typedef struct _s3dtk_function_list
{


/*
  1. Set/GetState functions.
*/


/********************************************************************
//
//    Set 3D engine states
//
//    ULONG S3DTK_SetState( void  * pFuncStruct,
//                            ULONG State, ULONG Value );
//
//    Function update the state of 3D engine. The updated state
//    will take effect from the next primitive will be rendered.
//
//        PARAMS:
//
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//        ULONG State  - KEY identifying one of the category of 3D engine state.
//                   ( see the list below ).
// 
//    ULONG Value  - VALUE representing one of the possible value
//                   from the given category.( see lists below ).
*********************************************************************/
  ULONG ( *   S3DTK_SetState )( void  * pFuncStruct,
                             ULONG State, ULONG Value );
/********************************************************************
//
//    Get 3D engine states
//
//    ULONG S3DTK_GetState( ULONG State,ULONG Value );
//
//    Function returns the current state of 3D engine.
//
//        PARAMS:
//
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//        ULONG State  - KEY identifying one of the category of 3D engine
//                   state. ( see the list below ).
// 
//    ULONG Value  - VALUE actually is a pointer on a buffer to be used
//                   for returning values.
*********************************************************************/
  ULONG (   * S3DTK_GetState )( void  * pFuncStruct,
                             ULONG State, ULONG Value );


/*
  2. 3D primitive functions.
*/


/***********************************************************************
//    Render triangles
//
//    ULONG S3DTK_TriangleSet( ULONG FAR * pVertexSet, ULONG NumVertexes,
//                         ULONG SetType );
//
//        The call causes S3D engine to render specified type of
//    the primitive set.
// 
//    PARAMS :
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    
//    ULONG FAR * pVertexSet - pointer of the array of S3D_LPVERTEX
//                             pointers
//    ULONG NumVertexes      - number of the pointers.
//    ULONG SetType          - primitive type( see list above ).
***********************************************************************/
  ULONG (  * S3DTK_TriangleSet )( void  * pFuncStruct,
          ULONG FAR * pVertexSet,
          ULONG NumVertexes, ULONG SetType );
/***********************************************************************
//    Render triangles extended
//
//    ULONG S3DTK_TriangleSetEx( ULONG FAR * pVertexSet, ULONG NumVertexes,
//               ULONG SetType, ULONG FAR * pSetState, ULONG NumStates );
//
//        The call causes S3D engine to set S3D engine to specified
//    state and after that to render specified type of primitive set.
// 
//    PARAMS :
//    
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    ULONG FAR * pVertexSet - pointer of the array of S3D_LPVERTEX
//                             pointers
//    ULONG NumVertexes      - number of the vertexes.
//    ULONG SetType          - primitive type( see list above ).
//    ULONG FAR * pSetState  - pointer of the array of the pairs 
//                             ULONG KEY ; ULONG VALUE with the same
//                             meaning as in S3DTK_SetState function
//                             ( see above ).
//    ULONG NumStates                - number of those pairs.
***********************************************************************/
  ULONG (   * S3DTK_TriangleSetEx )( void  * pFuncStruct,
          ULONG FAR * pVertexSet, ULONG NumVertexes,
          ULONG SetType, ULONG FAR * pSetState, ULONG NumStates );


/*
  3. 2D primitive functions.
*/


/**********************************************************************
//
//    Copy a rect area in video memory
//
//    ULONG S3DTK_BitBlt( S3DTK_LPSURFACE pDestSurface, S3DTK_LPRECTAREA pDestRect,
//          S3DTK_LPSURFACE pSrcSurface, S3DTK_LPRECTAREA pSrcRect );
//
//    The function causes the rectangle area from video memory defined
//    by the second     parameter to be copied to the location in the video
//    memory defined by the first parameter. 
//
//    PARAMS :
// 
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    S3DTK_LPRECTAREA pDest - destination location to copy to.
//    S3DTK_LPRECTAREA pSrc  - source area to be copied.
***********************************************************************/
  ULONG (  * S3DTK_BitBlt )( void  * pFuncStruct, S3DTK_LPSURFACE pDestSurface, S3DTK_LPRECTAREA pDestRect,
          S3DTK_LPSURFACE pSrcSurface, S3DTK_LPRECTAREA pSrcRect );
/**********************************************************************
//
//    Copy a rect area in video memory extended.
//
//    ULONG S3DTK_BitBltTransparent( S3DTK_LPSURFACE pDestSurface, S3DTK_LPRECTAREA pDestRect,
//          S3DTK_LPSURFACE pSrcSurface, S3DTK_LPRECTAREA pSrcRect,
//          ULONG TransparentColor );
//    The function causes the rectangle area from video memory defined
//    by the second     parameter to be copied to the location in the video
//    memory defined by the first parameter. If a source pixel color is 
//    equal to the color defined by the third parameter the destination
//    pixel will not be updated. Color format for transparent color respects
//    the surfaces' color foramat.
//
//    PARAMS :
// 
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    S3DTK_LPRECTAREA pDest - destination location to copy to.
//    S3DTK_LPRECTAREA pSrc  - source area to be copied.
//    ULONG TransparentColor - transparent color.
***********************************************************************/
  ULONG (   * S3DTK_BitBltTransparent )( void  * pFuncStruct, S3DTK_LPSURFACE pDestSurface, S3DTK_LPRECTAREA pDestRect,
          S3DTK_LPSURFACE pSrcSurface, S3DTK_LPRECTAREA pSrcRect,
          ULONG TransparentColor );
/**********************************************************************
//
//    Fill a rect area in video memory with a particular color
//
//    ULONG S3DTK_RectFill( S3DTK_LPRECTAREA pDest, ULONG FillColor );
//
//    The function causes the rectangle area defined by the first
//    parameter to be filled with the color from the second parameter.
//    Color format for filling respects the surface's color format.

//    PARAMS :
// 
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    S3DTK_LPRECTAREA pDest - destination to be filled.
//    ULONG FillPattern        - color for filling.
***********************************************************************/
  ULONG (  * S3DTK_RectFill )( void  * pFuncStruct,
          S3DTK_LPSURFACE pDestSurface,
          S3DTK_LPRECTAREA pDestRect,
          ULONG FillPattern );

/*
  4. Initialization and service functions.
*/

/**********************************************************************
//
//    Return last error occurred while calling the Toolkit
//    library.
//
//    ULONG S3DTK_GetLastError( );
//
//    The function will be used to obtain the code of
//    the last error if the call will immediately follow the
//    the another function call having returned S3DTK_ERR.
//
//    PARAMS :
// 
//        void  * pFuncStruct - pointer on the current instance of
//                              S3DTK_FUNCTIONLIST structure
//    NONE.
//
//    RETURN
//
//    Error code from the list( not ready ).
***********************************************************************/
  int (  * S3DTK_GetLastError )( void  * pFuncStruct );


#ifndef WIN32
/*
  5.  Hardware stretching function. (DOS only)
*/

/**********************************************************************
//
//    ULONG S3DTK_StretchDisplaySurface( void  * pFuncStruct,
//          S3DTK_LPSURFACE pDisplaySurface, ULONG width, ULONG height );
//
//    Stretch the surface pointed by pDisplaySurface to width by height.
//    This function is only supported in DOS only.  Use DirectDraw
//    functions in Win95.
//
//    PARAMS :
// 
//      pFuncStruct - pointer on the current instance of
//                    S3DTK_FUNCTIONLIST structure
//      width0 - width of the display area before stretched
//      height0 - height of the display area before stretched
//      width1 - width of the display area after stretched
//      height1 - height of the display area after stretched
//                        
//    RETURN
//
//      S3DTK_OK if successful
//      S3DTK_ERR if not successful
//    .
***********************************************************************/
  ULONG (  * S3DTK_StretchDisplaySurface )(  void  * pFuncStruct,
          ULONG width0, ULONG height0, ULONG width1, ULONG height1 );

#endif  /* not for WIN32 */

} S3DTK_FUNCTIONLIST, * S3DTK_LPFUNCTIONLIST;


/**********************************************************************
//
//    Maps given physical address and the size of the physical 
//    memory area to linear address.
//
//    ULONG S3DTK_PhysicalToLinear( ULONG PhysAddr, ULONG Size );
//
//    Caution : In the multi-threading environment linear address is valid
//    only in the calling thread's memory context.
//
//    PARAMS :
// 
//    ULONG PhysAddr - physical address( usually device address in
//                     386 4GB address space ).
//    ULONG Size         - size of the area to be mapped.
//
//    RETURN
//
//    Linear address or S3DTK_ERR.
***********************************************************************/


extern ULONG S3DTK_PhysicalToLinear( ULONG PhysAddr, ULONG Size );
/**********************************************************************
//
//    Finds a physical address being mapped on the given linear
//    address.
//
//    ULONG S3DTK_LinearToPhysical( ULONG Linear );
//
//    PARAMS :
// 
//    ULONG Linear - linear address in the current thread's memory context.
//
//    RETURN
//
//    Physical address or S3DTK_ERR.
***********************************************************************/
extern ULONG S3DTK_LinearToPhysical( ULONG Linear );


/**********************************************************************
//
//       Serializes access to shared resources in WIN95 multi-thread
//   environment. 
//
//       ULONG S3DTK_EnterCritical( void );
//
//   Function is for serializing an access to S3 2D/3D engine
//   among 32bit threads and WIN95 GUI subsystem. The call
//   to the function either permit the current thread to enter
//   the following code section or put the thread on hold until
//   the thread owing the section will call S3DTK_ReleaseCritical.
//
//   RETURNS
//
//       S3DTK_OK if it is successful.
//   S3DTK_ERR if sharing mechanism isn't in place.
***********************************************************************/

extern ULONG S3DTK_EnterCritical( void );

/**********************************************************************
//
//       Releases the critical section owned by the thread issued
//   S3DTK_EnterCritical
//
//       S3DTK_ReleaseCritical( void );
//
//
//   RETURNS
//
//       S3DTK_OK.
**********************************************************************/
extern ULONG S3DTK_ReleaseCritical( void );



/**********************************************************************
//
//    Init 2D/3D engine and Toolkit lobrary
//
//    ULONG S3DTK_InitLib( ULONG lParam );
//
//    The function does an initialization of the Toolkit library.
//
//    PARAMS :
// 
//    ULONG lParam  - pointer on S3DTK_LIB_INIT structure or 0.
//    if 0 - MMIO and internal video buffer and MMIO assumed.
//
//    RETURN
//
//    S3DTK_OK or one of the possible error code( see list ).
***********************************************************************/

/* FLAGS FOR INITIALIZATION */


extern ULONG S3DTK_InitLib( ULONG lParam );


/**********************************************************************
//
//    Clean up.
//
//    ULONG S3DTK_ExitLib( S3DTK_LPFUNCTIONLIST pFunctionList );
//
//    PARAMS :
//
//    RETURN
//
//    S3DTK_OK
***********************************************************************/


extern ULONG S3DTK_ExitLib( void );


/**********************************************************************
//
//    Create current instance of rendering device
//
//    ULONG S3DTK_CreateRenderer( ULONG Param1, void * * ppFunctionList );
//
//    The function does an initialization of the Toolkit library
//    and returns the pointer on the list of the S3DTK interface  
//    functions.
//
//    PARAMS :
// 
//    ULONG Param1 - pointer to the S3DTK_RENDERER_INITSTRUCT structure or 0.
//          if the value is 0 it is considered as if the structure has 
//          intFlags field with GENERIC value assigned( see structure section ).        
//        void * * ppFunctionList - placeholder for the
//                  pointer on the S3DTK functions( methods ) list.
//
//    RETURN
//
//    S3DTK_OK or one of the possible error code( see list ).
***********************************************************************/

extern ULONG S3DTK_CreateRenderer( ULONG Param1,
          void * * ppFunctionList );


/**********************************************************************
//
//    Destroy current rendering device object.
//
//    ULONG S3DTK_DestroyRenderer( S3DTK_LPFUNCTIONLIST * ppFunctionList );
//
//    PARAMS :
//
//    S3DTK_LPFUNCTIONLIST pFunctionList - pointer on the pointer of
///   list of current interface functions( methods )
//    obtained by S3DTK_Renderer.
//    Note : after this call the pFunctionList is invalid.
//
//    RETURN
//
//    S3DTK_OK
***********************************************************************/


extern ULONG S3DTK_DestroyRenderer( void * * ppFunctionList );


#ifdef __cplusplus
}
#endif


#endif
