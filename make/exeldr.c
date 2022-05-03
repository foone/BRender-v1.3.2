#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READSIZE 8*1024

typedef char BYTE;
typedef unsigned char UBYTE;

typedef short WORD;
typedef unsigned short UWORD;

typedef long DWORD;
typedef unsigned long UDWORD;

typedef void *PTR;
typedef void *NPTR;
typedef void far * FPTR;

struct ExeHeader {
	UWORD	signature;
	UWORD last_page_size;
	UWORD file_pages;
	UWORD reloc_items;
	UWORD header_paras;
	UWORD min_alloc;
	UWORD max_alloc;
	UWORD ss;
	UWORD sp;
	UWORD checksum;
	UWORD ip;
	UWORD cs;
	UWORD reloc_offset;
	UWORD overlay_num;
};

struct Reloc {
	UWORD offset;
	UWORD segment;
};

int Verbose = 0;

int main(int argc, char**argv)
{
	UBYTE *filedata;
	FILE *fh;
	size_t filesize;
	struct ExeHeader *ehp;
	UBYTE *image_start;
	UDWORD image_size;
	int i,j;
	UBYTE *read_ptr;
	struct Reloc *reloc_ptr;
	UWORD *wp;
	char *cp;
	/*
	 * Options controlled by command line
	 */
	UWORD reloc_base = 0;
	char *input_name = NULL;
	char *output_name = "exeldr.out";
	int max_relocs = -1;
	int keep_relocs = 0;
	
	while(argv++,--argc) {
		if(**argv == '-') {
			/*
			 * Got one or more flags
			 */
			for(cp=*argv+1; *cp; cp++) {

				if(strchr("rob",*cp) && (--argc == 0)) {
					fprintf(stderr,"The %c option requires an argument\n",*cp);
					exit(10);
				}

				switch(*cp) {
				case 'a':
					/*
					 * Append relocation info
					 * -a
					 */
					keep_relocs = 1;
					break;

				case 'o':
					/*
					 * Set output filename
					 * -o <output-name>
					 */
					output_name = *(++argv);
					break;

				case 'v':
					Verbose = !Verbose;
					break;

				case 'r':
					/*
					 * -r <maximum-relocation>
					 */
					max_relocs = strtol(*(++argv),0,0);
					break;

				case 'b':
					/*
					 * -b <relocation-base>
					 */
					reloc_base = strtol(*(++argv),0,0);
					break;

				default:
					fprintf(stderr,"unknown option '%c'\n",*cp);
					exit(10);
				}
			}
		} else {
			if(input_name == NULL)
				input_name = *argv;
			else {
				fprintf(stderr,"input name specified twice\n");
				exit(10);
			}
		}
	}

	if(input_name == NULL) {
		fprintf(stderr,"No input filename\n");
		exit(10);
	}

	fh=fopen(input_name,"rb");	
	
	if(fh == NULL) {
		fprintf(stderr,"Could not open \"%s\" for input\n",argv[1]);
		exit(10);
	}

	/*
	 * Work out size of file
	 */
	fseek(fh,0L,SEEK_END);
	filesize = ftell(fh);
	fseek(fh,0L,SEEK_SET);

	if(Verbose)
		printf("EXE size: %ld\n",filesize);

	/*
	 * Allocate memory for file
	 */
	filedata = malloc(filesize);

	if(filedata == NULL) {
		fprintf(stderr,"Could not allocate memory for .EXE file\n");
		exit(10);
	}

	/*
	 * Read file into buffer
	 */
	read_ptr = filedata;
	j=0;
	do {
		i = (filesize > READSIZE)?READSIZE:filesize;
		j += fread(read_ptr,1,i,fh);
		read_ptr += i;
		filesize -= i;
	} while (filesize > 0);

	if(Verbose)
		printf("Read %d bytes\n",j);

	fclose(fh);

	/*
	 * print out some stats from .EXE header
	 */
	ehp = (struct ExeHeader *)filedata;

	if(Verbose) {
		printf("Signature: %04X  Header Paragraphs: %d\n",
			ehp->signature,ehp->header_paras);

		printf("Last Page Size:%04x    File Pages: %04x\n",
			ehp->last_page_size,ehp->file_pages);

		printf("Relocation offset: %04x   Relocation items: %d\n",
			ehp->reloc_offset,ehp->reloc_items);

		printf("MINALLOC: %04x MAXALLOC: %04x\n",
			ehp->min_alloc,ehp->max_alloc);
	}

	/*
	 * Check the number of relocations allowed
	 */
	if(max_relocs >= 0 && ehp->reloc_items > max_relocs) {
		fprintf(stderr,"Number of relocation items (%d) is greater than %d\n",
			ehp->reloc_items,max_relocs);
	}

	image_start = filedata + ehp->header_paras * 16L;

	if(ehp->last_page_size)
		image_size = ((ehp->file_pages-1)* 512L + ehp->last_page_size)
			- ehp->header_paras * 16L;
	else
		image_size = ((ehp->file_pages)* 512L)	- ehp->header_paras * 16L;
		
	image_size += ehp->min_alloc * 16L;

	if(Verbose)
		printf("Image size = %ld\n",image_size);

	/*
	 * Perform any relocation
	 */
	if(Verbose || reloc_base != 0) {
		reloc_ptr = (struct Reloc *)(filedata + ehp->reloc_offset);
		for(i=0; i < ehp->reloc_items; i++, reloc_ptr++) {
			if(Verbose)
				printf("RELOC: %04x:%04x\n",reloc_ptr->segment,reloc_ptr->offset);
			wp = (UWORD *)(filedata+reloc_ptr->segment*16L+reloc_ptr->offset);
			*wp += reloc_base;
		}
	}

	if(keep_relocs) {
		/*
		 *	If relocations are to be kept, patch the count and offset
		 * into the first three words of the image
		 */
		*((UWORD *)image_start) = ehp->reloc_items;
		*((UDWORD *)(image_start+2)) = image_size;
	}

	/*
	 * Write out the image file
	 */
	fh = fopen(output_name,"wb");
	if(fh == NULL) {
		fprintf(stderr,"Could not open \"%s\" for output\n",output_name);
		exit(10);
	}

	fwrite(image_start,1,image_size,fh);

	if(keep_relocs) {
		/*
		 * Add relocations to end of image
		 */
		reloc_ptr = (struct Reloc *)(filedata + ehp->reloc_offset);
		for(i=0; i < ehp->reloc_items; i++, reloc_ptr++)
			fwrite(reloc_ptr,sizeof(struct Reloc),1,fh);
	}
	fclose(fh);

	/*
	 * Free file memory
	 */
	free(filedata);

	return 0;
}

