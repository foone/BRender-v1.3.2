#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <process.h>

/*
 * execenv <var> <value-file> <command> {<arguments>}
 */
int main(int argc, char **argv)
{
	char *name;
	char *value;
	FILE *fh;
	int size;
	int i;

	if(argc < 4) {
		fprintf(stderr,"Burble!\n");
		exit(10);
	}
	
	name = argv[1];

	fh = fopen(argv[2],"rb");
	if(fh == NULL) {
		fprintf(stderr,"Could not open '%s' for reading\n",argv[2]);
		exit(20);
	}

	fseek(fh,0,SEEK_END);
	size = ftell(fh);
	fseek(fh,0,SEEK_SET);

	value = calloc(size+1,1);

	if(value == NULL) {
		fprintf(stderr,"Could not allocate %d bytes\n", size);
		exit(30);
	}
	
	fread(value,1,size,fh);
	fclose(fh);

	/*
	 * replace all white space with real space
	 */
	for(i=0; i < size; i++)
		if(isspace(value[i]))
			value[i] = ' ';

	/*
	 * Trim any white space of end of value
	 */
	while((size >= 1) && isspace(value[size-1])) {
		value[--size] = '\0';
	}

	setenv(name,value);
	
	execvp(argv[3], argv+3);

	return 0;
}
