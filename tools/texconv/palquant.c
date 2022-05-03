#include <brender.h>
#include <dosio.h>
#include <string.h>

/*
 * get a br_colour from each pixelmap type
 */
br_colour get_pmt_index_8(br_pixelmap *pm,int x,int y,int offset,int byte);
br_colour get_pmt_rgb_555(br_pixelmap *pm,int x,int y,int offset,int byte);
br_colour get_pmt_rgb_565(br_pixelmap *pm,int x,int y,int offset,int byte);
br_colour get_pmt_rgb_888(br_pixelmap *pm,int x,int y,int offset,int byte);
br_colour get_pmt_rgbx_888(br_pixelmap *pm,int x,int y,int offset,int byte);
br_colour get_pmt_rgba_8888(br_pixelmap *pm,int x,int y,int offset,int byte);

/*
 * put a br_colour for each pixelmap type
 */
void put_pmt_rgb_555(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);
void put_pmt_rgb_565(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);
void put_pmt_rgb_888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);
void put_pmt_rgbx_888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);
void put_pmt_rgba_8888(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);

/*
 * pixelmap types supported by texconv for gettin' and puttin' br_colours
 */
static struct {
    	int value;
	char *name;
	char *description;
} PixelmapTypes[]={
	{BR_PMT_INDEX_8		,"BR_PMT_INDEX_8","8 bit indexed"},
	{BR_PMT_RGB_555		,"BR_PMT_RGB_555","RGB 16 bit 5 bits per colour"},
	{BR_PMT_RGB_565		,"BR_PMT_RGB_565","RGB 16 bit 5,6,5 bits per colour"},
	{BR_PMT_RGB_888		,"BR_PMT_RGB_888","RGB 24 bit 8 bits per pixel"},
	{BR_PMT_RGBX_888	,"BR_PMT_RGBX_888","RGB 32 bit 8 bits per pixel"},
	{BR_PMT_RGBA_8888	,"BR_PMT_RGBA_8888","RGBA 32 bit 8 bits per component"},
};

static struct {
    	int type;						/* pixelmap type */
    	br_colour (*function)(br_pixelmap *pm,int x,int y,int offset,int byte);	/* function to read pixel from pixelmap */
} GetPixelTypes[]={
	{BR_PMT_INDEX_8	,	get_pmt_index_8,	},
	{BR_PMT_RGB_555	,	get_pmt_rgb_555,	},
	{BR_PMT_RGB_565	,	get_pmt_rgb_565,	},
	{BR_PMT_RGB_888	,	get_pmt_rgb_888,	},
	{BR_PMT_RGBX_888,	get_pmt_rgbx_888,	},
	{BR_PMT_RGBA_8888,	get_pmt_rgba_8888,	},
};

static struct {
    	int type;							/* pixelmap type */
    	void (*function)(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);	/* function to save pixel to pixelmap */
} PutPixelTypes[]={
	{	BR_PMT_INDEX_8	,	NULL,			},
	{	BR_PMT_RGB_555	,	put_pmt_rgb_555,	},
	{	BR_PMT_RGB_565	,	put_pmt_rgb_565,	},
	{	BR_PMT_RGB_888	,	put_pmt_rgb_888,	},
	{	BR_PMT_RGBX_888	,	put_pmt_rgbx_888,	},
	{	BR_PMT_RGBA_8888,	put_pmt_rgba_8888,	},
};

static br_colour (*GetPixel)(br_pixelmap *pm,int x,int y,int offset,int byte);
static void (*PutPixel)(br_pixelmap *pm,br_colour colour,int x,int y,int offset,int byte);
static void CopyIdent(br_pixelmap *dst,br_pixelmap *src);

/*
 * current 32 bit pixelmap type
 */
static br_uint_32 alpha_channel = BR_PMT_RGBX_888;
/*
 * current alpha channel threshold (0-255)
 */
static br_uint_8 alpha_threshold = 127;

void BrQuantAddPixelmap(br_pixelmap *pm)
{
	int i,x,y,threshold;
	br_colour colour;
	br_uint_8 rgb[3];
	int offset,byte;
	
	/*
	 * Add a pixelmap to quantizer
	 */
	if(!PalettePresent(pm))
		BR_ERROR1("Indexed '%s' cannot be quantized without palette",pm->identifier);
		
	for(i=0; i<BR_ASIZE(GetPixelTypes); i++)
		if(GetPixelTypes[i].type == pm->type)
		{
			GetPixel = GetPixelTypes[i].function;
			break;
		}

	/*
	 * for all pixels...
	 */
	byte = BrPixelmapPixelSize(pm) >> 3;
	offset = pm->base_y*pm->row_bytes + pm->base_x*byte;
	
	for(y=0; y<pm->height; y++)
		for(x=0; x<pm->width; x++)
		{
			colour = GetPixel(pm,x,y,offset,byte);
			threshold = colour >> 24;

			/*
			 * if top 8 bits of br_colour is aplha, do not insert colour into quantizer
			 */
			if(threshold < alpha_threshold) threshold = 0;

			if(!threshold)
			{			
				rgb[0] = BR_RED(colour);
				rgb[1] = BR_GRN(colour);
				rgb[2] = BR_BLU(colour);

				/*
				 * insert RGB pixel into quantizer
				 */
				BrQuantAddColours(rgb,1);
			}
		}
}
 br_pixelmap *BrQuantMapPixelmap(int base,br_pixelmap *pm)
{
	br_pixelmap *new_pm;
	int x,y,i,threshold;
	br_colour colour;
	br_uint_8 rgb[3];
	int offset,byte;
	/*
	 * generate new pixelmap from quantizer
	 */
	if(!PalettePresent(pm))
		BR_ERROR1("Indexed '%s' cannot be quantized without palette",pm->identifier);
	
	new_pm = BrPixelmapAllocate(BR_PMT_INDEX_8,pm->width,pm->height,NULL,0);

	for(i=0; i<BR_ASIZE(GetPixelTypes); i++)
		if(GetPixelTypes[i].type == pm->type)
		{
			GetPixel = GetPixelTypes[i].function;
			break;
		}

	/*
	 * for all pixels...
	 */
	byte = BrPixelmapPixelSize(pm) >> 3;
	offset = pm->base_y*pm->row_bytes + pm->base_x*byte;
	
	for(y=0; y<pm->height; y++)
		for(x=0; x<pm->width; x++)
		{
		    	colour = GetPixel(pm,x,y,offset,byte);
			/*
			 * if top 8 bits of br_colour contains threshold, check against alpha_threshold
			 * if greater, set index to zero
			 */
			threshold = colour >> 24;
			
			if(threshold < alpha_threshold) threshold = 0;

			if(!threshold)
			{
				rgb[0] = BR_RED(colour);
				rgb[1] = BR_GRN(colour);
				rgb[2] = BR_BLU(colour);
			
				BrQuantMapColours(base,rgb,(char *)new_pm->pixels+y*new_pm->row_bytes+x,1);
			}
			else
				((char *)(new_pm->pixels))[y*new_pm->row_bytes + x] = 0;
		}
	
	return new_pm;
}
 void DoQuantize(br_pixelmap **mtable,int nmaps,int base,int range,br_pixelmap *palette)
{
    	int i;
	br_pixelmap *new_pm;
	/*
	 * perfrom quantize on all pixelmaps in mtable to base,range of entries in palette
	 */
	BrQuantBegin();

	/*
	 * add all pixel data
	 */
	for(i=0; i<nmaps; i++)
	{
#ifdef VERBOSE
		fprintf(stderr,
			"Quantizing '%s' to palette '%s' using %d colours from the range %d-%d\n",
			mtable[i]->identifier,palette->identifier,range,base,base+range-1);
#endif
		BrQuantAddPixelmap(mtable[i]);
	}

	/*
	 * make internal map to new palette
	 */
	BrQuantPrepareMapping(base,range,palette);

	/*
	 * extract pixel data from map of new palette
	 */
	for(i=0; i<nmaps; i++)
	{
	    new_pm = BrQuantMapPixelmap(base,mtable[i]);
		CopyIdent(new_pm,mtable[i]);
						
		BrMapRemove(mtable[i]);
		BrPixelmapFree(mtable[i]);

		new_pm->map = palette;
	
		BrMapAdd(new_pm);
	}
						
	BrQuantEnd();
}
 br_pixelmap *Remap(int base,int range,br_pixelmap *pm)
{
    	/*
	 * remap without palette information, just re-index 
	 */
	br_pixelmap *new_pm;
	int x,y,byte,offset;
	char *src,*dst;
	
	byte = BrPixelmapPixelSize(pm) >> 3;
	offset = pm->base_y*pm->row_bytes + pm->base_x*byte;

	new_pm = BrPixelmapAllocate(BR_PMT_INDEX_8,pm->width,pm->height,NULL,0);
	for(y=0, src=pm->pixels, dst=new_pm->pixels ; y<pm->height; y++, src+=pm->row_bytes, dst+=new_pm->row_bytes)
		for(x=0; x<pm->width; x++)
		{
			if((dst[x] = src[x] + base) > base+range-1)
				BR_ERROR1("Unable to reduce '%s' colour range without palette",pm->identifier);
			
		}

	return new_pm;
}
 void DoRemap(br_pixelmap **mtable,int nmaps,int base,int range)
{
    	int i;
	br_pixelmap *palette,*new_pm;
	/*
	 * remap pixelmaps in mtable, quantize if necessary to base,range
	 * cumulative effect, quantize all pixelmaps to one palette
	 */
	BrQuantBegin();
						
	palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);
	palette->identifier = "palette";

	for(i=0; i<nmaps; i++)
	{
#ifdef VERBOSE
		fprintf(stderr,
			"Remapping '%s' to the range %d-%d",
			mtable[i]->identifier,base,base+range-1);
#endif
		/*
		 * only add pixel data if there is a palette to find rgb
		 */
		if(PalettePresent(mtable[i]))
		{
#ifdef VERBOSE
			fprintf(stderr," (colour reduction)");
#endif							    
			BrQuantAddPixelmap(mtable[i]);
		}
#ifdef VERBOSE
		fprintf(stderr,"\n");
#endif							
	}

	BrQuantMakePalette(base,range,palette);
	BrQuantPrepareMapping(base,range,palette);

	for(i=0; i<nmaps; i++)
	{
		if(PalettePresent(mtable[i]))
		{
		    	/*
			 * quantize
			 */
		    	new_pm = BrQuantMapPixelmap(base,mtable[i]);
			new_pm->map = palette;
		}
		else
			/*
			 * if no palette present, just do remap of index
			 */
			new_pm = Remap(base,range,mtable[i]);

		CopyIdent(new_pm,mtable[i]);
								
		BrMapRemove(mtable[i]);
		BrPixelmapFree(mtable[i]);
	
		BrMapAdd(new_pm);
	}
	
	BrQuantEnd();
}


void DoRemapWithSupermaps(br_pixelmap **mtable,int nmaps,br_pixelmap **smtable,int nsmaps,int smap_power,int base,int range)
{
   	int i, j;
	br_pixelmap *palette,*new_pm;
	/*
	 * remap pixelmaps in mtable, quantize if necessary to base,range
	 * cumulative effect, quantize all pixelmaps to one palette
	 */
	BrQuantBegin();
						
	palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);
	palette->identifier = "palette";

	for(i=0; i<nmaps; i++)
	{
#ifdef VERBOSE
		fprintf(stderr,
			"Remapping '%s' to the range %d-%d",
			mtable[i]->identifier,base,base+range-1);
#endif
		/*
		 * only add pixel data if there is a palette to find rgb
		 */
		if(PalettePresent(mtable[i]))
		{
#ifdef VERBOSE
			fprintf(stderr," (colour reduction)");
#endif							    
			BrQuantAddPixelmap(mtable[i]);
		}
#ifdef VERBOSE
		fprintf(stderr,"\n");
#endif							
	}

	for(i=0; i<nsmaps; i++)
	{
#ifdef VERBOSE
		fprintf(stderr,
			"Remapping supermap '%s' to the range %d-%d",
			smtable[i]->identifier,base,base+range-1);
#endif
		/*
		 * only add pixel data if there is a palette to find rgb
		 */
		if(PalettePresent(smtable[i]))
		{
#ifdef VERBOSE
			fprintf(stderr," (colour reduction)");
#endif
			for (j=0; j<smap_power; j++)
				BrQuantAddPixelmap(smtable[i]);
		}
#ifdef VERBOSE
		fprintf(stderr,"\n");
#endif							
	}

	BrQuantMakePalette(base,range,palette);
	BrQuantPrepareMapping(base,range,palette);

	for(i=0; i<nmaps; i++)
	{
		if(PalettePresent(mtable[i]))
		{
		    /*
			 * quantize
			 */
		    new_pm = BrQuantMapPixelmap(base,mtable[i]);
			new_pm->map = palette;
		}
		else
			/*
			 * if no palette present, just do remap of index
			 */
			new_pm = Remap(base,range,mtable[i]);

		CopyIdent(new_pm,mtable[i]);
								
		BrMapRemove(mtable[i]);
		BrPixelmapFree(mtable[i]);
	
		BrMapAdd(new_pm);
	}
	
	BrQuantEnd();
}




 br_pixelmap *DoConvert(br_pixelmap *src,int new_type)
{
    	int x,y,i;
	br_pixelmap *dst,*palette;
	br_colour pixel;
	int src_offset,src_byte;
	int dst_offset,dst_byte;

	/*
	 * convert from one pixelmap type to another
	 */

	/*
	 * is current source type supported?
	 */
	for(i=0; i<BR_ASIZE(GetPixelTypes); i++)
		if(GetPixelTypes[i].type == src->type)
		{
			GetPixel = GetPixelTypes[i].function;
			break;
		}
	if(i >= BR_ASIZE(GetPixelTypes))
		BR_ERROR1("Unknown pixelmap type",src->type);
	/*
	 * is current destination type supported?
	 */
	for(i=0; i<BR_ASIZE(PutPixelTypes); i++)
		if(PutPixelTypes[i].type == new_type)
		{
			PutPixel = PutPixelTypes[i].function;
			break;
		}
	if(i >= BR_ASIZE(PutPixelTypes))
		BR_ERROR1("Unknown pixelmap type",new_type);

	/*
	 * if source is BR_PMT_INDEX_8, cannot convert without palette information
	 */
	if(!PalettePresent(src))
		BR_ERROR1("Indexed '%s' cannot be converted without palette",src->identifier);

	if(new_type == BR_PMT_INDEX_8)
	{
	    	/*
		 * if converting to BR_PMT_INDEX_8, do a quantize
		 */
	    	BrQuantBegin();
		
		palette = BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,0);
		palette->identifier = BrMemAllocate(strlen(src->identifier)+4,BR_MEMORY_STRING);
		strcpy(palette->identifier,src->identifier);
		strcat(palette->identifier,"pal");
		
		/*
		 * Add a pixelmap, make palette
		 */
		BrQuantAddPixelmap(src);
		BrQuantMakePalette(0,256,palette);
		BrQuantPrepareMapping(0,256,palette);

		/*
		 * make new pixelmap from quantized palette, assign new palette
		 */
	    	dst = BrQuantMapPixelmap(0,src);
		dst->map = palette;

		BrQuantEnd();
	}
	else
	{
		/*
		 * convert to any other pixelmap type
		 */
		dst = BrPixelmapAllocate(new_type,src->width,src->height,NULL,0);
	
		/*
		 * copy pixels from one to t'other
		 * munch pixels, spit out in other format (without drooling too much on the data)
		 */
		src_byte = BrPixelmapPixelSize(src) >> 3;
		src_offset = src->base_y*src->row_bytes + src->base_x*src_byte;
		dst_byte = BrPixelmapPixelSize(dst) >> 3;
		dst_offset = dst->base_y*dst->row_bytes + dst->base_x*dst_byte;
		
		for(y=0; y<src->height; y++)
			for(x=0; x<src->width; x++)
				PutPixel(dst,GetPixel(src,x,y,src_offset,src_byte),x,y,dst_offset,dst_byte);
	}

	CopyIdent(dst,src);

	return dst;
}				
 int SortMapTable(br_pixelmap **mtable,int nmaps)
{
    	int i,j;
	br_pixelmap *tmp;
	
	/*
	 * regenerate table of pixelmaps if it contains NULL entries
	 */
	for(i=0; i<nmaps; i++)
	    	if(mtable[i]==NULL)
		    	for(j=i; j<nmaps-1; j++)
			{
			    	tmp=mtable[j];
				mtable[j] = mtable[j+1];
				mtable[j+1] = tmp;
			}

	for(i=0;i<nmaps;i++)
	    	if(mtable[i] == NULL)
		    return i;

	return i;
}
static void CopyIdent(br_pixelmap *dst,br_pixelmap *src)
{
    	/*
	 * make copy of pixelmap->identifier
	 */
	dst->identifier = BrMemStrDup(src->identifier);
}

static int PalettePresent(br_pixelmap *pm)
{
    	int found=0;

	/*
	 * check (BR_PMT_INDEX_8) pixelmap->map is a valid palette (BR_PMT_RGBX_888)
	 */
    	if(pm->type == BR_PMT_INDEX_8)
	{
	    	if(pm->map != NULL)
		{
		    	if((pm->map->type == BR_PMT_RGBX_888) && (pm->map->pixels != NULL))
				found = 1;
		}
	}
	else found = 1;
	
	return found;
}
