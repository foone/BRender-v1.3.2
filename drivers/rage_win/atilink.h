/*
 * atilink.h :  Header for link between BRender device driver and ATI lib
 *
 */
#ifndef _ATILINK_H_


#include <brender.h>

#include "ati3dcif.h"
#include "devpixmp.h"
#include "plib.h"
#include "sbuffer.h"


extern C3D_HRC hRenderContext;      // Rendering context handle

extern C3D_3DCIFINFO   CIFInfo;     // Card capabilities


/* Make texture available for rendering; cache it, transfer it to the
 * card freeing up other textures as required, and register it with the
 * ATI library */
br_boolean ATICacheTexture(struct render_buffer *src, br_boolean force_reload);


/* Start up the library, set video mode */
br_boolean ATIInstantiateLibrary(void);

/* Perform all deinitialisation including restoring video mode */
br_boolean ATITerminateLibrary(void);

/* Set rendering output window */
br_boolean ATISetRenderWindow(LPDIRECTDRAWSURFACE surface,
                              br_int_32 x, br_int_32 y,
                              br_int_32 w, br_int_32 h,
							  br_device_pixelmap *orig_pm);

/* Set Z buffer */
br_boolean ATISetZBufferState(br_boolean enable, LPDIRECTDRAWSURFACE surface,
                              br_int_32 w, br_int_32 h);

void ATILoseRenderBuffer(struct render_buffer *buffer);
void ATIClearTextureCache(void);

#define _ATILINK_H_
#endif
