/*
 * s3link.h :  Header for link between BRender device driver and s3 lib
 *
 */
#ifndef _S3LINK_H_

#include <brender.h>

#include "drv.h"



/* Make texture available for rendering; cache it, transfer it to the
 * card freeing up other textures as required, and register it with the
 * S3 library */
br_boolean S3CacheTexture(struct render_buffer *src, br_boolean force_reload);


/* Start up the library, set video mode */
br_boolean S3InstantiateLibrary(br_boolean exclusive);

/* Perform all deinitialisation including restoring video mode */
br_boolean S3TerminateLibrary(void);

/* Set rendering output window */
br_boolean S3SetRenderWindow(LPDIRECTDRAWSURFACE surface,
                              br_int_32 x, br_int_32 y,
                              br_int_32 w, br_int_32 h, br_uint_32 format,
							  br_device_pixelmap *orig_pm);

/* Set Z buffer */
br_boolean S3SetZBufferState(br_boolean enable, LPDIRECTDRAWSURFACE surface,
                              br_int_32 w, br_int_32 h,br_device_pixelmap *orig_pm);

void S3LoseRenderBuffer(struct render_buffer *buffer);
void S3ClearTextureCache(void);


br_error S3ModeSet(br_uint_32 mode);
br_error S3ModeGet(br_uint_32 *mode);
br_error S3QueryState(br_uint_32 variable,br_uint_32 *value);


/*
 * Set the value of an S3 library register
 */
#if 1
br_error _inline _cdecl S3SetState(br_uint_32 variable,
							br_uint_32 value)
{
	extern S3DTK_LPFUNCTIONLIST g_pS3DTK_Funct;

   if(g_pS3DTK_Funct->S3DTK_SetState(g_pS3DTK_Funct,
                                 variable,
                                 value) != S3DTK_ERR)

		return BRE_OK;
   else
		return BRE_FAIL;
}
#endif

br_boolean  S3QueryBusy(void);


#define _S3LINK_H_
#endif
