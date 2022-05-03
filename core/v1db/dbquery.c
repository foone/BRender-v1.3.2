/*
 * Copyright (c) 1993-1996 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: dbquery.c 1.1 1997/12/10 16:41:29 jon Exp $
 * $Locker: $
 *
 * Routines for querying devices
 */
#include "brender.h"
#include "brddi.h"
#include "brassert.h"

BR_RCS_ID("$Id: dbquery.c 1.1 1997/12/10 16:41:29 jon Exp $")


STATIC br_boolean rendfctyEnum(br_object *rendfcty, br_rendfcty_enum_cbfn *cbfn,
                              void *args)
{
    char *identifier = NULL;
    br_rendfcty_desc desc;

    identifier = ObjectIdentifier(rendfcty);

	desc.renderer_facility = (struct br_renderer_facility *)rendfcty;

	desc.uses_primitive_library = BR_TRUE;
	ObjectQuery(rendfcty, (br_uint_32 *)&desc.uses_primitive_library, BRT_USES_PRIMITIVE_LIBRARY_B);

	return cbfn(identifier, &desc, args);
}

br_error BR_PUBLIC_ENTRY BrRendererFacilityEnum(br_pixelmap *destination,
                                                br_rendfcty_enum_cbfn *cbfn,
                                                void *args)
{
    br_device *device;
    br_object *objects[64];
    br_int_32 num_objects, i;
    br_error r;

	r = BrRendererFacilityListFind((br_renderer_facility **)objects,
		&num_objects, BR_ASIZE(objects), (br_device_pixelmap *)destination, BR_SCALAR_TOKEN);

	if (r != BRE_OK)
		return r;

    for(i = 0; i < num_objects; i++)
        if (rendfctyEnum(objects[i], cbfn, args))
			break;

    return BRE_OK;
}


STATIC br_boolean primlibEnum(br_object *primlib, br_primlib_enum_cbfn *cbfn,
                              void *args)
{
    char *identifier = NULL;
    br_primlib_desc desc;

    identifier = ObjectIdentifier(primlib);

	desc.primitive_library = (struct br_primitive_library *)primlib;

	return cbfn(identifier, &desc, args);
}

br_error BR_PUBLIC_ENTRY BrPrimitiveLibraryEnum(br_pixelmap *destination,
                                                br_primlib_enum_cbfn *cbfn,
                                                void *args)
{
    br_device *device;
    br_object *objects[64];
    br_int_32 num_objects, i;
    br_error r;

	r = BrPrimitiveLibraryListFind((br_primitive_library **)objects,
		&num_objects, BR_ASIZE(objects), (br_device_pixelmap *)destination, BR_SCALAR_TOKEN);

	if (r != BRE_OK)
		return r;

    for(i = 0; i < num_objects; i++)
        if (primlibEnum(objects[i], cbfn, args))
			break;

    return BRE_OK;
}




