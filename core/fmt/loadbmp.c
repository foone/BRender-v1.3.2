/*
 * Copyright (c) 1993-1995 by Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: loadbmp.c 1.1 1997/12/10 16:41:01 jon Exp $
 * $Locker: $
 *
 * Loader for Windows .BMP format
 * support for <=8 bit index palette based, 24 bit true colour
 * TODO 16,32 bit BI_BITFIELD compression (no compression!)
 */
#include "brender.h"
#include "fmt.h"

BR_RCS_ID("$Id: loadbmp.c 1.1 1997/12/10 16:41:01 jon Exp $")

int FileNumCol(void *tmp);

#define RED		2
#define GRN		1
#define BLU		0
#define PAD		3

#define BI_RGB		0L
#define BI_RLE8		1L
#define BI_RLE4		2L
#define BI_BITFIELDS	3L

#define WIDTHBYTES(i) ((i+31)/32*4)

#define RLE_ESCAPE  0
#define RLE_EOL     0
#define RLE_EOF     1
#define RLE_JMP     2

static void *(*compressed_line_func)(br_pixelmap *pm,int line,void *fh);

static void *Uncompressed_4(br_pixelmap *pm,int line,void *fh);
static void *Uncompressed_8(br_pixelmap *pm,int line,void *fh);
static void *Uncompressed_24(br_pixelmap *pm,int line,void *fh);


static void *Compressed_8(br_pixelmap *pm,int line,void *fh);
static void *Compressed_4(br_pixelmap *pm,int line,void *fh);


#pragma pack (1)

typedef struct {
	short bfType;
	int bfSize;		/* size of image file				*/
	short bfReserved1;
	short bfReserved2;
	int bfOffBits;		/* size of header	(including palette)	*/
} BITMAPFILEHEADER;

typedef struct {
	int biSize;		/* size of BITMAPINFOHEADER always 0x28		*/
	int biWidth;		/* width in pixels				*/
	int biHeight;		/* depth in pixels				*/
	short biPlanes;		/* always 1					*/
	short biBitCount;	/* bits per pixel: 1,4,8,16,24,32		*/
	int biCompression;	/* always 0, however spec exists for compression*/
	int biSizeImage;	/* number of bytes in image			*/
	int biXPelsPerMeter;	/* resolution					*/
	int biYPelsPerMeter;	/* resolution					*/
	int biClrUsed;		/* Number of colours users, 0=all ie. 8 bit=256	*/
	int biClrImportant;	/* Number of important colours? 0=all		*/
} BITMAPINFOHEADER, *ptrBITMAPINFOHEADER;

typedef struct {
    	int bcSize;		/* size of BITMAPINFOHEADER always 0x0c		*/
	short bcWidth;
	short bcHeight;
	short bcPlanes;
	short bcBitCount;
} BITMAPCOREHEADER, *ptrBITMAPCOREHEADER;

static struct {
    	char control;
	char data;
} RLEcontrol;

static struct {
    char blue,green,red;
} RGB3;

static struct {
    char blue,green,red,pad;
} RGB4;

#pragma pack ()

br_pixelmap * BR_PUBLIC_ENTRY BrFmtBMPLoad(char *name,br_uint_32 flags)
{
	void *fh;
	int open_mode = BR_FS_MODE_BINARY;
	br_pixelmap *pm,*palette;
	int headersize,palettesize;

	BITMAPINFOHEADER bi;
	BITMAPCOREHEADER bc;
	BITMAPFILEHEADER bf;
	int size;

	int NumColours;
	int dwWidth =0;
	int dwHeight=0;
	short wPlanes,wBitCount;

	br_uint_8 type;
	int i;
	
	fh = BrFileOpenRead(name,0,NULL,&open_mode);

	if(fh == NULL)
		BR_ERROR1("Could not open '%s' for reading",name);

	if(BrFileRead(&bf,1,sizeof(bf),fh) != sizeof(bf))
		BR_ERROR1("Unable to read header information from '%s'",name);

	/*
	 * load header, check for sig.
	 */
	if(BrMemCmp(&bf.bfType,"BM",2))
		BR_ERROR1("'%s' is not a valid BMP file",name);

	if(BrFileRead(&bi,1,sizeof(bi),fh) != sizeof(bi))
		BR_ERROR1("Unable to read bitmap info header from '%s'",name);

	NumColours = FileNumCol(&bi);
	palettesize = 0;

	/*
	 * check for old/new header type
	 */
	switch(size = bi.biSize)
	{
	    	case(sizeof(BITMAPINFOHEADER)):	headersize = sizeof(BITMAPFILEHEADER) + bi.biSize;
						break;

		case(sizeof(BITMAPCOREHEADER)): bc = *(BITMAPCOREHEADER*)&bi;

						headersize = sizeof(BITMAPFILEHEADER) + bc.bcSize;

						dwWidth = (int)bc.bcWidth;
						dwHeight = (int)bc.bcHeight;
						wPlanes = bc.bcPlanes;
						wBitCount = bc.bcBitCount;

						bi.biSize = sizeof(BITMAPINFOHEADER);
						bi.biWidth = dwWidth;
						bi.biHeight = dwHeight;
						bi.biPlanes = wPlanes;
						bi.biBitCount = wBitCount;

						bi.biCompression = BI_RGB;
						bi.biSizeImage = 0;
						bi.biXPelsPerMeter = 0;
						bi.biYPelsPerMeter = 0;
						bi.biClrUsed = NumColours;
						bi.biClrImportant = NumColours;

						BrFileAdvance(sizeof(BITMAPCOREHEADER) - sizeof(BITMAPINFOHEADER),fh);
						
						break;
		default:BR_ERROR1("'%s' is not a valid BMP file",name);
	}

	if(bi.biSizeImage == 0)
		bi.biSizeImage = WIDTHBYTES(bi.biWidth * bi.biBitCount) * bi.biHeight;

	if(bi.biClrUsed == 0)
		bi.biClrUsed = FileNumCol(&bi);

	/*
	 * read palette
	 */
	switch(bi.biBitCount)
	{
	    	case(1):BR_ERROR1("1 bit BMP '%s' not supported",name);
			break;
			
		case(4):	/* promote 4 to 8 bit */
		
		case(8):switch(bi.biCompression)
			{
			    	case(BI_RGB):compressed_line_func = (bi.biBitCount == 4)?Uncompressed_4:Uncompressed_8;
						break;
				case(BI_RLE4):compressed_line_func = Compressed_4;
						break;
				case(BI_RLE8):compressed_line_func = Compressed_8;
						break;
				case(BI_BITFIELDS):BR_ERROR1("BMP compression type BITFIELDS not supported in '%s'",name);
						break;
			}
		
			type = BR_PMT_INDEX_8;

			palette=BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);


			palette->identifier = BrResStrDup(palette, name);

			if(size == sizeof(BITMAPCOREHEADER))
			    	for(i=0; i<NumColours; i++, palettesize+=3)
				{
				    	if(BrFileRead(&RGB3,1,sizeof(RGB3),fh) != sizeof(RGB3))
						BR_ERROR1("Unable to read palette from '%s'",name);
					((char *)(palette->pixels))[i*4+RED] = RGB3.red;
					((char *)(palette->pixels))[i*4+GRN] = RGB3.green;
					((char *)(palette->pixels))[i*4+BLU] = RGB3.blue;
					((char *)(palette->pixels))[i*4+PAD] = 0;
	    			}
			else
				for(i=0; i<NumColours; i++,palettesize+=4)
				{
				    	if(BrFileRead(&RGB4,1,sizeof(RGB4),fh) != sizeof(RGB4))
						BR_ERROR1("Unable to read palette from '%s'",name);
					((char *)(palette->pixels))[i*4+RED] = RGB4.red;
					((char *)(palette->pixels))[i*4+GRN] = RGB4.green;
					((char *)(palette->pixels))[i*4+BLU] = RGB4.blue;
					((char *)(palette->pixels))[i*4+PAD] = RGB4.pad;
				}

			break;
		case(16):BR_ERROR1("16 bit BMP '%s' not supported",name);
			break;
		
		case(24):if(bi.biCompression == BI_RGB)
				compressed_line_func = Uncompressed_24;
			else BR_ERROR1("24 bit compressed file '%s' not supported",name);
			
			type = BR_PMT_RGB_888;

			break;

		case(32):BR_ERROR1("32 bit BMP '%s' not supported",name);
			break;
	}
	if(bf.bfOffBits != 0)
		BrFileAdvance(bf.bfOffBits-headersize-palettesize,fh);

	pm = BrPixelmapAllocate(type,bi.biWidth,bi.biHeight,NULL,0);

	pm->identifier = BrResStrDup(pm, name);

	pm->map = (type == BR_PMT_INDEX_8)?palette:NULL;
		
	switch(bi.biCompression)
	{
	   	/*
		 * No compression
		 */
	    	case(BI_RGB):for(i=bi.biHeight-1;i>=0;i--)
			    	if(compressed_line_func(pm,i,fh) == NULL)
				 	BR_ERROR1("Unable to read image data from '%s'",name);
			     break;
		/*
		 * Run length encoding
		 */
		case(BI_RLE4):
		case(BI_RLE8):if(compressed_line_func(pm,0,fh) == NULL)
			      		BR_ERROR1("Unable to read compressed image data from '%s'",name);
			      break;
			      
		case(BI_BITFIELDS):
				   break;
	}
	
	BrFileClose(fh);

	return pm;
}

int FileNumCol(void *tmp)
{
    	int bits;
	ptrBITMAPINFOHEADER bi;
	ptrBITMAPCOREHEADER bc;

	bi = ((ptrBITMAPINFOHEADER)tmp);
	bc = ((ptrBITMAPCOREHEADER)tmp);

	if (bi->biSize != sizeof(BITMAPCOREHEADER))
	{
	    	if(bi->biClrUsed != 0)
			return bi->biClrUsed;
		bits = bi->biBitCount;
	}
	else
		bits = bc->bcBitCount;

	switch(bits)
	{
	    	case(1):return 2;
			break;
		case(4):return 16;
			break;
		case(8):return 256;
			break;
		default:return 0;	/* 24 bit has no palette */
	}
}

static void *Uncompressed_4(br_pixelmap *pm,int line,void *fh)
{
   	int i;
	char c;

	for(i=0;i<pm->width;)
	{
	    	c = BrFileGetChar(fh);
		
	    	((char *)(pm->pixels))[(line*pm->row_bytes)+i++] = (c >>4) & 0x0f;
	    	((char *)(pm->pixels))[(line*pm->row_bytes)+i++] = (c & 0x0f);
	}
	
	if(((pm->width+1)/2) %4)
		BrFileAdvance(4-(((pm->width+1)/2) %4),fh);
		
	return pm;
}

static void *Uncompressed_8(br_pixelmap *pm,int line,void *fh)
{
    	if(BrFileRead(((char *)(pm->pixels)+line*pm->row_bytes),1,pm->width,fh) != pm->width)
		return NULL;
	if(pm->width %4) BrFileAdvance(4-(pm->width %4),fh);
	return pm;
}

static void *Uncompressed_24(br_pixelmap *pm,int line,void *fh)
{
	int i;

	for(i=0;i<pm->width;i++)
	{
	    	if(BrFileRead(&RGB3,1,sizeof(RGB3),fh) != sizeof(RGB3))
			return NULL;
			
		((char *)(pm->pixels))[line*pm->row_bytes+i*3+RED] = RGB3.red;
		((char *)(pm->pixels))[line*pm->row_bytes+i*3+GRN] = RGB3.green;
		((char *)(pm->pixels))[line*pm->row_bytes+i*3+BLU] = RGB3.blue;
	}
	if((pm->width*3)%4) BrFileAdvance(4-((pm->width*3)%4),fh);
	return pm;
}

static void *Compressed_8(br_pixelmap *pm,int line,void *fh)
{
    	char i;
	long dx,dy;
	char *mblock;

	mblock = BrMemAllocate(pm->width,BR_MEMORY_SCRATCH);
	
	for(dx=0,dy=pm->height-1;;)
	{
		if(BrFileRead(&RLEcontrol,1,sizeof(RLEcontrol),fh) != sizeof(RLEcontrol))
		{
		    	BrMemFree(mblock);
			return NULL;
		}
		if(RLEcontrol.control == RLE_ESCAPE)
		{
		    	switch(RLEcontrol.data)
			{
				case(RLE_EOF):BrMemFree(mblock);
					      return pm;
					      break;

				case(RLE_EOL):dx = 0;
					      BrMemCpy((char *)pm->pixels+ dy-- * pm->row_bytes,mblock,pm->width);
					      
					      break;

				case(RLE_JMP):dx += BrFileGetChar(fh);
					      dy += BrFileGetChar(fh);

						break;
					      
				default:for(i=0;i<RLEcontrol.data;i++)
						mblock[dx++] = BrFileGetChar(fh);
						
					if(i & 1) BrFileAdvance(1,fh);
					
					break;
			}
		}
		else
		    for(i=0;i<RLEcontrol.control;i++)
		    	mblock[dx++] = RLEcontrol.data;
	}
}

static void *Compressed_4(br_pixelmap *pm,int line,void *fh)
{
    	char i,pixel;
	long dx,dy;
	char *mblock;
	long offset;

	mblock = BrMemAllocate(pm->width,BR_MEMORY_SCRATCH);
	
	for(dx=0,dy=pm->height-1,offset=0;;)
	{
		if(BrFileRead(&RLEcontrol,1,sizeof(RLEcontrol),fh) != sizeof(RLEcontrol))
		{
		    	BrMemFree(mblock);
			return NULL;
		}
		offset+=sizeof(RLEcontrol);
		
		if(RLEcontrol.control == RLE_ESCAPE)
		{
		    	switch(RLEcontrol.data)
			{
				case(RLE_EOF):BrMemFree(mblock);
					      return pm;
					      break;

				case(RLE_EOL):dx = 0;
					      BrMemCpy((char *)pm->pixels+ dy-- * pm->row_bytes,mblock,pm->width);
					      if(dy == 0)
					      {
						  	BrMemFree(mblock);
							return pm;
					      }
					      
					      break;

				case(RLE_JMP):dx += BrFileGetChar(fh);
					      dy += BrFileGetChar(fh);

					      offset += 2;
					      
					      break;
					      
				default:i=RLEcontrol.data;
					do
					{	/* absolute */
					    
					    	pixel = BrFileGetChar(fh);
						offset++;
						
						mblock[dx++] = (pixel >> 4) & 0x0f;
						i--;

						if(i>0)
						{
							mblock[dx++] = (pixel & 0x0f);
							i--;
						}
					}
					while(i>0);
					
					if(offset & 1)
					{
						BrFileAdvance(offset & 1,fh);
						offset++;
					}
					
					break;
			}
		}
		else
		{	/* encoded */
		    
			i=RLEcontrol.control;
			do
			{
				mblock[dx++] = (RLEcontrol.data >> 4) & 0x0f;
				i--;
				if(i>0)
				{
				    	mblock[dx++] = (RLEcontrol.data & 0x0f);
					i--;
				}
			}
			while(i>0);
		}
	}
}
