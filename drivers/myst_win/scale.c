/*
 * scale.c :    Bitmap scaling module
 */


#include <brender.h>




void StretchCopy16(
			br_uint_16 *out_pixels,
			br_uint_32 out_width, br_uint_32 out_height, br_uint_32 out_row_bytes,
			br_uint_16 *in_pixels,
			br_uint_32 in_width, br_uint_32 in_height, br_uint_32 in_row_bytes,
			br_uint_32 key)
{
	br_uint_32 xfracinc, yfracinc, yfrac, skip;
	br_uint_32 i, j, h, w, xfrac;
	br_uint_16 *ipx, *ipy, *opx, *opx2, *opy, v;

	opy = out_pixels;
	ipy = in_pixels;
	xfracinc = (in_width << 16)  / out_width;
	yfracinc = (in_height << 16) / out_height;
	in_row_bytes /= 2;
	out_row_bytes /= 2;
	w = out_width;
	h = out_height;

	if ((xfracinc == 32768) && (yfracinc == 32768)) {
		/* Special case for 2x2 magnification */
		w /= 2;
		h /= 2;
		for(i=0; i<h; i++) {
			ipx = ipy;
			opx = opy;
			opx2 = opy+out_row_bytes;
#ifdef _WIN32
			/* Assembler version - not terribly faster, but there you go */
			__asm {
				mov esi, ipx
				mov edi, opx
				mov edx, opx2
				mov ecx, w
				mov ebx, key
				mov eax, 0		/* Only if top word of key 0 will key match */
sc16_loop:
				mov ax, [esi]
				add esi, 2
				cmp eax, ebx
				jz sc16_skip
				mov [edi], ax
				mov [edx], ax
				mov [edi+2], ax
				mov [edx+2], ax
sc16_skip:
				add edi, 4
				add edx, 4
				dec ecx
				jnz sc16_loop
			}
#else
			for(j=0;j<w;j++) {
				v = *ipx++;
				*opx++ = v;
				*opx2++ = v;
				*opx++ = v;
				*opx2++ = v;
			}
#endif
			ipy += in_row_bytes;
			opy += 2*out_row_bytes;
		}

	} else {
		/* General case */

		/*
		 *	This routine is (reasonably) fast in minification but slow in magnification
		 */
		yfrac = 0;
		for(i=0; i<h; i++) {
			xfrac = 0;
			ipx = ipy;
			opx = opy;
			for(j=0;j<w;j++) {
				*opx++ = *ipx;
				xfrac += xfracinc;
				ipx += xfrac>>16;
				xfrac = xfrac & 0xffff;
			}
			yfrac += yfracinc;
			skip = yfrac>>16;
			yfrac = yfrac & 0xffff;

			ipy += skip*in_row_bytes;
			opy += out_row_bytes;
		}
	}
}





