/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadgif.c 1.1 1997/12/10 16:41:02 jon Exp $
 * $Locker: $
 *
 * Loader for CompuServ .GIF format (PC format, not MAC)
 * support for 8 bit index palette based
 * always BR_PMT_INDEX_8, 0 < palette entries < 256
 *
 * LZW compression algorithm from Supercharged Bitmapped Graphics by Steve Rimmer.
 * algorithm from files taken by Rimmer from BBS. Author not mentioned
 */
#include "brender.h"
#include "fmt.h"

BR_RCS_ID("$Id: loadgif.c 1.1 1997/12/10 16:41:02 jon Exp $")

#define RED	2
#define GRN	1
#define BLU	0
#define PAD	3

#define IMAGEBLOCK	','		/* next block is image						*/
#define EXTENSION	'!'		/* next block is extension					*/
#define ENDOFBLOCK	':'		/* end of block							*/

#define NO_CODE		-1

#define UNEXPECTED_EOF	NULL
#define BAD_CODE	NULL
#define BAD_FIRSTCODE	NULL
#define BAD_SYMBOLSIZE	NULL

static void CopyLine(br_pixelmap *pm,char *linebuffer,short line);
static void *UnpackImage(br_pixelmap *pm,short bits,void *fh);
static void SkipExtension(void *fh);

enum {
    BR_GIF_COLOURMAP		= 0x80,	/* (GIFheader.colourmapflags & x) , colourmap present			*/
    BR_GIF_COLOURMAP_SIZE	= 0x07,	/* ((GIFheader.colourmapflags & x)+1) number of colour bits		*/
    					/* (1 << ((GIFheader.colourmapflags & x)+1)) number of colours		*/
					/* 3 bytes per entry in colourmap				*/
    BR_GIF_COLOURMAP_SORTED	= 0x08,	/* (GIFheader.colourmapflags > x) important colours first	(GIF89a)	*/
    BR_GIF_COLOURMAP_ORIGIN	= 0x04,	/* (1 << ((GIFheader.colourmapflags >> x) +1) is the 			*/
    					/* number of bits/pixel GIF derived from. (GIF89a)		*/
	
    BR_GIF_IMAGE_INTERLACED	= 0x40,	/* (ImageBlock.imageflags & x) image data is interlaced			*/
    BR_GIF_IMAGE_SORTED		= 0x20	/* (ImageBlock.imageflags & x) image colourmap sorted (GIF89a)		*/
};

#pragma pack (1)

static struct {
    char sig[6];			/* GIF signature, 'GIF87a' or 'GIF89a'				*/
    unsigned short screenwidth;		/* screen width in pixels					*/
    unsigned short screendepth;		/* screen depth in pixels					*/
    char colourmapflags;
    char background;
    char aspect;			/* GIF87a = 0, GIF89a != 0 aspect ratio of pixels in image is	*/
					/*			(aspect + 15) / 64			*/
} GIFheader;

static struct {
    unsigned short left;		/* coordinates of upper left of image relative to screen	*/
    unsigned short top;
    unsigned short width;		/* width of image in pixels, number of bytes per unpacked line	*/
    unsigned short depth;		/* depth of image in pixels					*/ 
    char imageflags;			/* Flags similar to GIFheader.colourmapflags but for image block		*/
} ImageBlock;

static struct {
    	char red;
	char green;
	char blue;
} RGB;					/* palette entry						*/

#pragma pack ()

static char BlockSize;			/* size of GIF block						*/

	
br_pixelmap * BR_PUBLIC_ENTRY BrFmtGIFLoad(char *name,br_uint_32 flags)
{
	void *fh;
	int open_mode = BR_FS_MODE_BINARY;
	br_pixelmap *pm,*palette;
	unsigned char ch,foundimage;

	br_int_32 global_palette_size,local_palette_size;
	int i;
	
	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	if(BrFileRead(&GIFheader,1,sizeof(GIFheader),fh) != sizeof(GIFheader))
	  	BR_ERROR1("Unable to read header information from '%s'",name);

	if(BrMemCmp(GIFheader.sig,"GIF",3))
		BR_ERROR1("'%s' is not a valid GIF file",name);

	if(GIFheader.colourmapflags & BR_GIF_COLOURMAP)
	{
	    	global_palette_size = (1 << ((GIFheader.colourmapflags & BR_GIF_COLOURMAP_SIZE)+1));
		
		palette=BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);

		palette->identifier=BrResStrDup(palette,name);

		for(i=0;i<global_palette_size;i++)
		{
			if(BrFileRead(&RGB,1,sizeof(RGB),fh) != sizeof(RGB))
				BR_ERROR1("Unable to read global palette data from '%s'",name);
				
			((char *)(palette->pixels))[i*4+RED] = RGB.red;
			((char *)(palette->pixels))[i*4+GRN] = RGB.green;
			((char *)(palette->pixels))[i*4+BLU] = RGB.blue;
		}
	}

	/*
	 * Only read the first image block encountered. Skip any extension blocks
	 */

	foundimage=0;

	while((ch=BrFileGetChar(fh)) == IMAGEBLOCK || ch == EXTENSION || ch == 0 || !foundimage)
	{
	    	switch(ch)
		{
		    	case(IMAGEBLOCK):foundimage = 1;
			
					 if(BrFileRead(&ImageBlock,1,sizeof(ImageBlock),fh) != sizeof(ImageBlock))
						BR_ERROR1("Unable to read image data from '%s'",name);

					/*
					 * check for local colourmap
					 */
					 
					if(ImageBlock.imageflags & BR_GIF_COLOURMAP)
					{
					    	/*
						 * does local map contain more entries than global map
						 */
						 
						local_palette_size = (1 << ((ImageBlock.imageflags & BR_GIF_COLOURMAP_SIZE)+1));
						if(local_palette_size > global_palette_size)
						{
						     	BrPixelmapFree(palette);
							palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);

							palette->identifier=BrResStrDup(palette,name);
						}
		
						for(i=0;i<local_palette_size;i++)
						{
							if(BrFileRead(&RGB,1,sizeof(RGB),fh) != sizeof(RGB))
								BR_ERROR1("Unable to read local palette data from '%s'",name);
					
							((char *)(palette->pixels))[i*4+RED] = RGB.red;
							((char *)(palette->pixels))[i*4+GRN] = RGB.green;
							((char *)(palette->pixels))[i*4+BLU] = RGB.blue;
							((char *)(palette->pixels))[i*4+PAD] = 0;
						}
					}
						
					/* get initial code size and allocate pixelmap */

					if(BrFileRead(&ch,1,1,fh) != 1)
						BR_ERROR1("Unable to read image data from '%s'",name);
						
					pm = BrPixelmapAllocate(BR_PMT_INDEX_8,ImageBlock.width,ImageBlock.depth,NULL,0);

					pm->identifier = BrResStrDup(pm, name);
					pm->map = palette;

					if(UnpackImage(pm,ch,fh) == NULL)
						BR_ERROR1("Unable to read image data from '%s'",name);
					
					break;

			case(EXTENSION):SkipExtension(fh);
		}			break;
	}
	
	BrFileClose(fh);

	return pm;
}

static void *UnpackImage(br_pixelmap *pm,short bits,void *fh)
{
    	short bits2;			/* bits + 1					*/
	short codesize;			/* current code size in bits			*/
	short codesize2;		/* next codesize				*/
	short nextcode;			/* next available table entry			*/
	short thiscode;			/* code being expanded				*/
	short oldtoken;			/* last symbol decoded				*/
	short currentcode;		/* code just read				*/
	short oldcode;			/* code read before this one			*/
	short bitsleft;			/* number of bits left in *p			*/
	short blocksize;		/* bytes in next block				*/
	short line=0;			/* next line to write				*/
	short byte=0;			/* next byte to write				*/
	short pass=0;			/* pass number for interlaced pictures		*/

	char *p;			/* pointer to current byte in read buffer	*/
	char *q;			/* pointer past last byte in read buffer	*/
	char b[255];			/* read buffer					*/
	char *u;			/* stack pointer into firstcodestack		*/
	char *linebuffer;		/* place to store current line			*/

	static char firstcodestack[4096];	/* stack for first codes		*/
	static char lastcodestack[4096];	/* stack for previous codes		*/
	static short codestack[4096];		/* stack for links			*/

	static short wordmasktable[] = {
	    				0x0000,0x0001,0x0003,0x0007,
					0x000f,0x001f,0x003f,0x007f,
					0x00ff,0x01ff,0x03ff,0x07ff,
					0x0fff,0x1fff,0x3fff,0x7fff
					};
					
	static short inctable[] = { 8,8,4,2,0 };	/* interlace increments		*/
	static short starttable[] = { 0,4,2,1,0 };	/* interlace starts		*/

	p = q = b;
	bitsleft = 8;

	if(bits < 2 || bits > 8) return BAD_SYMBOLSIZE;
	bits2 = 1 << bits;
	nextcode = bits2+2;
	codesize2 = 1 << (codesize = bits + 1);
	oldcode = oldtoken = NO_CODE;

	linebuffer = BrMemAllocate(pm->width,BR_MEMORY_SCRATCH);

	for(;;)
	{
	    	if(bitsleft == 8)
		{
		    	if(++p >= q &&
			(((blocksize = BrFileGetChar(fh)) < 1) ||
			(q = (p = b) + BrFileRead(b,1,blocksize,fh)) < (b+blocksize)))
			{
			    	BrMemFree(linebuffer);
				return UNEXPECTED_EOF;
			}
			bitsleft = 0;
		}
		thiscode = *p;
		if((currentcode=(codesize + bitsleft)) <= 8)
		{
		    	*p >>= codesize;
			bitsleft = currentcode;
		}
		else
		{
		    	if(++p >= q &&
			(((blocksize = BrFileGetChar(fh)) < 1) ||
			(q = (p = b)+BrFileRead(b,1,blocksize,fh)) < (b+blocksize)))
			{
			    	BrMemFree(linebuffer);
				return UNEXPECTED_EOF;
			}
			thiscode |= *p << (8-bitsleft);
			if(currentcode <= 16)
				*p >>= (bitsleft = currentcode-8);
			else
			{
			    	if(++p >= q &&
				(((blocksize = BrFileGetChar(fh)) <1) ||
				(q = (p = b) + BrFileRead(b,1,blocksize,fh)) < (b+blocksize)))
				{
				    	BrMemFree(linebuffer);
					return UNEXPECTED_EOF;
				}
				thiscode |= *p << (16-bitsleft);
				*p >>= (bitsleft = currentcode-16);
			}
			
		}
		thiscode &= wordmasktable[codesize];
		currentcode = thiscode;

		if(thiscode == (bits2+1)) break;	/* found EOI */
		if(thiscode > nextcode)
		{
		    	BrMemFree(linebuffer);
			return BAD_CODE;
		}

		if(thiscode == bits2)
		{
		    	nextcode = bits2+2;
			codesize2 = 1 << (codesize = (bits+1));
			oldtoken = oldcode = NO_CODE;
			continue;
		}
		u = firstcodestack;

		if(thiscode == nextcode)
		{
		    	if(oldcode == NO_CODE)
			{
			    	BrMemFree(linebuffer);
				return BAD_FIRSTCODE;
			}
			*u++ = (char)oldtoken;
			thiscode = oldcode;
		}

		while (thiscode >= bits2)
		{
		    	*u++ = lastcodestack[thiscode];
			thiscode = codestack[thiscode];
		}

		oldtoken = thiscode;
		do
		{
		    	linebuffer[byte++] = (char)thiscode;
			if(byte >= pm->width)
			{
			    	CopyLine(pm,linebuffer,line);
				byte = 0;

				/* check for interlaced image */

				if(ImageBlock.imageflags & BR_GIF_IMAGE_INTERLACED)
				{
				    	line += inctable[pass];
					if(line >= pm->height)
						line = starttable[++pass];
				}
				else ++line;
			}

			if(u<=firstcodestack) break;
			thiscode = *--u;
		}
		while(1);

		if(nextcode < 4096 && oldcode != NO_CODE)
		{
			codestack[nextcode] = oldcode;
			lastcodestack[nextcode] = (char)oldtoken;
			if(++nextcode >= codesize2 && codesize < 12)
				codesize2 = 1 << ++codesize;
		}
		oldcode = currentcode;
	}

	BrMemFree(linebuffer);
	return pm;
}

static void CopyLine(br_pixelmap *pm,char *linebuffer,short line)
{
    	if(line>=0 && line<pm->height)
		BrMemCpy((char *)pm->pixels+line*pm->row_bytes,linebuffer,pm->width);
}

static void SkipExtension(void *fh)
{
   	unsigned char ch;

	ch=BrFileGetChar(fh);	/* Extension type 			*/
				/*					*/
				/* 0x0001	Plain text descriptor	*/
				/* 0x00f9	Graphic control block	*/
				/* 0x00fe	Comment extension	*/
				/* 0x00ff	Application extension	*/

	ch=BrFileGetChar(fh);	/* block size */

	BrFileAdvance(ch,fh);
}
