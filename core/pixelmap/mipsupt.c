#include <brender.h>
#include <brassert.h>
#include "pmmem.h"

br_size_t BrPixelmapMipMemorySize(int dimension,br_uint_8 type){
	br_size_t mipSize=0,mipLevelSize;
	br_size_t pixelSize;

	switch(type){
	case BR_PMT_RGB_888:
		pixelSize=3;
		break;
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
		pixelSize=2;
		break;
	default:
		pixelSize=1;
	}

	for(mipLevelSize=dimension;mipLevelSize;mipLevelSize>>=1)
		mipSize+=mipLevelSize*mipLevelSize;
	return mipSize*pixelSize;
}


#define SQ(a) ((a)*(a))
br_pixelmap * BR_PUBLIC_ENTRY BrPixelmapMakeMipMap(br_pixelmap *source,br_uint_32 destinationType,br_pixelmap *palette,br_uint_32 base,br_uint_32 range,br_uint_32 quantizationMethod){
	br_pixelmap *destination;
	int dimension;
	br_colour colour;
	void *destPtr;
	double r[256],g[256],b[256],red,green,blue,y,i,q; // r=y g=i b=q for yiq quantization
	unsigned row,col;
	int area;
	unsigned lowest;
	double lowestValue,value;
	int *agregateMap,*agregateSourcePtr,*agregateDestinationPtr;
	unsigned paletteEntry;
	unsigned pixelSize;

	switch(destinationType){
	case BR_PMT_RGB_888:
		pixelSize=3;
		break;
	case BR_PMT_RGB_555:
	case BR_PMT_RGB_565:
		pixelSize=2;
		break;
	default:
		pixelSize=1;
	}

	UASSERT(source);
    UASSERT(destinationType<BR_PMT_MAX);
	UASSERT(quantizationMethod<BR_QUANTIZE_MAX);
    UASSERT(destinationType==BR_PMT_INDEX_8?(int)palette:BR_TRUE);
	UASSERT((base<256)&&(range<=256));

	UASSERT_MESSAGE("This function currently only supports square pixelmaps,with power of 2 dimensions.",
                (source->width==source->height)&&(!(source->width&(source->width-1))));

// Perform Allocations
	
	destPtr=BrResAllocate(NULL,BrPixelmapMipMemorySize(source->width,destinationType),BR_MEMORY_PIXELS);
	destination=BrPixelmapAllocate(destinationType,source->width,source->height,destPtr,BR_PMAF_NORMAL);
	BrResAdd(destination,destPtr);
	agregateMap=BrMemAllocate(SQ(source->width)*3*sizeof(*agregateMap),BR_MEMORY_SCRATCH);
	destination->row_bytes=destination->width*pixelSize;
	destination->mip_offset=source->height;
    destination->origin_x=source->origin_x;
    destination->origin_y=source->origin_y;
    destination->identifier=BrResStrDup(destination,source->identifier);


// Build representation of palette in form appropriate for quantization.

	if(destinationType==BR_PMT_INDEX_8){
		switch(quantizationMethod){

		case BR_QUANTIZE_YIQ:
			for(col=0;col<256;col++){
				colour=BrPixelmapPixelGet(palette,0,col);
				red=BR_RED(colour);
				green=BR_GRN(colour);
				blue=BR_BLU(colour);
				r[col]=0.299*red+0.587*green+0.114*blue;
				g[col]=0.596*red-0.275*green-0.321*blue;
				b[col]=0.212*red-0.523*green+0.311*blue;
			}
			break;

		default: // BR_QUANTTIZE_RGB
			for(col=0;col<256;col++){
				colour=BrPixelmapPixelGet(palette,0,col);
				r[col]=BR_RED(colour);
				g[col]=BR_GRN(colour);
				b[col]=BR_BLU(colour);
			}
		}
	}

// Build first level of agregate map

	agregateDestinationPtr=agregateMap;

	for(row=0;row<source->height;row++){
		for(col=0;col<source->width;col++){
			colour=BrPixelmapPixelGet(source,col,row);
			switch(source->type){

			case BR_PMT_RGB_555:
				red=(colour>>7)&0xff;
				green=(colour>>2)&0xff;
				blue=(colour<<3)&0xff;
				break;
			case BR_PMT_RGB_565:
				red=(colour>>8)&0xff;
				green=(colour>>3)&0xff;
				blue=(colour<<3)&0xff;
				break;
			case BR_PMT_INDEX_8:
				colour=BrPixelmapPixelGet(palette,0,colour);
			default:
				red=BR_RED(colour);
				green=BR_GRN(colour);
				blue=BR_BLU(colour);
			}
			agregateDestinationPtr[0]=red;
			agregateDestinationPtr[1]=green;
			agregateDestinationPtr[2]=blue;
			agregateDestinationPtr+=3;
		}
	}

// Build first mip level (a simple conversion)

	if(destination->type==source->type){
		BrPixelmapCopy(destination,source);
                destPtr=(char*)destPtr+SQ(destination->width)*pixelSize;
	}else{
		agregateSourcePtr=agregateMap;

		for(col=0;col<SQ(destination->width);col++){
			red=agregateSourcePtr[0];
			green=agregateSourcePtr[1];
			blue=agregateSourcePtr[2];

			colour=BR_COLOUR_RGBA(red+0.5,green+0.5,blue+0.5,0);

			switch(destination->type){

			case BR_PMT_RGB_555:
				*((short*)destPtr)=((BR_RED(colour)>>3)<<10)|((BR_GRN(colour)>>3)<<5)|(BR_BLU(colour)>>3);
				break;

			case BR_PMT_RGB_565:
				*((short*)destPtr)=((BR_RED(colour)>>3)<<11)|((BR_GRN(colour)>>2)<<5)|(BR_BLU(colour)>>3);
				break;
			
			case BR_PMT_RGB_888:
				*((br_colour*)destPtr)=colour;
				break;
			default:

				lowest=base;
				lowestValue=1000000000.0; //large value

				switch(quantizationMethod){

				case BR_QUANTIZE_YIQ:
					red=BR_RED(colour);
					green=BR_GRN(colour);
					blue=BR_BLU(colour);

					y=0.299*red+0.587*green+0.114*blue;
					i=0.596*red-0.275*green-0.321*blue;
					q=0.212*red-0.523*green+0.311*blue;

					for(paletteEntry=base;paletteEntry<base+range;paletteEntry++){
						value=SQ(y-r[paletteEntry])+SQ(i-g[paletteEntry])+SQ(q-b[paletteEntry]);
						if(value<lowestValue){
							lowestValue=value;
							lowest=paletteEntry;
						}
					}

					break;
				default:
					red=BR_RED(colour);
					green=BR_GRN(colour);
					blue=BR_BLU(colour);
					for(paletteEntry=base;paletteEntry<base+range;paletteEntry++){
						value=SQ(red-r[paletteEntry])+SQ(green-g[paletteEntry])+SQ(blue-b[paletteEntry]);
						if(value<lowestValue){
							lowestValue=value;
							lowest=paletteEntry;
						}
					}
				}

				*((char*)destPtr)=lowest;

			}

                        destPtr=(char*)destPtr+pixelSize;
			agregateSourcePtr+=3;
		}
	}

// Build subsequent levels

	area=4;

	for(dimension=source->width>>1;dimension;dimension>>=1){
		agregateSourcePtr=agregateDestinationPtr=agregateMap;
		for(row=0;row<dimension;row++,agregateSourcePtr+=3*dimension<<1){	
			for(col=0;col<dimension;col++,agregateSourcePtr+=6,agregateDestinationPtr+=3){
				agregateDestinationPtr[0]=agregateSourcePtr[0]+agregateSourcePtr[3]+agregateSourcePtr[0+3*(dimension<<1)]+agregateSourcePtr[3+3*(dimension<<1)];
				agregateDestinationPtr[1]=agregateSourcePtr[1]+agregateSourcePtr[4]+agregateSourcePtr[1+3*(dimension<<1)]+agregateSourcePtr[4+3*(dimension<<1)];
				agregateDestinationPtr[2]=agregateSourcePtr[2]+agregateSourcePtr[5]+agregateSourcePtr[2+3*(dimension<<1)]+agregateSourcePtr[5+3*(dimension<<1)];
			
				red=agregateDestinationPtr[0];
				green=agregateDestinationPtr[1];
				blue=agregateDestinationPtr[2];

				colour=BR_COLOUR_RGBA(red/area+0.5,green/area+0.5,blue/area+0.5,0);

				switch(destination->type){

				case BR_PMT_RGB_555:
					*((short*)destPtr)=((BR_RED(colour)>>3)<<10)|((BR_GRN(colour)>>3)<<5)|(BR_BLU(colour)>>3);
					break;

				case BR_PMT_RGB_565:
					*((short*)destPtr)=((BR_RED(colour)>>3)<<11)|((BR_GRN(colour)>>2)<<5)|(BR_BLU(colour)>>3);
					break;
				
				case BR_PMT_RGB_888:
					*((br_colour*)destPtr)=colour;
					break;
				default:

					lowest=base;
					lowestValue=1000000000.0; //large value

					switch(quantizationMethod){

					case BR_QUANTIZE_YIQ:
						red=BR_RED(colour);
						green=BR_GRN(colour);
						blue=BR_BLU(colour);

						y=0.299*red+0.587*green+0.114*blue;
						i=0.596*red-0.275*green-0.321*blue;
						q=0.212*red-0.523*green+0.311*blue;

						for(paletteEntry=base;paletteEntry<base+range;paletteEntry++){
							value=SQ(y-r[paletteEntry])+SQ(i-g[paletteEntry])+SQ(q-b[paletteEntry]);
							if(value<lowestValue){
								lowestValue=value;
								lowest=paletteEntry;
							}
						}

						break;
					default:
						red=BR_RED(colour);
						green=BR_GRN(colour);
						blue=BR_BLU(colour);
						for(paletteEntry=base;paletteEntry<base+range;paletteEntry++){
							value=SQ(red-r[paletteEntry])+SQ(green-g[paletteEntry])+SQ(blue-b[paletteEntry]);
							if(value<lowestValue){
								lowestValue=value;
								lowest=paletteEntry;
							}
						}
					}
					*((char*)destPtr)=lowest;
				}
		        destPtr=(char*)destPtr+pixelSize;
			}
		}
		area*=4;
	}
	BrMemFree(agregateMap);	
	return destination;
}
