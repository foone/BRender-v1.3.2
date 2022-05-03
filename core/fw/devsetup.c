/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devsetup.c 1.2 1997/12/11 13:33:10 jon Exp $
 * $Locker: $
 *
 * XXX should condense all the BrXXXFind() functions into one,
 * since the logic is the same for each!
 */
#include <stdio.h>
#include <stdarg.h>

#include "fw.h"
#include "host.h"
#include "brassert.h"

BR_RCS_ID("$Id: devsetup.c 1.2 1997/12/11 13:33:10 jon Exp $")

/*
 * XXX A very nasty hack to get around limitiations of v1.1.x
 *
 * BrZbBegin() does not get access to the destination screen!
 */
static br_pixelmap *last_begin_screen;

br_pixelmap * BrDevLastBeginQuery(void)
{
	return last_begin_screen;
}

void BrDevLastBeginSet(br_pixelmap *pm)
{
	last_begin_screen = pm;
}

/*
 * Produce a new output screen pixelmap from a list of token value pairs
 */
br_error BR_PUBLIC_ENTRY BrDevBeginVar(br_pixelmap **ppm, char *setup_string, ...)
{
	va_list vl;
	br_uint_32 i, n;
	br_token_value tv[64];

	/*
	 * Convert arguments to token/value pairs
	 */
	va_start(vl, setup_string);

	for(i = 0, n = BR_NULL_TOKEN;; i++) {

		if (i == BR_ASIZE(tv))
			BR_FAILURE("Too many token value pairs");

		if ((n = va_arg(vl, int)) == BR_NULL_TOKEN)
			break;

		*((int *)&tv[i].t) = n;

		n = va_arg(vl, int);
		*((int *)&tv[i].v.u32) = n;
	}

	tv[i].t = BR_NULL_TOKEN;
	tv[i].v.u32 = 0;

	va_end(vl);

	return BrDevBeginTV(ppm, setup_string, tv);
}

/*
 * Takes a setup string and produces a new output screen pixelmap 
 */
br_error BR_PUBLIC_ENTRY BrDevBegin(br_pixelmap **ppm, char *setup_string)
{
	return BrDevBeginTV(ppm, setup_string, NULL);
}

/*
 * Takes a setup string and NULL terminated list of setup token-value pairs
 * and produces a new output screen pixelmap 
 */
br_error BR_PUBLIC_ENTRY BrDevBeginTV(br_pixelmap **ppm, char *setup_string, br_token_value *setup_tv)
{
	char str[512], *args = NULL, *devname, *devargs;
	char devices_str[512], *device_name, *device_args, *next_device;
	br_token_value args_tv[64], *tv = NULL;
	br_output_facility *output_facility;
	br_device *device;
	br_pixelmap *screen;
	int i, n;
	br_error r;

	/*
	 * Find which argument string to use
	 */
	if(setup_string == NULL) {
		BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, str, BR_ASIZE(str));
	} else {
		BrStrNCpy(str, setup_string, BR_ASIZE(str));
		str[BR_ASIZE(str)-1] = '\0';
	}

	/*
	 * Parse argument string into device name and arguments string
	 */
	for(args = str; *args != '\0' && *args != ','; args++)
		;

	if(*args != '\0') {
		*args = '\0';
		args++;
	}

	/*
	 * Trim white space from name
	 */
	devname = str;

	while(BrIsSpace(*devname))
		devname++;

	n = BrStrLen(devname);
			
	if(n) {
		while(BrIsSpace(devname[n-1])) {
			devname[n-1] = '\0';
			n--;
		}
	}

	/*
	 * If device name is empty, search all loaded devices
	 */
	if(n == 0 || (n == 1 && devname[0] == '*')) {
		/*
		 * convert arguments to token/value pairs
		 */
		if(*args && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == BRE_OK)
			tv = args_tv;

		/*
	 	 * Merge in additional token/value pairs
	 	 *
	 	 * N.B. Existing arguments are assumed to be in args_tv
	 	 */
		if (setup_tv != NULL) {

			if (tv == NULL)

				tv = setup_tv;

			else {

				for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {

					for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++)
						if (args_tv[i].t == tv->t)
							break;

					if (args_tv[i].t == BR_NULL_TOKEN) {

						if (i+1 == BR_ASIZE(args_tv))
							BR_FAILURE("Too many token value pairs");

						args_tv[i+1].t = BR_NULL_TOKEN;
						args_tv[i+1].v.u32 = 0;
					}

					args_tv[i] = *tv;
				}

				tv = args_tv;
			}
		}

		/*
		 * Find output type in device
		 */
		r = BrDevContainedFind((br_object **)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
		if(r != BRE_OK)
			return r;
	} else {

		/*
		 * Try to find named device, if not there, attempt to add it
		 */
		if(BrDevFind(&device, devname) != BRE_OK) {

			devargs = NULL;

			/*
			 * Look for arguments in the device configuration string
			 */
			if (BrSystemConfigQueryString(BRT_BRENDER_DEVICES_STR, devices_str, BR_ASIZE(devices_str)) == BRE_OK) {

				for (device_name = devices_str; *device_name != '\0'; device_name = next_device) {

					/*
					 * Parse devices string into device name, arguments
					 * string and next entry
					 */
					for (device_args = device_name; *device_args != '\0' && *device_args != ',' && *device_args != ';'; device_args++)
						;

					if (*device_args == ',') {
						*device_args = '\0';
						device_args++;
					}

					for (next_device = device_args; *next_device != '\0' && *next_device != ';'; next_device++)
						;

					if (*next_device != '\0') {
						*next_device = '\0';
						next_device++;
					}

					/*
					 * Trim white space from name
					 */
					while (BrIsSpace(*device_name))
						device_name++;

					n = BrStrLen(device_name);
			
					if (n) {
						while (BrIsSpace(device_name[n-1])) {
							device_name[n-1] = '\0';
							n--;
						}
					}

					/*
					 * Look for the relevant entry
					 */
					if (BrStrICmp(devname, device_name) == 0) {
						devargs = device_args;
						break;
					}
				}
			}

			r = BrDevAdd(&device, devname, devargs);
			if(r != BRE_OK)
				return r;
		}

		/*
		 * convert arguments to token/value pairs
		 */
		if(*args && BrStringToTokenValue(args_tv, sizeof(args_tv), args) == BRE_OK)
			tv = args_tv;

		/*
	 	 * Merge in additional token/value pairs
	 	 *
	 	 * N.B. Existing arguments are assumed to be in args_tv
	 	 */
		if (setup_tv != NULL) {

			if (tv == NULL)

				tv = setup_tv;

			else {

				for (tv = setup_tv; tv->t != BR_NULL_TOKEN; tv++) {

					for (i = 0; args_tv[i].t != BR_NULL_TOKEN; i++)
						if (args_tv[i].t == tv->t)
							break;

					if (args_tv[i].t == BR_NULL_TOKEN) {

						if (i+1 == BR_ASIZE(args_tv))
							BR_FAILURE("Too many token value pairs");

						args_tv[i+1].t = BR_NULL_TOKEN;
						args_tv[i+1].v.u32 = 0;
					}

					args_tv[i] = *tv;
				}

				tv = args_tv;
			}
		}

		/*
		 * Find output type in device
		 */
		r = ObjectContainerFind(device, (br_object **)&output_facility, BRT_OUTPUT_FACILITY, NULL, tv);
		if(r != BRE_OK)
			return r;
	}

	/*
	 * Create screen pixelmap from output_facility
	 */
	r = OutputFacilityPixelmapNew(output_facility, (br_device_pixelmap **)&screen, tv);
	if(r != BRE_OK)
		return r;

	/*
	 * Horrible hack to let some V1.1 compatibility functions know what the
	 * last created screen was (a likely candidate for renderer destinations
	 * and palette ops.)
	 */

	BrDevLastBeginSet(screen);

	*ppm = screen;

	return BRE_OK;
}

/*
 * Functions for backwards comatibility
 */
br_pixelmap * BR_PUBLIC_ENTRY BrDevBeginOld(char *setup_string)
{
	br_pixelmap *s;

	if(BrDevLastBeginQuery())
		BR_ERROR0("Device already active");

	if(BrDevBegin(&s, setup_string) != BRE_OK)
		BR_ERROR0("Could not start driver");

	return s;
}

void BR_PUBLIC_ENTRY BrDevEndOld(void)
{
	if(BrDevLastBeginQuery())
		BrPixelmapFree(BrDevLastBeginQuery());

	BrDevLastBeginSet(NULL);
}

void BR_PUBLIC_ENTRY BrDevPaletteSetOld(br_pixelmap *pm)
{
	if(BrDevLastBeginQuery())
		BrPixelmapPaletteSet(BrDevLastBeginQuery(), pm);
}

void BR_PUBLIC_ENTRY BrDevPaletteSetEntryOld(int i,br_colour colour)
{
	if(BrDevLastBeginQuery())
		BrPixelmapPaletteEntrySet(BrDevLastBeginQuery(), i, colour);
}

/*
 * Find a renderer from a destination pixelmap according to standard rules
 */
br_error BR_RESIDENT_ENTRY BrRendererFacilityFind(br_renderer_facility **prf,
				br_device_pixelmap *destination, br_token scalar_type)
{
	br_renderer_facility *renderer_facility = NULL;
	br_error r;
	br_output_facility *ot;
	char object_name[] = "Default-Renderer-00000";
	char image_name[] = "softrnd0";
	br_boolean scalar_is_valid = BR_FALSE;

	/*
	 *	Construct names based on prefered scalar type
	 */
	switch(scalar_type) {
	case BRT_FLOAT:
		BrStrCpy(object_name+sizeof(object_name)-6,"Float");
		image_name[sizeof(image_name)-2] = 'f';
		scalar_is_valid = BR_TRUE;
		break;
	case BRT_FIXED:
		BrStrCpy(object_name+sizeof(object_name)-6,"Fixed");
		image_name[sizeof(image_name)-2] = 'x';
		scalar_is_valid = BR_TRUE;
		break;
	}

	/*
	 * Fire up a renderer
	 *
	 *	1) Query destination for renderer_facility
	 *	2) Query destination's output_facility for renderer
	 *	3) Try to find "Default-Renderer-XXX" in all devices
	 *	4) Add "softrndX" and try to find "Default-Renderer-XXX"
	 *	5) Try to find "Default-Renderer" in all devices
	 *	6) Add "softrend" and try to find "Default-Renderer"
	 *
	 *  (XXX is Fixed or Float) (X is x or f)
 	 */

	if(!renderer_facility && destination)
		ObjectQuery(destination, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_O);

	if(!renderer_facility && destination) {
		ot = NULL;
		r = ObjectQuery(destination, (br_uint_32 *)&ot, BRT_OUTPUT_FACILITY_O);

		if(r == BRE_OK && ot)
			ObjectQuery(ot, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_O);
	}

	if(scalar_is_valid) {
		if(!renderer_facility)
			BrDevContainedFind((br_object **)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
	}

	if(!renderer_facility)
		BrDevContainedFind((br_object **)&renderer_facility, BRT_RENDERER_FACILITY,"Default-Renderer",NULL);

	if(scalar_is_valid) {
		if(!renderer_facility) {
			BrDevCheckAdd(NULL,image_name,NULL);
			BrDevContainedFind((br_object **)&renderer_facility, BRT_RENDERER_FACILITY, object_name, NULL);
		}
	}

	if(!renderer_facility) {
		BrDevCheckAdd(NULL,"softrend",NULL);
		BrDevContainedFind((br_object **)&renderer_facility, BRT_RENDERER_FACILITY,"Default-Renderer",NULL);
	}

	if(!renderer_facility)
		return BRE_FAIL;

	*prf = renderer_facility;

	return BRE_OK;
}

/*
 * Find a list of available renderers from a destination pixelmap according to standard rules
 *
 * If the array was not large enough BRE_OVERFLOW is returned, the array is
 * filled with the as many facilities as possible and the count is the number
 * of renderer facilities that were found.
 */
br_error BR_RESIDENT_ENTRY BrRendererFacilityListFind(br_renderer_facility **prf,
				br_int_32 *num_rf, br_int_32 max_rf,
				br_device_pixelmap *destination, br_token scalar_type)
{
	br_renderer_facility **renderer_facility = NULL;
	br_int_32 renderer_facility_size = 0, renderer_facility_count, i, j;
	br_error r;
	br_output_facility *ot;
	char object_pattern[] = "*-Renderer-00000";
	br_boolean scalar_is_valid = BR_FALSE;
	br_token_value is_alternative[] = {
		{ BRT_ALTERNATIVE_TO_DEFAULT_B, BR_TRUE },
		{ BR_NULL_TOKEN }
	};
	char *identifier;
	br_size_t identifier_len;

	UASSERT(prf != NULL);
	UASSERT(max_rf >= 1);

	/*
	 *	Construct names based on prefered scalar type
	 */
	switch(scalar_type) {

	case BRT_FLOAT:

		BrStrCpy(object_pattern+sizeof(object_pattern)-6,"Float");
		scalar_is_valid = BR_TRUE;

		break;

	case BRT_FIXED:

		BrStrCpy(object_pattern+sizeof(object_pattern)-6,"Fixed");
		scalar_is_valid = BR_TRUE;

		break;
	}

	/*
	 * Find a list of renderers
	 *
	 *	1) Query destination for renderer_facility list
	 *	2) Query destination's output_facility for renderer_facility list
	 *  3) Find default renderer with BrRendererFacilityFind and
	 *     all other renderer facilities for which ALTERNATIVE_TO_DEFAULT
	 *     is TRUE (and eliminating the non-scalar-specific versions when
	 *     a scalar specific version exists with the same base)
	 */
	if(!renderer_facility && destination)
		ObjectQuery(destination, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_OL);

	if(!renderer_facility && destination) {
		ot = NULL;
		r = ObjectQuery(destination, (br_uint_32 *)&ot, BRT_OUTPUT_FACILITY_O);

		if(r == BRE_OK && ot)
			ObjectQuery(ot, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_OL);
	}

	/*
	 * Copy the list if it was found
	 */
	if (renderer_facility) {

		for (*num_rf = 0; renderer_facility[*num_rf] != NULL; num_rf++);

		if (*num_rf > max_rf) {

			BrMemCpy(prf, renderer_facility, max_rf * sizeof(*renderer_facility));
			return BRE_OVERFLOW;

		} else {

			BrMemCpy(prf, renderer_facility, *num_rf * sizeof(*renderer_facility));
			return BRE_OK;
		}
	}

	/*
	 * Build up a list otherwise
	 */
	*num_rf = 0;

	/*
	 * First get the default renderer facility
	 */
	if (BrRendererFacilityFind(prf, destination, scalar_type) == BRE_OK)
		(*num_rf)++;

	/*
	 * Now search for alternatives
	 */
	if (scalar_is_valid) {

		/*
		 * Search for all scalar-specific alternatives
		 */
		r = BrDevContainedFindMany((br_object **)&prf[*num_rf], max_rf - *num_rf,
			&renderer_facility_count, BRT_RENDERER_FACILITY,
			object_pattern, is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW)
			return r;

		*num_rf += renderer_facility_count;

		/*
		 * Now search for non-scalar-specific alternatives
		 */
		r = BrDevContainedCount(&renderer_facility_size, BRT_RENDERER_FACILITY,
			"*-Renderer", is_alternative);

		if (r != BRE_OK)
			return r;

		renderer_facility = BrResAllocate(NULL, renderer_facility_size * sizeof(*renderer_facility), BR_MEMORY_SCRATCH);

		if (renderer_facility == NULL)
			return BRE_NO_MEMORY;

		r = BrDevContainedFindMany((br_object **)renderer_facility, renderer_facility_size,
			&renderer_facility_count, BRT_RENDERER_FACILITY,
			"*-Renderer", is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW) {
			BrResFree(renderer_facility);
			return r;
		}

		if (renderer_facility_count != renderer_facility_size) {
			BrResFree(renderer_facility);
			return BRE_FAIL;
		}

		/*
		 * Weed out any for which there is an equivalent scalar-specific version
		 */
		for (i = 0; i < renderer_facility_count; i++) {

			identifier = ObjectIdentifier(renderer_facility[i]);
			identifier_len = (identifier != NULL)? BrStrLen(identifier): 0;

			for (j = 0; j < *num_rf; j++)
				if (BrStrNICmp(identifier, ObjectIdentifier(renderer_facility[j]), identifier_len) == 0)
					break;

			if (j != *num_rf)
				continue;

			if (*num_rf < max_rf)
				prf[*num_rf] = renderer_facility[i];

			(*num_rf)++;
		}

		BrResFree(renderer_facility);

	} else {

		/*
		 * Search for all non-scalar-specific alternatives
		 */
		r = BrDevContainedFindMany((br_object **)&prf[*num_rf], max_rf - *num_rf,
			&renderer_facility_count, BRT_RENDERER_FACILITY,
			"*-Renderer", is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW)
			return r;

		*num_rf += renderer_facility_count;
	}

	if (*num_rf > max_rf)
		return BRE_OVERFLOW;

	return BRE_OK;
}


/*
 * Find a primtive library from a destination pixelmap according to standard rules
 */
br_error BR_RESIDENT_ENTRY BrPrimitiveLibraryFind(br_primitive_library **ppl,
				br_device_pixelmap *destination, br_token scalar_type)
{
	br_primitive_library *primitive_library = NULL;
	br_error r;
	br_output_facility *ot;
	char object_name[] = "Default-Primitives-00000";
	char image_name[] = "softprm0";
	br_boolean scalar_is_valid = BR_FALSE;

	/*
	 *	Construct names based on prefered scalar type
	 */
	switch(scalar_type) {
	case BRT_FLOAT:
		BrStrCpy(object_name+sizeof(object_name)-6,"Float");
		image_name[sizeof(image_name)-2] = 'f';
		scalar_is_valid = BR_TRUE;
		break;
	case BRT_FIXED:
		BrStrCpy(object_name+sizeof(object_name)-6,"Fixed");
		image_name[sizeof(image_name)-2] = 'x';
		scalar_is_valid = BR_TRUE;
		break;
	}

	/*
	 * Fire up a renderer
	 *
	 *	1) Query destination for primitive_library
	 *	2) Query destination's output_facility for renderer
	 *	3) Try to find "Default-Primitives-XXX" in all devices
	 *	4) Add "softrndX" and try to find "Default-Renderer-XXX"
	 *	5) Try to find "Default-Primitives" in all devices
	 *	6) Add "softrend" and try to find "Default-Primitives"
	 *
	 *  (XXX is Fixed or Float) (X is x or f)
 	 */

	if(!primitive_library && destination)
		ObjectQuery(destination, (br_uint_32 *)&primitive_library, BRT_PRIMITIVE_LIBRARY_O);

	if(!primitive_library && destination) {
		ot = NULL;
		r = ObjectQuery(destination, (br_uint_32 *)&ot, BRT_OUTPUT_FACILITY_O);

		if(r == BRE_OK && ot)
			ObjectQuery(ot, (br_uint_32 *)&primitive_library, BRT_PRIMITIVE_LIBRARY_O);
	}

	if(scalar_is_valid) {
		if(!primitive_library)
			BrDevContainedFind((br_object **)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
	}

	if(!primitive_library)
		BrDevContainedFind((br_object **)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);

	if(scalar_is_valid) {
		if(!primitive_library) {
			BrDevCheckAdd(NULL, image_name, NULL);
			BrDevContainedFind((br_object **)&primitive_library, BRT_PRIMITIVE_LIBRARY, object_name, NULL);
		}
	}

	if(!primitive_library) {
		BrDevCheckAdd(NULL,"softprim",NULL);
		BrDevContainedFind((br_object **)&primitive_library, BRT_PRIMITIVE_LIBRARY, "Default-Primitives", NULL);
	}

	if(!primitive_library)
		return BRE_FAIL;

	*ppl = primitive_library;

	return BRE_OK;
}

/*
 * Find a list of available primitive libraries from a destination pixelmap according to standard rules
 *
 * If the array was not large enough BRE_OVERFLOW is returned, the array is
 * filled with the as many libraries as possible and the count is the number
 * of primitive libraries that were found.
 */
br_error BR_RESIDENT_ENTRY BrPrimitiveLibraryListFind(br_primitive_library **ppl,
				br_int_32 *num_pl, br_int_32 max_pl,
				br_device_pixelmap *destination, br_token scalar_type)
{
	br_primitive_library **primitive_library = NULL;
	br_int_32 primitive_library_size = 0, primitive_library_count, i, j;
	br_error r;
	br_output_facility *ot;
	char object_pattern[] = "*-Primitives-00000";
	br_boolean scalar_is_valid = BR_FALSE;
	br_token_value is_alternative[] = {
		{ BRT_ALTERNATIVE_TO_DEFAULT_B, BR_TRUE },
		{ BR_NULL_TOKEN }
	};
	char *identifier;
	br_size_t identifier_len;

	UASSERT(ppl != NULL);
	UASSERT(max_pl >= 1);

	/*
	 *	Construct names based on prefered scalar type
	 */
	switch(scalar_type) {

	case BRT_FLOAT:

		BrStrCpy(object_pattern+sizeof(object_pattern)-6,"Float");
		scalar_is_valid = BR_TRUE;

		break;

	case BRT_FIXED:

		BrStrCpy(object_pattern+sizeof(object_pattern)-6,"Fixed");
		scalar_is_valid = BR_TRUE;

		break;
	}

	/*
	 * Find a list of renderers
	 *
	 *	1) Query destination for primitive_library list
	 *	2) Query destination's output_facility for primitive_library list
	 *  3) Find default primitive library with BrPrimitiveLibraryFind and
	 *     all other renderer facilities for which ALTERNATIVE_TO_DEFAULT
	 *     is TRUE (and eliminating the non-scalar-specific versions when
	 *     a scalar specific version exists with the same base)
	 */
	if(!primitive_library && destination)
		ObjectQuery(destination, (br_uint_32 *)&primitive_library, BRT_PRIMITIVE_LIBRARY_OL);

	if(!primitive_library && destination) {
		ot = NULL;
		r = ObjectQuery(destination, (br_uint_32 *)&ot, BRT_OUTPUT_FACILITY_O);

		if(r == BRE_OK && ot)
			ObjectQuery(ot, (br_uint_32 *)&primitive_library, BRT_PRIMITIVE_LIBRARY_OL);
	}

	/*
	 * Copy the list if it was found
	 */
	if (primitive_library) {

		for (*num_pl = 0; primitive_library[*num_pl] != NULL; num_pl++);

		if (*num_pl > max_pl) {

			BrMemCpy(ppl, primitive_library, max_pl * sizeof(*primitive_library));
			return BRE_OVERFLOW;

		} else {

			BrMemCpy(ppl, primitive_library, *num_pl * sizeof(*primitive_library));
			return BRE_OK;
		}
	}

	/*
	 * Build up a list otherwise
	 */
	*num_pl = 0;

	/*
	 * First get the default renderer facility
	 */
	if (BrPrimitiveLibraryFind(ppl, destination, scalar_type) == BRE_OK)
		(*num_pl)++;

	/*
	 * Now search for alternatives
	 */
	if (scalar_is_valid) {

		/*
		 * Search for all scalar-specific alternatives
		 */
		r = BrDevContainedFindMany((br_object **)&ppl[*num_pl], max_pl - *num_pl,
			&primitive_library_count, BRT_PRIMITIVE_LIBRARY,
			object_pattern, is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW)
			return r;

		*num_pl += primitive_library_count;

		/*
		 * Now search for non-scalar-specific alternatives
		 */
		r = BrDevContainedCount(&primitive_library_size, BRT_PRIMITIVE_LIBRARY,
			"*-Primitives", is_alternative);

		if (r != BRE_OK)
			return r;

		primitive_library = BrResAllocate(NULL, primitive_library_size * sizeof(*primitive_library), BR_MEMORY_SCRATCH);

		if (primitive_library == NULL)
			return BRE_NO_MEMORY;

		r = BrDevContainedFindMany((br_object **)primitive_library, primitive_library_size,
			&primitive_library_count, BRT_PRIMITIVE_LIBRARY,
			"*-Primitives", is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW) {
			BrResFree(primitive_library);
			return r;
		}

		if (primitive_library_count != primitive_library_size) {
			BrResFree(primitive_library);
			return BRE_FAIL;
		}

		/*
		 * Weed out any for which there is an equivalent scalar-specific version
		 */
		for (i = 0; i < primitive_library_count; i++) {

			identifier = ObjectIdentifier(primitive_library[i]);
			identifier_len = (identifier != NULL)? BrStrLen(identifier): 0;

			for (j = 0; j < *num_pl; j++)
				if (BrStrNICmp(identifier, ObjectIdentifier(primitive_library[j]), identifier_len) == 0)
					break;

			if (j != *num_pl)
				continue;

			if (*num_pl < max_pl)
				ppl[*num_pl] = primitive_library[i];

			(*num_pl)++;
		}

		BrResFree(primitive_library);

	} else {

		/*
		 * Search for all non-scalar-specific alternatives
		 */
		r = BrDevContainedFindMany((br_object **)&ppl[*num_pl], max_pl - *num_pl,
			&primitive_library_count, BRT_PRIMITIVE_LIBRARY,
			"*-Primitives", is_alternative);

		if (r != BRE_OK && r != BRE_OVERFLOW)
			return r;

		*num_pl += primitive_library_count;
	}

	if (*num_pl > max_pl)
		return BRE_OVERFLOW;

	return BRE_OK;
}


br_error BR_RESIDENT_ENTRY BrGeometryFormatFind(br_geometry **pgf,
				br_renderer *renderer,
				br_renderer_facility *renderer_facility,
				br_token scalar_type,
				br_token format_type)
{
	br_error r;
	br_geometry *gf;
	char object_name[] = "Default-Format-00000";
	char image_name[] = "softfrm0";
	br_boolean scalar_is_valid = BR_FALSE;

	/*
	 *	Construct names based on prefered scalar type
	 */
	switch(scalar_type) {
	case BRT_FLOAT:
		BrStrCpy(object_name+sizeof(object_name)-6,"Float");
		image_name[sizeof(image_name)-2] = 'f';
		scalar_is_valid = BR_TRUE;
		break;
	case BRT_FIXED:
		BrStrCpy(object_name+sizeof(object_name)-6,"Fixed");
		image_name[sizeof(image_name)-2] = 'x';
		scalar_is_valid = BR_TRUE;
		break;
	}

	/*
	 * See if the renderer itself knows about the format
	 */
	r = ObjectContainerFind(renderer, (br_object **)&gf, format_type, NULL, NULL);

	if(r == BRE_OK) {
		*pgf = gf;
		return BRE_OK;
	}

	/*
	 * Find the renderer facility, if we don't know it already
	 */
	if(!renderer_facility)
		ObjectQuery(renderer, (br_uint_32 *)&renderer_facility, BRT_RENDERER_FACILITY_O);

	if(renderer_facility) {
		/*
		 * Try the renderer's facility object
		 */
		r = ObjectContainerFind(renderer_facility, (br_object **)&gf, format_type, NULL, NULL);

		if(r == BRE_OK) {
			*pgf = gf;
			return BRE_OK;
		}
	}

	if(scalar_is_valid) {
		/*
		 * Look for a default in all loaded devices
		 */
		r = BrDevContainedFind((br_object **)&gf, format_type, object_name, NULL);

		if(r == BRE_OK) {
			*pgf = gf;
			return BRE_OK;
		}
	}

	/*
	 * Look for a default in all loaded devices
	 */
	r = BrDevContainedFind((br_object **)&gf, format_type, "Default-Format", NULL);

	if(r == BRE_OK) {
		*pgf = gf;
		return BRE_OK;
	}

	if(scalar_is_valid) {
		/*
		 * Try adding a device called 'softfrmX'
		 */
		BrDevCheckAdd(NULL,image_name,NULL);
		r = BrDevContainedFind((br_object **)&gf, format_type, object_name, NULL);

		if(r == BRE_OK) {
			*pgf = gf;
			return BRE_OK;
		}		
	}

	/*
	 * Try adding a device called 'softform'
	 */
	BrDevCheckAdd(NULL,"softform",NULL);
	r = BrDevContainedFind((br_object **)&gf, format_type, "Default-Format", NULL);

	if(r == BRE_OK) {
		*pgf = gf;
		return BRE_OK;
	}		

	return BRE_FAIL;

}

