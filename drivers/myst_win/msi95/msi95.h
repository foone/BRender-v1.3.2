/****************************************************************************
*          name: msi95.h
*
*   description: Matrox Simple Interface for Windows 95 global include file.
*
*         NOTES: - Include <windows.h> before this file.
*
******************************************************************************/

#ifndef _MSI95_H_
#define _MSI95_H_

// Debug flags
//
#define msiDBG_Release       0
#define msiDBG_DumpToFile    0x00000001     /* Dump information to MSIDBUG.LOG */
#define msiDBG_DumpToFront   0x00000002     /* Render in front buffer, no flip */

// Compare codes used to specify depth compare function and other ...
//
#define msiCMP_NEVER    1
#define msiCMP_LESS     2
#define msiCMP_EQUAL    3
#define msiCMP_LEQUAL   4
#define msiCMP_GREATER  5
#define msiCMP_NOTEQUAL 6
#define msiCMP_GEQUAL   7
#define msiCMP_ALWAYS   8

#define DECAL           0x00000001
#define MODULATE        0x00000002

// Frame buffer format. At init time, the client receives a pointer to this
// structure to know about the actual frame buffer format. The Color section
// applies to the back buffer and the Depth section to the Z buffer.
//
typedef struct _T_msiInfo
   {
   SHORT  VersionMajor;    // 1
   SHORT  VersionMinor;    // 7

   struct //_msiColor
      {
      LONG                 BytesPerPixel;
      LONG                 rDepth;
      LONG                 gDepth;
      LONG                 bDepth;
      LONG                 aDepth;
      LONG                 rBitShift;
      LONG                 gBitShift;
      LONG                 bBitShift;
      LONG                 aBitShift;
      LONG                 Pitch;            // Value expressed in bytes
      volatile LPBYTE      pMem;
      LONG                 MemSize;          // Value expressed in bytes

      } msiColor;

   struct //_msiDepth
      { 
      LONG                 BytesPerPixel;
      LONG                 zDepth;
      LONG                 zBitShift;
      LONG                 Pitch;            // Value expressed in bytes
      volatile LPBYTE      pMem;
      LONG                 MemSize;          // Value expressed in bytes

      } msiDepth;

   struct // msiTexture
      {
      volatile LPBYTE      pMem;             // Texture Cache memory
      LONG                 MemSize;

      } msiTexture;

   } T_msiInfo;

// Vertex format. This vertex format supports 2D, 3D, gouraud shading and
// texture mapping with lighting, perspective, decal, etc ...
//
typedef struct _T_msiVertex
   { 
   float x, y, z, invW, r, g, b, mr, mg, mb, u, v;

   } T_msiVertex;

// Rendering parameters. This structure is used to control the rendering
// process specifying the different attributes that control texture mapping,
// depth compare and color processing.
//
typedef struct _T_msiParameters
   {
   struct //_msiTexture
      {
      BOOL     Enable;
      long     Width,         
               Height;
      long     Planes;
      LPBYTE   pMem;
      LPBYTE   pHeap;
      long     CacheOffset;

      struct //_msiLUT
         {
         LPBYTE   pMem;
         LPBYTE   pHeap;
         long     CacheOffset;
         } msiLUT;

      BOOL     Clamp_u, 
               Clamp_v;
      BOOL     Modulate;
      BOOL     Decal;
      BOOL     Transparency;
      WORD     KeyingColor,  
               KeyingColorMask;
      WORD     KeyingAlpha,  
               KeyingAlphaMask;    // !!! 1 bit value !!!

      } msiTexture;

   struct //_msiDepth
      {
      BOOL     Enable;
      DWORD    Compare;
      BOOL     Protect;

      } msiDepth;

   struct //_msiColor
      {
      BOOL    Dither;
      BOOL    Protect;

      } msiColor;

   } T_msiParameters;

// Z plane parameters. This structure describes the coordinates and color of
// a plane when using "msiRenderZPlane" function call.
//
typedef struct _T_msiZPlane 
   {
   long  x, y,             // Plane top-left coordinate
         u, v,             // Texture top-left coordinate
         xWidth, 
         yHeight,          // Plane dimension
         uWidth, 
         vHeight;          // Texture dimension
   float z,                // Plane's depth
         r, g, b,          // Plane's color
         mr, mg, mb;       // Plane's texture modulation
   } T_msiZPlane;

// Memory status block. This structure is used to know if a drawing
// operation is completed with function IsMemoryBusy()
//
typedef struct _T_msiMemoryStatus
   {
   DWORD reserved[4];
   } T_msiMemoryStatus;

// QueryInfo structure block, this is used for querying msi
// for information about the Board.
//
typedef struct QueryInfoStruct
   { 
   DWORD ErrorStatus;
   DWORD MemorySize;
   } QueryInfoStruct;

// Minimum allowed Size for QueryInfo structure block.
//   
#define QueryInfoStructMinSize      8           // Size in bytes

// Possible error codes returned in ErrorStatus
//
#define ESCAPECALLNOTSUPPORTED      1
#define DRIVERNOTSUPPORTED          2
#define BOARDNOTSUPPORTED           4

// Function prototypes
//   
#ifdef __cplusplus

// Overall functions

   extern "C" LPBYTE			   msiAllocTextureHeap(LONG Pages);
   extern "C" BOOL			   msiAwake(void);
   extern "C" BOOL			   msiExit();
   extern "C" BOOL			   msiFreeTextureHeap(LPBYTE pTextureHeap);
   extern "C" T_msiInfo*       msiInit( LONG Width, LONG Height, LONG Planes, BOOL ZBuffer, BOOL Debug, HWND hWnd);
   extern "C" BOOL			   msiIsMemoryBusy( T_msiMemoryStatus *pMemStatus );   
   extern "C" BOOL			   msiSleep(void);
   extern "C" BOOL			   msiSync(void);
   extern "C" BOOL            msiQueryInfo(LONG StructSize,LPBYTE StructPointer);

// Frame functions

	extern "C" BOOL			   msiClearBack(float r, float g, float b);
	extern "C" BOOL			   msiClearFront(float r, float g, float b);
	extern "C" BOOL			   msiClearZ(float z);
   extern "C" BOOL			   msiEndFrame(BOOL Dump, LONG Frame, BOOL Wait);
   extern "C" BOOL			   msiSetParameters(T_msiParameters* pParameters);
   extern "C" BOOL			   msiStartFrame(BOOL ClearRGB, float r, float g, float b, BOOL ClearZ, float z);
                              
// Context Functions          
                              
   extern "C" BOOL			   msiSetDepthCompare(DWORD Cmp);
   extern "C" BOOL			   msiSetDitherEnable(BOOL Dither);
   extern "C" BOOL			   msiSetTextureBlend(ULONG msiTexBlend);
   extern "C" BOOL			   msiSetTextureCLUT4(DWORD Offset, DWORD Index );
   extern "C" BOOL			   msiSetTextureCLUT8(DWORD Offset );
   extern "C" BOOL			   msiSetTextureEnable(BOOL Enable);
   extern "C" BOOL			   msiSetTextureOffset(DWORD Offset );
   extern "C" BOOL			   msiSetTexturePlanes(LONG Planes);
   extern "C" BOOL			   msiSetTextureSize(LONG Width, LONG Height);
   extern "C" BOOL			   msiSetTextureTransparency(DWORD MaskKey );
   extern "C" BOOL			   msiSetTextureWrap(BOOL ClampU, BOOL ClampV);
                              
// Rendering functions        
                              
   extern "C" BOOL			   msiBlitRect (  LPBYTE Pheap, 
                                             LPBYTE Pmemory, 
									                  T_msiMemoryStatus *pMemStatus,
									                  WORD SourcePitch, WORD SourcePlanes, 
									                  WORD BlitWidth, WORD BlitHeight, 
									                  DWORD DestinationOff, 
									                  DWORD KeyingColor, DWORD KeyingMask );
   extern "C" BOOL			   msiDrawSingleLine( DWORD color, DWORD XYStart, DWORD XYEnd, DWORD LineStyle );
   extern "C" BOOL			   msiRenderTriangle(T_msiVertex* pV0, T_msiVertex* pV1, T_msiVertex* pV2, BYTE Opacity);
   extern "C" BOOL			   msiRenderZPlane(T_msiZPlane* pZPlane, BYTE Opacity);
   extern "C" BOOL            msiSplashScreen(void);

#else

// Overall functions

   LPBYTE		   __cdecl msiAllocTextureHeap(LONG Pages);
   BOOL			   __cdecl msiAwake(void);
   BOOL			   __cdecl msiExit();
   BOOL			   __cdecl msiFreeTextureHeap(LPBYTE pTextureHeap);
   T_msiInfo*      __cdecl msiInit( LONG Width, LONG Height, LONG Planes, BOOL ZBuffer, BOOL Debug, HWND hWnd);
   BOOL			   __cdecl msiIsMemoryBusy( T_msiMemoryStatus *pMemStatus );   
   BOOL			   __cdecl msiSleep(void);
   BOOL			   __cdecl msiSync(void);
   BOOL           __cdecl msiQueryInfo(LONG StructSize,LPBYTE StructPointer);

// Frame functions

	BOOL			   __cdecl msiClearBack(float r, float g, float b);
	BOOL			   __cdecl msiClearFront(float r, float g, float b);
	BOOL			   __cdecl msiClearZ(float z);
   BOOL			   __cdecl msiEndFrame(BOOL Dump, LONG Frame, BOOL Wait);
   BOOL			   __cdecl msiSetParameters(T_msiParameters* pParameters);
   BOOL			   __cdecl msiStartFrame(BOOL ClearRGB, float r, float g, float b, BOOL ClearZ, float z);

// Context Functions 

   BOOL			   __cdecl msiSetDepthCompare(DWORD Cmp);
   BOOL			   __cdecl msiSetDitherEnable(BOOL Dither);
   BOOL			   __cdecl msiSetTextureBlend(ULONG msiTexBlend);
   BOOL			   __cdecl msiSetTextureCLUT4(DWORD Offset, DWORD Index );
   BOOL			   __cdecl msiSetTextureCLUT8(DWORD Offset );
   BOOL			   __cdecl msiSetTextureEnable(BOOL Enable);
   BOOL			   __cdecl msiSetTextureOffset(DWORD Offset );
   BOOL			   __cdecl msiSetTexturePlanes(LONG Planes);
   BOOL			   __cdecl msiSetTextureSize(LONG Width, LONG Height);
   BOOL			   __cdecl msiSetTextureTransparency(DWORD MaskKey );
   BOOL			   __cdecl msiSetTextureWrap(BOOL ClampU, BOOL ClampV);
  
// Rendering functions

   BOOL			   __cdecl msiBlitRect (  LPBYTE Pheap, 
                                  LPBYTE Pmemory, 
	 				                  T_msiMemoryStatus *pMemStatus,
	 				                  WORD SourcePitch, WORD SourcePlanes, 
	 				                  WORD BlitWidth, WORD BlitHeight, 
	 				                  DWORD DestinationOff, 
	 				                  DWORD KeyingColor, DWORD KeyingMask );
   BOOL			   __cdecl msiDrawSingleLine( DWORD color, DWORD XYStart, DWORD XYEnd, DWORD LineStyle );
   BOOL			   __cdecl msiRenderTriangle(T_msiVertex* pV0, T_msiVertex* pV1, T_msiVertex* pV2, BYTE Opacity);
   BOOL			   __cdecl msiRenderZPlane(T_msiZPlane* pZPlane, BYTE Opacity);
   BOOL           __cdecl msiSplashScreen(void);
#endif
  
#endif // _MSI95_H_


