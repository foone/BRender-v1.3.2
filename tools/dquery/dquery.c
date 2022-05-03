/*
 * Copyright (c) 1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dquery.c 1.1 1997/12/10 16:58:06 jon Exp $
 * $Locker: $
 *
 * Various tests
 */
#include <stdio.h>
#include <stdlib.h>

#include "brddi.h"

void ObjectInfo(br_object *h, br_boolean info);
void ObjectDump(br_object *dev, char * pattern, br_boolean info);

int main(int argc, char **argv)
{
	char *dev_name = "mcga";
	char *dev_args = NULL;
	br_device *device = NULL;
	br_error r;

	if(argc > 1)
		dev_name = argv[1];

	if(argc > 2)
		dev_args = argv[2];

	BrBegin();

	/*
	 * Set up named device
	 */
	if(BrDevFind(&device, dev_name) != BRE_OK) {
		r =BrDevAdd(&device, dev_name, dev_args);
		if(r != BRE_OK)
			BR_ERROR0("Could not load driver");
	}

	/*
	 * Dump the device's information
	 */
	ObjectInfo((br_object *)device, BR_TRUE);

	/*
	 * Enumerate the things that the driver contains...
	 */
	ObjectDump((br_object *)device, NULL, BR_TRUE);

	BrEnd();

	return 0;
}

void BR_CALLBACK Putline(char *s, void *arg)
{
	fputs(s,stdout);
	fputc('\n',stdout);
}

void ObjectInfo(br_object *h, br_boolean info)
{
	br_size_t s;
	void *mem;

	fprintf(stdout, "%08x %-32s %-20s %d\n", h,
		ObjectIdentifier(h),
		BrTokenIdentifier(ObjectType(h)),
		ObjectSpace(h));

	if(!info)
		return;

	ObjectQueryAllSize(h, &s);

	if(s) {
		mem = BrMemAllocate(s, BR_MEMORY_APPLICATION);
		ObjectQueryAll(h, mem, s);
		BrTokenValueDump( (br_token_value *) mem , "        ", Putline, NULL);

		BrMemFree(mem);
	}
}

void ObjectDump(br_object *dev, char * pattern, br_boolean info)
{						 
	br_object **objects;
	br_int_32 max,n;

	if(pattern)
		fprintf(stdout,"ObjectDump \"%s\":\n",pattern);
	else
		fprintf(stdout,"ObjectDump all\n");

	ObjectContainerCount(dev, &max, 0, pattern, NULL);
	objects = BrMemAllocate(max * sizeof(*objects), BR_MEMORY_APPLICATION);
 	ObjectContainerFindMany(dev, objects, max, &n, 0, pattern,  NULL);

	if(n != max)
		fprintf(stdout,"ObjectDump mismatch\n");

	for(n = 0; n < max; n++) {
		fprintf(stdout,"%5d: ",n);
		ObjectInfo(objects[n], info);
	}

	BrMemFree(objects);
}

