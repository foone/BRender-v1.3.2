/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadtga.c 1.1 1997/12/10 16:41:02 jon Exp $
 * $Locker: $
 *
 * Loader for .TGA format
 * support for:	8 bit index palette based (16,24 bit palette)
 *		15,16,24,32 bit true colour
 *		15 & 16 bit are the same except for 1 field in header. (bits)
 * mono (1 bit) bit not supported here
 */
#include "brender.h"
#include "fmt.h"

BR_RCS_ID("$Id: loadtga.c 1.1 1997/12/10 16:41:02 jon Exp $")

#define RED	2
#define GRN	1
#define BLU	0
#define PAD	3

#define YES 1
#define NO 0

static void * ReadLine_Uncomp_8(char *line,void *fh);		/* 8 bit indexed, uncompressed	*/
static void * ReadLine_Comp_8(char *line,void *fh);		/* 8 bit indexed, compressed	*/

static void * ReadLine_Uncomp_16(char *line,void *fh);		/* 16 bit, uncompressed		*/
static void * ReadLine_Comp_16(char *line,void *fh);		/* 16 bit, compressed		*/

static void * ReadLine_Uncomp_24(char *line,void *fh);		/* 24 bit, uncompressed		*/
static void * ReadLine_Comp_24(char *line,void *fh);		/* 24 bit, compressed		*/

static void * ReadLine_Uncomp_32(char *line,void *fh);		/* 32 bit, uncompressed		*/
static void * ReadLine_Comp_32(char *line,void *fh);		/* 32 bit, compressed		*/

static void CopyLine_8(br_pixelmap *pm,char *line,int row);
static void CopyLine_16(br_pixelmap *pm,char *line,int row);
static void CopyLine_24(br_pixelmap *pm,char *line,int row);
static void CopyLine_32(br_pixelmap *pm,char *line,int row);

enum {
    	BR_TGA_UNCOMPRESSED_PALETTE	=	0x01,
	BR_TGA_UNCOMPRESSED_RGB		=	0x02,
	BR_TGA_UNCOMPRESSED_MONO	=	0x03,	/* not supported here	*/
	BR_TGA_COMPRESSED_PALETTE	=	0x09,
	BR_TGA_COMPRESSED_RGB		=	0x0a,
	BR_TGA_COMPRESSED_MONO		=	0x0b	/* not supported here	*/
};

enum {
    	BR_TGA_REVERSED_WIDTH		=	0x10,	/* right to left 	*/
	BR_TGA_REVERSED_DEPTH		=	0x20	/* bottom to top	*/
};

static struct {
    
    	char image_type;
	char pixel_bits;
    	char supported;
	void * (*read_func)(char *line,void *fh);			/* read 1 line of image data from file */
	void (*copy_func)(br_pixelmap *map,char *line,int row);		/* copy 1 line of data to pixelmap */
	
} Supported_TGA_types[]={
    
   	{BR_TGA_UNCOMPRESSED_PALETTE	, 8,	YES,	ReadLine_Uncomp_8,	CopyLine_8},
	{BR_TGA_UNCOMPRESSED_RGB	, 15,	YES,	ReadLine_Uncomp_16,	CopyLine_16},
	{BR_TGA_UNCOMPRESSED_RGB	, 16,	YES,	ReadLine_Uncomp_16,	CopyLine_16},
	{BR_TGA_UNCOMPRESSED_RGB	, 24,	YES,	ReadLine_Uncomp_24,	CopyLine_24},
	{BR_TGA_UNCOMPRESSED_RGB	, 32,	YES,	ReadLine_Uncomp_32,	CopyLine_32},
	{BR_TGA_UNCOMPRESSED_MONO	, 1,	NO,	NULL,			NULL},
	{BR_TGA_COMPRESSED_PALETTE	, 8,	YES,	ReadLine_Comp_8,	CopyLine_8},
	{BR_TGA_COMPRESSED_RGB		, 15,	YES,	ReadLine_Comp_16,	CopyLine_16},
	{BR_TGA_COMPRESSED_RGB		, 16,	YES,	ReadLine_Comp_16,	CopyLine_16},
	{BR_TGA_COMPRESSED_RGB		, 24,	YES,	ReadLine_Comp_24,	CopyLine_24},
	{BR_TGA_COMPRESSED_RGB		, 32,	YES,	ReadLine_Comp_32,	CopyLine_32},
	{BR_TGA_COMPRESSED_MONO		, 1,	NO,	NULL,			NULL},
};

/*
 * TGA header, this format allows for structure packing by compiler
 */
#pragma pack(1)

static struct {
    	char identsize;			/* number of bytes between header and whatever next 	*/
	char colourmaptype;		/* type of colourmap in file 0=RGB, 1=palette		*/
    	char imagetype;			/* type of storage of image				*/
    	unsigned short colourmapstart;	/* first colour index used in palette			*/
	unsigned short colourmaplength;	/* number of colours in palette from colourmapstart	*/
	char colourmapbits;		/* size of entry in palette table			*/
    	unsigned short xstart;		/* offset between uppr lft of image & uppr lft of screen*/
	unsigned short ystart;
	unsigned short width;		/* width in pixels					*/
	unsigned short depth;		/* depth in pixels					*/
	char bits;			/* number of bits of colour: 1,8,16,24,32		*/
	char descriptor;			/* bit 5 : set, image stored starting with last line	*/
					/* bit 4 : set, image stored pixels right to left	*/
					/* bits 0-3: number of bits available for overlays (n/a)*/
} TGAheader;

static unsigned short RGB_16;
static struct {
    	char blue,green,red;
} RGB_24;
static struct {
    	char blue,green,red,x;
} RGB_32;

#pragma pack()

br_pixelmap * BR_PUBLIC_ENTRY BrFmtTGALoad(char *name,br_uint_32 flags)
{
	void *fh;
	int open_mode = BR_FS_MODE_BINARY,i;
	br_pixelmap *pm;

	br_uint_8 type;
	br_int_32 ystart,yend,dy;
	br_uint_32 bytes_per_line,palette_size;

	unsigned char *mblock,supported;

	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	if(BrFileRead(&TGAheader,1,sizeof(TGAheader),fh) !=sizeof(TGAheader))
		BR_ERROR1("Unable to read header information from '%s'",name);

	for(i=0,supported= 255; i<BR_ASIZE(Supported_TGA_types); i++)
		if((Supported_TGA_types[i].image_type == TGAheader.imagetype) &
		   (Supported_TGA_types[i].pixel_bits == TGAheader.bits) &&
		   (Supported_TGA_types[i].supported))
			supported=i;

	if(supported == 255)
		BR_ERROR2("TGA image type %d in file '%s' not supported",TGAheader.imagetype,name);

	switch(TGAheader.bits)
	{
	    	case(8):type = BR_PMT_INDEX_8;
			bytes_per_line = TGAheader.width;
			palette_size = TGAheader.colourmaplength;
			break;

		case(15):type = BR_PMT_RGB_555;
			bytes_per_line = TGAheader.width*2;
			palette_size = 0;
			break;

		case(16):type = BR_PMT_RGB_565;
			bytes_per_line = TGAheader.width*2;
			palette_size = 0;
			break;
			
		case(24):type = BR_PMT_RGB_888;
			bytes_per_line = TGAheader.width*3;
			palette_size = 0;
			break;
		case(32):/*
			  * If flags & BR_PMT_RGBA_8888, pixelmap->type = flags
			  */

			  
			type = (flags & BR_PMT_RGBA_8888)?BR_PMT_RGBA_8888:BR_PMT_RGBX_888;
			bytes_per_line = TGAheader.width*4;
			palette_size = 0;
			break;
			 
		default:BR_ERROR2("%d bit TGA file '%s' not supported",TGAheader.bits,name);
			break;
	}

	if(!(TGAheader.descriptor & BR_TGA_REVERSED_DEPTH))
	{
	    ystart = TGAheader.depth-1;
		yend = -1;
		dy = -1;
	}
	else
	{
	    ystart = 0;
		yend = TGAheader.depth;
		dy = 1;
	}

	/*
	 * Advance to start of palette/image, allocate pixelmap for image data
	 */

	BrFileAdvance(TGAheader.identsize,fh);
	pm = BrPixelmapAllocate(type,TGAheader.width,TGAheader.depth,NULL,0);
	
	pm->identifier=BrResStrDup(pm,name);

	/*
	 * Get palette,allocate pixelmap for palette data
	 */
	
	if(TGAheader.colourmaptype)
	{
	    	switch(TGAheader.bits)
		{
			/*
			 * Allow for nasties that have true colour image with palette
			 */
		    
			default:BrFileAdvance(TGAheader.colourmaplength * (( TGAheader.colourmapbits ) >> 3),fh);
				break;
				
		    	case(8):pm->map = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);

				pm->map->identifier=BrResStrDup(pm->map,name);
		
				switch(TGAheader.colourmapbits)
				{
					case(16):for(i=TGAheader.colourmapstart; i<TGAheader.colourmaplength; i++)
						{
							if(i >= 256) break;
							if(BrFileRead(&RGB_16,1,sizeof(RGB_16),fh) != sizeof(RGB_16))
								BR_ERROR1("Unable to read palette from '%s'",name);
		
							((char *)(pm->map->pixels))[i*4+RED]   = ((RGB_16      ) & 0x1f) << 3;
							((char *)(pm->map->pixels))[i*4+GRN] = ((RGB_16 >> 5 ) & 0x1f) << 3;
							((char *)(pm->map->pixels))[i*4+BLU] = ((RGB_16 >> 10) & 0x1f) << 3;
							((char *)(pm->map->pixels))[i*4+PAD] = 0;
						}
						break;
					
					case(24):for(i=TGAheader.colourmapstart; i<TGAheader.colourmaplength; i++)
						{
							if(i >= 256) break;
							if(BrFileRead(&RGB_24,1,sizeof(RGB_24),fh) != sizeof(RGB_24))
								BR_ERROR1("Unable to read palette from '%s'",name);
								
							((char *)(pm->map->pixels))[i*4+RED] = RGB_24.red;
							((char *)(pm->map->pixels))[i*4+GRN] = RGB_24.green;
							((char *)(pm->map->pixels))[i*4+BLU] = RGB_24.blue;
							((char *)(pm->map->pixels))[i*4+PAD] = 0;
						}	
						break;
				}
				break;
		}
	}

	/*
	 * Get image data
	 */

	mblock = BrMemAllocate(bytes_per_line,BR_MEMORY_SCRATCH);

	for(i=ystart; i!=yend; i+=dy)
	{
	    	if(Supported_TGA_types[supported].read_func(mblock,fh) == NULL)
			BR_ERROR1("Unable to read image data from '%s'",name);

		Supported_TGA_types[supported].copy_func(pm,mblock,i);
	}
	
	BrFileClose(fh);

	return pm;
}

static void *ReadLine_Uncomp_8(char *line,void *fh)
{
    	if(BrFileRead(line,1,TGAheader.width,fh) != TGAheader.width)
		return NULL;
	else
		return line;
}
static void * ReadLine_Comp_8(char *line,void *fh)
{
    	unsigned char ch;
	br_uint_32 size,n = 0;
	int i;

	do
	{
		if(BrFileRead(&ch,1,1,fh) != 1)
			return NULL;
		size = (ch & 0x7f)+1;
		
		if(ch & 0x80)	/* run of pixels */
		{
		    	if(BrFileRead(&ch,1,1,fh) != 1)
				return NULL;
	
			for(i=0; i< (int)size; i++)
				line[n+i] = ch;
		}
		else		/* string */
		{
		    	for(i=0; i<(int)size; i++)
			{
			    	if(BrFileRead(&ch,1,1,fh) != 1)
					return NULL;
					
				line[n+i] = ch;
			}
		}
		
		n += size;
	}
	while (n < TGAheader.width);
	return line;
}

static void * ReadLine_Uncomp_16(char *line,void *fh)
{
    	int i;

	for(i=0;i<TGAheader.width;i++)
	{
	    	if(BrFileRead(&RGB_16,1,sizeof(RGB_16),fh) != sizeof(RGB_16))
			return NULL;
			
		line[i*2] = RGB_16 & 0xff;
		line[i*2+1]   = (RGB_16 >> 8) &0xff;
	}
	return line;
}
static void * ReadLine_Comp_16(char *line,void *fh)
{
    	unsigned char ch;
	br_uint_32 size,n = 0;
	int i;

	do
	{
		if(BrFileRead(&ch,1,1,fh) != 1)
			return NULL;
		size = (ch & 0x7f)+1;
		
		if(ch & 0x80)	/* run of pixels */
		{
		    	if(BrFileRead(&RGB_16,1,sizeof(RGB_16),fh) != sizeof(RGB_16))
				return NULL;
	
			for(i=0; i<(int)size; i++)
			{
				line[(n+i)*2] = RGB_16 & 0xff;
				line[(n+i)*2+1]   = (RGB_16 >> 8) &0xff;
			}
		}
		else		/* string */
		{
		    	for(i=0; i<(int)size; i++)
			{
			    	if(BrFileRead(&RGB_16,1,sizeof(RGB_16),fh) != sizeof(RGB_16))
					return NULL;
					
				line[(n+i)*2] = RGB_16 & 0xff;
				line[(n+i)*2+1]   = (RGB_16 >> 8) &0xff;
			}
		}
		
		n += size;

	}
	while (n < TGAheader.width);
	return line;
}

static void * ReadLine_Uncomp_24(char *line,void *fh)
{
    	int i;

	for(i=0;i<TGAheader.width;i++)
	{
	    	if(BrFileRead(&RGB_24,1,sizeof(RGB_24),fh) != sizeof(RGB_24))
			return NULL;
			
		line[i*3]   = RGB_24.blue;
		line[i*3+1] = RGB_24.green;
		line[i*3+2] = RGB_24.red;
	}
	return line;
}
static void * ReadLine_Comp_24(char *line,void *fh)
{
    	unsigned char ch;
	br_uint_32 size,n = 0;
	int i;

	do
	{
		if(BrFileRead(&ch,1,1,fh) != 1)
			return NULL;
		size = (ch & 0x7f)+1;
		
		if(ch & 0x80)	/* run of pixels */
		{
		    	if(BrFileRead(&RGB_24,1,sizeof(RGB_24),fh) != sizeof(RGB_24))
				return NULL;
	
			for(i=0; i<(int)size; i++)
			{
				line[(n+i)*3]   = RGB_24.blue;
				line[(n+i)*3+1] = RGB_24.green;
				line[(n+i)*3+2] = RGB_24.red;
			}
		}
		else		/* string */
		{
		    	for(i=0; i<(int)size; i++)
			{
			    	if(BrFileRead(&RGB_24,1,sizeof(RGB_24),fh) != sizeof(RGB_24))
					return NULL;
					
				line[(n+i)*3]   = RGB_24.blue;
				line[(n+i)*3+1] = RGB_24.green;
				line[(n+i)*3+2] = RGB_24.red;
			}
		}
		
		n += size;

	}
	while (n < TGAheader.width);
	return line;
}

static void * ReadLine_Uncomp_32(char *line,void *fh)
{
    	int i;

	for(i=0;i<TGAheader.width;i++)
	{
	    	if(BrFileRead(&RGB_32,1,sizeof(RGB_32),fh) != sizeof(RGB_32))
			return NULL;
			
		line[i*4]   = RGB_32.blue;
		line[i*4+1] = RGB_32.green;
		line[i*4+2] = RGB_32.red;
		line[i*4+3] = RGB_32.x;
	}
	return line;
}
static void * ReadLine_Comp_32(char *line,void *fh)
{
    	unsigned char ch;
	br_uint_32 size,n = 0;
	int i;

	do
	{
		if(BrFileRead(&ch,1,1,fh) != 1)
			return NULL;
		size = (ch & 0x7f)+1;
		
		if(ch & 0x80)	/* run of pixels */
		{
		    	if(BrFileRead(&RGB_32,1,sizeof(RGB_32),fh) != sizeof(RGB_32))
				return NULL;
	
			for(i=0; i<(int)size; i++)
			{
				line[(n+i)*4]   = RGB_32.blue;
				line[(n+i)*4+1] = RGB_32.green;
				line[(n+i)*4+2] = RGB_32.red;
				line[(n+i)*4+3] = RGB_32.red;
			}
		}
		else		/* string */
		{
		    	for(i=0; i<(int)size; i++)
			{
			    	if(BrFileRead(&RGB_32,1,sizeof(RGB_32),fh) != sizeof(RGB_32))
					return NULL;
					
				line[(n+i)*4]   = RGB_32.blue;
				line[(n+i)*4+1] = RGB_32.green;
				line[(n+i)*4+2] = RGB_32.red;
				line[(n+i)*4+3] = RGB_32.x;
			}
		}
		
		n += size;

	}
	while (n < TGAheader.width);
	return line;
}

static void CopyLine_8(br_pixelmap *pm,char *line,int row)
{
   	br_int_32 xstart,xend,dx;
	int i,j;

    	if(TGAheader.descriptor & BR_TGA_REVERSED_WIDTH)
	{
	    xstart = TGAheader.width-1;
	    xend = -1;
	    dx = -1;
	}
	else
	{
	    xstart = 0;
	    xend = TGAheader.width;
	    dx = 1;
	}

	for(i=xstart,j=0; i!=xend; i+=dx,j++)
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j]=line[i];
}

static void CopyLine_16(br_pixelmap *pm,char *line,int row)
{
   	br_int_32 xstart,xend,dx;
	int i,j;

	if(TGAheader.descriptor & BR_TGA_REVERSED_WIDTH)
	{
	    xstart = TGAheader.width-1;
	    xend = -1;
	    dx = -1;
	}
	else
	{
	    xstart = 0;
	    xend = TGAheader.width;
	    dx = 1;
	}

	for(i=xstart*2,j=0; i!=xend*2; i+=dx*2,j+=2)
	{
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j]   = line[i   ];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+1] = line[i +	dx];
	}

}
static void CopyLine_24(br_pixelmap *pm,char *line,int row)
{
   	br_int_32 xstart,xend,dx;
	int i,j;

	if(TGAheader.descriptor & BR_TGA_REVERSED_WIDTH)
	{
	    xstart = TGAheader.width-1;
	    xend = -1;
	    dx = -1;
	}
	else
	{
	    xstart = 0;
	    xend = TGAheader.width;
	    dx = 1;
	}

	for(i=xstart*3,j=0; i!=xend*3; i+=dx*3,j+=3)
	{
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j]   = line[i ];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+1] = line[i + dx];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+2] = line[i + dx*2];
	}
}

static void CopyLine_32(br_pixelmap *pm,char *line,int row)
{
   	br_int_32 xstart,xend,dx;
	int i,j;

	if(TGAheader.descriptor & BR_TGA_REVERSED_WIDTH)
	{
	    xstart = TGAheader.width-1;
	    xend = -1;
	    dx = -1;
	}
	else
	{
	    xstart = 0;
	    xend = TGAheader.width;
	    dx = 1;
	}

	for(i=xstart*4,j=0; i!=xend*4; i+=dx*4,j+=4)
	{
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j]   = line[i];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+1] = line[i + dx];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+2] = line[i + dx*2];
	    	((char *)(pm->pixels))[(row*pm->row_bytes)+j+3] = line[i + dx*3];
	}
}
