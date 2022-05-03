/*
 * Build a string describing the current call trace
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "calltrak.h"

static int FunctionName(void *addr, char *buffer, int limit)
{
	char *cp = addr;
	int l,i;
	char tmp_str[] = "0x12345678";

	/*
	 * Does the address have an ID prefixing it?
	 */
	cp --;
	l = *cp;	/* Length Byte */
	cp -= l;

	/*
	 * If length is less than 32 and every 
	 * character of string if printable, then
	 * consider it a valid ID
	 */
	if(l > 32)
		cp = NULL;
	else {
		for(i=0; i < l; i++) {
			if(!isprint(*cp)) {
				cp = NULL;
				break;
			}
		}
	}

	/*
	 * Generate an address if no ID
	 */
	if(!cp) {
		l = sprintf(tmp_str,"0x%08x",addr);
		cp = tmp_str;
	}

	/*
	 * Make sure buffer is not overun
	 */
	if(l > limit)
		l = limit;

	/*
	 * Transfer to destination and return size
	 */
	memcpy(buffer,cp,l);
	return l;
}

int CallTrace(void *stack[], int depth, char *buffer, int limit)
{
	int i,n,total=0;

	/*
	 * Make space for NUL at end
	 */
	limit--;

	for(i=0; i < depth; i++) {
		n = FunctionName(stack[i],buffer,limit-total);
		buffer += n;
		total += n;

		if(total >= (limit-1))
			break;

		if(i < depth) {
			*buffer++ = '.';
			total++;
		}
	}

	*buffer = '\0';

	return total;
}

