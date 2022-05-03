/*
 * Copyright (c) 1993-1996 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: devquery.c 1.2 1997/12/11 13:34:07 jon Exp $
 * $Locker: $
 *
 * Routines for querying devices
 */
#include "brender.h"
#include "fw.h"
#include "brassert.h"

BR_RCS_ID("$Id: devquery.c 1.2 1997/12/11 13:34:07 jon Exp $")

STATIC br_boolean deviceEnum(br_device *dev, br_device_enum_cbfn *cbfn,
                             void *args)
{
    br_size_t s;
    void *mem;
    br_token_value *tvp;
    char *identifier = NULL, *creator = NULL, *title = NULL, *product = NULL,
         *product_version = NULL;
    br_uint_32 version = 0;
    br_boolean terminate;

	ObjectQueryAllSize((br_object *)dev, &s);

	if(s) {
        mem = BrMemAllocate(s, BR_MEMORY_APPLICATION);
		tvp = (br_token_value *)mem;
		ObjectQueryAll((br_object *)dev, mem, s);

        while(tvp->t != BR_NULL_TOKEN){
            switch(tvp->t){
                case BRT_IDENTIFIER_CSTR :
                    identifier = tvp->v.cstr;
                    break;
                case BRT_VERSION_U32 :
                    version = tvp->v.u32;
                    break;
                case BRT_CREATOR_CSTR :
                    creator = tvp->v.cstr;
                    break;
                case BRT_TITLE_CSTR :
                    title = tvp->v.cstr;
                    break;
                case BRT_PRODUCT_CSTR :
                    product = tvp->v.cstr;
                    break;
                case BRT_PRODUCT_VERSION_CSTR :
                    product_version = tvp->v.cstr;
                    break;
                default :
                    break;
            }
            tvp++;
        }

        terminate = cbfn(identifier, version, creator, title, product,
                         product_version, args);

		BrMemFree(mem);
	}

    return terminate;
}

br_error BR_PUBLIC_ENTRY BrDeviceEnum(br_device_enum_cbfn *cbfn, void *args)
{
	char devices_str[512], *device_name, *device_args, *next_device;
	br_device *device;
	br_error r;
	br_boolean pass_args = BR_TRUE;
	br_boolean terminate = BR_FALSE;
	int n;

	UASSERT(cbfn != NULL);

	BrSystemConfigQueryString(BRT_BRENDER_DEVICES_STR, devices_str,
	                          BR_ASIZE(devices_str));

	// If BRENDER_DEVICES is not set, use whatever value is
	// in BRENDER_DEFAULT_DEVICE instead.
	
	if (*devices_str == '\0') {
		BrSystemConfigQueryString(BRT_DEFAULT_DEVICE_STR, 
		                          devices_str, BR_ASIZE(devices_str));
		pass_args = BR_FALSE;
	}
	
	for (device_name = devices_str; !terminate && *device_name != '\0'; device_name = next_device) {

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
		 * Pass the device to the callback, adding and removing if necessary
		 */
		if (n) {

			r = BrDevFind(&device, device_name);

			if (r == BRE_OK) {

				terminate = deviceEnum(device, cbfn, args);

			} else {

				r = BrDevAdd(&device, device_name, pass_args? device_args: NULL);

				if (r == BRE_OK) {

					terminate = deviceEnum(device, cbfn, args);
					BrDevRemove(device);
				}
			}
		}
	}

    return BRE_OK;
}

STATIC br_boolean outfctyEnum(br_object *outfcty, br_outfcty_enum_cbfn *cbfn,
                              void *args)
{
    br_size_t s;
    void *mem;
    br_token_value *tvp;
    char *identifier = NULL;
    br_outfcty_desc desc;
    br_uint_32 version = 0;
    br_boolean terminate = BR_FALSE;

	desc.output_facility = (struct br_output_facility *)outfcty;

    identifier = NULL;
    desc.width = NULL;
    desc.width_min = NULL;
    desc.width_max = NULL;
    desc.height = NULL;
    desc.height_min = NULL;
    desc.height_max = NULL;
    desc.pmtype = 0;
    desc.pmbits = 0;
    desc.indexed = BR_FALSE;
    desc.fullscreen = BR_TRUE;

	ObjectQueryAllSize(outfcty, &s);

	if(s) {
        mem = BrMemAllocate(s, BR_MEMORY_APPLICATION);
		tvp = (br_token_value *)mem;
		ObjectQueryAll(outfcty, mem, s);

        while(tvp->t != BR_NULL_TOKEN){
            switch(tvp->t){
                case BRT_IDENTIFIER_CSTR :
                    identifier = tvp->v.cstr;
                    break;
                case BRT_WIDTH_I32 :
                    desc.width = tvp->v.i32;
                    break;
                case BRT_WIDTH_MIN_I32 :
                    desc.width_min = tvp->v.i32;
                    break;
                case BRT_WIDTH_MAX_I32 :
                    desc.width_max = tvp->v.i32;
                    break;
                case BRT_HEIGHT_I32 :
                    desc.height = tvp->v.i32;
                    break;
                case BRT_HEIGHT_MIN_I32 :
                    desc.height_min = tvp->v.i32;
                    break;
                case BRT_HEIGHT_MAX_I32 :
                    desc.height_max = tvp->v.i32;
                    break;
                case BRT_PIXEL_TYPE_U8 :
                    desc.pmtype = tvp->v.u8;
                    break;
                case BRT_PIXEL_BITS_I32 :
                    desc.pmbits = tvp->v.i32;
                    break;
                case BRT_INDEXED_B :
                    desc.indexed = tvp->v.b;
                    break;
                case BRT_WINDOW_FULLSCREEN_B :
                    desc.fullscreen = tvp->v.b;
                    break;
                default :
                    break;
            }
            tvp++;
        }

        terminate = cbfn(identifier, &desc, args);

		BrMemFree(mem);
	}

    return terminate;
}

br_error BR_PUBLIC_ENTRY BrOutputFacilityEnum(char *name,
                                              br_outfcty_enum_cbfn *cbfn,
                                              void *args)
{
    br_device *device;
    br_object **objects;
    br_int_32 max, n;
    br_error r;
    br_boolean terminate = BR_FALSE, remove = BR_FALSE;

    if(BrDevFind(&device, name) == BRE_FAIL){
		r = BrDevAdd(&device, name, NULL);
        if(r != BRE_OK)
            return r;
        remove = BR_TRUE;
    }

    ObjectContainerCount(device, &max, BRT_OUTPUT_FACILITY, NULL, NULL);
    objects = BrMemAllocate(max * sizeof(*objects), BR_MEMORY_APPLICATION);
    ObjectContainerFindMany(device, objects, max, &n, BRT_OUTPUT_FACILITY, NULL, NULL);

    ASSERT(max == n);

    for(n = 0; (n < max) && !terminate; n++)
        terminate = outfctyEnum(objects[n], cbfn, args);

   	BrMemFree(objects);

    if(remove)
        BrDevRemove(device);

    return BRE_OK;
}


