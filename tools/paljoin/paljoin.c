#include<brender.h>
#include<dosio.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#define APPLICATION_NAME "PalJoin"
#define MAX_LINE 1024
#define MAX_ENTRIES 256
#define MAXIMAL_ENTRY 255
#define MINIMAL_ENTRY 0
#define DELIMITERS ",\t "

void main(int argc,char *argv[]){
	char inStr[MAX_LINE];
	char *inFileName,*sourceBaseString,*rangeString,*destinationBaseString;
	int sourceBase,range,destinationBase;
	int lineCount=0;
	int y;
	FILE *fileHandle;
	br_pixelmap *destinationMap, *sourceMap;
	br_colour colour;

// Give user options list if inappropriate argc
	if(argc!=3){
		fprintf(stderr,"\nUsage: %s <input-text-file> <output-palette>\n\n",APPLICATION_NAME);
		fprintf(stderr,"This utility is designed to allow you to construct a new palette out of bits\n");
		fprintf(stderr,"of existing palettes.\n\n");
		fprintf(stderr,"Empty lines in the input file, or lines begining with # or ; will be ignored.\n\n");
		fprintf(stderr,"A valid line in the input file will contain the following delimited by space,\n");
		fprintf(stderr,"tab or comma:\n");
		fprintf(stderr,"\t<string> : Palette file name.\n");
		fprintf(stderr,"\t<number> : Source Index-Base - where to start reading colours from.\n");
		fprintf(stderr,"\t<number> : Index-Range - number of colours to copy.\n");
		fprintf(stderr,"\t<number> : Destination Index-Base - where to start writing colours to.\n");
		fprintf(stderr,"\nExample:\n");
		fprintf(stderr,"std.pal 0 64 192\n\n");
		fprintf(stderr," - Copies first 64 colours from std.pal to last 64 colours of output.\n");
		fprintf(stderr,"\nThe output file will be written once every line of the input file has been\n");
		fprintf(stderr,"processed.\n");

		exit(0);
	}

// Get parameter file
	fileHandle=fopen(argv[1],"r");
	if(!fileHandle){
		fprintf(stderr,"Failed to open input file %s.\n",argv[1]);
		exit(0);
	}

	BrBegin();
	destinationMap=BrPixelmapAllocate(BR_PMT_RGBX_888,1,256,NULL,BR_PMAF_NORMAL);

	while(!feof(fileHandle)){
		lineCount++;

// Read in line from file
                inStr[0]='\n'; // make sure it can handle empty lines
		fgets(inStr,MAX_LINE,fileHandle);
// Break it into parameters
		inFileName=strtok(inStr,DELIMITERS);
		sourceBaseString=strtok(NULL,DELIMITERS);
		rangeString=strtok(NULL,DELIMITERS);
		destinationBaseString=strtok(NULL,DELIMITERS);

// Skip comments and blank lines
                if(!((*inFileName=='#')||(*inFileName==';')||(*inFileName=='\n'))){

// Pre-process parameters

// Source file
			sourceMap=BrPixelmapLoad(inFileName);
			if(!sourceMap){
				fprintf(stderr,"Unable to open palette %s for input.",inFileName);
				exit(0);
			}

// SourceIndexBase
			if(isdigit(*sourceBaseString)){
				sourceBase=atoi(sourceBaseString);
			}else{
				fprintf(stderr,"Poorly formed SourceIndexBase on line %d.",lineCount);
				exit(0);
			}
			if(sourceBase<MINIMAL_ENTRY||sourceBase>MAXIMAL_ENTRY){
				fprintf(stderr,"SourceIndexBase should be in the range [0,255] on line %d.",lineCount);
				exit(0);
			}

// IndexRange
			if(isdigit(*rangeString)){
				range=atoi(rangeString);
			}else{
				fprintf(stderr,"Poorly formed 'Range' on line %d.",lineCount);
				exit(0);
			}
			if(range<MINIMAL_ENTRY||sourceBase>MAXIMAL_ENTRY){
				fprintf(stderr,"'Range' should be in the range [1,256] on line %d.",lineCount);
				exit(0);
			}

// DestinationIndexBase
			if(isdigit(*destinationBaseString)){
				destinationBase=atoi(destinationBaseString);
			}else{
				fprintf(stderr,"Poorly formed DestinationIndexBase on line %d.",lineCount);
				exit(0);
			}
			if(destinationBase<MINIMAL_ENTRY||destinationBase>MAXIMAL_ENTRY){
				fprintf(stderr,"DestinationIndexBase should be in the range [0,255] on line %d.",lineCount);
				exit(0);
			}

// Tell the user what is happening
			printf("Processing line %d: %s %d %d %d\n",lineCount,inFileName,sourceBase,range,destinationBase);
			if(sourceBase+range>MAX_ENTRIES)fprintf(stderr,"\tSourceIndexBase+Range>256 will not read past end of palette\n");
			if(destinationBase+range>MAX_ENTRIES)fprintf(stderr,"\tDestinationIndexBase+Range>256 will not write past end of palette\n");

// reset origin

sourceMap->origin_x=0;
sourceMap->origin_y=0;

// Do the real work
			for(y=0;(y<range)&&(y+sourceBase<MAX_ENTRIES);y++){
				colour=BrPixelmapPixelGet(sourceMap,0,y+sourceBase);
				if(destinationBase+y<MAX_ENTRIES)BrPixelmapPixelSet(destinationMap,0,y+destinationBase,colour);
			}
		}
	}

// Write out the results
	if(!BrPixelmapSave(argv[2],destinationMap)){
		fprintf(stderr,"Failed to write output file");
	}
	printf("Successfully created %s.\n",argv[2]);

// Clean up and exit
	BrPixelmapFree(destinationMap);
	BrEnd();
}



