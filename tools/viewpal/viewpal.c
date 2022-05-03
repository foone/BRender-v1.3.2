#include "brender.h" 
#include<stdio.h>
#include<conio.h>
#include "dosio.h"
#include<stdlib.h>

void main(int argc,char *argv[]){
	br_pixelmap *palette;
	br_pixelmap *onScreenBuffer,*offScreenBuffer;
	int x,sum,maxcol=0,mincol=765,textcol,min_index,max_index;
	char *number="       ";
	char *Palcolour,red,blue,green;

	if(argc!=2){
		fprintf(stderr,"Usage: viewpal <palette>",argv[0]);
		exit(0);
	}

	BrBegin();

	onScreenBuffer=DOSGfxBegin("MCGA");
	offScreenBuffer=BrPixelmapMatch(onScreenBuffer,BR_PMMATCH_OFFSCREEN);

	palette=BrPixelmapLoad(argv[1]);
	if(!palette){
		fprintf(stderr,"Unable to load palette.");
		exit(0);
	}
	DOSGfxPaletteSet(palette);

	/*scan for brightest and darkest colour*/
	
	Palcolour = palette->pixels;
	for(x=0;x<256;x++)
	{
		blue = Palcolour[x<<2];
		green = Palcolour[(x<<2)+1];
		red = Palcolour[(x<<2)+2];
		sum = (unsigned)blue + (unsigned)red + (unsigned)green;
		if (sum < mincol) 
		{
			min_index = x;
			mincol = sum;
		}
		if (sum > maxcol) 
		{
			maxcol = sum;
			max_index = x;
		}
	}

	for(x=0;x<256;x++)
	{
		BrPixelmapRectangleFill(offScreenBuffer,(x%16)*20,(x/16)*12,20,12,x);
		blue = Palcolour[x<<2];
		green = Palcolour[(x<<2)+1];
		red = Palcolour[(x<<2)+2];
		sum = (int)blue + (int)red + (int)green;
		if (sum < 383 ) textcol = max_index ;
		else  textcol = min_index ;
//        BrPixelmapText(offScreenBuffer,(x%16)*20,(x/16)*12,255-x,BrFontFixed3x5,itoa(x,number,10));
		BrPixelmapText(offScreenBuffer,(x%16)*20,(x/16)*12,textcol,BrFontFixed3x5,itoa(x,number,10));
	}

	BrPixelmapDoubleBuffer(onScreenBuffer,offScreenBuffer);
	while(!kbhit()){}
        getch();
	DOSGfxEnd();
	
/*    fprintf(stderr,"min_index=%i  max_index=%i\n",min_index,max_index);
	fprintf(stderr,"red=%i  blue=%i  green=%i\n",red,blue,green);
	fprintf(stderr,"sum=%i\n",sum);
	getch();
	while(!kbhit()){}
*/

	BrEnd();
}

