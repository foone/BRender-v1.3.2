/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * Device pixelmap methods
 */
#include <stddef.h>
#include <string.h>

#include "drv.h"
#include "pm.h"
#include "host.h"
#include "shortcut.h"
#include "brassert.h"


#define MSI_LFB_DISABLE		((T_msiParameters*) -1)

br_error MystiqueAllocateSysMemPixelmap(br_device_pixelmap *self, br_device_pixelmap **newpm, int w, int h);

br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, copyTo)(struct br_device_pixelmap *self, br_device_pixelmap *src);


/*
 * Default dispatch table for device pixelmap (defined at end of file)
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch;

/*
 * Device pixelmap info. template
 */
#define F(f)	offsetof(struct br_device_pixelmap, f)

static struct br_tv_template_entry devicePixelmapTemplateEntries[] = {
	{BRT_WIDTH_I32,			0,	F(pm_width),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_HEIGHT_I32,		0,	F(pm_height),		BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U16, },
	{BRT_PIXEL_TYPE_U8,		0,	F(pm_type),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_I32_U8, },
	{BRT_OUTPUT_FACILITY_O,	0,	F(output_facility),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_FACILITY_O,		0,	F(output_facility),	BRTV_QUERY,				BRTV_CONV_COPY, },
	{BRT_IDENTIFIER_CSTR,	0,	F(pm_identifier),	BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
	{BRT_CLUT_O,			0,	F(clut),			BRTV_QUERY | BRTV_ALL,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template devicePixelmapTemplate = {
	BR_ASIZE(devicePixelmapTemplateEntries),
	devicePixelmapTemplateEntries
};


#ifdef _WIN32
/*
 * Structures used to translate tokens and values
 */
struct hwnd_tokens {
	HWND hwnd;
};

#define F(f)	offsetof(struct hwnd_tokens, f)
static struct br_tv_template_entry hwndTemplateEntries[] = {
	{BRT(WINDOW_HANDLE_H),			F(hwnd),			BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template hwndTemplate = {
	BR_ASIZE(hwndTemplateEntries),
	hwndTemplateEntries
};
#endif




/*
 * Structure used to unpack the 'match' tokens/values
 */
struct match_tokens {
	br_token	use;
	br_int_32	pixel_bits;
	br_int_32	width;
	br_int_32	height;
	br_object  *renderer;
};

#define F(f)	offsetof(struct match_tokens, f)

static struct br_tv_template_entry matchTemplateEntries[] = {
	{BRT_USE_T,				0,	F(use),					BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_PIXEL_BITS_I32,	0,	F(pixel_bits),			BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_RENDERER_O,		0,	F(renderer),			BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_WIDTH_I32,			0,	F(width),				BRTV_SET,	BRTV_CONV_COPY, },
	{BRT_HEIGHT_I32,		0,	F(height),				BRTV_SET,	BRTV_CONV_COPY, },
};
#undef F

static struct br_tv_template matchTemplate = {
	BR_ASIZE(matchTemplateEntries),
	matchTemplateEntries
};


#define HWND_INTERFACE



#ifdef _WIN32

#ifndef HWND_INTERFACE
/*
 * For the Mystique's slightly noddy interface, create a driver window class
 * and front-window which passes all its messages down to the application window
 */
HWND local_window_handle, application_hwnd;

LRESULT CALLBACK MystiqueWindowProc(HWND hwnd, unsigned msg,
									UINT wParam, LONG lParam )
{
	/* Kick message down to the application */
	SendMessage(application_hwnd, msg, wParam, lParam);
	return( DefWindowProc( hwnd, msg, wParam, lParam ) );
}


char MystiqueDeviceClass[64]="MBRDevClass";
br_error MystiqueWindowSetup(HWND hWnd)
{
    WNDCLASS    wc;
	HANDLE		instance;

	application_hwnd = hWnd;
	instance = GetModuleHandle(NULL);//(HANDLE) GetWindowLong(hWnd, GWL_HINSTANCE);

    /*
     * set up and register window class
     */
    wc.style       = 0;
    wc.lpfnWndProc = MystiqueWindowProc;
    wc.cbClsExtra  = 0;
    wc.cbWndExtra  = 0;
    wc.hInstance   = instance;
    wc.hIcon       = NULL;
    wc.hCursor     = NULL;
    wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = MystiqueDeviceClass;
    if (!RegisterClass(&wc))
		return(BRE_DEV_FAIL);

	local_window_handle = CreateWindowEx(
		0,
        MystiqueDeviceClass,
        "Mystique Device Driver",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
		400,
		300,
        NULL,
        NULL,
        instance,
        NULL
        );

	if (!local_window_handle)
		return(BRE_DEV_FAIL);

	return(BRE_OK);
}

#endif	// HWND_INTERFACE

#endif	// _WIN32








T_msiInfo	*msiInfo;

/*
 * Create a new device pixelmap and set a display mode
 */
br_error DevicePixelmapMystAllocateMode(br_device *dev, br_output_facility *facility,
                                    br_device_pixelmap **newpm, br_token_value *tv)
{
	br_device_pixelmap *self;
	br_uint_32 w, h;
	br_uint_16 qual;

	syslog("DevicePixelmapMystAllocateMode: Allocating front buffer");

    if (dev->active_buffers[BT_FRONTSCREEN])
		return(BRE_DEV_ALREADY_INIT);

	CLEAR_TEMPLATE(devicePixelmap);
	CLEAR_TEMPLATE(match);

	w = facility->width;
	h = facility->height;

#ifdef _WIN32
	{
		struct hwnd_tokens ht;
		br_int_32 count;
//		br_error result;

		/* Parse for hwnd */
		CLEAR_TEMPLATE(hwnd);
		ht.hwnd = NULL;
		hwndTemplate.res = DeviceMystResource(dev);
		BrTokenValueSetMany(&ht, &count, NULL, tv, &hwndTemplate);

#ifdef HWND_INTERFACE
		dev->msiInfo = msiInit(w, h, facility->colour_bits, TRUE, FALSE, ht.hwnd);

#else	// HWND_INTERFACE
		result = MystiqueWindowSetup(ht.hwnd);
		if (result != BRE_OK)
			return(result);

		/* Start up MSI, 16 bpp, with Z-buffer, debug info */
		dev->msiInfo = msiInit(w, h, facility->colour_bits, TRUE, TRUE, MystiqueWindowProc);
#endif	// HWND_INTERFACE
	}

#else
	syslog(" -> Starting MSI");
	/* Start up MSI, 16 bpp, with Z-buffer, debug info */
	dev->msiInfo = msiInit(w, h, facility->colour_bits, TRUE, TRUE);

#endif
	msiInfo = dev->msiInfo;

	if (!dev->msiInfo)
		return(BRE_DEV_HARDWARE_SET);

	syslog(" -> MSI up OK");
    self = BrResAllocate(DeviceMystResource(dev), sizeof(*self), BR_MEMORY_OBJECT);
	if (!self)
		return(BRE_DEV_NO_MEMORY);

	self->dispatch = &devicePixelmapDispatch;
 	self->pm_identifier = ObjectIdentifier(facility);

	self->pm_pixels = 0;
	HostSelectorDS(&qual);
	self->pm_pixels_qualifier = qual;

    self->pm_type = (br_uint_8) OutputFacilityMystType(facility);
	self->pm_width = (br_uint_16) w;
	self->pm_height = (br_uint_16) h;
	self->pm_row_bytes = 0; 
	self->pm_origin_x = 0;
	self->pm_origin_y = 0;

	self->pm_flags = BR_PMF_NO_ACCESS;
	self->pm_base_x = 0;
	self->pm_base_y = 0;
	self->output_facility = facility;
	self->clut = dev->clut;

	self->buffer_type = BT_FRONTSCREEN;
	dev->active_buffers[BT_FRONTSCREEN] = self;

	ObjectContainerAddFront(facility, (br_object *)self);

    syslog(" -> complete");

	*newpm = self;
	return(BRE_OK);
}

static void BR_CMETHOD_DECL(br_device_pixelmap_mystique, free)(br_device_pixelmap *self)
{

    if (self->buffer_type == BT_FRONTSCREEN) {
		/* Clear out texture heaps */
		ClearHeap();

        /* Restore and shutdown: should probably check that back buffer
         * has already gone */
		ObjectContainerRemove(self->output_facility, (br_object *)self);
		self->output_facility->num_instances--;

		ObjectDevice(self)->active_buffers[self->buffer_type] = NULL;

        msiExit();
    }

	/*
	 * Free up pixelmap structure
	 */
	BrResFreeNoCallback(self);
}

br_token BR_CMETHOD_DECL(br_device_pixelmap_mystique, type)(br_device_pixelmap *self)
{
	return BRT_DEVICE_PIXELMAP;
}

br_boolean BR_CMETHOD_DECL(br_device_pixelmap_mystique, isType)(br_device_pixelmap *self, br_token t)
{
	return (t == BRT_DEVICE_PIXELMAP) || (t == BRT_OBJECT);
}

br_int_32 BR_CMETHOD_DECL(br_device_pixelmap_mystique, space)(br_device_pixelmap *self)
{
	return sizeof(br_device_pixelmap);
}

struct br_tv_template * BR_CMETHOD_DECL(br_device_pixelmap_mystique,queryTemplate)
	(br_device_pixelmap *self)
{
    devicePixelmapTemplate.res = DeviceMystResource(ObjectMystDevice(self));
	return &devicePixelmapTemplate;
}

br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, flush)(
	struct br_device_pixelmap *self)
{
	return BRE_OK;
}
br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, synchronise)(
	struct br_device_pixelmap *self,
	br_token sync_type,
	br_boolean block)
{
	return BRE_OK;
}




static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, match)(br_device_pixelmap *self,
	br_device_pixelmap **newpm, br_token_value *tv)
{
	struct match_tokens mt = {BRT_NONE, 0};
	br_int_32 count;
	br_device_pixelmap *pm;
	br_device *dev = ObjectDevice(self);
	br_uint_16 qual;

    syslog("Pixelmap Match");

    matchTemplate.res = DeviceMystResource(dev);

	/*
	 * Default width and height
	 */
	mt.width = self->pm_width;
	mt.height = self->pm_height;

	BrTokenValueSetMany(&mt, &count, NULL, tv, &matchTemplate);

	/*
	 * Create the pixelmap structure
	 */

	if(mt.use == BRT_OFFSCREEN) {
        syslog(" -> offscreen match");
		/*
		 * Check that the source buffer is of the correct type
		 */
		if(self->buffer_type != BT_FRONTSCREEN)
			return BRE_DEV_UNSUPPORTED;

		if(dev->active_buffers[BT_BACKSCREEN]) {
//			return BRE_DEV_ALREADY_INIT;
			/* OK we've already allocated the backscreen - now we have the hack */
			return(MystiqueAllocateSysMemPixelmap(self, newpm, mt.width, mt.height));
		}

		/*
		 * Create an offscreen buffer pixelmap
		 */
        pm = BrResAllocate(DeviceMystResource(dev),sizeof(*pm), BR_MEMORY_OBJECT);

		pm->pm_width = self->pm_width;
		pm->pm_height = self->pm_height;
		pm->pm_pixels = 0;
		HostSelectorDS(&qual);
		self->pm_pixels_qualifier = qual;

		pm->pm_origin_x = self->pm_origin_x;
		pm->pm_origin_y = self->pm_origin_y;

		pm->pm_flags = BR_PMF_NO_ACCESS;

		pm->output_facility = self->output_facility;

		pm->pm_type = self->pm_type;
		pm->dispatch = &devicePixelmapDispatch;
		pm->pm_identifier = "Offscreen";

		pm->buffer_type = BT_BACKSCREEN;
		dev->active_buffers[BT_BACKSCREEN] = pm;

		self->offscreen = pm;

	} else if(mt.use == BRT_DEPTH) {

        syslog(" -> depth match");

		/*
		 * Check that the source buffer is of the correct type
		 */
		if(self->buffer_type != BT_FRONTSCREEN &&
			self->buffer_type != BT_BACKSCREEN)
			return BRE_DEV_UNSUPPORTED;

		if(dev->active_buffers[BT_DEPTH])
			return BRE_DEV_ALREADY_INIT;

		/*
		 * Create an depth buffer pixelmap
		 */
        pm = BrResAllocate(DeviceMystResource(dev),sizeof(*pm), BR_MEMORY_OBJECT);
		if (!pm)
			return(BRE_DEV_NO_MEMORY);

		pm->pm_width = self->pm_width;
		pm->pm_height = self->pm_height;
		pm->pm_pixels = 0;
		HostSelectorDS(&qual);
		self->pm_pixels_qualifier = qual;

		pm->pm_origin_x = self->pm_origin_x;
		pm->pm_origin_y = self->pm_origin_y;

		pm->pm_flags = BR_PMF_NO_ACCESS;

		pm->output_facility = self->output_facility;

		pm->pm_type = BR_PMT_DEPTH_16;
		pm->dispatch = &devicePixelmapDispatch;
		pm->pm_identifier = "Depth";

		pm->buffer_type = BT_DEPTH;
		dev->active_buffers[BT_DEPTH] = pm;
		self->depth = pm;

	} else if (mt.use == BRT_NO_RENDER) {
		/* No render pixelmaps require a device system memory pixelmap */
		return(MystiqueAllocateSysMemPixelmap(self, newpm, mt.width, mt.height));

	} else {
		return BRE_DEV_UNSUPPORTED;
	}

    syslog(" -> complete");
	*newpm = pm;

	return BRE_OK;
}




/*
 * Because of the limited availabilty of the Mystique's pixelmap fills,
 * have to buffer them and do them here
 *
 * This start function is called from anywhere by which point the frame must
 * definitely have started; that is, at the first renderbegin block or
 * at the double buffer call (cos that requires EndFrame)
 */
br_boolean frame_started = BR_FALSE;
BOOL mystique_rgb_fill = FALSE;
br_colour mystique_rgb_fill_colour;
BOOL mystique_z_fill = FALSE;
float mystique_z_fill_value;

void MystiqueBuffersStartFrame(void)
{
	if (!frame_started) {
        syslog("BuffersStartFrame");
        msiStartFrame(mystique_rgb_fill,
                      (float) BR_RED(mystique_rgb_fill_colour),
                      (float) BR_GRN(mystique_rgb_fill_colour),
                      (float) BR_BLU(mystique_rgb_fill_colour),
                      mystique_z_fill,
                      mystique_z_fill_value);
		frame_started = BR_TRUE;
		mystique_rgb_fill = FALSE;
		mystique_z_fill   = FALSE;
        syslog(" -> complete");
	}
}




static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, doubleBuffer)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	/*
	 * fail if doubleBuffer is not invoked on front/back pair
	 */
	if(self != ObjectDevice(self)->active_buffers[BT_FRONTSCREEN] ||
		src != ObjectDevice(self)->active_buffers[BT_BACKSCREEN])
		return BRE_DEV_UNSUPPORTED;


	MystiqueBuffersStartFrame();

    syslog("DoubleBuffer");
	/*
     * Do the flip... wait for it to be ready.
     * Setting wait to FALSE may well be possible here.
	 */
    msiEndFrame(FALSE, 0, TRUE);

	/* Clear out the startframe stuff */
	frame_started     = BR_FALSE;
	mystique_rgb_fill = FALSE;
	mystique_z_fill   = FALSE;

    syslog(" -> complete");

// Temporary hack	
mystique_rgb_fill = TRUE;
mystique_z_fill = TRUE;

	return BRE_OK;
}



br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, allocateSub)\
	(br_device_pixelmap *self, br_device_pixelmap **newpm, br_rectangle *rect)
{
	br_device_pixelmap   *pm;
	br_rectangle         out;

	// If the parent is null then ooops!
	if(!self)
		return(BRE_FAIL);

	/*
	 * Create the new structure and copy
	 */
	pm = BrResAllocate(DeviceMystResource(ObjectMystDevice(self)),sizeof(*pm),BR_MEMORY_PIXELMAP);
	if (!pm)
		return(BRE_DEV_NO_MEMORY);

	// Set all the fields to be the same as the parent pixelmap for now
	*pm = *self;

	/*
	 * Pixel rows are not contiguous
	 */
	pm->pm_flags &= ~BR_PMF_LINEAR;

	/*
	 * Create sub-window (clipped against original)
	 */
	if(PixelmapRectangleClip(&out, rect, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_FAIL;


	pm->pm_base_x += out.x;
	pm->pm_base_y += out.y;

	pm->pm_width   = out.w;
	pm->pm_height  = out.h;

	pm->pm_origin_x = 0;
	pm->pm_origin_y = 0;

	*newpm = (br_device_pixelmap *)pm;

	return BRE_OK;
}










static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleFill)
	(br_device_pixelmap *self, br_rectangle *rect, br_uint_32 colour)
{
	br_uint_16 *dptr, *dp;
	br_int_32 i, j, w, h;
	br_uint_32 col;
	br_rectangle arect;
	br_boolean end;

    if(self != ObjectDevice(self)->active_buffers[BT_BACKSCREEN])
        return(BRE_DEV_UNSUPPORTED);

	if(PixelmapRectangleClip(&arect, rect, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);
	dptr = ((br_uint_16 *) msiInfo->msiColor.pMem) +
						(arect.x+self->pm_base_x) +
						((arect.y+self->pm_base_y) * (msiInfo->msiColor.Pitch/2));
	w = arect.w;
#if _WIN32
	if (w&1) end = BR_TRUE;
	else end = BR_FALSE;
	w >>= 1;	// dword moves
	col = colour + (colour<<16);
#endif
	h = arect.h;
	for(j=0; j<h; j++) {
		dp = dptr;
#if _WIN32
		if (end) *dp++ = (br_uint_16) colour;
		_asm {
			mov ecx, w
			mov edi, dp
			mov eax, col
			cld
			rep stosd
		}
#else
		for(i=0; i<w; i++)
			*dp++ = colour;
#endif
		dptr += (msiInfo->msiColor.Pitch/2);
	}

	msiSetParameters(MSI_LFB_DISABLE);

	return BRE_OK;
}




static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, fill)(br_device_pixelmap *self, br_uint_32 colour)
{
    syslog("Fill");
    /* This is problematic. The fastest way under MSI to clear is to
     * use StartFrame, but this is not guaranteed to be correct with
     * 2D operations, plus Z and colour clears must be specified
     * simulataneously. */

	if ((self->pm_base_x != 0) || (self->pm_base_y != 0)) {
		br_rectangle rect;

		rect.x = - self->pm_origin_x;
		rect.y = - self->pm_origin_y;
		rect.w = self->pm_width;
		rect.h = self->pm_height;
		return(BR_CMETHOD(br_device_pixelmap_mystique, rectangleFill)(self, &rect, colour));
	}

    if(self == ObjectDevice(self)->active_buffers[BT_FRONTSCREEN])
        return(BRE_DEV_UNSUPPORTED);

	if(self == ObjectDevice(self)->active_buffers[BT_BACKSCREEN]) {
		if (!frame_started) {
			mystique_rgb_fill = TRUE;

			switch (self->pm_type) {

			case BR_PMT_RGB_555:

				mystique_rgb_fill_colour = BR_COLOUR_RGB(
					(colour >> 7) & 0xf8 | (colour >> 12) & 0x07,
					(colour >> 2) & 0xf8 | (colour >>  7) & 0x07,
					(colour << 3) & 0xf8 | (colour >>  2) & 0x07);
				break;

			case BR_PMT_RGB_565:

				mystique_rgb_fill_colour = BR_COLOUR_RGB(
					(colour >> 8) & 0xf8 | (colour >> 13) & 0x07,
					(colour >> 3) & 0xfc | (colour >>  9) & 0x03,
					(colour << 3) & 0xf8 | (colour >>  2) & 0x07);
				break;

			case BR_PMT_RGB_888:

				mystique_rgb_fill_colour = colour;
				break;

			default:

				mystique_rgb_fill_colour = 0;	// shrug!
				break;
			}

		} else {
			/* Fill manually */
		}
	}

	if(self == ObjectDevice(self)->active_buffers[BT_DEPTH]) {
		if (!frame_started) {
			mystique_z_fill = TRUE;
			mystique_z_fill_value = ((float) colour / (float) 0xffffffff);
		} else {
			/* Fill manually */
		}
	}

    syslog(" -> complete");
	return BRE_OK;
}




br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, copy)(br_device_pixelmap *self, br_device_pixelmap *src)
{
	if (src->buffer_type == BT_SYSMEM)
		return BR_CMETHOD(br_device_pixelmap_mystique, copyTo)(self, src);
	
	return(BRE_DEV_UNSUPPORTED);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, copyTo)(struct br_device_pixelmap *self, br_device_pixelmap *src)
{
	br_uint_16 *dptr, *d;
	br_uint_16 *sptr, *s;
	br_int_32 i, j, w, h;
	br_boolean end;
	
	if ((self->pm_width != src->pm_width) || (self->pm_height != src->pm_height))
		return(BRE_DEV_FAIL);

	if (src->pm_type != BR_PMT_RGB_565)
		return(BRE_DEV_UNSUPPORTED);

    if(self != ObjectDevice(self)->active_buffers[BT_BACKSCREEN])
        return(BRE_DEV_UNSUPPORTED);

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);
	dptr = ((br_uint_16 *) msiInfo->msiColor.pMem) +
						self->pm_base_x +
						(self->pm_base_y * (msiInfo->msiColor.Pitch/2));
	sptr = src->pm_pixels;

	w = src->pm_width;
#if _WIN32
	if (w&1) end = BR_TRUE;
	else end = BR_FALSE;
	w >>= 1;	// dword moves
#endif
	h = src->pm_height;
	for(j=0; j<h; j++) {
		d = dptr;
		s = sptr;
#if _WIN32
		if (end) *d++ = *s++;
		_asm {
			mov ecx, w
			mov esi, s
			mov edi, d
			cld
			rep movsd
		}
#else
		for(i=0; i<w; i++)
			*d++ = *s++;
#endif
		dptr += (msiInfo->msiColor.Pitch/2);
		sptr += (src->pm_row_bytes/2);
	}

	msiSetParameters(MSI_LFB_DISABLE);

	return(BRE_OK);
}


br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, copyFrom)(struct br_device_pixelmap *self, br_device_pixelmap *src)
{
	return(BRE_DEV_UNSUPPORTED);
}






static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleCopyTo)
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *src, br_rectangle *r)
{
	br_uint_16 *dptr, *sptr, *dp, *sp;
	br_rectangle ar;
	br_point ap;
	br_int_32 i, j, w, h;

    if(self != ObjectDevice(self)->active_buffers[BT_BACKSCREEN])
        return(BRE_DEV_UNSUPPORTED);

	if(PixelmapRectangleClipTwo(&ar, &ap, r, p, (br_pixelmap *)self, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);
	dptr = ((br_uint_16 *) msiInfo->msiColor.pMem) +
							(ap.x+self->pm_base_x) +
							((ap.y+self->pm_base_y) * (msiInfo->msiColor.Pitch/2));
	sptr = ((br_uint_16 *) src->pm_pixels) + ar.x + (ar.y * src->pm_row_bytes / 2);
	w = ar.w;
#if _WIN32
	w >>= 1;	// dword moves
#endif
	h = ar.h;
	for(j=0; j<h; j++) {
#if _WIN32
		_asm {
			mov ecx, w
			mov esi, sptr
			mov edi, dptr
			cld
			rep movsd
		}
#else
		dp = dptr;
		sp = sptr;
		for(i=0; i<w; i++)
			*dp++ = *sp++;
#endif
		dptr += (msiInfo->msiColor.Pitch/2);
		sptr += (src->pm_row_bytes/2);
	}

	msiSetParameters(MSI_LFB_DISABLE);

	return BRE_OK;
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleCopyFrom)
	(br_device_pixelmap *self, br_point *p, br_device_pixelmap *mem, br_rectangle *r)
{
	return BRE_DEV_UNSUPPORTED;
}





static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, pixelSet)
	(br_device_pixelmap *self, br_point *p, br_uint_32 colour)
{
	br_point ap;
	br_uint_16 *ptr;

//return(BRE_DEV_UNSUPPORTED);
	
	if(PixelmapPointClip(&ap, p, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);

	ptr = ((br_uint_16 *) msiInfo->msiColor.pMem) +
							(ap.x+self->pm_base_x) +
							((ap.y+self->pm_base_y) * (msiInfo->msiColor.Pitch/2));
	*ptr = (br_uint_16) colour; 

	msiSetParameters(MSI_LFB_DISABLE);

	return BRE_OK;
}


/* VERY important for text speed */
br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, copyBits)
	(br_device_pixelmap *self, br_point *point, br_uint_8 *src, br_uint_16 s_stride, br_rectangle *bit_rect, br_uint_32 colour)
{
	br_error result;

	if ((self->buffer_type != BT_BACKSCREEN) && (self->buffer_type != BT_DEPTH))
		return(BRE_DEV_UNSUPPORTED);

//return(BRE_DEV_UNSUPPORTED);

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);
	if (self->buffer_type == BT_BACKSCREEN) {
		self->pm_pixels		= msiInfo->msiColor.pMem;
		self->pm_row_bytes	= msiInfo->msiColor.Pitch;
	} else {
		self->pm_pixels		= msiInfo->msiDepth.pMem;
		self->pm_row_bytes	= msiInfo->msiDepth.Pitch;
	}

	result = BR_CMETHOD(br_device_pixelmap_mem, copyBits)(self, point, src, s_stride, bit_rect, colour);

	msiSetParameters(MSI_LFB_DISABLE);
	self->pm_pixels = NULL;
	self->pm_row_bytes = 0;

	return result;
}








extern void StretchCopy16(
			br_uint_16 *out_pixels,
			br_uint_32 out_width, br_uint_32 out_height, br_uint_32 out_row_bytes,
			br_uint_16 *in_pixels,
			br_uint_32 in_width, br_uint_32 in_height, br_uint_32 in_row_bytes,
			br_uint_32 key);



br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleStretchCopy)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	br_rectangle srect, drect;
	br_uint_32 key, i;

	if ((s->w == d->w) &&
		(s->h == d->h) &&
		(self->buffer_type != BT_SYSMEM)) {
		br_point p;

		p.x = d->x;
		p.y = d->y;
		return(BR_CMETHOD(br_device_pixelmap_mystique, rectangleCopyTo)(self, &p, src, s));
	}


	if(PixelmapRectangleClip(&drect, d, (br_pixelmap *)self) == BR_CLIP_REJECT)
		return BRE_OK;
	if(PixelmapRectangleClip(&srect, s, (br_pixelmap *)src) == BR_CLIP_REJECT)
		return BRE_OK;

	if (src->pm_type != self->pm_type)
		return(BRE_DEV_UNSUPPORTED);
	if ((src->pm_type != BR_PMT_RGB_555) &&
		(src->pm_type != BR_PMT_RGB_565))
		return(BRE_DEV_UNSUPPORTED);

	MystiqueBuffersStartFrame();

	if (src->pm_copy_function == BR_PMCOPY_SRC_KEYED) key = src->pm_src_key.low;
	else key = 0xffff0000;

	if (self->buffer_type == BT_SYSMEM) {
		if (src->buffer_type != BT_SYSMEM)
			return(BRE_DEV_UNSUPPORTED);	/* Don't support stretches from card */

		/* System-system stretch */
		StretchCopy16(((br_uint_16 *)self->pm_pixels) + drect.x + (drect.y*self->pm_row_bytes/2),
					  drect.w, drect.h, self->pm_row_bytes,
					  ((br_uint_16 *)src->pm_pixels) + srect.x + (srect.y*src->pm_row_bytes/2),
					  srect.w, srect.h, src->pm_row_bytes, key);

	} else {
		/* System-card stretch */
		br_uint_16 *ptr;

		if (self->buffer_type != BT_BACKSCREEN)
			return(BRE_DEV_UNSUPPORTED);

		msiSetParameters(NULL);
		ptr = (br_uint_16 *) msiInfo->msiColor.pMem;
		StretchCopy16(ptr + drect.x + (drect.y*self->pm_width),
					  drect.w, drect.h, msiInfo->msiColor.Pitch,
					  ((br_uint_16 *)src->pm_pixels) + srect.x + (srect.y*src->pm_row_bytes/2),
					  srect.w, srect.h, src->pm_row_bytes, key);
		msiSetParameters(MSI_LFB_DISABLE);
	}


	return(BRE_OK);
}


static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleStretchCopyTo)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	return(BRE_DEV_UNSUPPORTED);
}

static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, rectangleStretchCopyFrom)\
	(br_device_pixelmap *self, br_rectangle *d, br_device_pixelmap *src, br_rectangle *s)
{
	return(BRE_DEV_UNSUPPORTED);
}



/*
 * Lock and unlock are supported here - why not...
 */
static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, directLock)(br_device_pixelmap *self, br_boolean block)
{
#ifndef _WIN32
    return(BRE_DEV_UNSUPPORTED);
#else
	if ((self->buffer_type != BT_BACKSCREEN) && (self->buffer_type != BT_DEPTH))
		return(BRE_DEV_UNSUPPORTED);

	MystiqueBuffersStartFrame();

	msiSetParameters(NULL);
	if (self->buffer_type == BT_BACKSCREEN) {
		self->pm_pixels		= msiInfo->msiColor.pMem;
		self->pm_row_bytes	= msiInfo->msiColor.Pitch;
	} else {
		self->pm_pixels		= msiInfo->msiDepth.pMem;
		self->pm_row_bytes	= msiInfo->msiDepth.Pitch;
	}

	return(BRE_OK);
#endif
}

static br_error BR_CMETHOD_DECL(br_device_pixelmap_mystique, directUnlock)(br_device_pixelmap *self)
{
#ifndef _WIN32
    return(BRE_DEV_UNSUPPORTED);
#else
	if ((self->buffer_type != BT_BACKSCREEN) && (self->buffer_type != BT_DEPTH))
		return(BRE_DEV_UNSUPPORTED);

	msiSetParameters(MSI_LFB_DISABLE);
	self->pm_pixels = NULL;
	self->pm_row_bytes = 0;

	return(BRE_OK);
#endif
}



/*
 * Default dispatch table for device pixelmap
 */
static struct br_device_pixelmap_dispatch devicePixelmapDispatch = {
	NULL,
	NULL,
	NULL,
	NULL,
    BR_CMETHOD_REF(br_device_pixelmap_mystique, free),
    BR_CMETHOD_REF(br_object_mystique,          identifier),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, type),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, isType),
    BR_CMETHOD_REF(br_object_mystique,          device),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, space),

    BR_CMETHOD_REF(br_device_pixelmap_mystique, queryTemplate),
	BR_CMETHOD_REF(br_object,					query),
	BR_CMETHOD_REF(br_object,					queryBuffer),
	BR_CMETHOD_REF(br_object,					queryMany),
	BR_CMETHOD_REF(br_object,					queryManySize),
	BR_CMETHOD_REF(br_object,					queryAll),
	BR_CMETHOD_REF(br_object,					queryAllSize),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		validSource),
	BR_CMETHOD_REF(br_device_pixelmap_null,		resize),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, match),
	BR_CMETHOD_REF(br_device_pixelmap_mystique, allocateSub),

	BR_CMETHOD_REF(br_device_pixelmap_mystique,	copy),
	BR_CMETHOD_REF(br_device_pixelmap_mystique,	copyTo),
	BR_CMETHOD_REF(br_device_pixelmap_mystique,	copyFrom),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, fill),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, doubleBuffer),

	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyToDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		copyFromDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		fillDirty),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		doubleBufferDirty),

	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		rectangle2),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleCopyTo),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleCopyTo),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleCopyFrom),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleStretchCopy),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleStretchCopyTo),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleStretchCopyFrom),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, rectangleFill),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, pixelSet),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		line),
	BR_CMETHOD_REF(br_device_pixelmap_mystique,	copyBits),


	BR_CMETHOD_REF(br_device_pixelmap_gen,		text),
	BR_CMETHOD_REF(br_device_pixelmap_gen,		textBounds),

	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowSize),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowQuery),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		rowSet),

	BR_CMETHOD_REF(br_device_pixelmap_null,		pixelQuery),
	BR_CMETHOD_REF(br_device_pixelmap_null,		pixelAddressQuery),

	BR_CMETHOD_REF(br_device_pixelmap_null,		pixelAddressSet),
	BR_CMETHOD_REF(br_device_pixelmap_mem,		originSet),

	BR_CMETHOD_REF(br_device_pixelmap_mystique, flush),
	BR_CMETHOD_REF(br_device_pixelmap_mystique, synchronise),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, directLock),
    BR_CMETHOD_REF(br_device_pixelmap_mystique, directUnlock),

	BR_CMETHOD_REF(br_device_pixelmap_gen,	getControls),
	BR_CMETHOD_REF(br_device_pixelmap_gen,	setControls),
};

