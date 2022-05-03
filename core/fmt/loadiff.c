/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadiff.c 1.1 1997/12/10 16:41:02 jon Exp $
 * $Locker: $
 *
 * Loader for Amiga .IFF/.LBM format
 * support for 8 bit index palette based
 */
#include "brender.h"
#include "fmt.h"

BR_RCS_ID("$Id: loadiff.c 1.1 1997/12/10 16:41:02 jon Exp $")

static int readline(char *mblock,int bytes,void *fh);
static void planes2bytes(br_pixelmap *pm,char *block,int line);

#define RED	2
#define GRN	1
#define BLU	0
#define PAD	3

#define PACKET_SIZE 4		/* block size	*/

#define MAX_PALETTE 768		/* 256 colours	*/

#define RUN_LENGTH  0x80	/* rle bit field */

#define BITMAP_HEADER	"BMHD"
#define COLOUR_MAP	"CMAP"
#define IMAGE_DATA	"BODY"

#define swapl(n)	(((n & 0xff000000) >> 24) +		\
			((n & 0x00ff0000) >> 8)	+		\
			((n & 0x0000ff00) << 8) +		\
			((n & 0x000000ff) <<24))

#define swapi(n)	(((n & 0xff00) >> 8) |			\
			((n & 0x00ff) << 8))

#define pixels2bytes(n)	((n+7)/8)

#pragma pack (1)

static struct {
	char type[4];				/* iff/lbm sig, 'FORM','LIST' or 'CAT '			*/
	unsigned int size;			/* swapl(n) approx. size of file			*/
	char subtype[4];			/* 'ILBM'  - image planes or 'PBM ' - byte oriented	*/
} IFFheader;

static struct {
    	unsigned short w;			/* width of image in pixels				*/
	unsigned short h;			/* height of image in pixels				*/
	short x;				/* x coord of uppr left of image relative to screen	*/
	short y;				/* y coord or uppr left of image relative to screen	*/
	char nPlanes;				/* number of colour planes required to display image (n/a)*/
	char masking;				/* image data interleaved with mask			*/
						/* 0 - no mask, 1 - mask				*/
						/* 2 - mask with transparent colour, 3 - lasso mask	*/
	char compression;			/* 0 - uncompressed image data, 1 - compressed (rle)	*/
	char padl;				/* pad byte n/a						*/
	unsigned short transparentColour;	/* colour index in image considered transparent		*/
	char xAspect;				/* aspect ratio						*/
	char yAspect;				/* aspect ratio						*/
	short pageW;				/* size of source screen				*/
	short pageH;				/* size of source screen				*/
} BMHD;

static struct {
	char red;
	char green;
	char blue;
} RGB;

static unsigned char masktable[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01 };
static unsigned char bittable[8]  = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80 };
	
#pragma pack ()

br_pixelmap * BR_PUBLIC_ENTRY BrFmtIFFLoad(char *name,br_uint_32 flags)
{
	void *fh;
	int open_mode = BR_FS_MODE_BINARY;
	br_pixelmap *pm,*palette;

	char block[4],*mblock;
	unsigned int block_length;
	br_uint_16 i,palette_size;
	int bytes_per_line;
	
	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	if(BrFileRead(&IFFheader,1,sizeof(IFFheader),fh) != sizeof(IFFheader))
	  	BR_ERROR1("Unable to read header information from '%s'",name);

	if(!BrMemCmp(IFFheader.type,"FORM",4) ||
	   !BrMemCmp(IFFheader.type,"LIST",4) ||
	   !BrMemCmp(IFFheader.type,"CAT ",4))
	{
	    	do
		{
		    	if(BrFileRead(block,1,PACKET_SIZE,fh) != PACKET_SIZE)
				BR_ERROR1("Unable to read block descriptor from '%s'",name);

		    	if(BrFileRead(&block_length,1,PACKET_SIZE,fh) != PACKET_SIZE)
				BR_ERROR1("Unable to read block length from '%s'",name);

			block_length = swapl(block_length);
			if(block_length & 1)
				++block_length;		/* make block size even */

			if(!BrMemCmp(block,BITMAP_HEADER,4))
			{
			    	/*
				 * Image block information header
				 */
				 
			    	if(BrFileRead(&BMHD,1,sizeof(BMHD),fh) != sizeof(BMHD))
					BR_ERROR1("Unable to read bitmap header in '%s'",name);
					
				BMHD.w = swapi(BMHD.w);
				BMHD.h = swapi(BMHD.h);

				if(!BrMemCmp(IFFheader.subtype,"ILBM",4))
					bytes_per_line = pixels2bytes(BMHD.w) * BMHD.nPlanes;
				else
					bytes_per_line = BMHD.w;
			}
			else if(!BrMemCmp(block,COLOUR_MAP,4))
			{
			    	/*
				 * Palette
				 */
				 if(block_length <= MAX_PALETTE)
				 {
				     	palette_size = block_length/3;

					palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);

					palette->identifier = BrResStrDup(palette, name);

					for(i=0; i<palette_size; i++)
					{
					    	if(BrFileRead(&RGB,1,sizeof(RGB),fh) != sizeof(RGB))
							BR_ERROR1("Unable to read palette from '%s'",name);

						((char *)(palette->pixels))[i*4+RED] = RGB.red;
						((char *)(palette->pixels))[i*4+GRN] = RGB.green;
						((char *)(palette->pixels))[i*4+BLU] = RGB.blue;
						((char *)(palette->pixels))[i*4+PAD] = 0;
					}	
				 }
			}
			else if(!BrMemCmp(block,IMAGE_DATA,4))
			{
			    	/*
				 * Raw image data
				 */

				pm = BrPixelmapAllocate(BR_PMT_INDEX_8,BMHD.w,BMHD.h,NULL,0);

				pm->identifier = BrResStrDup(pm, name);

				mblock = BrMemAllocate(bytes_per_line,BR_MEMORY_SCRATCH);

				for(i=0;i<BMHD.h;i++)
				{
				    	if(BMHD.compression)
					{
						if(readline(mblock,bytes_per_line,fh) != bytes_per_line)
							BR_ERROR1("Unable to read image data from '%s'",name);
					}
					else
					{
						if(BrFileRead(mblock,1,bytes_per_line,fh) != bytes_per_line)
							BR_ERROR1("Unable to read image data from '%s'",name);
					}
					if(!BrMemCmp(IFFheader.subtype,"ILBM",4) ||
					  (!BrMemCmp(IFFheader.subtype,"PBM ",4) &&
					  BMHD.nPlanes < 8))
					    	/* bitmap split into image planes */
						
						planes2bytes(pm,mblock,i);
					else
					    	/* planar data */
						
						BrMemCpy((char *)pm->pixels+i*pm->row_bytes,mblock,BMHD.w);
				}

				BrMemFree(mblock);
			}
			else BrFileAdvance(block_length,fh);	/* skip unknown block */
		}
		while(!BrFileEof(fh) && BrMemCmp(block,IMAGE_DATA,4));
		
		pm->map = palette;		/* palette may be after image block in file */
		
		BrFileClose(fh);
		
		return pm;
	}
	else
		BR_ERROR1("'%s' is not a valid IFF file",name);
		
	return NULL;
}

static int readline(char *mblock,int bytes,void *fh)
{
    	short c,i, n = 0;

	do
	{
	    	c = BrFileGetChar(fh) & 0xff;
		if(c & RUN_LENGTH)	/* run length	*/
		{
		    	if(c != 0x80)
			{
			    	i = ((~c) & 0xff)+2;
				c = BrFileGetChar(fh);
				while(i--)
					mblock[n++] = (char)c;
			}
		}
		else			/* string	*/
		{
		    	i = (c & 0xff)+1;
			while(i--)
				mblock[n++] = BrFileGetChar(fh);
		}
		
	}
	while(n < bytes);

	return n;
}

static void planes2bytes(br_pixelmap *pm,char *block,int line)
{
    	int i,j,n;
	
	char *src;

	n = pixels2bytes(BMHD.w);

	for(i=0;i<BMHD.w;++i)	/* scan through pixels */
	{	
	    	((char *)(pm->pixels))[line*pm->row_bytes+i] = 0;
		src = block;
		
		for(j=0; j<BMHD.nPlanes; ++j)	/* fetch each planar pixel */
		{
			if(src[(i >> 3)] & masktable[i & 0x0007])
				((char *)(pm->pixels))[line*pm->row_bytes+i] |= bittable[j];
			src += n;
		}
		
	}
}
